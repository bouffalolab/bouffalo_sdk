## crash tool capture multiple coredump from log

### usage：

./tools/crash_tools/crash_capture.pl your_log

This command will generate tools/crash_tools/output/coredump${n} file.


### output:

if find coredump, output:
```
[crash_capture] find 1 coredump 
[crash_capture] find 2 coredump 
[crash_capture] find 3 coredump 
[crash_capture] find 4 coredump 
```

else not find:
```
[crash_capture] no valid coredump find
```

else with crc check fail section:
```
[crash_capture] find 1 coredump 
crc check addr: 62fc8000, length: 00048000, end_crc: 62462220, crc: a9f4d84a 
[crash_capture] find 2 coredump 
[crash_capture] find 3 coredump 
[crash_capture] find 4 coredump 
```

If the crc check of a section of a coredump fails, the generated coredump file will not contain this section. This may be caused by an error in the middle of the uart transfer.

### NOTE:

support log file format: utf8 utf16be utf16le, with fixed length timestamp.
