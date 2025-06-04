# Toolbox

## What is Toolbox?

Toolbox is a compact, statically linked binary that combines many common command line utilities into a single executable. It is designed as a minimalist and efficient toolkit providing essential `*nix` commands, all accessible through one binary. This approach simplifies deployment and usage, especially in resource-constrained or embedded environments.

Similar to BusyBox and ToyBox, Toolbox offers streamlined replacements for many standard utilities typically found in GNU coreutils, util-linux, and other common `*nix` toolsets. While Toolbox commands may not include every advanced feature of their full-fledged GNU counterparts, they cover the core functionality expected by most users and scripts, ensuring familiar behavior and compatibility.

By consolidating multiple utilities into one executable, Toolbox reduces disk space usage, simplifies updates, and eases distribution, making it highly practical for minimal Linux setups, containers, rescue environments, and embedded systems.

## Why Choose Toolbox?

### Single Binary Convenience

Instead of installing dozens of separate utilities, Toolbox bundles them into one executable, reducing complexity and storage requirements.

### Static Linking

Toolbox is built as a statically linked binary, meaning it does not depend on external shared libraries at runtime. This enhances portability across various Linux distributions and environments without worrying about missing dependencies.

### Minimalist and Efficient

Commands are implemented with size and simplicity in mind, avoiding unnecessary complexity. This makes Toolbox ideal for environments with limited resources.

### Extensible Command Dispatch

Toolbox uses a clean and maintainable command dispatch mechanism, allowing easy addition of new commands without bloating the main program logic.

### Custom Utilities

Beyond standard `*nix` commands, Toolbox includes useful custom tools such as `ascii2hex` for converting ASCII strings to hexadecimal notation, enhancing its utility in scripting and debugging scenarios.

## How to Build Toolbox

To build Toolbox from source, follow these steps:

1. Run `build_musl_libc.sh` to build the musl libc statically.
2. Run `build_toolchain.sh` to build the statically linked binutils, GCC, and glibc toolchain.
3. Use the `Makefile` to compile `toolbox.c` into the final executable.

This setup ensures that all binaries are statically linked, providing portability and independence from the host system's libraries.

```
./build_toolchain.sh
./musl_libc_build.sh
make
make install
man toolbox
```

This process compiles the statically linked binary and installs it along with its man page.

## How It Works

Toolbox operates as a single statically linked executable that determines which command to run based on the first argument passed to it. For example:

```
./toolbox ls /proc
./toolbox cp /proc/cpuinfo /tmp
./toolbox mkdir /tmp/testdir
./toolbox cat /proc/cpuinfo
./toolbox echo "Hack the World"
./toolbox clear
./toolbox rm /tmp/cpuinfo
./toolbox head toolbox.c 20
./toolbox file toolbox
./toolbox grep "main()" toolbox.c
./toolbox rmdir /tmp/testdir
./toolbox ascii2hex
./toolbox sync
./toolbox true
./toolbox false
./toolbox tty
./toolbox kmsg
```

You can also use Toolbox commands in combination with other shell utilities:

```
/usr/bin/sleep 1024 & ps aux | grep "[s]leep 1024" | awk '{print $2}' | xargs -I{} ./toolbox kill 9 {}
```

## Supported Commands

- `./toolbox ls` - list directory contents  
- `./toolbox cp` - copy files  
- `./toolbox mkdir` - create a directory  
- `./toolbox cat` - concatenate files and print on stdout  
- `./toolbox echo` - print arguments  
- `./toolbox kill` - terminate a process  
- `./toolbox clear` - clear the terminal screen  
- `./toolbox rm` - remove files  
- `./toolbox head` - output the first part of a file  
- `./toolbox file` - determine file type by magic bytes signature  
- `./toolbox grep` - global regular expression print  
- `./toolbox rmdir` - remove directory  
- `./toolbox sync` - commit filesystem caches to disk  
- `./toolbox ascii2hex` - convert ASCII characters to their hexadecimal representation  
- `./toolbox true` - do nothing successfully  
- `./toolbox false` - do nothing unsuccessfully  
- `./toolbox tty` - print the file name of the terminal connected to stdin  
- `./toolbox kmsg` - print messages from the kernel ring buffer  

## Files Description

- **build_musl_libc.sh**  
  POSIX shell script that builds the musl libc library statically. This script compiles musl libc to be used as the C standard library for the toolchain.

- **build_toolchain.sh**  
  POSIX shell script that builds a complete statically linked toolchain including binutils, GCC, and glibc.

- **Makefile**  
  Build instructions for the main program. You can select which toolchain to use by specifying the CC variable when running make, for example, 'make CC=musl-gcc' to use the musl toolchain or 'make CC=gcc' for the glibc toolchain.

- **toolbox.c**  
  The main C source file containing the program logic. This is the primary application that is compiled and linked using the custom-built toolchain.

- **toolbox.1**  
  The man page for the main program, providing usage instructions and documentation.

- **LICENSE**  
  The license file governing the use and distribution of the project.

- **README.md**  
  This file, providing an overview and instructions for building and using the project.

## Summary

Toolbox is an efficient, single-binary `*nix` command toolkit designed for simplicity, portability, and ease of use. Its static linking and minimalist design make it especially suitable for embedded systems, containers, and environments where resources are limited or where deployment simplicity is crucial. By combining many common utilities into one executable, Toolbox streamlines system maintenance and scripting tasks while maintaining compatibility with standard `*nix` behaviors.
