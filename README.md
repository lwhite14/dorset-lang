<img align="left" width="64" height="64" src="branding/icon.png" alt="Dorset-Lang">

# Dorset-Lang

Very fragile, handle with care :worried:

### Syntax
```
fn test(x) double
{
    return x + 12;
}

fn main() void
{ 
    var num = 1 + 2;

    printf("num = %f", num); // expecting 3
    printf("\n", 0);

    printf("num = %f", test(num)); // expecting 15
    printf("\n", 0);

    num = num + 4;

    printf("num = %f", num); // expecting 7
    printf("\n", 0);
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
build/dorset --help
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
ctest -C Debug --verbose
```
> **Note**
> Replace <i>-C Debug</i> with Release if that is the configuration that you used.


