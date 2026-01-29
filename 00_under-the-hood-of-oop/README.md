By building an interface in C first, you will realize that classes in object
oriented languages are not doing anything magical, they are just providing
*syntactic sugar* for patterns that are tedious to write in C.

> `\begin{aside}`\
> *Syntactic Sugar:* Is syntax within a programming language that is designed to
> make things easier to read or to express. It makes the language "sweeter" for
> human use: things can be expressed more clearly, more concisely, or in an
> alternative style that some may prefer. Syntactic sugar is usually a shorthand
> for a common operation that could also be expressed in an alternate, more
> verbose, form. [*Wikipedia*] \
>  Some example of syntactic sugar that all of you are familiar with:
> `x += y;` (`x = x + y;`), `foo->bar` `(*foo).bar`\
> `\end{aside}`


# Under the Hood of OOP: Manual Object Interface
In this introductory module, the goal is to simulate an interface and
polymorphism using C programming langulage.

We shall start by explaining what is polymorphism.

## What is Polymorphism? (The "Many Forms" Principle)
In C, functions are usually bound at compile-time. If you call
`calculate_circle_area()`, the compiler knows exactly which memory address to
jump to. Polymorphism breaks this static link.

> `\begin{unnecessary_info}`\
> Polymorphism derived from Greek for "many shapes".\
> `\end{unnecessary_info}`

Polymorphism allows a single interface to represent different underlying
implementations. E.g. in your C code, this means you can hold a pointer to a
generic shape struct, but when you call `area(shape)`, the program decides at
*runtime* whether to execute the circle logic or the square logic.

## Function Pointers: Pillar of Dynamic Dispatch
In C, a function pointer is a variable that stores the memory address of a
function. This allows you to decide *at runtime* which function to call.

Its syntax is a bit messy but you will get it after writing a few function
pointers.

**Basic Syntax:**
```c
// Declaring a function pointer
double (*operation)(double, double);

// Assigning a function to it
double add(double a, double b) { return a + b; }
operation = add;

// Calling through the pointer
double result = operation(5.0, 3.0);  // Calls add(5.0, 3.0)
```

We put parenthesis to distinguish return type from function pointer syntax:
```c
// concatenate_arrays variable is a pointer to a function that returns an int
// pointer
int *(*concatenate_arrays)(int *arr1, int *arr2, int size1, int size2);
```

The key insight: **function pointers let you treat functions as variable**. You
can pass them around, store them in structs, and decide which one to call based
on runtime conditions.

[Here](https://www.w3schools.com/c/c_functions_pointers.php) is a quick
reference for function pointers.

---

Enough introduction. Now you need to implement a dynamic dispatch logic to
practice OOP logic -
[with what you have already known](./00_traditional-approach/README.md).
