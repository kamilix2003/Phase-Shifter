import serial
import time
from Command import Command
from PhaseShifterInterface import PhaseShifterInterface

""" 
COMMANDS
    Service commands
    - COMMAND_OK 0x00
    - COMMAND_ECHO 0x01
    - COMMAND_ERROR 0xff

    Settings commands
    - COMMAND_SPI_BUFFER_SIZE 0x03
    - COMMAND_AUTO_LATCH 0x04
    - COMMAND_SERIALIZE_PHASE_SHIFTER 0x05

    Buffer commands
    - COMMAND_SET_BUFFER 0x11
    - COMMAND_GET_BUFFER 0x12
    - COMMAND_GET_BUFFER_RE 0x13
    - COMMAND_CLEAR_BUFFER 0x14 
"""

serialPort = serial.Serial("COM5", 115200, timeout=1)

interface = PhaseShifterInterface(serialPort)

interface.set_phase_shifter_count(8)
interface.set_buffer(bytearray([0x04, 0x02, 0x03, 0x04]))
buffer = interface.get_buffer()
print(f"Buffer: {buffer}")

while True:
    for i in range(0, 15):
        interface.set_buffer(bytearray([i] * 8))
        interface.send_buffer()
        time.sleep(1)  # Sleep for 100ms between transmissions




