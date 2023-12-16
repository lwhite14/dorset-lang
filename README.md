<img align="left" width="64" height="64" src="branding/icon.png" alt="Dorset-Lang">

# Dorset-Lang

A programming language and compiler.

Very fragile, handle with care :worried:

### Syntax
```
fn binary>10(L, R) double {
    return R < L;
}

fn calculation(x) double {
    return (x + 12) * 0.25;
}

fn main() void { 
    var num = 4;
    var numAfterCalc = calculation(num);

    if (numAfterCalc > 3) {
        print("Our calculation results in a number bigger than 3!");
    }
    else {
        print("Our calculation results in a number smaller than 3...");
    }

    newLine();
    printf("Our final number was %f", numAfterCalc);
    newLine();
}

```
### Building
Dorset requires cmake, LLVM, and of course a C++ compiler (gcc, msvc, clang) to build. The dorsetc compiler uses clang, so you will need to install the clang package on Linux or install the pre-built LLVM binaries on Windows.
> **Note**
> It is recommended to use Clang for building dorset-lang. If building LLVM from source, make sure to include the Clang subproject.
```
git clone https://github.com/lwhite14/dorset-lang.git
cd dorset-lang
cmake -B build -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ dorsetc
cmake --build build
build/bin/dorsetc --help
```

### Installing
Consult the [install](./docs/INSTALL.md) pages for more detailed installation instructions.
```
cmake --build build --config Release
sudo cmake --install build --prefix /usr/local
```

### Testing
Firstly, configure and build the project, the above sections detail this process. Then, run the following commands.
```
cd build/test
ctest --build-config Debug --verbose
```
> **Note**
> Replace <i>--build-config Debug</i> with <i>--build-config Release</i> if Release is the configuration that you used.


### Third-Party Contributions
 - Powered by the [LLVM Project](https://github.com/llvm/llvm-project).
 - Help from the [Kaleidoscope Tutorials](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html)
