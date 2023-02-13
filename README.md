# Unholy C Language Documentation

## Variables

Local variables can be only be declared at the beginning of every function with the `var` keyword.
Every local variable is 32-bit in size.
Variables can not be initialized during declaration.

Variable names can only contain with letters, numbers or underscores.
Additionally, the first character can not be a number.

Example:

    fn test()
    {
        var value, number;
        var count;
    }

Global variables are also supported.

## Types

There are NO types (and no type safety):
The meaning of the bits in a variable is defined only by what
YOU (the programmer) do with it.

Doing `var1 = var2` simply copies the bits from var2 into var1.

## Expressions

Expressions, used for example on the right hand side of an assignment,
consist only out of function calls.

Example:

    x = add(x, 1);

## Built-in Functions

### Basic Integer Arithmetic
`add`, `sub`, `mul`, `div`, `mod`

### Equality
| C Operator |      |           |
|------------|------|-----------|
|    `==`    | `eq` | Equal     |
|    `!=`    | `ne` | Not Equal |

### Integer Comparison

There are two sets of integer comparison functions for signed
and unsigned numbers.

### Unsigned
| C Operator | Mnemonic | Meaning          |
|------------|----------|------------------|
|    `>`     |   `hi`   | Higher           |
|    `>=`    |   `hs`   | Higher or Same   |
|    `<=`    |   `ls`   | Lower or Same    |
|    `<`     |   `lo`   | Lower            |

### Signed
| C Operator | Mnemonic | Meaning          |
|------------|----------|------------------|
|    `>`     |   `gt`   | Greater Than     |
|    `>=`    |   `ge`   | Greater or Equal |
|    `<=`    |   `le`   | Less or Equal    |
|    `<`     |   `lt`   | Less Than        |

### Bitwise operators
| C Operator | Function |                      |
|------------|----------|----------------------|
|    `|`     |  `or`    | Bitwise OR           |
|    `&`     |  `and`   | Bitwise AND          |
|    `~`     |  `inv`   | Bitwise NOT          |
|    `^`     |  `xor`   | Bitwise Exclusive OR |
|    `<<`    |  `shl`   | Bitwise Shift Left   |
|    `>>`    |  `shr`   | Bitwise Shift Right  |

### Boolean operators
`not`: if argument 0, returns 1, else returns 0

### Floating Point

|         |                  |
|---------|------------------|
| `itf`   | Int to float     |
| `fti`   | Float to int     |
| `addf`  | Add              |
| `subf`  | Subtract         |
| `mulf`  | Multiply         |
| `divf`  | Divide           |
| `modf`  | Modulo           |
| `ltf`   | Less Than        |
| `gtf`   | Greater Than     |
| `lef`   | Less or Equal    |
| `gef`   | Greater or Equal |
| `absf`  | Absolute value   |
| `ceil`  | Ceiling function |
| `floor` | Floor function   |

### Math

|       |
|-------|
| sin   |
| cos   |
| tan   |
| asin  |
| acos  |
| atan  |
| atan2 |
| sinh  |
| cosh  |
| tanh  |
| exp   |
| log   |
| log10 |
| pow   |
| sqrt  |

### Characters

|          |
|----------|
| isupper  |
| islower  |
| isdigit  |
| isalpha  |
| isalnum  |
| iscntrl  |
| isgraph  |
| isprint  |
| ispunct  |
| isspace  |
| isxdigit |
| tolower  |
| toupper  |

### Random

|         |               |
|---------|---------------|
| `rand`  | Random Number |
| `srand` | Set RNG Seed  |

## Functions

The program starts at the `main` function.

Example:

    fn function_name(param0, param1, param2)
    {
        return add(mul(param0, param1), param2);
    }

All functions have a 32-bit return value.
If there return statement is empty (`return;`) or does not exist,
the implicit default return value is 0.

## Control Flow

Curly braces are mandatory for all control flow contstructs.

### Loops

There are only two types of loops: while and do..while

break and continue are supported. There is no goto.

### if-elif-else Branch

Example:

    if lt(number, 10) {
        printf("Your number is less than 10\n");
    }
    elif eq(number, 42) {
        printf("Good choice!\n");
    }
    else {
        printf("Your number %d\n", number);
    }

##### jump Statement

The `jump` statement is in some ways similar to the classic `switch-case`,
with some limitations.

First, there are no case labels. The result of the expression at the top
will be evaluated, and interpreted as a unsigned integer.
It will then jump to the corresponding block, numbered starting from zero.
This enables the jump statement to ALWAYS be compiled into a jump table.

For example, the expression for a `jump` statement for the inputs [ 2, 4, 6, 8, 10 ]
would look something like this:

    sub(shr(input, 1), 1)

The division by two, and subtraction of one is done to map the inputs to [ 0..n-1 ]

     2 -> 0
     4 -> 1
     6 -> 2
     8 -> 3
    10 -> 4

If the input cannot be mapped to [ 0..n-1 ], if-elif-else should be used instead.

There is no `default` branch, that can be done with an `if` beforehand.
The arrow operator after a block denotes fall-through behaviour.

Example (notice the square brackets):

    jump n [
        {
            printf("n is zero\n");
        },
        {
            printf("n is one\n");
        } ->
        {
            printf("n is one or two\n");
        },
        {
            printf("n is three\n");
        }
    ]


# Development

# KNOWN BUGS
- Function parameter/Arguments trailing comma

# TODO

- Implement jump statement
- Global variables/constants
- Safe printf
	%s   String
	%c   Character

	%f   Float
		Optional pad both sides

	%d   Integer
	%x   Hex
	%b   Binary
		Optional pad left with zero or space

- Memory Access / Arrays

- Cleanup
- Automated testing

