#!/bin/bash

# Get the current user's UID and GID
export HOST_UID=$(id -u)
export HOST_GID=$(id -g)

# Output the values being used
echo "Using HOST_UID=$HOST_UID and HOST_GID=$HOST_GID"

# Run docker compose with the current user's UID and GID
docker compose -f docker-compose-xnxpilot.yml down && docker compose -f docker-compose-xnxpilot.yml up -d --build

# Provide instructions for connecting to the container
echo "Development environment is ready!"
echo "To connect to the container, run: docker compose exec xnxpilot bash"
