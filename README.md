![flectron](docs/flectron.png)

[![GitHub](https://img.shields.io/github/license/morswin22/flectron)](https://github.com/morswin22/flectron/blob/main/LICENSE)
[![codecov](https://codecov.io/github/morswin22/flectron/branch/main/graph/badge.svg?token=8IUQUQVPKO)](https://codecov.io/github/morswin22/flectron)

## Requirements
* [cmake](https://cmake.org/)

## Build
Clone the repository
```sh
git clone https://github.com/morswin22/flectron.git
cd flectron
```

Create a build directory
```sh
mkdir build
cd build
```

Build the project
```sh
cmake ..
cmake --build . --config Release
```

## Run the tests
```sh
cd tests/Release
./flectron-demo
./flectron-terrain
./flectron-piday
```

## Run in Docker
```sh
docker build -t flectron -f ./docker/Dockerfile.base .
docker build -t flectron-prod -f ./docker/Dockerfile.prod .
xhost +
docker run -ti --rm -e DISPLAY=host.docker.internal:0.0 flectron-prod
xhost -
```

More Docker commands can be found in [docker/README.md](docker/README.md).

## Dependencies
* [glfw](https://github.com/glfw/glfw/tree/076bfd55be45e7ba5c887d4b32aa03d26881a1fb)
* [glew](https://github.com/Perlmint/glew-cmake/tree/883e35a3d493d93fa27da5abb6225654c360f9d0)
* [glm](https://github.com/g-truc/glm/tree/2759ceca04db5c36d2806a2280f51b83a6749c12)
* [stb_image](https://github.com/nothings/stb)
* [optick](https://github.com/bombomby/optick)
* [entt](https://github.com/skypjack/entt)
* [spdlog](https://github.com/gabime/spdlog)
