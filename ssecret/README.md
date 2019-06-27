# Dependencies:

## YAJL

Library for JSON parsing/generation.

```bash
git clone git://github.com/lloyd/yajl
cd yajl
apt-get install cmake
./configure && make && make install
```

# Compile

* Compiled with gcc version 5.4.0 20160609
* Compiled with clang version 3.8.0-2ubuntu4

```bash
make
```

# Execute

## Client

```bash
./file-parser test 127.0.0.1 8086
```

## Server

```bash
nc -ul 127.0.0.1 8086
```

## Generate file

```bash
./generate_files.sh test
```

# Limitations

*  The folder should exist before starting file-parser.

*  The program only check new files in folder, existing files are not processed.
