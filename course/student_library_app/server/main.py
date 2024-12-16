from fastapi import FastAPI, Depends, HTTPException, status
from sqlalchemy.orm import Session
from database import SessionLocal, engine
import models
import schemas
from passlib.context import CryptContext
from jose import JWTError, jwt
from datetime import datetime, timedelta
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm

app = FastAPI()

# Создание таблиц в базе данных
models.Base.metadata.create_all(bind=engine)

# Настройка bcrypt для хэширования паролей
pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

# Конфигурация JWT
SECRET_KEY = "your_secret_key"  # Заменить на более надёжный ключ
ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30

# OAuth2 схема для получения токена
oauth2_scheme = OAuth2PasswordBearer(tokenUrl="login")

# Зависимость для получения сессии базы данных
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

# Функция для создания JWT-токена
def create_access_token(data: dict, expires_delta: timedelta | None = None):
    to_encode = data.copy()
    expire = datetime.utcnow() + (expires_delta or timedelta(minutes=15))
    to_encode.update({"exp": expire})
    return jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)

# Функция для проверки токена и получения текущего пользователя
def get_current_user(token: str = Depends(oauth2_scheme), db: Session = Depends(get_db)):
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        username: str = payload.get("sub")
        if username is None:
            raise HTTPException(status_code=401, detail="Invalid credentials")
        user = db.query(models.User).filter(models.User.username == username).first()
        if user is None:
            raise HTTPException(status_code=401, detail="Invalid credentials")
        return user
    except JWTError:
        raise HTTPException(status_code=401, detail="Invalid credentials")

# Функция для проверки роли пользователя
def check_role(required_role: str):
    def role_checker(current_user: models.User = Depends(get_current_user)):
        if current_user.role != required_role:
            raise HTTPException(status_code=403, detail=f"Only {required_role}s are allowed to perform this action")
        return current_user
    return role_checker

# Регистрация пользователя
@app.post("/register/", response_model=schemas.UserResponse)
def register_user(user: schemas.UserCreate, db: Session = Depends(get_db)):
    db_user = db.query(models.User).filter(models.User.username == user.username).first()
    if db_user:
        raise HTTPException(status_code=400, detail="Username already registered")
    hashed_password = pwd_context.hash(user.password)
    new_user = models.User(username=user.username, hashed_password=hashed_password, role=user.role)
    db.add(new_user)
    db.commit()
    db.refresh(new_user)
    return new_user

# Аутентификация пользователя
@app.post("/login/")
def login_user(form_data: OAuth2PasswordRequestForm = Depends(), db: Session = Depends(get_db)):
    user = db.query(models.User).filter(models.User.username == form_data.username).first()
    if not user or not pwd_context.verify(form_data.password, user.hashed_password):
        raise HTTPException(status_code=400, detail="Incorrect username or password")
    access_token_expires = timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    access_token = create_access_token(data={"sub": user.username}, expires_delta=access_token_expires)
    return {"access_token": access_token, "token_type": "bearer"}

# Получение текущего пользователя
@app.get("/users/me/", response_model=schemas.UserResponse)
def read_users_me(current_user: models.User = Depends(get_current_user)):
    return current_user

# Обновление пароля пользователя
@app.put("/users/update_password/")
def update_password(
    user_update: schemas.UserUpdatePassword,
    current_user: models.User = Depends(get_current_user),
    db: Session = Depends(get_db),
):
    if not pwd_context.verify(user_update.old_password, current_user.hashed_password):
        raise HTTPException(status_code=400, detail="Old password is incorrect")
    current_user.hashed_password = pwd_context.hash(user_update.new_password)
    db.commit()
    return {"message": "Password updated successfully"}

# Обновление роли пользователя (доступно для преподавателей и администраторов)
@app.put("/users/{user_id}/update_role/")
def update_role(
    user_id: int,
    user_update: schemas.UserUpdateRole,
    current_user: models.User = Depends(get_current_user),
    db: Session = Depends(get_db),
):
    if current_user.role not in ["teacher", "admin"]:
        raise HTTPException(status_code=403, detail="Not authorized to update roles")
    user = db.query(models.User).filter(models.User.id == user_id).first()
    if not user:
        raise HTTPException(status_code=404, detail="User not found")
    user.role = user_update.role
    db.commit()
    return {"message": f"User role updated to {user.role}"}

# Создание книги (только для преподавателей)
@app.post("/books/", response_model=schemas.BookResponse)
def create_book(
    book: schemas.BookCreate,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(check_role("teacher")),
):
    db_book = models.Book(**book.dict(), owner_id=current_user.id)
    db.add(db_book)
    db.commit()
    db.refresh(db_book)
    return db_book

# Получение всех книг
@app.get("/books/", response_model=list[schemas.BookResponse])
def get_books(db: Session = Depends(get_db)):
    return db.query(models.Book).all()

# Получение книги по ID
@app.get("/books/{book_id}", response_model=schemas.BookResponse)
def get_book_by_id(book_id: int, db: Session = Depends(get_db)):
    db_book = db.query(models.Book).filter(models.Book.id == book_id).first()
    if not db_book:
        raise HTTPException(status_code=404, detail="Book not found")
    return db_book

# Удаление книги (только для владельца книги)
@app.delete("/books/{book_id}")
def delete_book(
    book_id: int,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user),
):
    db_book = db.query(models.Book).filter(models.Book.id == book_id).first()
    if not db_book:
        raise HTTPException(status_code=404, detail="Book not found")
    if db_book.owner_id != current_user.id:
        raise HTTPException(status_code=403, detail="Not authorized to delete this book")
    db.delete(db_book)
    db.commit()
    return {"detail": "Book deleted successfully"}
