unix-shell-v4: A Custom POSIX-Compliant Shell
🌟 Overview
unix-shell-v4 is a custom, feature-rich command-line interpreter designed to emulate the core functionality of a modern POSIX-compliant Unix shell.

Developed primarily to deepen the understanding of operating system principles—specifically process management, file system interaction, and command execution—this project provides a robust foundation for building more complex shell environments.

✨ Key Features
This shell supports the essential modes and commands necessary for basic system interaction, process control, and path management.

1. Execution Modes
Interactive Mode: Runs in a continuous loop, prompting the user for input and executing commands directly (similar to Bash or Zsh).

Batch Mode: Accepts a single file argument containing a list of commands, executes them sequentially, and then exits. Ideal for automation and scripting.

2. Command Processing
Process Management: Correctly handles the creation and management of child processes using system calls (fork() and execvp()) to execute external programs (e.g., ls, grep).

Path Searching: Implements intelligent searching for external commands based on the current execution path, mimicking the behavior of the $PATH environment variable.

3. Built-in Commands
The following commands are handled internally by the shell and do not require spawning a separate child process:

Command

Description

Arguments

exit

Terminates the shell process.

Must be called without arguments.

cd

Changes the current working directory.

Requires exactly one argument (the target path).

path

Manages the list of directories where the shell searches for external commands.

Accepts zero or more arguments, completely overwriting the existing search path.

🛠️ Technology Stack
This project is implemented using core system programming tools and practices.

Language: C / C++ (Standard practice for shell development due to reliance on low-level system calls).

Core APIs: Extensive use of standard Unix system calls (fork, exec, wait, chdir, access).

Environment: Developed and tested primarily on a Linux/Unix environment.

🚀 Installation & Setup
Requirements
A C/C++ compiler (e.g., GCC or Clang).

A standard Unix-like environment (Linux, macOS, WSL).

Build Instructions
Clone the Repository:

git clone [https://github.com/yeojustin/unix-shell-v4.git](https://github.com/yeojustin/unix-shell-v4.git)
cd unix-shell-v4

Compile the Shell:
(Assuming a standard Makefile is provided)

make

This will generate the executable file (likely named tash or shell).

💡 Usage
Interactive Mode
Run the executable without any arguments:

./tash
tash> ls -l
tash> path /bin /usr/bin
tash> cd ~
tash> exit

Batch Mode
Run the executable with a single file argument:

# Assuming batch.txt contains one command per line
./tash batch.txt

Author: Yeo Justin

Version: 4.0
