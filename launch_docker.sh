#!/usr/bin/bash -x

xhost +

sudo docker run -it --name jet-dev \
  --privileged \
  --runtime=nvidia \
  --network=host \
  -v $(pwd):/home/nvidia/workspace \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -v /dev:/dev \
  -v /etc/localtime:/etc/localtime:ro \
  -e HOME=/home/nvidia \
  -e DISPLAY=$DISPLAY \
  -w /home/nvidia/workspace \
  jetpack5-image

