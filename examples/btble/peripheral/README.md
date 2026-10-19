# Peripheral

This sample description ble peripheral function with READ, WRITE/WRITE NO RESPONSE, INDICATE and NOTIFY properties. Can be linked and communicated by ble app.

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

## Generate BlE OTA file

```
python components/wireless/bluetooth/tools/GenerateOAD.py FW.bin FW.bin.sha256
```

## Service Configuration

This peripheral example implements a comprehensive BLE test service with multiple characteristics for testing various BLE operations.

### Primary Service
- **Service UUID**: `07af27a5-9c22-11ea-9afe-02fcdc4e7412`
- **Service Type**: Primary service
- **Purpose**: Test service for BLE throughput and functionality testing

### Characteristics

#### 1. Read Characteristic
- **UUID**: `07af27a6-9c22-11ea-9afe-02fcdc4e7412`
- **Properties**: Read
- **Handle Offset**: 2
- **Usage**: Test read operations from central devices

#### 2. Write Characteristic
- **UUID**: `07af27a7-9c22-11ea-9afe-02fcdc4e7412`
- **Properties**: Write without response
- **Handle Offset**: 4
- **Usage**: Receive data from central devices

#### 3. Indicate Characteristic
- **UUID**: `07af27a8-9c22-11ea-9afe-02fcdc4e7412`
- **Properties**: Indicate
- **Handle Offset**: 6
- **Usage**: Send indications to central devices (acknowledged)

#### 4. Notify Characteristic
- **UUID**: `07af27a9-9c22-11ea-9afe-02fcdc4e7412`
- **Properties**: Notify
- **Handle Offset**: 9
- **Usage**: Send notifications to central devices (unacknowledged)

### Characteristic Handle Mapping

| Characteristic | Value Handle | Description |
|---------------|--------------|-------------|
| Read          | Handle + 2   | Read-only data |
| Write         | Handle + 4   | Write without response |
| Indicate      | Handle + 6   | Indicate (acknowledged) |
| Notify        | Handle + 9   | Notify (unacknowledged) |

## Usage Examples

### Central Device Testing
When paired with the central example:
1. Central scans for "BTBLE-DEV" device name
2. Central connects and discovers this service
3. Central writes test data to the write characteristic
4. Peripheral processes the data (validates length < MTU size -3)

### Data Transfer Testing
- **Write Testing**: Central sends data to peripheral's write characteristic
- **Read Testing**: Central reads from peripheral's read characteristic
- **Notify Testing**: Peripheral sends notifications to subscribed central
- **Indicate Testing**: Peripheral sends indications to subscribed central

## Configuration Parameters

- **Device Name**: Configured via `CONFIG_BT_DEVICE_NAME`
- **Advertising**: Automatic advertising with device name
- **Connection Parameters**: Default BLE connection parameters
- **MTU**: Uses default MTU size
- **Security**: No encryption by default
