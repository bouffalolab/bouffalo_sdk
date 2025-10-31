
### tydb

**Network Audio Recording**
- Server Command

Note: Turn off the PC firewall and ensure the device and PC are on the same network.
```
tydb.exe record web 0.0.0.0:8090 ./
```
- Device Command
```
record start ws://192.168.50.156:8090 data.pcm
record stop
```

**Usage Instructions**

Reference File: tydb/READMR.md
