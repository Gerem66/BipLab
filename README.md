# BipLap

## Description

BipLap est un projet de simulation d'évolution cellulaire qui permet de visualiser et d'étudier le comportement des BipBoup, des petites cellules virtuelles qui évoluent un environnement 2D.

Le projet est développé en C et utilise la bibliothèque SDL2 pour le rendu graphique.

Les BipBoup peuvent interagir entre eux et avec leur environnement, créant ainsi un écosystème dynamique et évolutif.
L'objectif est d'observer l'émergence de comportements complexes à partir de règles simples.

![BipBoup](./ressources/images/project_cellsevolution.gif)

## Dépendances

- [GCC](https://gcc.gnu.org/)
- [CMake](https://cmake.org/download/)
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)

<details>
<summary>Installation pour chaque OS</summary>

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

## Compilation

### Build simple depuis la racine

```bash
make              # Build en mode Debug
make run          # Build + Exécution
make release      # Build en mode Release
make clean        # Nettoyer les fichiers temporaires
```

### Build manuel (si nécessaire)

1. Créer un dossier build et s'y rendre
```bash
mkdir build ; cd build
```

2. Compiler le projet
```bash
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
```

## References
- [C - Basic SDL game](https://gitlab.com/aminosbh/basic-c-sdl-game.git)
- [JS - Deep Learning Cars](https://github.com/dcrespo3d/DeepLearningCars/)
