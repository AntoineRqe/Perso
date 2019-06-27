#Dependencies:

##YAJL

Library for JSON parsing/generation.

```bash
git clone git://github.com/lloyd/yajl
cd yajl
apt-get install cmake
./configure && make && make install
```

#Compile

```bash
make
```

#Execute

##Client

```bash
./file-parser test 127.0.0.1 8086
```

##Server

```bash
nc -ul 127.0.0.1 8086
```

#Limitation

The folder should exist before starting file-parser.
The program only check new files in folder, existing files are not processed.
