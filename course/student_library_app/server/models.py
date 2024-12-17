from sqlalchemy import Column, Integer, String, ForeignKey, Table
from sqlalchemy.orm import relationship
from database import Base

# Промежуточная таблица для связи многие-ко-многим между книгами и авторами
book_authors = Table(
    'book_authors',
    Base.metadata,
    Column('book_id', Integer, ForeignKey('books.id', ondelete="CASCADE"), primary_key=True),
    Column('author_id', Integer, ForeignKey('authors.id', ondelete="CASCADE"), primary_key=True)
)

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

    books = relationship("Book", secondary=book_authors, back_populates="authors")

class Book(Base):
    __tablename__ = "books"

    id = Column(Integer, primary_key=True, index=True)
    title = Column(String, index=True)
    publication_city = Column(String)
    publisher = Column(String)
    publication_year = Column(Integer)
    page_count = Column(Integer)
    additional_info = Column(String)
    download_link = Column(String)
    owner_id = Column(Integer, ForeignKey("users.id"))

    owner = relationship("User", back_populates="books")
    authors = relationship("Author", secondary=book_authors, back_populates="books")

