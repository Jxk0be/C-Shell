# Custom C Shell Project

This project involved developing jsh, a simple shell resembling csh. The goal is to create a command line interpreter capable of executing commands and handling input/output redirection. The shell supports operations such as `<`, `>`, `>>`, `&`, and `|` and follows the syntax of the bourne/c shell.

This project consists of three parts that progressively enhance the functionality of the shell:

1. **Basic Shell:** It supports an optional prompt and waits for command completion unless an ampersand (&) is present. Exiting the shell is possible through CTRL-D or the "exit" command.

2. **Input/Output Redirection:** This feature allows commands to read input from or write output to specified files.

3. **Pipes:** Pipes enable the combination of commands and parallel execution. The shell waits for all processes in a pipe to complete, unless an ampersand is specified.

In the end, we have a functional shell capable of executing commands, handling input/output redirection, and utilizing pipes for command composition.
