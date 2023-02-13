# Unholy C Language Documentation

- *[ It's a reference to the programming language Holy C of Temple OS ]*

## Variables

Local variables can be only be declared at the beginning of a function with the `var` keyword. Every local variable is 32-bit in size. Variables can not be initialized during declaration.

Variable names can only contain with letters, numbers or underscores.
Additionally, the first character can not be a number.

**Example:**

    fn test()
    {
        var value, number;
        var count;
    }

Global variables are not supported.

## Constants

Global numeric constants must be defined at the beginning of the file, before all functions.

**Example:**

    const ANSWER = 42;
    const PI = 3.1416;

## Types

There are **no** types (and no type safety):
The meaning of the bits in a variable is defined only by what
**you** (the programmer) do with it.

Doing `var1 = var2` simply copies the bits from var2 into var1.

However, there are typed literals.

- Integer:
	- Decimal: `-25`, `0`, `1`, `77`, etc.
	- Hexadecimal: `0xFF`, `0xAA`, etc.
	- Binary: `0b1010`, etc.

- Floating Point: `-42.57`, `3.14`, etc.

- Boolean: `true`, `false`

- Character: `'A'`, `'7'`, `'\n'`, `'\\'`, `'!'`, etc.

- String (char array): `"Hello World!"`
	- Note: The string will be placed in the programs readonly data area. In an expression, it will evaluate to a pointer to the first character of the string. The string is terminated by a `'\0'` character.

## Comments

Only multiline comments are supported:

    /* This is a comment */

## Expressions

Expressions, used for example on the right hand side of an assignment,
consist only out of function calls.

**Example:**

	/* x = x^2 + x + 1 */
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
| `|`        | `or`     | Bitwise OR           |
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

| Function   | Meaning                            |
|------------|------------------------------------|
| `isupper`  | Is uppercase? (A-Z)                |
| `islower`  | Is lowercase? (a-z)                |
| `isdigit`  | Is digit? (0-9)                    |
| `isalpha`  | Is in alphabet? (a-z, A-Z)         |
| `isalnum`  | Is Alphanumeric (0-9, a-z, A-Z)    |
| `iscntrl`  | Is control character?
| `isgraph`  | Is graphical?
| `isprint`  | Is printable?
| `ispunct`  | Is punctuation?
| `isspace`  | Is whitespace?
| `isxdigit` | Is hexadecimal? (0-9, a-f, A-F)    |
| `tolower`  | If uppercase, convert to lowercase |
| `toupper`  | If lowercase, convert to uppercase |

### Random

| Function | Meaning       |
|----------|---------------|
| `rand`   | Random Number |
| `srand`  | Set RNG Seed  |

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

There are three types of loops:

- **top-controlled**

```
    i = 1;
    while le(i, 10) {
        print_number(i);
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

`break;` and `continue;` statements are supported. There is no goto.

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

### jump Statement

The `jump` statement is similar to the classic `switch-case`, but with a few key differences. First, there are no case labels. The result of the expression at the top will be evaluated, and interpreted as a unsigned integer. It will then jump to the corresponding block, numbered starting from zero. This enables the jump statement to **always** be compiled into a jump table.

For example, the expression for a `jump` statement for the inputs [ 2, 4, 6, 8, 10 ] would look something like this:

    sub(shr(input, 1), 1)

The division by two, and subtraction of one is done to map the inputs to [ 0..n-1 ]

     2 -> 0
     4 -> 1
     6 -> 2
     8 -> 3
    10 -> 4

If the input cannot be mapped to [ 0..n-1 ], if-elif-else should be used instead.

There is no `default` branch, since that can be done with an `if` beforehand.
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

## Known Bugs
- Function parameter/Arguments trailing comma
- String literals comment inbetween

## Untested
- Break and continue
- Function with no return actually 0?

## TODO
- finish docs for char functions
- add memory access functions
- add int abs function
- add float round function
- booleans
- infinite loop
- jump statement
- global constants
- i/o and file access
- printf
	%s   String
	%c   Character

	%f   Float
		Optional pad both sides

	%d   Integer
	%x   Hex
	%b   Binary
		Optional pad left with zero or space

- Add Code Examples

- Code Cleanup
- Automated testing
- Optimizations
