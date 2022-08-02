# Files for Assignment 1 - Fall 2020
You can compile the C++ code by running

```
make
```

# `dup.py` utility

To generate a really big file, you can use `dup.py`. It is a simple
python program that accepts a single argument "N" a number of bytes to
output. It reads in data from stdin, and the copies it onto stdout. If
the data size is smaller than N, the program repeats it until N bytes
is output.

Here is an example of how to feed 2GB of data to your program, generated
by repeating `t3.txt`:
```
$ ./dup.py 2000000000 < t3.txt | ./fast-pali
```
To time your code on the same data:
```
$ ./dup.py 2000000000 < t3.txt | time ./fast-pali
```
