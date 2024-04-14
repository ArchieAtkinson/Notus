# Notus

Notus is a testing grounds for embedded development projects using Zephyr and C++. It is arranged in the same way as the Zephyr repo is, with the addition of the `applications` directory.

The mains goals of this project is to investigating different ways of developing with Zephyr and C++, this includes but isn't limited to:
- Architectural Designs
- Design Patterns
- Build Systems
- Testings
- Error Handling
- Low Level Drivers
- DevOps (CI/CD, Version Control)
- Documentation

The project is in very early stages of development, but I please take a look around. Feels free to ask question in the issues tracker and if you have a interesting demo to share or improvement to take, pull requests are welcome!

## Getting Starting

This repository makes use of [Docker](https://www.docker.com/), [VSCode](https://code.visualstudio.com/) and [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension to provide a Linux development environment inside a [dev container](https://microsoft.github.io/code-with-engineering-playbook/developer-experience/devcontainers/) that has all the tools required to jump straight in. It can be used standalone but it is up to the reader to setup their own environment (see the `.devcontainer/Dockerfile` for the required dependencies).

Once you the required programs installed, clone this repo and open it in VSCode. Run the `Dev Containers: Open Folder in Container...` command from the Command Palette and select the this repos folder. This will start the devcontainer, on first boot this will take 10 - 20 minutes to get started as it downloads all the dependencies and tools.

Once the container has finished building, you will be entered into the container and you can start developing.

### Building

Currently all projects are based using the `native_sim` board that allows you to run Zephyr programs on you host system. To build and run any of the programs you run:
```bash
west -b native_sim <dir with prj.conf in>
./build/zephyr/zephyr.exe
```

Alternatively the VSCode Task. Just open any file in a directory or subdirectory that has a `prj.conf` like `tests/button/src/main.cpp` and run the `Build Local Native Sim`. 

