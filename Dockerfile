# Базовый образ с Ubuntu
FROM ubuntu:24.04

# Обновляем пакеты и устанавливаем зависимости для сборки
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    clang \
    lldb \
    lld \
    git \
    gdb \
    vim \
    nano \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Создаем рабочую директорию
WORKDIR /app

# Копируем CMakeLists.txt и минимальные файлы (для кэширования слоев)
COPY CMakeLists.txt /app/

# Переключаемся в build
WORKDIR /app

# По умолчанию запускаем bash, чтобы можно было заходить в контейнер
CMD ["/bin/bash"]
