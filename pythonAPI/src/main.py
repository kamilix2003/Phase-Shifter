import asyncio
from beam_controller import BeamController
from COM_message import Message
from commands import *
import cmd


async def main():
    bc: BeamController = BeamController(port="COM14", baudrate=115200, phase_shifter_count=4, phase_shifter_resolution=4)
    await bc.open_with_retry()

    await asyncio.sleep(1)  # Wait for connection to establish

    # Example command usage
    bc.set_phase_shifts([0, 1, 2, 3])

    await asyncio.sleep(1)  # Wait for connection to establish

    try:
        bc.set_phase_shifts([0, 1, 2, 17])
    except ValueError as e:
        print("Error:", e)


if __name__ == "__main__":
    # asyncio.run(main())
    import serial
    ser = serial.Serial(port="COM6", baudrate=115200, timeout=1)

    a = [
        bytearray([0x01, 0x04, 0x00, 0x00, 0x00, 0x00]),
        bytearray([0x01, 0x04, 0x00, 0x01, 0x02, 0x03]),
        bytearray([0x01, 0x04, 0x00, 0x02, 0x04, 0x06]),
        bytearray([0x01, 0x04, 0x00, 0x03, 0x06, 0x09]),
        bytearray([0x01, 0x04, 0x00, 0x04, 0x08, 0x0C]),
        bytearray([0x01, 0x04, 0x00, 0x05, 0x0A, 0x0F]),
    ]

    b = [
        bytearray([0x01, 0x04, 0x00, 0x00, 0x00, 0x00]),
        bytearray([0x01, 0x04, 0x03, 0x02, 0x01, 0x00]),
        bytearray([0x01, 0x04, 0x06, 0x04, 0x02, 0x00]),
        bytearray([0x01, 0x04, 0x09, 0x06, 0x03, 0x00]),
        bytearray([0x01, 0x04, 0x0C, 0x08, 0x04, 0x00]),
        bytearray([0x01, 0x04, 0x0F, 0x0A, 0x05, 0x00]),
    ]

    print("Linear progression 1")
    pause = input("Press Enter to continue...")
    for i in a:
        ser.write(i)
        resp = ser.read(6)
        print("Sent: ", list(i), " Received: ", list(resp))
        pause = input("Press Enter to continue...")

    print("Linear progression 2")
    pause = input("Press Enter to continue...")
    for i in b:
        ser.write(i)
        resp = ser.read(6)
        print("Sent: ", list(i), " Received: ", list(resp))
        pause = input("Press Enter to continue...")