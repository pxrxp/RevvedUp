FROM ubuntu:22.04

# Install system dependencies
RUN apt-get update && apt-get install -y \
    git build-essential cmake clang pkg-config \
    libx11-dev libxrandr-dev libxcursor-dev libxi-dev libxinerama-dev \
    libgl1-mesa-dev libudev-dev libopenal-dev libflac-dev libvorbis-dev \
    libfreetype-dev libjpeg-dev libsndfile1-dev wget gnupg lsb-release \
    && rm -rf /var/lib/apt/lists/*

# Build SFML 2.6.2
RUN git clone --branch 2.6.2 https://github.com/SFML/SFML.git /tmp/SFML && \
    mkdir /tmp/SFML/build && cd /tmp/SFML/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/usr/local && \
    make -j$(nproc) && make install && rm -rf /tmp/SFML

# Build ImGui 1.89
RUN git clone --branch v1.89 https://github.com/ocornut/imgui.git /tmp/imgui

# Build ImGui-SFML 2.6
RUN git clone --branch v2.6 https://github.com/eliasdaler/imgui-sfml.git /tmp/imgui-sfml && \
    mkdir /tmp/imgui-sfml/build && cd /tmp/imgui-sfml/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON \
             -DIMGUI_DIR=/tmp/imgui -DCMAKE_INSTALL_PREFIX=/usr/local && \
    make -j$(nproc) && make install && rm -rf /tmp/imgui /tmp/imgui-sfml

# Set workspace
WORKDIR /workspace

