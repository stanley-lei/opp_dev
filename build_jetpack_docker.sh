#!/bin/bash

# Output the values being used
echo "Using HOST_UID=$HOST_UID and HOST_GID=$HOST_GID"

# Run docker compose with the current user's UID and GID
sudo docker-compose -f docker-compose-jetpack5.yml down && sudo docker-compose -f docker-compose-jetpack5.yml up -d --build
# --remove-orphans

# Provide instructions for connecting to the container
echo "Development environment is ready!"
echo "To connect to the container, run: sudo docker-compose exec jetpack bash"
