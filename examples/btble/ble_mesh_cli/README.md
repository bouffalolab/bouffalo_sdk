# ble_mesh_cli

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

- Run the following mesh commands in the serial terminal

```bash
bouffalolab />blemesh_init
......

bouffalolab />blemesh_pb 2 1 
......

```

- Open the app and select ADD NODE in the Network column

![img1](img/img1.jpg)

- Select your device

![img2](img/img2.jpg)

- Click the IDENTIFY button

![img3](img/img3.jpg)

- Click the PROVISION button

![img4](img/img4.jpg)

- Select No OOB in the pop-up window and click ok

![img5](img/img5.jpg)

- Click ok

![img6](img/img6.jpg)

- Your node is automatically added to the Network column, click on your connected device
- Locate Elements option and click it’s drop-down button, Select the Generic On Off Server option

![img7](img/img7.jpg)

- Click the BIND KEY button

![img8](img/img8.jpg)

- Click Application key 1

![img9](img/img9.jpg)

- Click on the ON and OFF buttons to control the LED switch

![img10](img/img10.jpg)

- The following information can be seen in the serial monitor, indicating that the LED is successfully controlled

![img11](img/img11.jpg)

- Click the SUBSCRIBE button

![img12](img/img12.jpg)

- Select “Create a new group to subscribe” option

- In the group, clicking on ON and OFF buttons prints the LED information in the serial monitor, indicating that the mesh is working

![img13](img/img13.jpg)
