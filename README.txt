# SIBR setup

## Install requirements

- Visual Studio 2017.
- Use Cmake >= 3.12.
- 7zip should be installed.
- Python 3 should be installed and in the path for the shaders installation scripts.
- Doxygen should be installed and in the path for generating the documentation.
- CUDA 9 or higher should be installed if you need projects requiring it.

## Generating the solution and compiling

- Checkout this repository.
- Run Cmake-gui once, select the repo root as a source directory, build/ as the build directory. Configure, select the Visual Studio C++ Win64 compiler.
- Select the projects you want to generate among the BUILD elements in the list (you can group Cmake flags by categories to access those faster).
- Generate.
- (Not sure this is needed anymore) Download glfw3 from https://gforge.inria.fr/frs/download.php/file/36912/glfw-3.2.1.7z and copy the glfw-3.1.2 directory into extlibs/win64-msvc2017/.
- Open the generated Visual Studio solution (build/sibr_projects.sln).
- Build the ALL_BUILD target, and then the INSTALL target.
- The compiled executables will be put in install/bin.
- TODO: are the DLLs properly installed?


# Scripts

Some scripts will require PIL (pip install pillow, in an admin command if needed) and convert (install ImageMagick).


# Troubleshooting

- Cmake complaining about the version: if you are the first to use a very recent Camke version, you will have to update CHECKED_VERSION in the root CmakeLists.txt.
- Weird OpenCV error: you probably selected the 32-bits compiler in Cmake-gui.
- 'Cmd.exe failed wither error 009' or similar: make sure Python is installed and in the path. 
- BUILD_ALL or INSTALL fail because of a project you don't really need: build and install each project separately by selecting the proper targets.
