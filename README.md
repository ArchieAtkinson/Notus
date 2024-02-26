# Coffee Scale Project

The goal of this project is to create open source firmware for a basic coffee scale.

Use `task.json` for building

## Setup

This repo is design to be used inside the docker container provided in `.devcontainer/Dockerfile`.

Run:
1. `west init -l .` - Inits the west workspace
2. `west update` - Pulls in all the west repos
3. `sudo scripts/install.sh` - Installs any projects that need installing