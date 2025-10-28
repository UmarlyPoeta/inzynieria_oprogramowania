# NetSimCPP Docker Image
FROM ubuntu:22.04

# Prevent interactive prompts during build
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    g++ \
    libcpprest-dev \
    nlohmann-json3-dev \
    libssl-dev \
    libgtest-dev \
    libmysqlcppconn-dev \
    mysql-client \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

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

# Expose REST API port
EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/status || exit 1

# Run the server
CMD ["./netsim"]
