# Указываем компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Igoogletest/googletest/include -I./src --coverage
GTEST_LIBS = googletest/build/lib/libgtest.a googletest/build/lib/libgtest_main.a -pthread --coverage

# Исходные файлы и тесты
SRC = src/mainHeader.cpp
TEST_SRC = tests/test.cpp
OBJ = $(SRC:.cpp=.o)
TEST_OBJ = $(TEST_SRC:.cpp=.o)

# Директория для отчета о покрытии
COVERAGE_DIR = coverage_report

# Цель для запуска всех тестов
all: RunTests coverage_report

# Цель для создания исполняемого файла с тестами
RunTests: $(OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o RunTests $(OBJ) $(TEST_OBJ) $(GTEST_LIBS)

# Компиляция исходных файлов
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Генерация отчета о покрытии
coverage_info: RunTests
	./RunTests
	lcov --capture --directory . --output-file coverage.info --ignore-errors mismatch,inconsistent || { echo "Ошибка при выполнении lcov!"; exit 1; }
	lcov --remove coverage.info '/usr/*' '*/googletest/*' --output-file coverage.info || { echo "Ошибка при фильтрации lcov!"; exit 1; }

# Создание HTML-отчета
coverage_report: coverage_info
	genhtml coverage.info --output-directory $(COVERAGE_DIR) --ignore-errors inconsistent || { echo "Ошибка при создании HTML-отчета!"; exit 1; }

# Чистка сгенерированных файлов
clean:
	rm -f $(OBJ) $(TEST_OBJ) RunTests coverage.info
	rm -rf $(COVERAGE_DIR)
