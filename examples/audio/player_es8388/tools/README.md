
### tydb

**网络录音**
- 服务端命令

注意:关闭PC端防火墙，让设备和PC在同一个网络中
```
tydb.exe record web 0.0.0.0:8090 ./
```
- 设备端命令
```
record start ws://192.168.50.156:8090 data.pcm
record stop
```

**使用介绍**

参考文件：tydb/READMR.md
