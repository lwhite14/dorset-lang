# Dorset-Lang
Very fragile, handle with care :worried:

### Syntax
```
fn main()
{
    var number = 4 + 12;
    var stringExample = "Hello World";
    return number;
}
```
### Building
Dorset requires cmake, LLVM, and of course a C++ compiler (gcc, msvc, clang [<--- recommended]).
```
cd dorset-lang
cmake -B build -S .
cmake --build build
build/dorset test.sm
```
