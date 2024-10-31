# OpenGL Depth Peeling Demo
The goal of this short capsule is to demonstrate an implementation of traditional depth-peeling in OpenGL.

Quick Disclaimer: I am not claiming that this is the only way (or the most efficient way) to run a depth-peeling program in OpenGL. The goal of this is to be able to learn and show others the fundamentals of depth peeling.

## Compilation
1. Clone the repository using:
```
git clone https://github.com/brandon-tol/opengl-transparency/
```
2. Create the build folder and generate build files using CMake
*NOTE: This project requires GLFW, GLEW, and GLM. Seeing a this program was intended as a part of a school project, the paths are hard-coded to work on the computers there. Change the `CMakeLists.txt` file in the root of the repo if you want to compile this on a personal machine*
```
mkdir build
cd build
cmake .. 
```
3. Compile it using the method of your choice.
For the sake of this project, the build was generated in Visual Studio 2022 and Visual Studio 2019.

## Run
*For the scope of this project, all default paths and objects are hard-coded into the program. 
However, feel free to play with the code as much as your heart desires. It's very simplistic and the classes are simple to use (albeit not very well encapsulated).*

If you choose to apply your own texture to the windows, you can specify that by running the executable with the filepath of the texture as a command line argument. Note that the texture must be in RGBA format (.png files work best).

Otherwise, run with no command line arguments and the default texture will be applied.

## Controls:
* `1`-`5` will display that "depth layer" which is being rendered (with 1 being the first and frontmost and 5 being the furthest back).
  * (By default, 5 layers will be shown).
* `0` will display what the image looks like without depth peeling enabled.
* `-` will toggle single layer mode and accumulation mode (where `2` will display only the first 2 layers blended in accumulation, but will only display strictly the second layer in single layer mode).
* `W`, `A`, `S`, `D` to move through the space, using `SPACEBAR` and `LEFT CONTROL` to fly up and down, respectively
* `ESC` to exit
