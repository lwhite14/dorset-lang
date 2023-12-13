<img align="left" width="64" height="64" src="../branding/icon.png" alt="Dorset-Lang">

# Dorset-Lang

- [Dorset-Lang](#dorset-lang)
  - [Introduction](#introduction)
    - [Versions](#versions)
    - [Technology](#technology)
  - [Getting Starting](#getting-starting)
    - [Installing](#installing)
      - [Dependancies](#dependancies)
      - [Building](#building)
    - [Usage](#usage)
  - [Features](#features)
    - [Basic Rules and Features](#basic-rules-and-features)
    - [Functions](#functions)
    - [Variables](#variables)
    - [Conditionals](#conditionals)
    - [Loops](#loops)
    - [Arrays](#arrays)
    - [Operator Overloading](#operator-overloading)
  - [More Info](#more-info)

## Introduction
Dorset lang is a language and compiler developed to be fast and strong.

### Versions
Current stable version: 0.2.0-alpha. Working towards version 1.0.0.

### Technology
Written with C++ (Clang) and LLVM.

## Getting Starting

### Installing
For more detailed installation instructions, see the [install pages](INSTALL.md).

#### Dependancies
 - CMake.
 - LLVM.
 - C++ Compiler (Clang comes with LLVM).

#### Building
You will need an install of LLVM to use dorsetc. You can find more information on building and installing LLVM and Clang [here](https://llvm.org/docs/GettingStarted.html).
First, clone the dorset-lang repository. It is advised to clone from a stable version:
```
git clone --branch 0.2.0-alpha https://github.com/lwhite14/dorset-lang.git
```
Configure the CMake cache:
```
cmake -B build -S . -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
```
Build dorset-lang:
```
cmake --build build --config Release
```
And install dorset-lang:
```
sudo cmake --install build --prefix /usr/local
```

### Usage
Run <em>dorsetc --help</em> for usage details.
To compiler dorset-lang source code, run <em>dorsetc file.ds</em> to compile file.ds into an executable.

## Features

### Basic Rules and Features
Dorset-lang is a semi-functional, non object oriented language, so every expression and statement must live within a function. Any top level code must be a function declaration.
Code like this is not allowed:
```
fn test(x) double 
{ 
    return x + 2; 
}

printf("%f", test(3));
```                
The main entry point of your dorset-lang executable is the main function. So for the previous example, to make it work within the semi-functional framework you would write:
```
fn test(x) double 
{ 
    return x + 2; 
}

fn main() void
{
    printf("%f", test(3));
}
 ```         
There are three in-built functions which will help you output data to the screen. 'newLine', 'print', and 'printf'. newLine outputs the new line character, priming the next output on the next line. print outputs the inputted string to the terminal. Finally, printf outputs the string to the terminal, using the %f characters, you can feed a double to the output string as well. <em>printf("The number is: %f", 3)</em>. <br>        
The only types in dorset-lang 0.2.0 are doubles and voids. Any variables declared will automatically be a double to represent a number, however, you will have to specify whether a function returns a double or is void, returning nothing at all.
Another rule in dorset-lang is that every function needs a least one expression in its body.

### Functions
Functions have five key components which are all necessary to compile.
 - A function keyword.
 - The function indentifier, or function name.
 - The parameter list.
 - The return type.
 - The function body.

A basic function may look like this:
```
fn test(x) double 
{ 
    return x + 2; 
}
```              
The fn tells the compiler that the following tokens represent a function definition.
test is the function identifier, this is what the programmer will use when they want to call this function. The (x) is the parameter list. This function only has one parameter, x. More parameters can be added by using commas.
```
fn test(x, y, z, anotherParameter) double
```                    
The last thing in the function prototype is the return type. This particular function will return a double value, representing a number. The other return type is void, which represents a function that doesn't return anything.
The last thing for the entire function is the function body, the code that is executed when the function is called. This will be zero or more lines of code surrounded by an open curly brace and a closed curly brace.

### Variables
Declaring a double variable is very simple, use the var keyword and then give you variable a name, like so:
```
var num;
```                
This variable does not have an initializer, so when used it will have a default value of 0, however you can initialize a variable with a value by using the = operator, like so:
var num = 43;
                    
You could use this variable in the previous test function call:
```
var num = 3;
test(num);
```
            
### Conditionals
Conditionals consist of an if and then an else. Both are needed in a if block.
```
if (3 < x)
{
    printf("X is bigger than 3!", 0);
}
else
{
    printf("X is not bigger than 3!", 0);
}
```               
Firstly, declare your if and write your conditional that, when true, will execute the code inside the if block. <em>if (3 < x) { ...</em>. 
This condition must be encapsulated by open and closed parentheses.
After the condition, you will need the { and } tokens so the compiler knows when the following block begins and ends.
After the if block, else controls the code that will execute if the condition fails: <em>else { ...</em>. The else block is optional, you can omit it entirely.

### Loops
Loops are implementing using the following syntax:
```
for (i = 1, i < x, 1.0)
{
    printf("*", 0);
}
```                 
Firstly, start the for loop with the for keyword. You will then need a start, an end, and a step for the loop.
In dorset-lang 0.2.0, you can only initialize new variables, but you can use any expression to initialize it, for example:
```
for (i = add(3, 4), i < 45, 3.0)
```                    
The first part represents the new variables used in the loop. The middle part is the condition which needs to be satisfied to continue looping. Once this condition is false, the loop ends. The last part is the step. The step is the number which is added to the newly declared variable every loop. So for the previous example, the variable i will have 3 added to it for each loop.

### Arrays
In dorset-lang, you can initialize double arrays and use and re-assign the various index values. To declare an array, use the syntax:
```
var numbers[3];
```
If you declare an array like this without initializing values, each index value will be 0. <br>
To declare initial values, use the syntax:
```
var numbers[3] = (1, 3, 12);
```
This syntax will initialize numbers[0] as 1, numbers[1] as 3, and numbers[2] as 12. <br>
In dorset-lang 0.2.0, you can't pass in arrays as function parameters. Any arrays you use must be used in the function they're first declared.
```
var numbers[3] = (1, 3, 12);

var soloNumber = numbers[2];

printf("The third number in the first array is: %f", soloNumber);
newLine();
```

### Operator Overloading
In dorset-lang, you can define your own operators. There are two types of operator overloading, unary operators and binary operator.
Unary operators take one argument and perform some operation on that parameter before returning the result. Binary operators take two parameters and do the same, perform some operation and return. Here's an example of both:
```
fn unary!(v) double
{
    if v 
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

fn binary> 10 (LHS, RHS) double
{
    return RHS < LHS;
}
```
The ! unary operator takes an argument, v, and returns the opposite of the argument with an if statement. The binary operator > takes two arguments and returns essentially the opposite of the < operator, returning true if the left side is larger and false if it is not.
You can use this operator overloading functionality to essentially define your own functions for a predefined set of operators. The operators you can use are: +, -, *, /, \, |, :, &, ^, !, <, and >.

## More Info
The dorset compiler is very fragile, handle with care. <br>
Visit the [github repository](https://github.com/lwhite14/dorset-lang) for more information or to submit an issue.