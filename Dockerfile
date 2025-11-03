# NetSimCPP Docker Image
FROM ubuntu:22.04

# Prevent interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    pkg-config \
    libcpprest-dev \
    nlohmann-json3-dev \
    libssl-dev \
    libgtest-dev \
    libmysqlcppconn-dev \
    libwebsocketpp-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libhiredis-dev \
    libargon2-dev \
    mysql-client \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Build and install yaml-cpp from source (Ubuntu 22.04 version lacks CMake config)
RUN git clone https://github.com/jbeder/yaml-cpp.git /tmp/yaml-cpp && \
    cd /tmp/yaml-cpp && \
    mkdir build && cd build && \
    cmake .. -DBUILD_SHARED_LIBS=ON -DYAML_BUILD_SHARED_LIBS=ON -DYAML_CPP_BUILD_TESTS=OFF && \
    make -j$(nproc) && make install && \
    ldconfig && \
    rm -rf /tmp/yaml-cpp && \
    echo "yaml-cpp built and installed from source"

# Install picojson (header-only library, required by jwt-cpp)
RUN curl -L -o /usr/local/include/picojson.h https://raw.githubusercontent.com/kazuho/picojson/master/picojson.h && \
    mkdir -p /usr/local/include/picojson && \
    cp /usr/local/include/picojson.h /usr/local/include/picojson/picojson.h && \
    ls -la /usr/local/include/picojson/ && \
    echo "PicoJSON installed successfully"

# Install jwt-cpp (header-only library)
RUN git clone https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp && \
    cp -r /tmp/jwt-cpp/include/jwt-cpp /usr/local/include/ && \
    rm -rf /tmp/jwt-cpp && \
    echo "JWT-CPP installed successfully"

# Set working directory
WORKDIR /app

# Copy project files
COPY project/backend /app/backend
COPY project/database /app/database
COPY README.md /app/

# Build the project
WORKDIR /app/backend
RUN cmake . && make -j$(nproc)

# Run tests to verify build
RUN ./netsim_tests

# Expose ports
EXPOSE 8080  
EXPOSE 9001  

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/status || exit 1

# Run the server
CMD ["./netsim"]
