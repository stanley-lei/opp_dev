# opp_dev
development env for openpilot

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
### Build Docker for xnxpilot

```
./build_xnxpilot_docker.sh

```

### Build xnxpilot in Docker

```
docker compose exec xnxpilot bash
cd xnxpilot
./tools/openpilot_env.sh
python -m venv venv
pip install -r ../requirements.txt
scons -j$(nproc)
```

## Build xnxpilot

### Build Docker for openpilot
```
./build_xnxpilot_docker.sh

```
### Build openpilot in Docker

```
docker compose exec orin bash
cd openpilot
./tools/openpilot_env.sh
python -m venv venv
pip install -r ../requirements.txt
scons -j$(nproc)
```



