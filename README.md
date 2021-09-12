# Facebook Hacker Cup just for fun.

https://www.facebook.com/codingcompetitions/hacker-cup/

## Results 2021


[Certificate](https://www.facebook.com/codingcompetitions/hacker-cup/2021/certificate/2945131779054766)

### Online Qualifications

Score: `43/100` points

Rank: `4037`th out of `12692` (total participants `34588`)

### Round 1

Score: `28/100` points

Rank: `4191`th out of `6811` (total participants `12692`)

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