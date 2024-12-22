from fastapi import FastAPI, Depends, HTTPException, Query
from fastapi.middleware.cors import CORSMiddleware
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from passlib.context import CryptContext
from jose import jwt, JWTError
from datetime import datetime, timedelta
from sqlalchemy.orm import Session, joinedload
from typing import List
from database import SessionLocal, engine
import models
import schemas
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

SECRET_KEY = "your_secret_key"  # Поменяйте на более надежный
ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30

pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
oauth2_scheme = OAuth2PasswordBearer(tokenUrl="login")

app = FastAPI()

# Создаём таблицы (если их нет)
models.Base.metadata.create_all(bind=engine)

# Настройка CORS (если нужно)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


def create_access_token(data: dict, expires_delta: timedelta | None = None):
    to_encode = data.copy()
    expire = datetime.utcnow() + (expires_delta or timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES))
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)
    return encoded_jwt


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

@app.on_event("startup")
def startup_event():
    logger.info("Application started.")

@app.on_event("shutdown")
def shutdown_event():
    logger.info("Application stopped.")

@app.get("/health")
def health_check():
    return {"status": "ok"}


@app.post("/register/", response_model=schemas.UserResponse)
def register_user(user_data: schemas.UserCreate, db: Session = Depends(get_db)):
    # Проверяем, что пользователя с таким именем нет
    existing_user = db.query(models.User).filter(models.User.username == user_data.username).first()
    if existing_user:
        raise HTTPException(status_code=400, detail="Username already registered")

    # Ищем или создаём роль
    role_name = user_data.role  # "teacher", "student", "admin" и т.д.
    role_obj = db.query(models.Role).filter(models.Role.name == role_name).first()
    if not role_obj:
        role_obj = models.Role(name=role_name)
        db.add(role_obj)
        db.flush()

    hashed_password = pwd_context.hash(user_data.password)
    new_user = models.User(
        username=user_data.username,
        hashed_password=hashed_password,
        role_id=role_obj.id
    )
    db.add(new_user)
    db.commit()
    db.refresh(new_user)

    return new_user


@app.post("/login/")
def login_user(form_data: OAuth2PasswordRequestForm = Depends(), db: Session = Depends(get_db)):
    user = db.query(models.User).filter(models.User.username == form_data.username).first()
    if not user or not pwd_context.verify(form_data.password, user.hashed_password):
        raise HTTPException(status_code=400, detail="Incorrect username or password")

    access_token_expires = timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES)
    access_token = create_access_token(data={"sub": user.username}, expires_delta=access_token_expires)

    # Пример: возвращаем и роль (строкой), извлекая её из связанного Role
    role_name = user.role_rel.name if user.role_rel else None
    return {
        "access_token": access_token,
        "token_type": "bearer",
        "role": role_name
    }


@app.post("/books/", response_model=schemas.BookResponse)
def create_book(
    book_data: schemas.BookCreate,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    # "Get or create" publisher (если указан)
    publisher_id = None
    if book_data.publisher:
        existing_publisher = db.query(models.Publisher).filter_by(
            name=book_data.publisher.name,
            city=book_data.publisher.city
        ).first()
        if existing_publisher:
            publisher_id = existing_publisher.id
        else:
            new_publisher = models.Publisher(
                name=book_data.publisher.name,
                city=book_data.publisher.city
            )
            db.add(new_publisher)
            db.flush()
            publisher_id = new_publisher.id

    # Создаём книгу
    db_book = models.Book(
        title=book_data.title,
        download_link=book_data.download_link,
        publication_year=book_data.publication_year,
        page_count=book_data.page_count,
        additional_info=book_data.additional_info,
        owner_id=current_user.id,
        publisher_id=publisher_id
    )
    db.add(db_book)
    db.flush()

    # Привязка авторов
    for author_data in book_data.authors:
        db_author = db.query(models.Author).filter_by(
            first_name=author_data.first_name,
            last_name=author_data.last_name,
            middle_name=author_data.middle_name
        ).first()
        if not db_author:
            db_author = models.Author(
                first_name=author_data.first_name,
                last_name=author_data.last_name,
                middle_name=author_data.middle_name
            )
            db.add(db_author)
            db.flush()

        book_author = models.BookAuthor(book_id=db_book.id, author_id=db_author.id)
        db.add(book_author)

    db.commit()
    db.refresh(db_book)
    return db_book


@app.get("/books/", response_model=List[schemas.BookResponse])
def get_books(db: Session = Depends(get_db)):
    # Подгружаем связанные модели
    books = (
        db.query(models.Book)
        .options(
            joinedload(models.Book.book_authors).joinedload(models.BookAuthor.author),
            joinedload(models.Book.publisher_rel),
            joinedload(models.Book.owner)
        )
        .all()
    )

    result = []
    for book in books:
        # Формируем список авторов
        authors_list = []
        for ba in book.book_authors:
            if ba.author:
                authors_list.append({
                    "id": ba.author.id,
                    "first_name": ba.author.first_name,
                    "last_name": ba.author.last_name,
                    "middle_name": ba.author.middle_name
                })
        
        # Готовим publisher
        publisher_data = None
        if book.publisher_rel:
            publisher_data = {
                "id": book.publisher_rel.id,
                "name": book.publisher_rel.name,
                "city": book.publisher_rel.city
            }

        # Собираем итоговый объект
        book_dict = {
            "id": book.id,
            "title": book.title,
            "download_link": book.download_link,
            "publication_year": book.publication_year,
            "page_count": book.page_count,
            "additional_info": book.additional_info,
            "owner_id": book.owner_id,
            "owner_username": book.owner.username if book.owner else None,
            "publisher_rel": publisher_data,
            "authors": authors_list
        }
        result.append(book_dict)

    return result

@app.delete("/books/{book_id}")
def delete_book(
    book_id: int,
    db: Session = Depends(get_db),
    current_user: models.User = Depends(get_current_user)
):
    db_book = db.query(models.Book).filter(models.Book.id == book_id).first()
    if not db_book:
        raise HTTPException(status_code=404, detail="Book not found")

    # Проверяем, что только владелец может удалить
    if db_book.owner_id != current_user.id:
        raise HTTPException(status_code=403, detail="Not authorized to delete this book")

    db.delete(db_book)
    db.commit()
    return {"detail": "Book deleted successfully"}
