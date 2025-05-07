sudo docker build \
  --build-arg USER_ID=$(id -u) \
  --build-arg GROUP_ID=$(id -g) \
  -f Dockerfile.jetpack_5 \
  -t jetpack5-image \
  .
