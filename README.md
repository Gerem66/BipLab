# Cells Evolution

## Dependencies

Install dependencies:
- [GCC](https://gcc.gnu.org/)
- [CMake](https://cmake.org/download/)
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)

<details>
<summary>Displays the installation controls specific to each OS</summary>

### Ubuntu
```bash
sudo apt-get install gcc cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev libsdl2-mixer-dev libsdl2-gfx-dev
```
### Fedora
```bash
sudo dnf install gcc cmake SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_net-devel SDL2_mixer-devel SDL2_gfx-devel
```
### Arch
```bash
sudo pacman -S gcc cmake sdl2 sdl2_image sdl2_ttf sdl2_net sdl2_mixer sdl2_gfx
```
### MacOS
```bash
brew install gcc cmake sdl2 sdl2_image sdl2_ttf sdl2_net sdl2_mixer sdl2_gfx
```

</details>

## Command to start & use
1. Init build:
```bash
mkdir build ; cd build
```

2. Start build & run
```bash
clear && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && ./CellsEvolution

# OR

clear && cmake -DCMAKE_BUILD_TYPE=Release .. && make && ./CellsEvolution
```

## Commands
- `Mouse wheel` => Zoom/Dezoom & move view
- `n` => Show/Hide neural network representation of the best cell
- `r` => Go to next generation (disabled)
- `t` => Show/Hide texts
- `y` => Enable/Disable render of rays
- `i` => Enable/Disable render of cells
- `o` => Enable/Disable vertical sync
- `p` => Pause/Unpause cells evolution
- `Esc` => Quit

## References
- [C - Basic SDL game](https://gitlab.com/aminosbh/basic-c-sdl-game.git)
- [JS - Deep Learning Cars](https://github.com/dcrespo3d/DeepLearningCars/)
