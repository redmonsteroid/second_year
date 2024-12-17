# Используем образ для сервера
FROM node:18

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем файлы проекта
COPY . .

# Устанавливаем live-server для запуска приложения
RUN npm install -g live-server

# Открываем порт 3001
EXPOSE 3001

# Команда для запуска live-server
CMD ["live-server", "--port=3001", "--host=0.0.0.0", "--no-browser"]
