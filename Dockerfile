FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake libsqlite3-dev libssl-dev pkg-config git ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Create third_party directory and download libraries
RUN mkdir -p /app/third_party && \
    git clone --depth 1 --branch v1.0.5 https://github.com/CrowCpp/Crow.git /app/third_party/crow && \
    git clone --depth 1 https://github.com/nlohmann/json.git /app/third_party/nlohmann && \
    git clone --depth 1 https://github.com/chriskohlhoff/asio.git /app/third_party/asio

COPY . /app

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j"$(nproc)"

EXPOSE 8080
CMD ["/app/build/swifttrack"]
