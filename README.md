# Usage

**Clone repo:**

    git clone https://github.com/anton-tchekov/unholy-c.git

**Regular build:**

    make

**Clean build:**

    make clean all

**Run:**

    ./nanoc [file.uhc]

# Unholy C Language Documentation

![Logo](logo.png)

## Source File Extension

`.uhc` for obvious reasons.

## Example Code

See `examples/` directory for code examples.

- Conway's Game of Life: `life.uhc`

## Variables

Local variables can be only be declared at the beginning of a function with the `var`
keyword. Every local variable is 32-bit in size. Variables can not be initialized
during declaration.

Variable names can only contain with letters, numbers or underscores.
Additionally, the first character can not be a number.

**Example:**

    fn test()
    {
        var value, number;
        var count;
    }

Global variables and constants must be defined at the beginning of the file,
before all functions.

**Example:**

    var _counter;
    const ANSWER = 42;
    const PI = 3.1416;

## Types

There are **no** types (and no type safety):
The meaning of the bits in a variable is defined only by what
**you** (the programmer) do with it.

Doing `var1 = var2` simply copies the bits from var2 into var1.

However, there are typed literals:

- Integer:
	- Decimal: `-25`, `0`, `1`, `77`, etc.
	- Hexadecimal: `0xFF`, `0xAA`, etc.
	- Binary: `0b1010`, etc.

- Floating Point: `-42.57`, `3.14`, etc.

- Boolean: `true`, `false`

- Character: `'A'`, `'7'`, `'\n'`, `'\\'`, `'!'`, etc.

- String (char array): `"Hello World!"`

**Note:** The string will be placed in the programs readonly data area. In an
expression, it will evaluate to a pointer to the first character of the string.
The string is terminated by a `'\0'` character.

## Comments

Only multiline comments are supported:

    /* This is a comment */

## Expressions

Expressions (used for example on the right hand side of an assignment)
consist only out of function calls.

**Example:**

	/* y = x^2 + x + 1 */
    y = add(add(mul(x, x), x), 1);

## Built-in Functions

### Basic Integer Arithmetic
| C Operator | Function |
|------------|----------|
| `+`        | `add`    |
| `-`        | `sub`    |
| `*`        | `mul`    |
| `/`        | `div`    |
| `%`        | `mod`    |
|            | `abs`    |

### Equality
| C Operator | Function | Meaning   |
|------------|----------|-----------|
| `==`       | `eq`     | Equal     |
| `!=`       | `ne`     | Not Equal |

### Integer Comparison
There are two sets of integer comparison functions for signed
and unsigned numbers.

### Unsigned
| C Operator | Function | Meaning          |
|------------|----------|------------------|
| `>`        | `hi`     | Higher           |
| `>=`       | `hs`     | Higher or Same   |
| `<=`       | `ls`     | Lower or Same    |
| `<`        | `lo`     | Lower            |

### Signed
| C Operator | Function | Meaning          |
|------------|----------|------------------|
| `>`        | `gt`     | Greater Than     |
| `>=`       | `ge`     | Greater or Equal |
| `<=`       | `le`     | Less or Equal    |
| `<`        | `lt`     | Less Than        |

### Bitwise operators
| C Operator | Function | Meaning              |
|------------|----------|----------------------|
| `\|`       | `or`     | Bitwise OR           |
| `&`        | `and`    | Bitwise AND          |
| `~`        | `inv`    | Bitwise NOT          |
| `^`        | `xor`    | Bitwise Exclusive OR |
| `<<`       | `shl`    | Bitwise Shift Left   |
| `>>`       | `shr`    | Bitwise Shift Right  |

### Boolean operators
| C Operator | Function | Meaning                                  |
|------------|----------|------------------------------------------|
| `!`        | `not`    | If argument 0, returns 1, else returns 0 |

### Floating Point
| C Operator | Function | Meaning          |
|------------|----------|------------------|
|            | `itf`    | Int to float     |
|            | `fti`    | Float to int     |
| `+`        | `addf`   | Add              |
| `-`        | `subf`   | Subtract         |
| `*`        | `mulf`   | Multiply         |
| `/`        | `divf`   | Divide           |
|            | `modf`   | Modulo           |
| `>`        | `gtf`    | Greater Than     |
| `>=`       | `gef`    | Greater or Equal |
| `<=`       | `lef`    | Less or Equal    |
| `<`        | `ltf`    | Less Than        |
|            | `absf`   | Absolute value   |
|            | `ceil`   | Ceiling function |
|            | `floor`  | Floor function   |
|            | `round`  | Round function   |

### Math
Exactly the same as in `<math.h>`

| Function |
|----------|
| `sin`    |
| `cos`    |
| `tan`    |
| `asin`   |
| `acos`   |
| `atan`   |
| `atan2`  |
| `sinh`   |
| `cosh`   |
| `tanh`   |
| `exp`    |
| `log`    |
| `log10`  |
| `pow`    |
| `sqrt`   |

### Characters
| Function   | Meaning                            | true for                                      |
|------------|------------------------------------|-----------------------------------------------|
| `isupper`  | Is uppercase?                      | `A-Z`                                         |
| `islower`  | Is lowercase?                      | `a-z`                                         |
| `isdigit`  | Is digit?                          | `0-9`                                         |
| `isalpha`  | Is in alphabet?                    | `a-z`, `A-Z`                                  |
| `isalnum`  | Is alphanumeric?                   | `a-z`, `A-Z`, `0-9`                           |
| `iscntrl`  | Is control character?              | ASCII `0-31` and `127`                        |
| `isgraph`  | Is graphical?                      | ASCII `33-126`                                |
| `isprint`  | Is printable?                      | ASCII `32-126` (Opposite of iscntrl)          |
| `ispunct`  | Is punctuation?                    | ``!"#$%&'()*+,-./:;<=>?@[\]^_`{\|}~``         |
| `isspace`  | Is whitespace?                     | `' '`, `'\n'`, `'\t'`, `'\v'`, `'\f'`, `'\r'` |
| `isxdigit` | Is hexadecimal?                    | `0-9`, `a-f`, `A-F`                           |
| `tolower`  | If uppercase, convert to lowercase |                                               |
| `toupper`  | If lowercase, convert to uppercase |                                               |

### Random
| Function | Meaning       |
|----------|---------------|
| `rand`   | Random Number |
| `srand`  | Set RNG Seed  |

## Functions

**Example:**

    fn function_name(param0, param1, param2)
    {
        return add(mul(param0, param1), param2);
    }

All functions have a 32-bit return value.
If there return statement is empty (`return;`) or does not exist,
the implicit default return value is 0.

The program starts at the `main` function.

## Control Flow

Curly braces are required for all control flow contstructs.

### Loops

There are four types of loops:

- **counting (not yet implemented)**

```
for c = 'A' to 'Z' {
    print_char(0, c);
}

for i = 0 to 100 inc 5 {
    print_dec(0, i);
    print_char(0, '\n');
}
```

**Note:** for loops only work with signed integers.

- **top-controlled**

```
i = 1;
while le(i, 10) {
    print_dec(0, i);
    print_char(0, '\n');
    i = add(i, 1);
}
```

- **bottom-controlled**

```
do {
    c = get_char();
} while ne(c, '\n');
```

- **infinite**

```
loop {
    /* Main Loop */
}
```

`break;` and `continue;` statements are supported. There is no `goto`.

### if-elif-else Branch

**Example:**

    if lt(number, 10) {
        print_str("Your number is less than 10\n");
    }
    elif eq(number, 42) {
        print_str("Good choice!\n");
    }
    else {
        print_str("Your number ");
        print_dec(number);
		print_char('\n');
    }

### switch Branch (not yet implemented)

The `switch` statement is similar to the classic `switch-case`, but with a few key
differences. First, there are no case labels. The result of the expression at the
top will be evaluated, and interpreted as a unsigned integer. It will then jump to
the corresponding block, numbered starting from zero. This enables the switch to
**always** be compiled into a jump table.

For example, the expression for a `switch` for the inputs `[ 2, 4, 6, 8, 10 ]` would look something like this:

    sub(shr(input, 1), 1)

The division by two, and subtraction of one is done to map the inputs to `[ 0..n-1 ]`

     2 -> 0
     4 -> 1
     6 -> 2
     8 -> 3
    10 -> 4

If the input cannot be mapped to `[ 0..n-1 ]`, if-elif-else should be used instead.

There is no `default` branch, since that can be done with an `if` beforehand.
The arrow operator after a block denotes fall-through behaviour.

**Example (notice the square brackets):**

    switch n [
        {
            print_str("n is zero\n");
        },
        {
            print_str("n is one\n");
        } ->
        {
            print_str("n is one or two\n");
        },
        {
            print_str("n is three\n");
        }
    ]

# Development

## Design Decisions

There are some unusual design decisions that I am going to explain here:

**1.** `memory_XX` functions:

One of the goals for this project to be able to be run on an AVR with external
memory (23LC1024). Since the chips have very little internal RAM (eg. 2048
bytes for an ATMega328, the one on an Arduino UNO), I am using these functions
to access the external memory. When compiling for an OS target, the external
memory is simulated by an equally sized, flat u8-array.

**2.** In some places, the Tokenizer does things that are actually the job of
the parser, (best example: string literals)

It makes the code shorter, more efficient and easier to understand by eliminating
unneccessary passing around of data.

**3.** (Excessive?) Use of macros: `RETURN_IF`, `TRACE`, `EXPECT`, etc.

I am using macros mainly to make error handling easier.
The parser uses recursion to handle expressions, nested blocks, etc.
When there is an error, it needs to be propagated up to
where it can be handled.

`TRACE` "throws" an error, by returning the error code. In debug mode,
also prints file, function and line number to make finding bugs easier.

`RETURN_IF`
If the called function returns 0 (success), return_if does nothing.
If the called function returns a negative status code (error),
return_if returns the return value

Exceptions in C, yay!

`EXPECT` is used to shorten the commonly needed:

    if(_token.Type != SOME_TOKEN_TYPE)
    {
        TRACE(ERROR_EXPECTED_SOME_TOKEN_TYPE);
    }

`error.c` uses macros to generate the error messages automatically.

**4.** Including `.c` files

By including all other C files into one single file, the whole program is in one
translation unit, which enables the C compiler to make much better optimizations.
This is further improved by making all functions `static`, which tells the compiler
that the function cannot be called externally.

This is essentially my adaptation of the "[SQLite Amalgamation](https://www.sqlite.org/amalgamation.html)"

Quote from the link:

> Combining all the code for SQLite into one big file makes SQLite easier to
> deploy - there is just one file to keep track of. And because all code is in a
> single translation unit, compilers can do better inter-procedure and inlining
> optimization resulting in machine code that is between 5% and 10% faster.

## Code Overview

**Utility:**

- `types.h`: Contains integer type definitions and platform dependant

- `error.c`: Error handling macros and error definitions

`TRACE` to throw a previously defined error\
`RETURN_IF` to propagate the error\
New errors are added in the `FOREACH_ERROR` macro, the preprocessor
generates the enum and error messages automatically.

- `util.c`: Misc. utility functions

`_string_find` is used to detect keywords and builtin functions
by finding the position of a string in another, delimiter separated string.

- `memory.c`: External memory access interface / emulation

- `instr.c`: Bytecode instruction definitions (e.g. `INSTR_CALL`)

- `token.c`: Token type and keyword definitions like `TT_IF`, `TT_WHILE`, etc.

**Main:**

`builtin.c`: Contains builtin functions like `mul`, `sin`, `isupper`, etc.

`tokenizer.c`: Extracts the next token from the source file
(keyword, identifier, literal, etc.) and passes it to the parser.

`parser.c`: Predictive recursive descent parser. Requests tokens from
the Tokenizer and outputs bytecode instructions.

An IdentifierMap stores the locations of identifiers
like function and variable names in the source code.

An AddressStack is used to handle break and continue statements.

It is needed because the parser reads the file from start to finish in
a single pass. So when a `break;` is encountered, at that point the
parser does not know where the end of the loop is going to be.
So the parser leaves a gap in the bytecode, and stores the location
of the gap in the AddressStack. When the end of the loop is reached,
the gap can be filled in with an unconditional jump to the current
output address.

The same technique is also utilized with function calls, when a function
is used before the parser has seen it's implementation.

**TODO**: At this point `optimizer.c` might be added to perform simple
optimizations like constant folding or inlining.

Generating actual machine code could also be an interesting project,
however the current aim is to have a sort of scripting language for
embedded systems with Harvard ISA that can only execute code from
their flash memory so the interpreter part is to work around that.

`interpreter.c`: Runs the bytecode

The instruction set only needs 13 instructions.
It uses two stacks, the regular call stack and an operand stack.
I am still looking for a clean way to remove the need for
two stacks and the parameter copying when calling a function.

**Driver:**

`main.c`: Used for Testing/driving the main code

## TODO

### Priority

- Optimize "Undefined reference"
- error handling

- terminal i/o - UART
- file i/o  - SD-Card
- memory functions (memcpy, memchr, etc.) - XMEM

### Features

- function pointers
- constants
- switch-case

### Builtin Functions

- data structure / algorithm standard library
	- sorting, hashing, etc.

### Known Bugs

### Untested
