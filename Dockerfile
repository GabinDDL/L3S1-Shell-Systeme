# Use Debian latest as the base image
FROM debian:latest

# Set working directory in the container
WORKDIR /workspace

# Install necessary packages
RUN apt-get update && apt-get install -y \
    git \
    gcc \
    make \
    valgrind \
    clang-format \
    libreadline-dev \
    # Add any other dependencies your project may need
    && rm -rf /var/lib/apt/lists/*

# Copy your project files into the container
COPY . /workspace

# Default command to keep the container running
CMD ["tail", "-f", "/dev/null"]