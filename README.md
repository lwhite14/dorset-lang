# Summit-Lang
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
Summit requires cmake, LLVM, and of course a C++ compiler (gcc [<--- recommended], msvc, clang).
```
cd summit-lang
cmake -B build -S .
cmake --build build
build/summit test.sm
```
