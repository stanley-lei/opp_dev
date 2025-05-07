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

# Sync the code to local

```
git clone https://github.com/stanley-lei/opp_dev.git
cd opp_dev

git clone --recursive https://github.com/eFiniLan/xnxpilot.git

git clone --recursive https://github.com/commaai/openpilot.git

cd openpilot
git checkout v0.8.16
git submodule update --init --recurisive
patch -p1 < ../SConstruct.patch
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
./build_xnxpilot_docker.sh

```
### Build openpilot in Docker

This Docker already has preinstalled system dependencies installed which
are defined in Dockerfile. So it only needs to install the dependent python packages.

```
docker compose exec orin bash
cd openpilot
python -m venv venv
pip install --upgrade pip setuptools wheel
pip install ../onnxruntime_gpu-1.8.0-cp38-cp38-linux_aarch64.whl
scons -j$(nproc)
```

## Notes

### Solution to build substitue for pyopencl

```
cd ~/workspace
git clone --single-branch --branch ${POCL_VERSION} https://github.com/pocl/pocl.git
cd ${WORKSPACE}/pocl
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_LLVM_CONFIG=/usr/lib/${LLVM_INSTALL_FOLDER}/bin/llvm-config -DENABLE_CUDA=ON -DSTATIC_LLVM=ON ..
make -j $(nproc)
sudo make install
mkdir -p /etc/OpenCL/vendors/
echo "/usr/local/lib/libpocl.so" > /etc/OpenCL/vendors/pocl.icd
```
