
### tydb

**Network recording**
- Server-side command

Note: Turn off the firewall on the PC and ensure the device and the PC are on the same network.
```
tydb.exe record web 0.0.0.0:8090 ./
```
- Device-side command
```
record start ws://192.168.50.156:8090 data.pcm
record stop
```

**Usage introduction**

Reference file：tydb/READMR.md
