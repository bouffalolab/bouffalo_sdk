## FreeRTOS+POSIX: Portable Operating System Interface (POSIX threading wrapper) for FreeRTOS

The Portable Operating System Interface (POSIX) is a family of standards specified by the IEEE Computer Society for maintaining compatibility between operating systems. FreeRTOS+POSIX implements *a small subset* of the [POSIX threading](https://pubs.opengroup.org/onlinepubs/7908799/xsh/threads.html) API. This subset allows application developers familiar with POSIX API to develop a FreeRTOS application using POSIX like threading primitives. FreeRTOS+POSIX does not implement more than 80% of the POSIX API. Therefore, an existing POSIX compliant application or a POSIX compliant library cannot be ported to run on FreeRTOS Kernel using only this wrapper. 


This repository only contains source code. For demo applications, please visit https://github.com/FreeRTOS/FreeRTOS-Labs. 

## To consume FreeRTOS+POSIX 
It is recommended to use this repository as a submodule. Please refer to [Git Tools — Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules). 

## Notes
This project is undergoing optimizations or refactorization to improve memory usage, modularity, documentation, demo usability, or test coverage. 
