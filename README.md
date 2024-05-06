# Cells Evolution

## Source code used
- [C - Basic SDL game](https://gitlab.com/aminosbh/basic-c-sdl-game.git)
- [JS - Deep Learning Cars](https://github.com/dcrespo3d/DeepLearningCars/)

## Install MacOS dependencies
brew install sdl2 sdl2_image sdl2_ttf sdl2_net sdl2_mixer sdl2_gfx

## Command to start & use
Init build:
```bash
mkdir build ; cd build
```

Start build & run
```bash
clear && cmake .. && make && ./CellsEvolution
```

## Commands
- Mouse wheel => Zoom/Dezoom & move view
- n => Show/Hide neural network representation of the best cell
- r => Go to next generation (disabled)
- t => Show/Hide texts
- y => Enable/Disable render of rays
- i => Enable/Disable render of cells
- o => Enable/Disable vertical sync
- p => Pause/Unpause cells evolution
- Esc => Quit
