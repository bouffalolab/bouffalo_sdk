# openamp

## 芯片支持列表

|  芯片  | 备注 |
| :----: | :--: |
| BL61xD |      |

## 编译

- BL61xD

```bash
make
```

## Flash

- BL61xD

```bash
make CHIP=BL616D flash COMX=xxx ## xxx is your com name
```

## 说明

​	本demo提供了多种调用openamp的方式，`demo_openamp_*.c`的demo是直接调用openamp相关api，其写法灵活，用户可以根据需要实现对应的功能，使用` set(CONFIG_RPMSG_SERVICE n)` 配置即可开启此功能。 `rpmsg_service_*.c`的demo是调用对openamp封装后的`rpmsg service api`，其api简洁，使用简单方便，使用` set(CONFIG_RPMSG_SERVICE y)` 配置即可开启此功能。

## 宏介绍

| 宏定义                               | 说明                                                         |
| ------------------------------------ | ------------------------------------------------------------ |
| CONFIG_IPC                           | IPC功能总开关                                                |
| CONFIG_OPENAMP_VIRTIO_DRIVER_SUPPORT | openamp中virtio功能，作为master的核需要开启                  |
| CONFIG_OPENAMP_VIRTIO_DEVICE_SUPPORT | openamp中virtio功能，作为slave的核需要开启                   |
| CONFIG_OPENAMP_PROXY                 | openamp的代理功能，如网络传输，文件系统等需要异步操作的需要开启 |
| CONFIG_RPMSG_SERVICE                 | 对openamp的封装，提供更加简洁方便的api共用户使用             |
