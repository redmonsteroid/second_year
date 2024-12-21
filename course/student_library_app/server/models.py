from sqlalchemy import Column, Integer, String, ForeignKey
from sqlalchemy.orm import relationship
from database import Base

class Role(Base):
    __tablename__ = "roles"

    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, nullable=False)

    # Один Role -> много Users
    users = relationship("User", back_populates="role_rel")


class User(Base):
    __tablename__ = "users"

    id = Column(Integer, primary_key=True, index=True)
    username = Column(String, unique=True, index=True, nullable=False)
    hashed_password = Column(String, nullable=False)
    role_id = Column(Integer, ForeignKey("roles.id"))

    # Один пользователь -> много книг
    books = relationship("Book", back_populates="owner")

    # Связь с таблицей Role (многие -> один)
    role_rel = relationship("Role", back_populates="users")


class Author(Base):
    __tablename__ = "authors"

    id = Column(Integer, primary_key=True, index=True)
    first_name = Column(String)
    last_name = Column(String)
    middle_name = Column(String)

    # Связь M:N через BookAuthor
    book_authors = relationship("BookAuthor", back_populates="author")


class Publisher(Base):
    __tablename__ = "publishers"

    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, nullable=False)
    city = Column(String)

    # Один издатель -> много книг
    books = relationship("Book", back_populates="publisher_rel")


class Book(Base):
    __tablename__ = "books"

    id = Column(Integer, primary_key=True, index=True)
    title = Column(String, index=True, nullable=False)
    download_link = Column(String)
    owner_id = Column(Integer, ForeignKey("users.id"))
    publication_year = Column(Integer)
    page_count = Column(Integer)
    additional_info = Column(String)
    publisher_id = Column(Integer, ForeignKey("publishers.id"))

    # Связи
    owner = relationship("User", back_populates="books")
    publisher_rel = relationship("Publisher", back_populates="books")
    book_authors = relationship("BookAuthor", back_populates="book", cascade="all, delete-orphan")


class BookAuthor(Base):
    __tablename__ = 'book_authors'

    book_id = Column(Integer, ForeignKey('books.id', ondelete='CASCADE'), primary_key=True)
    author_id = Column(Integer, ForeignKey('authors.id', ondelete='CASCADE'), primary_key=True)

    # Удобные связи для ORM
    book = relationship("Book", back_populates="book_authors")
    author = relationship("Author", back_populates="book_authors")
