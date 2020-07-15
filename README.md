# Litchi ![Build](https://github.com/BugenZhao/Litchi/workflows/Build/badge.svg)

Bugen's toy kernel for x86 systems implemented in modern C++.

![litchi](docs/v0.5.6.png)

## Get Started

### Install Toolchains

Litchi is compiled to i386-elf target, which may require a cross-compiler. For macOS user, some Homebrew formulae and bottles are provided (see [BugenZhao/homebrew-litchi](https://github.com/BugenZhao/homebrew-litchi)).

Make sure you have Homebrew installed, then simply run:

```bash
$ brew tap bugenzhao/litchi
$ brew install i386-litchi-elf-binutils \
               i386-litchi-elf-gcc      \
               i386-litchi-elf-gdb      \
               litchi-qemu
```

### Build & Run

Litchi is built by CMake, which is fairly easy to build and run:

```bash
$ git clone https://github.com/BugenZhao/Litchi.git && cd Litchi
$ mkdir build && cd build && cmake .. && make qemu
```




## Progress
- [x] Adopt bootloader from JOS / xv6
- [ ] Bootloader support for floppy drive
- [x] Port old Litchi codes
- [x] CMake build support
- [x] Kernel level standard output
- [x] Color output in console
- [x] Serial port color output
- [ ] Serial port input
- [x] Kernel panic and warning
- [x] Programmed keyboard input support
- [x] Kernel level standard input
- [ ] Input with format
    - [x] `stringToLong`
    - [ ] `scanFmt`
- [x] Basic debugging information
- [ ] Basic kernel commands
    - [x] `help`
    - [x] `echo`
    - [x] `vm-*`
        - [x] `map`
        - [x] `dump`
    - [ ] `...`
- [x] Physical memory detection
- [x] Physical memory management
- [x] Virtual memory
- [x] Kernel address space
- [ ] Kernel heap management
- [x] User level task
- [x] Interrupt / Rrap
- [x] System call

---
- [ ] Test for standard library
- [ ] Interrupt based keyboard input
- [ ] Shell
- [ ] Multitasking
- [ ] Multi-core support
- [ ] Port to x86-64
- [ ] ...

## Reference
1. [JOS / xv6](https://pdos.csail.mit.edu/6.828/2018/schedule.html)
2. [Stack Overflow](https://stackoverflow.com)
3. [Hurlex](http://hurlex.0xffffff.org)
4. [Orange'S: 一个操作系统的实现](https://book.douban.com/subject/3735649/)
5. [OSDev.org](https://wiki.osdev.org)
