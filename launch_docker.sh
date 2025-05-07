sudo docker run -it --name jet-dev \
  --privileged \
  --runtime=nvidia \
  -v $(pwd):/home/nvidia/workspace \
  -v /tmp/.X11-unix:/tmp/.X11-unix \
  -p 18000:8000 \
  -e HOME=/home/nvidia \
  -e DISPLAY=$DISPLAY \
  -w /home/nvidia/workspace \
  jetpack5-image
