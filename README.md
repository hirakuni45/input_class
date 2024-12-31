input class  
C++ class for converting strings to numbers with a specification similar to scanf
=========

## Overview

 C++ has a culture of not using functions with variable arguments, which is common in the C language.   
   
 The main problem is that arguments are passed through the stack, and it is not known how many arguments are stored on the stack.   
 This can overload the stack and have a negative impact on the system.   
 This can overload the stack and adversely affect the system.   
   
 A typical implementation is the “printf/scanf” function.   
 The compiler parses the format statement and checks the integrity of the arguments, but it cannot check them completely.   
 but not completely.   
 On the other hand, “printf/scanf” provides great flexibility and makes it easy to work with characters and numbers.   
   
 Boost has a format.hpp file that takes into account the flexibility and safety of printf.   
 It can accept multiple arguments by using the overloading mechanism of the “%” operator.   
 boost::format is an excellent implementation, but it depends on iostream, which is problematic for embedded microcontrollers.    
 Incorporating iostreams bloats the capacity of the system.   
  
 Similarly, scanf, which does the opposite of printf, has the same safety issues as printf.   
 It has the same problem of safety as printf.   
 Therefore, we have implemented an input class that inherits from format.hpp and replaces scanf.   
   
---

## Specifications

- Can accept binary, octal, decimal, hexadecimal, floating point, characters, etc.
- The namespace is “utils”.
- To receive strings, define a functor and use it as a template parameter
- A standard functor “def_chainp” class is defined and typedefed as follows

```C++
   	typedef basic_input<def_chainp> input;
```

-The “def_chainp” class is also included in “input.hpp”.

The standard functor accepts characters from standard input, but a character type pointer   
can be defined and functions like “sscanf”.   
   
In consideration of its use in an embedded microcontroller, it does not send an “exception” when handling errors.   
Errors that occur during input conversion can be retrieved as error types.   

- In general, using exceptions consumes a lot of memory.
- Using exceptions can be fatal if an error occurs and the correct recipient is not available.
- If multiple conversions generate errors at the same time, the last error remains.
- There are regular expressions that can handle multiple separated characters
- %b ---> binary digit
- %o ---> octal digit
- %d ---> decimal digit
- %u ---> Unsigned decimal (obsolete in v111, done in %d)
- %x ---> hexadecimal digit
- %f ---> floating point（float、double）
- %c ---> One character character
- %a ---> Auto, binary(bnnn), octal(onnn), decimal, hexadecimal(xnnn), floating point
- %c supports only one-byte characters
- Use “backslash” when using special characters such as '%', '[', ']', etc. as separating characters

---
## Usage

### Include input.hpp.

```C++
#include "input.hpp"
```

- Link to the arithmetic library is required to use the “std::pow” function for component arithmetic.
- Only “input.hpp” header is required to use all functions.

### The namespace is “utils”.

### Sample

- Receives a decimal number from standard input into the variable “a”.
- When receiving from standard input, terminate the input with '\n' (new line)

```C++
   int a;
   utils::input("%d") % a;
```

- You can detect conversion failures by receiving the conversion state as follows 

```C++
   int a;
   if((utils::input("%d") % a).state()) {
       // OK
   } else {
       // NG
   }
```

- If you want to determine what kind of error was made upon receipt, you can retrieve the error type as follows

```C++
   int a;
   auto err = (utils::input("%d") % a).get_error();
   if(err == utils::input::error::none) {
       // OK
   } else {
       // NG: 「err」種別により、相当する処理
   }
```
   
- The error type is defined by the enum class

```C++
  enum class error : uint8_t {
none, ///< no error
    cha_sets,		///< character set mismatch
    partition,		///< mismatch of separated characters
    input_type,		///< invalid input type
    not_integer,	///< integer type mismatch
    different_sign,	///< sign mismatch (obsolete in v111)
    sign_type,		///< minus sign for unsigned integer
    not_float,		///< mismatch of floating point type
    terminate,		///< mismatch of terminating character
    overflow,		///< overflow
  };
```
   
- When receiving from a string (equivalent to sscanf), do the following
- If the second argument is omitted, the standard input is used, as in the example above

```C++
   int a;
   static const char* inp = { "1234" };
   utils::input("%d", inp) % a;
```

- When using the special character “%, [, ]” as a separator character, use “\” (backslash) to exclude it
- “\” (backslash) to include “\” as a single character in a string

```C++
    static const char* inp = { "123%456[789]5678" };
    int a[4] = { -1 };
    auto err = (input("%d\\%%d\\[%d\\]%d", inp) % a[0] % a[1] % a[2] % a[3]).get_error();
    std::cout << "Special character separator as '" << inp << "': "
        << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3];
    if(err == input::error::none) {

    }
```

---
## Extensions, separator character

The character separating a numeric block can be any non-numeric value (0 to 9 in decimal).   
In addition, you can specify any character enclosed in “[”, “]”. Any character enclosed in “[” or “]” can be specified.   
In the sample below, the separator character can be either “ ‘ (space) or ’,”.    

```C++
    int a = 0;
    uint32_t b = 0;
    int c = 0;
    static const char* inp = { "-99 100,200" };
    auto n = (input("%d[, ]%x[ ,]%d", inp) % a % b % c).num();
    std::cout << "Test05, multi scan for integer: " << a << ", " << b << ", " << c;
    if(n == 3 && a == -99 && b == 0x100 && c == 200) {
        std::cout << "  Pass." << std::endl;
        ++pass;
    } else {
        std::cout << "  Scan NG!" << std::endl;
    }
```

---
## Extensions, “%a” auto

- Ability to accept “b,0b” binary, “o,0o” octal, “x,0x” hexadecimal, “none” decimal/floating point as prefixes
- Leading “0” can be omitted

```C++
    static const char* inp = { "100 0x9a 0b1101 0o775" 123.456 };
    int a[4] = { -1 };
	float b;
    auto n = (input("%a %a %a %a %a", inp) % a[0] % a[1] % a[2] % a[3] % b).num();
    std::cout << "Multi scan for 'auto': "
        << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3] << " (" << n << ")";
    if(n == 4 && a[0] == 100 && a[1] == 0x9a && a[2] == 0b1101 && a[3] == 0775 && b == 123.456f) {
        std::cout << "  Pass." << std::endl;
        ++pass;
    } else {
        std::cout << "  Scan NG!" << std::endl;
    }
```

---
## If a conversion error occurs:

- If a conversion error occurs, no result is returned to the reference and it is discarded.

```C++
    int a = -1;
    input("%d", "1O5") % a;
```

If the conversion fails (contains the letter O) as described above, the initialization value “-1” is retained.    

---

## Overflow error behavior:

- For floating point decimals, numbers after the overflow are ignored and no overflow error is returned
- For real numbers, “utils::input::error::overflow” is set
- In case of an overflow error, the value before the overflow is returned to the reference specified by the operator.

---

## Project (Overall Test)

The input class is provided with the overall test.   

```sh
make
```

The overall test is compiled in   

```sh
make run
```

The overall test runs, and if all tests pass, the test ends normally.   
If the test fails, -1 is returned.   

---

Translated with DeepL.com (free version)

License   
----

[MIT](LICENSE)

```
Copyright (c) 2020, Hiramatsu Kunihito
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the <organization> nor the　names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
