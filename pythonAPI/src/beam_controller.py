import serial
import asyncio
import serial_asyncio
from COM_message import MessageBase, Message
from commands import *

class SerialProtocol(asyncio.Protocol):
    def __init__(self):
        super().__init__()
        self.buffer = bytearray()

    def connection_made(self, transport: asyncio.Transport):
        self.transport = transport
        print("Serial connection established.", transport)
        transport.write(b'Hello, device!\n')

    def connection_lost(self, exc):
        return super().connection_lost(exc)

    def data_received(self, data: bytes):
        self.buffer.extend(data)
        while b'\n' in self.buffer:
            line, _, self.buffer = self.buffer.partition(b'\n')
            self.handle_line(line)

    def handle_line(self, line: bytes):
        msg = Message()
        msg.load(line)
        command = msg.get_command()
        if command:
            print("Received Command:", command.__class__.__name__, command.__dict__)

class BeamControllerBase:
    def __init__(self, port) -> None:
        self.port = port
        self.serial = serial.Serial(port, baudrate=115200, timeout=10)

    def close(self) -> None:
        self.serial.close()

    def send_bytes(self, data: bytes) -> None:
        self.serial.write(data + b'\n')

    def send_command(self, command: MessageBase) -> None:
        self.serial.write(command.encode() + b'\n')

    
class BeamController(BeamControllerBase):
    def __init__(self, port) -> None:
        super().__init__(port)

    def set_phase_shifts(self, phase_shifters: list[int]) -> None:
        cmd = CommandSetPhaseShift(phase_shifter_count=len(phase_shifters))
        cmd.phase_shifters = phase_shifters
        msg = Message()
        msg.load_command(cmd)
        self.send_command(msg)

