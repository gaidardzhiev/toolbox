# Toolbox

## What is Toolbox?

Toolbox is a compact, statically linked binary that combines many common command line utilities into a single executable. It is designed as a minimalist and efficient toolkit providing essential Unix commands, all accessible through one binary. This approach simplifies deployment and usage, especially in resource-constrained or embedded environments.

Similar to BusyBox and ToyBox, Toolbox offers streamlined replacements for many standard utilities typically found in GNU coreutils, util-linux, and other common Unix toolsets. While Toolbox commands may not include every advanced feature of their full-fledged GNU counterparts, they cover the core functionality expected by most users and scripts, ensuring familiar behavior and compatibility.

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

Beyond standard Unix commands, Toolbox includes useful custom tools such as `ascii2hex` for converting ASCII strings to hexadecimal notation, enhancing its utility in scripting and debugging scenarios.

## How to Build Toolbox

To build Toolbox from source, follow these steps:

```
./toolchain_build.sh
make
make install
man toolbox
```

This process compiles the statically linked binary and installs it along with its manual page.

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

- `ls` - list directory contents  
- `cp` - copy files  
- `mkdir` - create a directory  
- `cat` - concatenate files and print on stdout  
- `echo` - print arguments  
- `kill` - terminate a process  
- `clear` - clear the terminal screen  
- `rm` - remove files  
- `head` - output the first part of a file  
- `file` - determine file type by magic bytes signature  
- `grep` - global regular expression print  
- `rmdir` - remove directory  
- `sync` - commit filesystem caches to disk  
- `ascii2hex` - convert ASCII characters to their hexadecimal representation  
- `true` - do nothing successfully  
- `false` - do nothing unsuccessfully  
- `tty` - print the file name of the terminal connected to stdin  
- `kmsg` - print messages from the kernel ring buffer  

## Summary

Toolbox is an efficient, single-binary Unix command toolkit designed for simplicity, portability, and ease of use. Its static linking and minimalist design make it especially suitable for embedded systems, containers, and environments where resources are limited or where deployment simplicity is crucial. By combining many common utilities into one executable, Toolbox streamlines system maintenance and scripting tasks while maintaining compatibility with standard Unix behaviors.
