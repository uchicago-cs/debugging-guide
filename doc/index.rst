The Debugging Guide
===================

Even the most seasoned programmer makes mistakes, so implementing software will 
always involve some amount of *debugging*: finding and fixing errors in your code. 
While there are many tools that can aid in debugging, using these tools is not necessarily hard; 
what is hard is understanding the errors you encounter, and devising a strategy to 
resolve those errors. 

In this guide, we are going to cover the following:

* **Common types of programming errors**. When beginner programmers encounter an 
  error (any error) they usually throw their arms in the air and try random changes 
  until the bug is fixed (or immediately ask for help). Understanding what type of 
  error you're encountering can help you select a course of action to resolve that 
  bug (it is important to understand that not all errors are the same: the best 
  course of action for a given error may not be the same for another).

* **Debugging techniques**. There are a number of different debugging techniques 
  you can apply to resolve errors in your code.

    * *Reactive*: Actions you take once a bug happens.
    * *Preemptive*: Actions you can take *before* a bug happens to make that bug 
       easier to resolve.

* **Asking for help**. Sometimes, you will not be able to resolve a bug by yourself, 
  and will need to ask a more expert programmer for help. However, you will need to
  provide that person with enough information for them to help you.

We will use C examples and the GDB debugger to explain some of these concepts. 
Readers are assumed to be familiar with how to use GCC from the command-line, as
well as the compiling and linking in general. No prior knowledge of GDB is assumed. 

The examples can be found in the ``examples`` directory of our GitHub 
repository: https://github.com/uchicago-cs/debugging-guide

This guide was originally written for `CMSC 22000 Introduction to Software Development <https://uchicago-cs.github.io/cmsc22000/>`_
at the University of Chicago.

Types of Programming Errors
---------------------------

We can classify programming errors into three general categories:

* **Compiler/Linker errors**: Errors that prevent your code from compiling/linking in 
  the first place. These are often syntax errors, but may include other types of 
  errors (e.g., undefined references when linking)

* **Runtime errors**: Errors that make your program crash in some way.

* **Logic errors**: Errors that don’t make your program crash, but result in 
  incorrect behavior.

Compiler/Linker Errors
~~~~~~~~~~~~~~~~~~~~~~

Compiler/Linker errors occur when we try to build our program, and are unable to do 
so because of an issue in our code (this means we can't even run our code)

This type of error is, arguably, the easiest to fix, because modern compilers will 
often tell us exactly what is wrong in our code (down to the line where the error
was detected). However, interpreting these error messages can sometimes be challenging

In the ``examples/compile-error`` directory, try compiling ``compile-error.c``::

    gcc compile-error.c -o compile-error

It should produce the following error::

    compile-error.c: In function 'main':
    compile-error.c:7:5: error: expected ';' before 'return'
         return 0;
         ^

This is a **compiler error** and it seems pretty helpful: it is telling us the file, 
function, and line (7) where the error is detected. It’s even telling us the *character* 
in that line that triggered the error (the fifth character).

However, if we look at the code, we can see that the error actually originates in line 5::

    printf("Hello, world!")

The line is missing a semicolon at the end, but this issue is not detected until line 7 
(when the compiler encounters a "return" statement when it expected a semicolon)

.. admonition:: Debugging Rule of Thumb #1

   When a compiler gives you a line number, it doesn’t necessarily mean that line is incorrect 
   (and that is has to be fixed). Always look at the offending lines *and* the lines before 
   it to see if you can spot the issue.

In the ``examples/link-error/`` directory, try doing this::

    gcc main.c greet.c -o greet

It should produce the following error::

    /tmp/ccyLGeRy.o: In function `main':
    main.c:(.text+0x14): undefined reference to `bye'
    collect2: error: ld returned 1 exit status

This is a **linker error**, not a compiler error. Remember what’s happening behind the scenes:

* The compiler first compiles ``main.c`` and ``greet.c`` into object files. Let’s do this explicitly::

    gcc main.c -c -o main.o
    gcc greet.c -c -o greet.o

  (the "-c" flag tells gcc to only run the compile phase, and not the linking phase)

  Notice how we get no errors.

* Next, it links them together to produce the executable. If we do this explicilty, we get 
  the same error as before::

    gcc greet.o main.o -o main

When compiling a single C file, an object file can have *undefined references*. This is normal, 
and even expected: the linker will take care of resolving those undefined references.
What the error above is telling us is that the linker was unable to resolve one such reference.

If we look at ``main.c`` and ``greet.h`` (which is included in ``main.c``). We can see that we 
call a function called ``bye``, and that it is declared in ``greet.h``. This is why we are 
able to compile ``main.c`` individually. However, if we look at ``greet.c``, we can see that 
it doesn’t include a ``bye`` function, it includes a ``goodbye`` function.

This is why we get an error:

* The ``greet.h`` header file includes a declaration for ``bye()``. This basically
  says "there is an implementation for ``bye()`` somewhere else". So, compiling 
  ``main.c`` by itself results in the call to ``bye()`` having an undefined 
  reference, under the expectation that we will simply link with the implementation 
  of ``bye`` at a later point.
* The ``greet.c`` file contains no ``bye()`` function so, when we link ``main.o`` 
  and ``greet.o``, the linker complains that it was unable to find an implementation for ``bye``.

Linker errors can be more cryptic than compiler errors (notice how the error message doesn’t tell
us the line where the undefined reference happens). We can make them slightly less cryptic by
using GCC's ``-g`` flag::

    $ gcc -g main.c greet.c -o greet
    /tmp/ccXCYkEk.o: In function `main':
    /home/borja/cs220/link-error/main.c:6: undefined reference to `bye'
    collect2: error: ld returned 1 exit status

This flag tells GCC to include debugging information in the object files, which allows the linker
to print a more helpful message (because it now knows the exact line where the undefined reference 
happens). You should get into the habit of always using the ``-g`` flag, except for producing the final 
version of your program (the ``-g`` flag increases the size of your object files, and is unnecessary 
once the software is in production).

Let's look at another example. In the ``examples/spurious`` directory, try doing this::

    gcc spurious.c -o spurious

This will produce *a lot* of compiler warnings, but the compilation will ultimately succeed. 
However, if we try to run the program, we will get a segmentation fault.

How do we go about solving this error? We need to follow these rules of thumb:

.. admonition:: Debugging Rule of Thumb #2

   If a compiler spits out multiple errors/warnings, start by trying to resolve *only* the first one. 
   It is likely that the rest of the errors/warning are **spurious errors** (errors that result 
   from a previous error) that will go away as soon as you resolve the first error.

   *Corollary*: Don’t get overwhelmed when you get a huge number of errors. Focus on the first one, solve 
   it, and try compiling again. If you get a different set of errors, repeat the process: focus only on 
   the first one, solve it, etc.

.. admonition:: Debugging Rule of Thumb #3

   Treat warnings as seriously as compiler errors. While a warning will still allow your program to 
   compile, it may be a sign of trouble further down the road.

   *Corollary*: Do not try to debug runtime errors (like segfaults) until you have resolved all 
   compiler warnings.

In the above example, the warning message is directly related to why we get a segfault later on::

    spurious.c: In function 'main':
    spurious.c:9:7: warning: assignment makes integer from pointer without a cast [-Wint-conversion]
         s = malloc(100);

This warning can make us realize that we should’ve declared ``s`` as ``char*`` not as ``char``. 
If we fix this, our program will compile and run correctly.

**Note**: In this simple example, we could’ve spotted the error by code inspection. However, a 
silly typo like this (forgetting a ``*``) is not uncommon and, in a much larger codebase, it 
can be hard to track down the issue just by code inspection (unless you heed the compiler's 
warnings to focus your search).

Runtime Errors
~~~~~~~~~~~~~~

Runtime errors are errors that make a program crash while it is running. In C, this often results
in a "Segmentation fault" (or "segfault") resulting from your program trying to access regions of 
memory it shouldn’t access. This error is actually triggered by the operating system itself: when 
it detects that you’re trying to access an illegal memory address, it will simply kill your program 
and, at least in C, this results in a single very unhelpful error message: "Segmentation fault".

In other programming languages, like Java or Python, runtime errors may provide more helpful error 
messages, typically telling you the exact place where the error happened. For example, in the 
``examples/runtime/`` directory we have two implementations of the same program in C and Python. This 
program takes two integers as command-line parameters and divides them::

    $ gcc runtime-error.c -o runtime-error
    $ ./runtime-error 10 2
    a / b = 5
    $ python3 runtime-error.py 10 2
    a / b = 5

However, if we try dividing by zero (which will also make a program crash), the level of detail 
in the errors is much different::

    $ ./runtime-error 10 0
    Floating point exception
    $ python3 runtime-error.py 10 0
    Traceback (most recent call last):
      File "runtime-error.py", line 19, in <module>
        x = bar(a, b)
      File "runtime-error.py", line 9, in bar
        return foo(a, b)
      File "runtime-error.py", line 6, in foo
        return a // b
    ZeroDivisionError: integer division or modulo by zero

As we’ll see later on, we will have to use a debugger (like GDB) to track down the origin of the 
error in C but, ultimately, it is generally possible to track down the origin of runtime errors 
down to the exact line that triggers them (some languages, like Python and Java, will give us 
that information right away; others will require using a debugger as an additional step).

Logic errors
~~~~~~~~~~~~

When your program compiles and runs (without crashes) but exhibits incorrect behavior, that is 
known as a logic error (i.e., there is an error in the "logic" of your program). For example, 
let’s look at ``distance.c`` in the ``examples/logic/`` directory. It takes four command-line 
parameters X1 Y1 X2 Y2 representing two points in 2-dimensional space, and computes the distance 
between them::

    $ gcc distance.c -o distance -lm
    $ ./distance 0 0 1 0
    The distance from (0.00, 0.00) to (1.00, 0.00) is 1.00
    $ ./distance 0 0 3 4
    The distance from (0.00, 0.00) to (3.00, 4.00) is 5.00

This looks like this program is working fine. However, if we try a few other values, we get 
incorrect distances::

    $ ./distance -1 0 1 0
    The distance from (-1.00, 0.00) to (1.00, 0.00) is 0.00
    $ ./distance -1 -1 2 3
    The distance from (-1.00, -1.00) to (2.00, 3.00) is 2.24

The correct distances are 2 and 5, respectively.

Logic errors are probably the hardest errors to debug, because, unlike compiler and runtime errors, 
we are not given a specific line to look at. This is specially true when the program exhibits correct 
behaviour in some cases but not in others. As we'll describe soon, we have to be specially methodic 
to track down these kind of errors.

Debugging Techniques
--------------------

Not all errors are debugged in the same way, and we need to make sure we use the right strategy for 
the right error.

Most debugging is *reactive*: an error happens, and we try to find the cause of that error so we can fix it. 
Debugging is said to be like investigating a murder where you are both the detective and the murderer.

Solving compiler/linker errors can generally be done by applying the rules of thumb 1-3 we have already described.

Solving runtime errors can usually be done by using a debugger, which can provide additional information 
about the error. In the case of segfaults, it can tell you the exact line where the segfault is happening.

.. note:: 

   The guide currently does not include specific instructions on how to use a debugger to do this. In the interim,
   you may want to take a look at the `Debugging Lab <https://uchicago-cs.github.io/cmsc22000/labs/lab4/>`_ 
   from CMSC 22000 (the class this guide was written for).

Solving logic errors is trickier (as we saw earlier, we don’t get clues on where the error may be located).

.. admonition:: Debugging Rule of Thumb #4

   When debugging an error (but specially a logic error). Narrow down your search as much as possible. 
   Start by focusing on the likely source of the error, and then refine your search using the techniques 
   described below. Don’t waste your time looking through the parts of the code that actually work.

Below we describe a number of debugging techniques that can be used to narrow down the origin of runtime
and logic errors. The first two, printf debugging and using a debugger, are *reactive* techniques: we
use them when a bug manifests itself. The remaining ones are *preemptive*: they involve writing code 
that doesn’t impact the functionality of our program, but which either helps us catch bugs sooner, 
or makes it easier for us to debug our program when bugs do surface.

printf debugging
~~~~~~~~~~~~~~~~

We add printf statements in our code to print out the values of variables (or, more generally, to get a 
better sense of the state of the running program) and re-run the program. Sometimes, seeing those values 
will help us to immediately realize what the problem is (specially when a variable has a value we did not expect)

For example, let’s say that your program is printing an incorrect value that is computed by calling 
functions A, B, and C. You should print the return value of each of those functions, to see if it is 
correct. Let’s say that B returns an incorrect value; you then narrow down your search to function B. 
You could then print out the values of the local variables at certain key points, to see whether 
they are being updated correctly.

.. admonition:: Debugging Rule of Thumb #5

   **Assume nothing, verify everything.**

   If you find yourself thinking "I’m pretty sure this variable has the 
   expected value" and "No need to bother debugging this function, I’m sure it works correctly", you 
   should still verify that this is the case.

Using a debugger
~~~~~~~~~~~~~~~~

"print debugging" is an easy technique that sometimes yields immediate results. However, it is a 
tedious technique if you’re dealing with an insidious bug that requires tracing through several 
functions, variables, etc. to find the issue. If you find yourself writing more than ten printf 
statements, that’s a sign you may need to switch to using a debugger.

A debugger (like GDB) provides you with powerful tools to trace the execution of your program, 
and to observe the evolution of its state (e.g., the values of all the variables) as the program 
runs. In a sense, this is not that different from printf debugging, except you get to "print" 
every single variable at every single line of code.

Assertions
~~~~~~~~~~

Most modern programming languages include the ability to add *assertions* in our code. An assertion 
is a boolean condition which, if false, will make our program exit immediately, with a helpful message
telling us what boolean condition failed.

For example, see the ``array.c`` file in ``examples/assertions/``. It includes an ``add_array``
function that includes an assertion::

    int add_array(int *a, int length)
    {
        assert(a != NULL);

If we were to call ``add_array`` with a NULL pointer, our code would crash. This would result in a segfault 
and, as we saw previously, we would likely be able to trace the exact cause of the error, but it would
mean firing up a debugger, etc. With an assertion, on the other hand, the same situation will result 
in the program crashing with an error message like this::

    array: array.c:7: add_array: Assertion `a != NULL' failed.

Assertion errors will tell us the file and line where the assertion failed. This gives us a lot more 
information than just a ``Segmentation fault`` message, and will make the error much easier to debug.

Logging
~~~~~~~

When debugging a logic error, we often have to trace through the execution of the program with a 
debugger to see whether anything looks "off" (e.g., a variable has a value we did not expect, etc.). 
We can facilitate this process by adding *logging statements* that provide information on what the 
program is doing. For example, we could add the following statement at the top of the ``add_array``
function::

    printf("Adding up array with %i elements.\n", length)

Note that we wouldn’t add this in reaction to an error: we would add it from the get-go, 
because the information included in the log message could be useful if we do have to debug 
the program. In particular, it would allow us to immediately *verify* whether the function is 
receiving the expected inputs (if not, that would give us a clue on where to narrow our search)

Note that while you can implement basic logging with printf statements, there are also logging 
libraries that allow you to control the level of logging (so that you can run your program without 
logging when you are not debugging it). For C, we could use something like this: https://github.com/rxi/log.c

Unit Tests
~~~~~~~~~~

Unit tests are beyond the scope of this guide, but the guide will eventually include
a discussion of `regression bugs <https://en.wikipedia.org/wiki/Software_regression>`_


Asking for Help
---------------

Sometimes, you will get completely stumped when trying to debug an issue, and will need 
to ask someone for help. Ideally, you will be able to sit down with someone and walk
through the issue with them, to see if they can spot something you missed. However,
it is very common to have to ask for help through a discussion forum, a chat, etc.
In that case, it will be more challenging for someone to provide assistance because
they can't see *exactly* what you're seeing, nor can they interactively debug
the issue with you.

Below we have compiled a list of guidelines and tips on how to ask effective questions:

* Make sure you're asking an actual question. You need to describe a specific issue 
  you're encountering, and *why* you’re stuck on it (e.g., you are not getting the 
  expected result, the tests are failing in a way you do not understand, etc.). 
  Writing a post that says "I can’t get this test to work, I’ve pushed my code. 
  Please look at it." is not a question! 

* It will be much easier for someone to help you if they are able to **reproduce** the 
  **exact** issue you are encountering (i.e., when they run your code, they must be able 
  to observe the exact same issue you’re encountering). You usually want to make sure
  you do the following:

  * If your code lives in a repository, make sure you push your latest code to that 
    repository. If the person helping you can't see your code, they won't be able
    to run it and reproduce the error.
  * Include a *detailed* description of the *exact* chain of events that leads to 
    the issue you're encountering. This includes providing the *exact* commands you
    are using to compile your code and run it, even if they seem completely foolproof
    to you (remember Rule of Thumb #5: Assume nothing!)
  * If something is "wrong", describe in what way it seems wrong to you. 
    For example, were you expecting a particular output but got a different one? 
    If a piece of code behaving in a way you were not expecting? etc. It can be useful 
    explain what you were expecting the code to do, and what you encountered instead.
  * Provide information on where you are running the code. In some cases, a bug will
    manifest itself on one operating system, but not in another. If you don't specify
    where you're encountering the bug, the person helping you may run it in an environment
    that does not reproduce the bug, and may just reply "works for me".

* If you encounter an error message (or any other unexpected output), please make sure you 
  include the **full and unabridged** error message (or unexpected output). While you may
  think you're being helpful by providing only the portion of the error message that
  is relevant, this also means the person helping you has less information to be able
  to help you with. Summarizing the message (e.g., "The test says something about an 
  unexpected sequence number") is similarly unhelpful. In general, the more information 
  you provide, the easier it will be for us to help you.

* Don't post screenshots of the output. Screenshots are not searchable and not copy-pasteable.
