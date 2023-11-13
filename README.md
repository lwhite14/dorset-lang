<img align="left" width="64" height="64" src="branding/icon.png" alt="Dorset-Lang">

# Dorset-Lang

Very fragile, handle with care :worried:

### Syntax
```
fn newLine() void {
    printf("\n", 0);
}

fn binary>10(L, R) double {
    return R < L;
}

fn calculation(x) double {
    return (x + 12) * 0.25;
}

fn main() void { 
    var num = 4;
    var numAfterCalc = calculation(num);

    if (numAfterCalc > 3)
    {
        printf("Our calculation results in a number bigger than 3!", 0);
    }
    else
    {
        printf("Our calculation results in a number smaller than 3...", 0);
    }

    newLine();
    printf("Our final number was %f", numAfterCalc);
    newLine();
}

```
### Building
Dorset requires cmake, LLVM, and of course a C++ compiler (gcc, msvc, clang).
> **Note**
> It is recommended to use Clang. If building LLVM from source, make sure to include the Clang subproject.
```
git clone https://github.com/lwhite14/dorset-lang.git
cd dorset-lang
cmake -B build -S . -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build
build/dorsetc --help
```

### Installing
```
cmake --build build --config Release
sudo cmake --install build --prefix /usr/local
```

### Testing
Firstly, configure and build the project, the above sections detail this process. Then, run the following commands.
```
cd build
ctest --build-config Debug --verbose
```
> **Note**
> Replace <i>--build-config Debug</i> with <i>--build-config Release</i> if Release is the configuration that you used.


