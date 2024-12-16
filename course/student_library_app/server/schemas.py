from pydantic import BaseModel
from typing import Optional

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

# Базовая схема для книги
class BookBase(BaseModel):
    title: str
    author: str

# Схема для создания книги
class BookCreate(BookBase):
    pass

# Схема для ответа при создании книги
class BookResponse(BookBase):
    id: int
    owner_id: int

    class Config:
        from_attributes = True  # Используем 'from_attributes' вместо 'orm_mode' для Pydantic v2
