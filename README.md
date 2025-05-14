# Development Env
`openpilot` and `xnxpilot` can be built in Orin device directly, but it means Orin needs to be setup correctly to access internet to download necessary packages during building phase. To help developer work on the environment, the docker based environment is used.

## Docker Image Source
There are two base images used for Docker:

1. Standard ARM64 Ubuntu Image
2. Nvidia JetPacket Image


# Prerequisite
## Install Docker

```
sudo apt update
sudo apt install ca-certificates curl gnupg lsb-release
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt install docker-ce docker-ce-cli containerd.io docker-compose-plugin
```
### Run Docker without root priveleges
```
sudo groupadd docker
sudo usermod -aG docker $USER
```
Log ou and lo back in to re-evaluate the group membership.

### Configure Docker to start on boot with systemd

```
sudo systemctl enable docker.service
sudo systemctl enable containerd.service
```

To disable it:
```
sudo systemctl disable docker.service
sudo systemctl disable containerd.service
```

## Install QEMU Dependencies

```
sudo apt update
sudo apt install binfmt-support qemu-user-static
```

# Sync code to local

```
#git clone --recursive https://github.com/eFiniLan/xnxpilot.git

#git clone --recursive https://github.com/commaai/openpilot.git

git clone --branch orin_branch --recursive git@github.com:stanley-lei/openpilot_orin.git

```

# Setup Build Environment

## Build xnxpilot 
### Build Docker for xnxpilot (JetPack base)

```
./build_xnxpilot_docker.sh

```

### Build xnxpilot in Docker

```
docker compose exec xnxpilot bash
cd xnxpilot
./tools/openpilot_env.sh
python -m venv venv
pip install --upgrade pip setuptools wheel
pip install -r ../requirements.txt
pip install ../onnxruntime_gpu-1.8.0-cp38-cp38-linux_aarch64.whl
scons -j$(nproc)
```

## Build openpilot

### Build Docker for openpilot (ARM64 Ubuntu Base)

```
./build_jetpack_docker.sh
docker compose exec jetpack bash

```
### Build openpilot in Docker

This Docker already has preinstalled system dependencies installed which
are defined in Dockerfile. So it only needs to install the dependent python packages.

```
python -m venv venv
pip install --upgrade pip setuptools wheel
pip install ../onnxruntime_gpu-1.8.0-cp38-cp38-linux_aarch64.whl
source launch_env.sh
scons -j$(nproc)
```

## Build openpilot onboard  (Inside orin)

### Build Docker for openpilot (ARM64 Ubuntu Base)

```
./simple_build_docker.sh
./launch_docker.sh

```

### Reenter Docker Container
```
./run_docker.sh
```

### Build openpilot inorin

#### Install Dependencies
```
./install_dependencies_orin.sh
```

#### Cleanup Local Python Pre-Installed Packages
```
sudo rm -rf /usr/lib/python3.8/site-packages/*
```

#### Build Openpilit
```
sudo python3 -m venv venv
sudo pip install -r requirements.txt

cd openpilot_orin
source launch_env.sh
scons -j$(nproc)
python selfdrive/manager/manager.py

```

## Build POCL to enable opencl with CUDA backended (No Image Feature supported)
### Upgrade CMake

```
cd ~
wget https://github.com/Kitware/CMake/releases/download/v3.27.9/cmake-3.27.9-linux-aarch64.tar.gz
tar -xzvf cmake-3.27.9-linux-aarch64.tar.gz
export PATH=~/cmake-3.27.9-linux-aarch64/bin:$PATH
which cmake
cmake --version
```

### Install LLVM 14

```
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 14
sudo apt install libclang-14-dev

```

### Build pyopencl

```
sudo copy libdevice.10.bc /usr/local/cuda/nvvm/libdevice/libdevice.10.bc

git clone --branch release_4_0 https://github.com/pocl/pocl.git
cd ${WORKSPACE}/pocl
mkdir build
cd build
CC=clang-16 CXX=clang++-16 cmake ..   -DCMAKE_BUILD_TYPE=Release   -DLLVM_CONFIG=/usr/bin/llvm-config-16   -DCMAKE_C_FLAGS="-mcpu=cortex-a78  -I/usr/lib/llvm-16/include"  -DCMAKE_CXX_FLAGS="-mcpu=cortex-a78  -I/usr/lib/llvm-16/include"  -DLLC_HOST_CPU=cortex-a78   -DENABLE_CUDA=ON   -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda   -DC_LIBFILE_clang-cpp=/usr/lib/llvm-16/lib/libclang-cpp.so.16 -DOCL_ICD_INCLUDE_DIRS=/usr/include
make -j $(nproc)
sudo make install
mkdir -p /etc/OpenCL/vendors/
echo "/usr/local/lib/libpocl.so" > /etc/OpenCL/vendors/pocl.icd
```
