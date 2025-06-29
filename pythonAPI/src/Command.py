
""" 
COMMANDS
    Service commands
    - COMMAND_OK 0x00
    - COMMAND_ECHO 0x01
    - COMMAND_ERROR 0xff

    Settings commands
    - COMMAND_PHASE_SHIFTER_COUNT 0x03
    - COMMAND_AUTO_LATCH 0x04
    - COMMAND_SERIALIZE_PHASE_SHIFTER 0x05

    Buffer commands
    - COMMAND_SEND_BUFFER 0x10
    - COMMAND_SET_BUFFER 0x11
    - COMMAND_GET_BUFFER 0x12
    - COMMAND_GET_BUFFER_RE 0x13
    - COMMAND_CLEAR_BUFFER 0x14 
"""

class Command:

    def __init__(self, command_id, data: bytearray):
        self.command_id = command_id
        self.data = data
        self.data_length = len(data)
        self.crc = self.compute_crc()

    def recieve(self, buffer: bytearray):
        if len(buffer) < 3:
            raise ValueError("Buffer too short to contain a valid command")

        self.command_id = buffer[0]
        self.data_length = buffer[1]
        self.data = buffer[2:2 + self.data_length]
        self.crc = buffer[2 + self.data_length]

        # if self.crc != self.compute_crc():
        #     raise ValueError("CRC mismatch, command data may be corrupted")

    def buffer(self) -> bytearray:
        buffer = bytearray()
        buffer.append(self.command_id)
        buffer.append(self.data_length)
        buffer.extend(self.data)
        buffer.append(self.crc)
        return buffer

    def __str__(self):
        return f"Command ID: 0x{self.command_id:02X},\
                Data Length: 0x{self.data_length:02X},\
                Data_decimal: {self.data},\
                CRC: 0x{self.crc:02X}"

    def compute_crc(self) -> int:
        polynomial = 0x04C11DB7
        crc = 0xFFFFFFFF

        buffer = bytearray()
        buffer.append(self.command_id)
        buffer.append(self.data_length)
        buffer.extend(self.data)

        for byte in buffer:
            crc ^= (byte << 24)
            for _ in range(8):
                if crc & 0x80000000:
                    crc = (crc << 1) ^ polynomial
                else:
                    crc <<= 1
                crc &= 0xFFFFFFFF  # Ensure 32-bit result

        return crc & 0xFF  # No final XOR


    
