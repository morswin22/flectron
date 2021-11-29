# ![flectron](flectron.png)

## Dependencies
* [glfw](https://github.com/glfw/glfw/tree/076bfd55be45e7ba5c887d4b32aa03d26881a1fb)
* [glew](https://github.com/Perlmint/glew-cmake/tree/883e35a3d493d93fa27da5abb6225654c360f9d0)
* [glm](https://github.com/g-truc/glm/tree/2759ceca04db5c36d2806a2280f51b83a6749c12)
* [stb_image](https://github.com/nothings/stb)
* [optick](https://github.com/bombomby/optick)

## Download
Download with dependencies
```sh
git clone --recurse-submodules -j8 https://github.com/morswin22/flectron.git
```

Download without dependencies (You will be required to manually copy the dependencies into `lib/` directory)
```sh
git clone https://github.com/morswin22/flectron.git
```

If you change your mind and want to download dependencies after cloning
```sh
git submodule update --init
```
