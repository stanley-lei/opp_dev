FROM ubuntu:20.04

# Set non-interactive installation mode
ENV DEBIAN_FRONTEND=noninteractive

ARG USER_ID=1000
ARG GROUP_ID=1000
ARG USERNAME=dockeruser

# Set up timezone
ENV TZ=UTC
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install basic build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    pkg-config \
    software-properties-common \
    apt-transport-https \
    ca-certificates \
    gnupg \
    lsb-release \
    sudo \
    vim \
    # ARM64 cross-compilation toolchain
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    binutils-aarch64-linux-gnu \
    # Libraries and dependencies
    libffi-dev \
    libssl-dev \
    zlib1g-dev \
    libbz2-dev \
    libreadline-dev \
    libsqlite3-dev \
    libncurses5-dev \
    libncursesw5-dev \
    xz-utils \
    tk-dev \
    liblzma-dev \
    libgdbm-dev \
    libnss3-dev \
    libc6-dev \
    # Python dependencies
    python3.8 \
    python3.8-dev \
    python3.8-venv \
    python3-pip \
    # OpenCL dependencies (for CUDA cross-compilation)
    opencl-headers \
    ocl-icd-opencl-dev \
    # Additional dependencies for openpilot
    libopencv-dev \
    libcapnp-dev \
    libzmq3-dev \
    libsystemd-dev \
    libusb-1.0-0-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Set up Python alternatives to use 3.8
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 1 && \
    update-alternatives --set python3 /usr/bin/python3.8 && \
    ln -sf /usr/bin/python3 /usr/bin/python && \
    ln -sf /usr/bin/pip3 /usr/bin/pip && \
    pip install --no-cache-dir --upgrade pip setuptools wheel

# Install cross-compiler for aarch64
RUN apt-get install -y gcc-aarch64-linux-gnu g++-aarch64-linux-gnu

# Verify GCC version (should be 9.4.0 for Ubuntu 20.04)
RUN aarch64-linux-gnu-gcc --version

# Optional: Set environment variables for cross-compilation
#ENV CC=aarch64-linux-gnu-gcc
#ENV CXX=aarch64-linux-gnu-g++

# Handle existing user with same UID if exists
RUN set -e; \
    # Check if a user with USER_ID exists
    if getent passwd ${USER_ID} > /dev/null; then \
        existing_username=$(getent passwd ${USER_ID} | cut -d: -f1); \
        # Only delete if username is not empty
        if [ -n "${existing_username}" ]; then \
            userdel -f "${existing_username}"; \
        fi; \
    fi

# Handle existing group with same GID if exists
RUN if getent group ${GROUP_ID} > /dev/null; then \
        existing_groupname=$(getent group ${GROUP_ID} | cut -d: -f1); \
        groupdel ${existing_groupname}; \
    fi

# Create group with the specified GID
RUN groupadd -g ${GROUP_ID} ${USERNAME}

# Create user with the specified UID and GID
RUN useradd -u ${USER_ID} -g ${GROUP_ID} -m -s /bin/bash ${USERNAME} && \
    usermod -aG sudo ${USERNAME} && \
    echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME}

# Set up the workspace directory
WORKDIR /home/${USERNAME}/workspace/orin

# Install pyenv for Python version management
RUN apt-get update && apt-get install -y \
    libffi-dev \
    libssl-dev \
    zlib1g-dev \
    libbz2-dev \
    libreadline-dev \
    libsqlite3-dev \
    curl \
    git \
    && rm -rf /var/lib/apt/lists/*

# Ensure directory exists and set permissions
RUN mkdir -p /home/${USERNAME}/workspace/orin && \
    chown -R ${USERNAME}:${USERNAME} /home/${USERNAME}

# Default to the user
USER ${USERNAME}
# [Optional] Copy your OpenPilot source here:
# COPY ./openpilot /home/dev/openpilot

CMD ["bash"]
