<img align="left" width="64" height="64" src="branding/icon.png" alt="Dorset-Lang">

# Dorset-Lang

Very fragile, handle with care :worried:

### Syntax
```
fn binary : 1 (x, y) { y; }

fn fibi(x)
{
    var a = 1, b = 1, c in
    (for i = 3, i < x in
        c = a + b :
        a = b :
        b = c) :
    b;
}

fn main() 
{ 
    printf("%f", fibi(10)) : 
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
