from fastapi import FastAPI, Depends, HTTPException, status, Query
from fastapi.middleware.cors import CORSMiddleware
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from sqlalchemy.orm import Session, joinedload
from database import SessionLocal, engine
import models
import schemas
from passlib.context import CryptContext
from jose import JWTError, jwt
from datetime import datetime, timedelta

# Создание экземпляра приложения
app = FastAPI()

# Создание таблиц в базе данных
models.Base.metadata.create_all(bind=engine)

# Настройка CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Указать конкретные источники в продакшене
    allow_credentials=True,
    allow_methods=["*"],  # Разрешить все методы (GET, POST, PUT, DELETE)
    allow_headers=["*"],  # Разрешить все заголовки
)

# Настройка bcrypt для хэширования паролей
pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")

# Конфигурация JWT
SECRET_KEY = "your_secret_key"  # Замените на более надёжный ключ
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
    expire = datetime.utcnow() + (expires_delta or timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES))
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

# Регистрация пользователя
@app.post("/register/", response_model=schemas.UserResponse)
def register_user(user: schemas.UserCreate, db: Session = Depends(get_db)):
    db_user = db.query(models.User).filter(models.User.username == user.username).first()
    if db_user:
        raise HTTPException(status_code=400, detail="Username already registered")
    if user.role not in ["teacher", "student"]:
        raise HTTPException(status_code=400, detail="Invalid role. Choose 'teacher' or 'student'.")
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
    return {"access_token": access_token, "token_type": "bearer", "role": user.role}

# Добавление книги
@app.post("/books/", response_model=schemas.BookResponse)
def create_book(book: schemas.BookCreate, db: Session = Depends(get_db), current_user: schemas.User = Depends(get_current_user)):
    # Создаём книгу без авторов
    db_book = models.Book(
        title=book.title,
        download_link=book.download_link,
        publication_city=book.publication_city,
        publisher=book.publisher,
        publication_year=book.publication_year,
        page_count=book.page_count,
        additional_info=book.additional_info,
        owner_id=current_user.id
    )

    # Добавляем книгу в базу данных
    db.add(db_book)
    db.commit()
    db.refresh(db_book)

    # Добавляем авторов и связываем их с книгой
    for author_data in book.authors:
        # Проверяем, существует ли автор в базе данных
        db_author = db.query(models.Author).filter_by(
            first_name=author_data.first_name,
            last_name=author_data.last_name,
            middle_name=author_data.middle_name
        ).first()

        # Если автор не существует, создаём нового
        if not db_author:
            db_author = models.Author(
                first_name=author_data.first_name,
                last_name=author_data.last_name,
                middle_name=author_data.middle_name
            )
            db.add(db_author)
            db.flush()  # Используем flush(), чтобы получить id нового автора до commit

        # Создаём связь между книгой и автором
        book_author = models.BookAuthor(book_id=db_book.id, author_id=db_author.id)
        db.add(book_author)

    # Один раз коммитим все изменения
    db.commit()
    db.refresh(db_book)

    return db_book




# Получение всех книг или с фильтрацией
@app.get("/books/", response_model=list[schemas.BookResponse])
def get_books(
    author: str | None = Query(None),
    title: str | None = Query(None),
    db: Session = Depends(get_db)
):
    # Подгружаем информацию о владельце книги
    books = db.query(models.Book).options(joinedload(models.Book.book_authors).joinedload(models.BookAuthor.author)).all()


    # Формируем результат с owner_username
    result = []
    for book in books:
        authors = [
            {"id": ba.author.id, "first_name": ba.author.first_name, "last_name": ba.author.last_name, "middle_name": ba.author.middle_name}
            for ba in book.book_authors
        ]
        book_data = {
            **book.__dict__,
            "owner_username": book.owner.username if book.owner else None,
            "authors": authors
        }
        result.append(book_data)

    return result


# Удаление книги
@app.delete("/books/{book_id}")
def delete_book(book_id: int, db: Session = Depends(get_db), current_user: models.User = Depends(get_current_user)):
    db_book = db.query(models.Book).filter(models.Book.id == book_id).first()
    if not db_book:
        raise HTTPException(status_code=404, detail="Book not found")
    if db_book.owner_id != current_user.id:
        raise HTTPException(status_code=403, detail="Not authorized to delete this book")
    db.delete(db_book)
    db.commit()
    return {"detail": "Book deleted successfully"}
