from pydantic import BaseModel
from typing import Optional, List

# ---------- ROLE ----------
class RoleResponse(BaseModel):
    id: int
    name: str

    class Config:
        from_attributes = True

# ---------- USER ----------
class UserCreate(BaseModel):
    username: str
    password: str
    # Роль приходит строкой (например, "teacher"/"student"), мы потом ищем или создаём ее
    role: str

class UserResponse(BaseModel):
    id: int
    username: str
    # Возвращаем роль объектом
    role_rel: Optional[RoleResponse] = None

    class Config:
        from_attributes = True


# ---------- AUTHOR ----------
class AuthorCreate(BaseModel):
    first_name: Optional[str] = None
    last_name: Optional[str] = None
    middle_name: Optional[str] = None

class AuthorResponse(BaseModel):
    id: int
    first_name: Optional[str] = None
    last_name: Optional[str] = None
    middle_name: Optional[str] = None

    class Config:
        from_attributes = True

# ---------- PUBLISHER ----------
class PublisherCreate(BaseModel):
    name: str
    city: Optional[str] = None

class PublisherResponse(BaseModel):
    id: int
    name: str
    city: Optional[str] = None

    class Config:
        from_attributes = True

# ---------- BOOK ----------
class BookCreate(BaseModel):
    title: str
    download_link: Optional[str] = None
    publication_year: Optional[int] = None
    page_count: Optional[int] = None
    additional_info: Optional[str] = None

    # Авторы (список)
    authors: List[AuthorCreate] = []

    # Издатель
    publisher: Optional[PublisherCreate] = None

    class Config:
        from_attributes = True

class BookResponse(BaseModel):
    id: int
    title: str
    download_link: Optional[str] = None
    publication_year: Optional[int] = None
    page_count: Optional[int] = None
    additional_info: Optional[str] = None
    owner_id: int

    # Издатель
    publisher_rel: Optional[PublisherResponse] = None

    # Авторы
    authors: List[AuthorResponse] = []

    # Для вывода имени владельца (необязательно)
    owner_username: Optional[str] = None

    class Config:
        from_attributes = True
