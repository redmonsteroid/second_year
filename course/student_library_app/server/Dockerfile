# Используем минимальный базовый образ
FROM python:3.11

# Установка зависимостей
RUN apt-get update && apt-get install -y libpq-dev && rm -rf /var/lib/apt/lists/*

# Установка рабочей директории
WORKDIR /app

# Копирование файлов приложения
COPY . /app

# Установка Python-зависимостей
RUN pip install --no-cache-dir -r requirements.txt

# Создание пользователя tester
ARG UID=100001
ARG GID=100001
RUN groupadd -g $GID tester && useradd -u $UID -g $GID -m tester

# Переключение на пользователя tester
USER tester

# Команда запуска
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
