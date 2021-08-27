# Facebook Hacker Cup just for fun.

https://www.facebook.com/codingcompetitions/hacker-cup/

## Online Qualifications 2021

To be processed

## C++ requirements

1. [cmake](https://cmake.org/) >= 2.7
2. [C++17](https://en.wikipedia.org/wiki/C%2B%2B17)

## Build C++ code

```bash
$ cd qualification
$ mkdir build
$ cd build
$ cmake ..
...
-- Build files have been written to: ~/hackercup/qualification/build
$ make
...
[100%] Built target hackercup
$ ./hackercup
Usage: ./hackercup [-io] [-i input][-o output] ...
...
```

## Run C++ code


```bash
$ ./hackercup --input=../input/a.txt \
              --output=../output/a.txt
```