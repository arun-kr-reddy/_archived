# Misc

1. [To Do](#to-do)
2. [Links](#links)
3. [Linux](#linux)
4. [CMake](#cmake)
5. [Makefile](#makefile)
6. [GDB](#gdb)
7. [GProf](#gprof)

## To Do

- [Tools](https://cs.baylor.edu/~donahoo/tools/)
- git

## Links

- [CMake](https://codevion.github.io/#!cpp/cmake.md)
- [Makefile](https://makefiletutorial.com/)

## Linux

```sh
arun@IG3:~$
arun        # username
IG3         # machine
~           # /home/
$           # normal user, root user #
```

```sh
ls
-l          # list
-a          # show all files
-R          # recursive
```

```sh
mkdir
-p          # create subdirectories
```

```sh
file        # file info
```

```sh
man         # command manual page
```

```sh
cp          # copy file
-r          # recursive
```

```sh
mv          # move file, always recursive
```

```sh
rm          # remove file
-r          # recursive
```

```sh
cat         # print file
```

```sh
>           # write
>>          # append
```

```sh
nano        # basic text editor
```

```sh
sort        # print file sorted by lines
-n          # numerical sort, default string sort
-uonly      # unique entries
```

```sh
find <path> -name "<string>"  # search for string filename in path
```

```sh
grep <string> <file>    # search string within file
-n                      # print line numbers
-r                      # recursive
-i                      # case insensitive
```

```sh
size ./test_exe     # memory layout sizes
```

**chaining commands**  
```sh
command1 ; command2     # sequential
command1 && command2    # if previous command successful
command1 | command2     # current stdout to next stdin (stderr to terminal)
```

**placeholder**  
```sh
*           # any set of characters
?           # single char
[a-f]       # single char in
[^a-c]      # single char not in
```

**file permissions**  3 bits each (**r**ead, **w**rite & e**x**ecute) for user, group & everyone else  
```sh
chmod +700 file       # change mode (only user can r, w or x)
```

**tar**  
```sh
-c    # compress
-x    # extract
-v    # show files
-f    # file
```

**dynamic lib versions**  
lib ⟶ lib_major_ver ⟶ lib_minor_ver  
`libtest.so ⟶ libtest.so.2 ⟶ libtest.so.2.1`  

## CMake

CMake ⟶ MakeFiles ⟶ exe  

`CMakeLists.txt`  

**basic**  
```cmake
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)

set(SOURCES main.cpp)
include_directories(include)
project(test_project)
add_executable(main_exe SOURCES)
```

**library**  
```cmake
add_library(mylib STATIC lib.cpp)
find_library(PTHREAD_LIB pthread)
target_link_libraries(main_exe mylib PTHREAD_LIB)
```

## Makefile

which part of large program to rebuild (compare timstamp of `*.o` & `*.c`)  

**basic**  
```make
test_exe: test.o
  cc test.o -o test_exe     # TABs only

test.o: test.cpp
  cc -c test.cpp -o test.o

test.cpp:
  echo "file not found"

clean:
  rm *.o test_exe
```

**variables**  
```make
files = test.cpp   #   $(files)
```

**all target**  
```make
all: one two

one:
  touch one

two:
  touch two

clean:
  rm -f one two
```

**multiple targets**  
```make
all: f1.o f2.o

f1.o f2.o:
  echo $@
# Equivalent to:
# f1.o:
#	 echo f1.o
# f2.o:
#	 echo f2.o
```
```make
  echo $@       # target name
  echo $?       # all prerequisites newer than target
  echo $^       # all prerequisites
```

**implicit rules**  
```make
CC            # C compiler (default cc)
CXX           # CPP compiler (default g++)
CFLAGS        # C flags 
CXXFLAGS      # CPP flags
CPPFLAGS      # C preprocessor flags
LDFLAGS       # linker flags
```
```make
CXX = g++
CXXFLAGS = -g -O3

test_exe: test.o

test.cpp:
  echo "file not found"

clean:
  rm test*
```

## GDB

```sh
gdb <program>             # load program
run [args]                # run with args
start [args]              # run with args, temporary breakpoint on main()
break <location>          # breakpoint, [function]/[file]:[line]
break <location> if <expr> # conditional breakpoint
clear [location]          # delete breakpoint
delete [number]           # delete breakpoint (`info breakpoints`)
disable [number]          # disable breakpoint (`enable [number]`)
continue [ignore_count]   # continue after breakpoint
next [count]              # step over, instruction nexti
step [count]              # step in, instruction stepi
until <location>          # run until
return <expr>             # return prematurely
finish                    # finish current function
print <expr>              # print once (`print array@size`)
display <expr>            # print each time
watch <expr>              # print when modified
set variable <var> = <expr> # set variable
backtrace [count]         # call stack
layout name               # TUI, src/asm/split
refresh                   # refresh TUI display
kill                      # kill running program
quit                      # quit GDB
```

## GProf

```sh
./test_gprof
gprof test_gprof gmon.out > analysis.txt
```