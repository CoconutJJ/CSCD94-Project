# CSCD94 Project - Fall 2021: A modern programming language with built-in asynchronous primitives

The original idea to embark on this project stems from a joint fascination for
different kinds of language parsers and the craving desire to obtain the arsenal
of skills required to implement an imperative programming language. Ever since
asking the question "How are programming languages made?", I have always been
interested in implementing compilers.

Before I came across Bob Nystrom's *Crafting Interpreters* book, the closest I
ever came to implementing some form of parser could be found in my first year
computer science course (CSCA48) assignment called Formula Game. It was
thrilling at the time, having just learned about the basics of recursion, and
yet being able to implement something which I believed at the time to be very
neat. I saw it as a harvest of the skills I developed and a reassurance that I
was learning something useful.   

Through the lens of my first-year self, this project is the culmination of a
wide variety of skills I developed here at U of T's Computer Science program and
also a significant milestone in my ongoing exploration into compilers. On the
other hand, this project is also a starting point in a field of Computer Science
I wish to eventually build a career from.

The work I've done here is from reading and carefully following the *Crafting
Interpreters* guide. Apart from the book, I've also introduced the ideas of
asynchronous primitives and mechanics popularized by Go and NodeJS. Alongside
this project I have also been given an introduction to the theoretical side of
programming languages through Robert Harper's *Practical Foundations in
Programming Languages* book and a quick taste of what areas of research are
being done in programming languages. 

This book has opened my eyes to a variety of programming paradigms and most
importantly given me a formal outline for the many different semantics seen in
modern programming languages. To that end, it has also allowed me to realize the
subtle distinctions between language constructs and how they behave during
execution.

It has been a thought-provoking and breathtaking experience to start in
September with little knowledge about programming languages, and in the period
of three to four months, implement a small programming language, and also
receive a brief overview on the theoretical side of this subject.

### Tree Walk Interpreter

The Tree Walk Interpreter builds an entire Abstract Syntax Tree and runs your
code by recursively visiting the appropriate nodes. Implementing this
interpreter in C unlike the use of Java in the book proved to present some
challenges. For example, the book implements return statements through the use
of Java Exceptions whereas the C does not have such luxury. The reason is that
Return statements require unwinding the recursive call stack and abruptly
stopping execution. I, after a bit of researching,  managed to overcome this
problem using C's `setjmp` and `longjmp` API. 

`setjmp` saves the stack pointer and instruction pointer the first it is called.
When the code eventually reaches a return statement, it calls `longjmp` with the
`jmp_buf` that was created with `setjmp`, this causes the code to abruptly jump
back to the location `setjmp` was called (function entry point) and unwind the
stack. The return value is saved and returned (`loxfunction.c:29`).

The code that powers this is unfortunately still a bit buggy, after `longjmp` is
called, the environments that were created need to be freed very carefully
(`loxfunction.c:62`). There are still a few issues left to address in the
implementation of return statements that are causing segmentation faults.

### Byte Code Virtual Machine

The Byte Code Virtual Machine chapter proved to be the most interesting portion
of the book. In some ways, this portion was easier to implement and less prone
to bugs than the Treewalk Interpreter. Whereas the Treewalk interpreter had 2
major nightmare-causing components: building the AST and executing the code, the
VM portion simplified both of these. No AST is ever explicitly built, instead,
the bytecode is generated as the program is parsed and written to a buffer. This
meant not having manage the memory of thousands of AST nodes and eliminated a
huge class of memory access errors. 

Secondly, code execution could not have been simpler. The implementation of a VM
is surprisingly simple from a high level, it is just an infinite loop reading
instructions in a linear order and executing them faithfully. It does not care
whether the computations it makes is semantically what the user wanted, we can
just blame the compiler for these issues.

### Asynchronous Primitives

In addition to the clox language given by the book, I also implemented
asynchronous behaviour in the language which, in turn, can be utilized using the
`async` and `await` keywords. The main inspiration was from the Go language,
unlike NodeJS where asynchronism is forced upon, everything in clox runs
synchronously by default. To make a routine (function call) run asynchronously
the `async` keyword is prefixed before the function call. So for example,

```
var proc = async f("a");
```
calls the function `f` with parameter `"a"` asynchronously. We can then use
`await` on `proc` to wait for the asynchronous process to complete and retrieve
it's return value.

```
var result = await proc;
```

The aynchronous behaviour is implemented using processes instead of threads
which is an unfortunate drawback/downfall to this project. The Garbage collector
and many data structures are written assuming only a single thread will access
them at a time. This would not have been a problem if it was only the data
structures, as mutexes could be used and enforced on the set of functions that
access these data structures directly - allowing for threads to be used.
However, the Garbage Collector may be run on each object allocation for each
thread, this leads to a variety of nasty problems (eg. potential double-free
errors on the heap) and would lead to a huge refactoring of the codebase.

### How to run code

~~~sh
$ git clone git@github.com:CoconutJJ/CSCD94-Project.git

$ cd CSCD94-Project

$ cd vm

$ make

$ ./clox yourProgramFile
~~~

### References
- Crafting Interpreters - https://craftinginterpreters.com/
- Practical Foundations in Programming Languages (PFPL) -
  https://www.cs.cmu.edu/~rwh/pfpl/2nded.pdf

### Credits

- Thanks to Dr. Thierry Sans for his supervision and support in the past four
  months.
- Thanks to Robert Nystrom for his Crafting Interpreters book.
- Thanks to Robert Harper for his Practical Foundations in Programming Languages
  book.
