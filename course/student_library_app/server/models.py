from sqlalchemy import Column, Integer, String, ForeignKey, Table
from sqlalchemy.orm import relationship
from database import Base

# Промежуточная таблица для связи многие-ко-многим между книгами и авторами


class BookAuthor(Base):
    __tablename__ = 'book_authors'

    book_id = Column(Integer, ForeignKey('books.id'), primary_key=True)
    author_id = Column(Integer, ForeignKey('authors.id'), primary_key=True)

    # Опционально добавить связи для удобного доступа
    book = relationship("Book", back_populates="book_authors")
    author = relationship("Author", back_populates="book_authors")

class User(Base):
    __tablename__ = "users"

    id = Column(Integer, primary_key=True, index=True)
    username = Column(String, unique=True, index=True)
    hashed_password = Column(String)
    role = Column(String, default="student")  # Роль пользователя

    books = relationship("Book", back_populates="owner")

class Author(Base):
    __tablename__ = "authors"

    id = Column(Integer, primary_key=True, index=True)
    first_name = Column(String)
    last_name = Column(String)
    middle_name = Column(String)

    
    book_authors = relationship("BookAuthor", back_populates="author")

class Book(Base):
    __tablename__ = "books"

    id = Column(Integer, primary_key=True, index=True)
    title = Column(String, index=True)
    download_link = Column(String)
    owner_id = Column(Integer, ForeignKey("users.id"))
    publication_city = Column(String)
    publisher = Column(String)
    publication_year = Column(Integer)
    page_count = Column(Integer)
    additional_info = Column(String)

    owner = relationship("User", back_populates="books")
    book_authors = relationship("BookAuthor", back_populates="book", cascade="all, delete-orphan")




