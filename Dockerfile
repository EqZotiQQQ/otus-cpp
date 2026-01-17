FROM ubuntu:24.04

# Устанавливаем переменные окружения
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем clang 18 (поддержка C++23) и базовые инструменты
RUN apt-get update && apt-get install -y \
    clang-18 \
    clangd-18 \
    clang-tidy-18 \
    clang-format-18 \
    lldb-18 \
    cmake \
    ninja-build \
    git \
    gdb \
    python3 \
    python3-pip \
    doxygen \
    graphviz \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Создаем символические ссылки для clang-18
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-18 100 \
    && update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-18 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100


# Создаем пользователя разработчика (совпадает с хостовым)
ARG USERNAME=eqzotiqqq
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Создаем пользователя и группу
# RUN groupadd --gid $USER_GID $USERNAME \
#     && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
#     && apt-get update \
#     && apt-get install -y sudo \
#     && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
#     && chmod 0440 /etc/sudoers.d/$USERNAME

# Переключаемся на пользователя
USER $USERNAME

# Настраиваем рабочую директорию
WORKDIR /workspace

# Устанавливаем команду по умолчанию
CMD ["sleep", "infinity"]