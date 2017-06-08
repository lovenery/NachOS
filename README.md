# NachOS

- Version: 4.0
- [Author Site](https://homes.cs.washington.edu/~tom/nachos/)

## Install

1. Test on ubuntu 32 bits
1. Download gcc-mips-xcompiler
1. Install g++ make csh

## Run

```shell
# test
cd code
make
./userprog/nachos -e ./test/test1
./userprog/nachos -e ./test/test2

# new feature
./userprog/nachos -e ./test/test1 -e ./test/test2
./userprog/nachos -e ./test/sleep
```

