# Docker commands

## Build the base image

```sh
docker build -t flectron -f ./docker/Dockerfile.base .
```

## Build the development image

```sh
docker build -t flectron-dev -f ./docker/Dockerfile.dev .
```

## Run the development image

```sh
xhost +
docker run -ti --rm -e DISPLAY=host.docker.internal:0.0 flectron-dev
xhost -
```

## Run the development image with a volume

```sh
xhost +
docker run -v $(PWD)/build/flectron-demo-coverage-html/:/flectron/build/flectron-demo-coverage-html -ti --rm -e DISPLAY=host.docker.internal:0.0
xhost -
```

## Build and run the coverage image

```sh
docker build -t flectron-coverage -f ./docker/Dockerfile.coverage .
xhost +
docker run -ti --rm -e DISPLAY=host.docker.internal:0.0 -e "CODECOV_TOKEN=..." flectron-coverage
xhost -
```

## Build and run the release image

```sh
docker build -t flectron-prod -f ./docker/Dockerfile.prod .
xhost +
docker run -ti --rm -e DISPLAY=host.docker.internal:0.0 flectron-prod
xhost -
```
