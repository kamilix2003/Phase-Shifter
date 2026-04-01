
import serial

class Message:
    def __init__(self):
        self.msg_type = None
        self.length = None
        self.data = [0] * 62

    def encode(self):
        return bytes([self.msg_type, self.length] + self.data[:self.length])

class PhaseShifter:
    def __init__(self, port, baudrate=115200, timeout=1, phase_shifter_count=4, bit_resolution=4):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        self.phase_shifter_count = phase_shifter_count
        self.bit_resolution = bit_resolution

    phase_shifter_command_map = {
        "SET_PHASE": 0x10,
        "TRANSMIT": 0x11,
        "SET_LATCH": 0x12,
    }

    def wait_ack(self, msg_type):
        ack = self.ser.read(4)
        if ack != bytes([msg_type]):
            raise ValueError(f"Expected ACK for message type 0x{msg_type:02X}, but received 0x{ack:02X}")
        else:
            print(f"Received ACK for message type 0x{msg_type:02X}")

    def set_phase(self, phase: list, await_ack=False):
        
        if len(phase) != self.phase_shifter_count:
            raise ValueError(f"Phase list must have {self.phase_shifter_count} elements.")
        
        if any(p < 0 or p >= (1 << self.bit_resolution) for p in phase):
            raise ValueError(f"Phase values must be between 0 and { (1 << self.bit_resolution) - 1 }.")

        msg = Message()
        msg.msg_type = self.phase_shifter_command_map["SET_PHASE"]
        msg.length = self.phase_shifter_count
        msg.data = phase
        self.ser.write(msg.encode())
        if await_ack:
            self.wait_ack(msg.msg_type)

    def set_latch(self, latch: int, await_ack=False):
        msg = Message()
        msg.msg_type = self.phase_shifter_command_map["SET_LATCH"]
        msg.length = 1
        msg.data = [latch]
        self.ser.write(msg.encode())
        if await_ack:
            self.wait_ack(msg.msg_type)

    def transmit(self, await_ack=False):
        msg = Message()
        msg.msg_type = self.phase_shifter_command_map["TRANSMIT"]
        msg.length = 0
        self.ser.write(msg.encode())
        if await_ack:
            self.wait_ack(msg.msg_type)

    def close(self):
        self.ser.close()
