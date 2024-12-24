package main

import (
	"image/color"
	"math/rand"
	"time"

	"github.com/faiface/pixel"
	"github.com/faiface/pixel/pixelgl"
	"github.com/faiface/pixel/text"
	"golang.org/x/image/font/basicfont"
)

type Cave struct {
	width, height int
	grid          [][]bool
	birthLimit    int
	deathLimit    int
	birthChance   float64
}

// NewCave создает новую пещеру
func NewCave(width, height, birthLimit, deathLimit int, birthChance float64) *Cave {
	cave := &Cave{
		width:       width,
		height:      height,
		birthLimit:  birthLimit,
		deathLimit:  deathLimit,
		birthChance: birthChance,
		grid:        make([][]bool, height),
	}
	for y := range cave.grid {
		cave.grid[y] = make([]bool, width)
	}
	return cave
}

// Initialize заполняет пещеру случайными данными
func (c *Cave) Initialize() {
	rand.Seed(time.Now().UnixNano())
	for y := 0; y < c.height; y++ {
		for x := 0; x < c.width; x++ {
			c.grid[y][x] = rand.Float64() < c.birthChance
		}
	}
}

// Step выполняет одну итерацию клеточного автомата
func (c *Cave) Step() {
	newGrid := make([][]bool, c.height)
	for y := range newGrid {
		newGrid[y] = make([]bool, c.width)
	}

	for y := 0; y < c.height; y++ {
		for x := 0; x < c.width; x++ {
			aliveNeighbors := c.countAliveNeighbors(x, y)
			if c.grid[y][x] {
				newGrid[y][x] = aliveNeighbors >= c.deathLimit
			} else {
				newGrid[y][x] = aliveNeighbors > c.birthLimit
			}
		}
	}

	c.grid = newGrid
}

// countAliveNeighbors считает количество живых соседей для клетки
func (c *Cave) countAliveNeighbors(x, y int) int {
	alive := 0
	for dy := -1; dy <= 1; dy++ {
		for dx := -1; dx <= 1; dx++ {
			if dx == 0 && dy == 0 {
				continue
			}
			newX, newY := x+dx, y+dy
			if newX >= 0 && newX < c.width && newY >= 0 && newY < c.height && c.grid[newY][newX] {
				alive++
			}
		}
	}
	return alive
}

// Draw отрисовывает пещеру в окне
func (c *Cave) Draw(win *pixelgl.Window, cellSize float64) {
	for y := 0; y < c.height; y++ {
		for x := 0; x < c.width; x++ {
			if c.grid[y][x] {
				rect := pixel.R(float64(x)*cellSize, float64(y)*cellSize, float64(x+1)*cellSize, float64(y+1)*cellSize)
				pixel.NewSprite(nil, rect).Draw(win, pixel.IM.Moved(rect.Center()))
			}
		}
	}
}

// drawText рисует текст на экране
func drawText(win *pixelgl.Window, content string, pos pixel.Vec) {
	basicAtlas := text.NewAtlas(basicfont.Face7x13, text.ASCII)
	txt := text.New(pos, basicAtlas)
	txt.Color = color.Black
	_, _ = txt.WriteString(content)
	txt.Draw(win, pixel.IM)
}

func run() {
	width, height := 50, 50
	birthLimit, deathLimit := 4, 3
	birthChance := 0.4
	cellSize := 15.0

	cave := NewCave(width, height, birthLimit, deathLimit, birthChance)
	cave.Initialize()

	cfg := pixelgl.WindowConfig{
		Title:  "Cave Generator",
		Bounds: pixel.R(0, 0, float64(width)*cellSize, float64(height)*cellSize+50), // Добавляем место для текста
		VSync:  true,
	}
	win, err := pixelgl.NewWindow(cfg)
	if err != nil {
		panic(err)
	}

	for !win.Closed() {
		win.Clear(color.White)
		cave.Draw(win, cellSize)

		// Рисуем текстовые подсказки
		drawText(win, "Press R for new generation", pixel.V(10, (float64(height)+1)*cellSize))
		drawText(win, "Press SPACE for next iteration", pixel.V(10, (float64(height)+2)*cellSize))

		// Проверка нажатия клавиш
		if win.JustPressed(pixelgl.KeySpace) {
			cave.Step()
		}
		if win.JustPressed(pixelgl.KeyR) {
			cave.Initialize()
		}

		win.Update()
	}
}

func main() {
	pixelgl.Run(run)
}
