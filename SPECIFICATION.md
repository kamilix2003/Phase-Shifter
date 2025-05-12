# MAPS-010144 Phase Shifter Interface as Part of Phased Antenna Array

## Description

A microcontroller serves as an interface for phased control, managing multiple phase shifters connected in a daisy chain via SPI. Bidirectional communication between the PC and MCU is established via UART/USB.

## Goals

in **bold** features in scope of SKM project

- **Serial interface with IC via SPI peripheral**
- **PC-MCU communication by UART/USB (virtual COM)**
- **Controlling multiple phase shifter in daisy chain[^1]**
- API for controlling beam direction
- I2C bus for communication with multiple arrays where one becomes master

## SPI

Serial communication specification according to MAPS-010144 datasheet[^2]

|Pin|Description|
|---------|---------|
|SERIN|MOSI|
|SEROUT|MSIO - connected to the next IC|
|CLK|Clock|
|LE|Output (phase) stays constant when set LOW|
|P/S|Parallel/Serial select, HIGH for Serial|

Internal register of the IC has 6 bits, where most significant bit represent most significant phase shift

|Option|Value|
|---------|---------|
|Data size|6 Bits|
|Frist bit|MSB first|
|Clock period|> 100ns|
|CPOL|LOW|
|CPHA|LOW|

## UART / USB

### Frame

Full frame is built of multiple bytes where:

|Bytes |Function|
|---------|---------|
|Byte 1|Command ID|
|Byte 2|Payload size|
|Byte 3|Data 0|
|Byte 4|Data 1|
|Byte ...|...|
|Byte N|Data N|
|Byte N + 1|CRC|

### Commands

#### Send phase shift buffer

- Data n - phase shifter address
- Data n+1 - phase shift

#### Request phase shift buffer

- Data n - phase shifter address

#### Load phase shift buffer

- No data

#### Clear phase shift buffer

- No data

#### Error

- Data n - contains error code

[^1]: [SPI daisy chain](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface#Daisy_chain_configuration)
[^2]: [MAPS-010144 datasheet](https://cdn.macom.com/datasheets/MAPS-010144.pdf)
