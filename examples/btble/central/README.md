# Central

This sample demonstrates the BLE central (client) role. It scans for a target peripheral device, establishes a connection, discovers services and characteristics, and continuously writes test data.

## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Target Peripheral Configuration

This central example is designed to connect to a peripheral device with the following configuration:

### Target Peripheral Device
- **Device Name**: BTBLE-DEV (configured via `CONFIG_BT_DEVICE_NAME`)
- **Connection Type**: BLE Central role scanning for specific device name

### Service Configuration

The peripheral device should expose the following BLE service:

#### Primary Service
- **Service UUID**: `07af27a5-9c22-11ea-9afe-02fcdc4e7412`
- **Service Type**: Primary service
- **Purpose**: Test service for BLE throughput and functionality testing

#### Characteristic Details
- **Characteristic UUID**: `07af27a7-9c22-11ea-9afe-02fcdc4e7412`
- **Properties**: Write without response
- **Usage**: Used for data transmission testing from central to peripheral

### Connection Workflow

1. **Scanning Phase**: Central device scans for devices named "BTBLE-DEV"
2. **Connection Phase**: Upon finding the target device, initiates connection
3. **Service Discovery**: Automatically discovers the target service UUID
4. **Characteristic Discovery**: Finds the writable characteristic
5. **Data Transmission**: Continuously writes test data to the characteristic

### Data Transmission

The central example performs the following operations:
- **Write Task**: Automatically created after discovering the writable characteristic
- **Data Pattern**: Sequential bytes (0x00, 0x01, 0x02, ...)
- **Data Size**: 244 bytes per write operation
- **Interval**: 1000ms between writes
- **Write Type**: Write without response (unacknowledged)

### Configuration Parameters

- **Scan Parameters**: Fast scan mode with active scanning
- **Connection Parameters**: Default BLE connection parameters
- **MTU**: Uses default MTU size (can be negotiated higher)
- **Security**: Optional SMP pairing (controlled by `BLE_SMP_TEST` macro)
