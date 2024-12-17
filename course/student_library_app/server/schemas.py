from pydantic import BaseModel, Field
from typing import Optional, List

class UserUpdatePassword(BaseModel):
    old_password: str
    new_password: str

class UserUpdateRole(BaseModel):
    role: str


# Базовая схема для пользователя
class UserBase(BaseModel):
    username: str
    role: str

# Схема для создания пользователя (с паролем)
class UserCreate(UserBase):
    password: str

# Схема для ответа при создании пользователя
class UserResponse(UserBase):
    id: int

    class Config:
        from_attributes = True  # Используем 'from_attributes' вместо 'orm_mode' для Pydantic v2

# Схема для входа пользователя
class UserLogin(BaseModel):
    username: str
    password: str

# Схема для JWT-токена
class Token(BaseModel):
    access_token: str
    token_type: str

# Схема для создания автора
class AuthorCreate(BaseModel):
    first_name: Optional[str] = None
    last_name: Optional[str] = None
    middle_name: Optional[str] = None

# Схема для отображения автора
class AuthorResponse(AuthorCreate):
    id: int

    class Config:
        orm_mode = True

# Схема для создания книги
class BookCreate(BaseModel):
    title: str
    author: Optional[str] = None
    download_link: Optional[str] = None
    publication_city: str | None = "Unknown"
    publisher: str
    publication_year: int
    page_count: int
    additional_info: Optional[str] = None
    authors: List["AuthorCreate"] = Field(default_factory=list)

    class Config:
        from_attributes = True

# Схема для отображения книги
class BookResponse(BookCreate):
    id: int
    owner_id: int
    authors: List[AuthorResponse]

    class Config:
        from_attributes = True


# Схема для создания пользователя
class UserCreate(BaseModel):
    username: str
    password: str
    role: str

# Схема для отображения пользователя
class UserResponse(BaseModel):
    id: int
    username: str
    role: str

    class Config:
        orm_mode = True

