# SIBR setup

## Install requirements

- **Visual Studio 2019**
- **Cmake >= 3.16**
- **7zip**
- **Python 3** for the shaders installation scripts
- **Doxygen** for documentation
- **CUDA 10.1** if projects requires it

Make sure Python, CUDA and Doxygen are in the PATH

If you have Chocolatey, you can grab most of these with this command:

```sh
choco install cmake 7zip python3 doxygen.install cuda

# Visual Studio is available on Chocolatey,
# though we do advise to set it from Visual Studio Installer and to choose your licensing accordingly
choco install visualstudio2019community
```

## Generation of the solution

- Checkout this repository's master branch:
  
  ```sh
  # through HTTPS
  git clone https://gitlab.inria.fr/sprakash/sibr_basic2.git -b master
  # through SSH
  git clone git@gitlab.inria.fr:sprakash/sibr_basic2.git -b master
  ```
- Run Cmake-gui once, select the repo root as a source directory, `build/` as the build directory. Configure, select the Visual Studio C++ Win64 compiler
- Select the projects you want to generate among the BUILD elements in the list (you can group Cmake flags by categories to access those faster)
- Generate

## Compilation

- Open the generated Visual Studio solution (`build/sibr_projects.sln`)
- Build the `ALL_BUILD` target, and then the `INSTALL` target
- The compiled executables will be put in `install/bin`
- TODO: are the DLLs properly installed?

## Compilation of the documentation

- Open the generated Visual Studio solution (`build/sibr_projects.sln`)
- Build the `CompileDocs` target
- Run `docs/Documentation.html` in a browser


# Scripts

Some scripts will require you to install `PIL`, and `convert` from `ImageMagick`.

```sh
# To install pillow
python -m pip install pillow

# If you have Chocolatey, you can install imagemagick from this command
choco install imagemagick
```

# Troubleshooting

- **Cmake complaining about the version**

if you are the first to use a very recent Cmake version, you will have to update `CHECKED_VERSION` in the root `CmakeLists.txt`.

- **Weird OpenCV error**

you probably selected the 32-bits compiler in Cmake-gui.

- **`Cmd.exe failed with error 009` or similar**

make sure Python is installed and in the path. 

- **`BUILD_ALL` or `INSTALL` fail because of a project you don't really need**

build and install each project separately by selecting the proper targets.
