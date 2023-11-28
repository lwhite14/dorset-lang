<img align="left" width="64" height="64" src="../branding/icon.png" alt="Dorset-Lang">

# Install

## Requirements
For using dorsetc:
 - Clang (you can use a package or pre-built binary).

For building dorset-lang from source:
 - CMake.
 - LLVM.
 - A C++ compiler (it is recommended to use Clang).

## Installing Pre-Built Binaries

Pre-built binaries have been provided for Linux platforms, however, these are experimental and are subject to bugs. You may need to update libstdc++ to use dorsetc:
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install --only-upgrade libstdc++6
```

Install dorsetc in <em>/usr/local/bin</em> and libdorsetDriver.a into <em>/usr/local/lib</em>.

## Using LLVM Packages

To use dorsetc, you will need to build it from source which requires LLVM libraries. <br>

On Linux, it is not neccessary to build LLVM from source to be able to build dorset-lang, you can install packages if you are on Debian or Ubuntu. Consult the [LLVM packages pages](https://apt.llvm.org/) for more information on this process. <br>

dorset-lang uses LLVM 17, it is recommended to build with this version. The process of downloading the required packages on Ubuntu Jammy goes as follows:
```
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo add-apt-repository 'deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-17 main'
sudo add-apt-repository 'deb-src http://apt.llvm.org/jammy/ llvm-toolchain-jammy-17 main'
sudo apt-get install libllvm-17-ocaml-dev libllvm17 llvm-17 llvm-17-dev llvm-17-doc llvm-17-examples llvm-17-runtime clang-17 clang-tools-17 clang-17-doc libclang-common-17-dev libclang-17-dev libclang1-17 clang-format-17 python3-clang-17 clangd-17 clang-tidy-17 lld-17
```

This process is similar for other versions of Ubuntu or Debian, but will change slightly. Check LLVM packages pages. <br>

Now that you have all the needed packages for building dorsetc, you can [build from source](#building-from-source).

## Building LLVM from Source

You can always take the plunge and build LLVM from source. If so, make sure to build LLVM 17 (17.0.5), and include the subprojects LLVM, Clang, lld, and clang-tools-extra. Make sure to include all targets.
```
git clone --branch llvmorg-17.0.5 https://github.com/llvm/llvm-project.git
cd llvm-project
mkdir build
cd build
cmake -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld;" -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=all -DLLVM_TARGETS_TO_BUILD=all -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../llvm
```

After configuring, build and install:
```
make -j4
make install
```

After installing LLVM, you can start the process of [building dorset-lang from source](#building-from-source).

## Building from Source
```
git clone https://github.com/lwhite14/dorset-lang.git
cd dorset-lang
cmake -B build -S . -DCMAKE_C_COMPILER="/usr/bin/clang-17" -DCMAKE_CXX_COMPILER="/usr/bin/clang++-17" -DLLVM_DIR="/usr/lib/llvm-17/cmake"
cmake --build build --config Release
```
> **Note**
> The arguments passed to CMAKE_C_COMPILER/CMAKE_CXX_COMPILER/LLVM_DIR will change based on which method you used to install LLVM. <br>
> The example used here is based on on installing Ubuntu packages.

Now you can install:
```
sudo cmake --install build
dorsetc --version
```