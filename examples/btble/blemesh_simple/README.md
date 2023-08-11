# blemesh_simple

This example shows how to use ble mesh.

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## How to use

 In this example, the mesh is started after power-on. You can refer to the documentation in blemesh_cli to use the app to connect nodes to the network and configure network parameters. If you need to issue control commands, you can use the "gen_cli_send dstaddress" command to send control data to the specified device address or this group of addresses.
