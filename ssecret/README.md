#Dependencies:

##YAJL

```bash
git clone git://github.com/lloyd/yajl
cd yajl
apt-get install cmake
./configure && make && make install
```

#Compile

```bash
gcc inotify-test.c -o notify -g -lyajl
```

#Execute

##Client

```bash
./notify repertory_test 127.0.0.1 8086
```

##Server

```bash
nc -ul 127.0.0.1 8086
```
