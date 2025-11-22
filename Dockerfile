FROM ubuntu:22.04

# Install system dependencies
RUN apt-get update && apt-get install -y \
    git build-essential cmake clang pkg-config \
    libx11-dev libxrandr-dev libxcursor-dev libxi-dev libxinerama-dev \
    libgl1-mesa-dev libudev-dev libflac-dev libvorbis-dev \
    libfreetype-dev libjpeg-dev libsndfile1-dev wget gnupg lsb-release \
    && rm -rf /var/lib/apt/lists/*

# Build OpenAL 1.22.2 statically
RUN git clone --branch 1.22.2 https://github.com/kcat/openal-soft.git /tmp/openal-soft && \
    mkdir -p /tmp/openal-soft/build && cd /tmp/openal-soft/build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DALSOFT_EXAMPLES=OFF \
        -DBUILD_TESTS=OFF \
        -DLIBTYPE=STATIC && \
    make -j$(nproc) && \
    make install && \
    rm -rf /tmp/openal-soft

# Inject a fake OpenAL CMake target so SFML's dependency logic stops erroring.
RUN echo 'add_library(OpenAL STATIC IMPORTED)' > /usr/local/lib/cmake/OpenALFix.cmake && \
    echo 'set_target_properties(OpenAL PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libopenal.a")' >> /usr/local/lib/cmake/OpenALFix.cmake && \
    echo 'set_target_properties(OpenAL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "/usr/local/include")' >> /usr/local/lib/cmake/OpenALFix.cmake

# Build SFML 2.6.2 statically
RUN git clone --branch 2.6.2 https://github.com/SFML/SFML.git /tmp/SFML && \
    mkdir /tmp/SFML/build && cd /tmp/SFML/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DBUILD_SHARED_LIBS=OFF \
             -DCMAKE_INSTALL_PREFIX=/usr/local \
             -DCMAKE_PREFIX_PATH=/usr/local && \
    make -j$(nproc) && make install && rm -rf /tmp/SFML

# Build ImGui 1.89
RUN git clone --branch v1.89 https://github.com/ocornut/imgui.git /tmp/imgui

# Build ImGui-SFML 2.6
RUN git clone --branch v2.6 https://github.com/eliasdaler/imgui-sfml.git /tmp/imgui-sfml && \
    mkdir /tmp/imgui-sfml/build && cd /tmp/imgui-sfml/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \
             -DIMGUI_DIR=/tmp/imgui -DCMAKE_INSTALL_PREFIX=/usr/local && \
    make -j$(nproc) && make install && rm -rf /tmp/imgui /tmp/imgui-sfml

# Set workspace
WORKDIR /workspace
