from turtle import delay
import serial
import asyncio
import serial_asyncio
from COM_message import MessageBase, Message
from commands import *

class SerialProtocol(asyncio.Protocol):
    def __init__(self, controller):
        self.controller = controller
        self.transport = None

    def connection_made(self, transport: asyncio.Transport):
        self.transport = transport
        print("Serial connection established.")

    def connection_lost(self, exc):
        print("Serial connection lost.")
        asyncio.get_event_loop().stop()

    def data_received(self, data: bytes):
        print("Data received:", data)
        self.handle_line(data)

    def handle_line(self, line: bytes):
        msg = Message()
        msg.load(line)
        command = msg.get_command()
        if not command:
            print("Unknown message received.")
            return
        print("Received Command:", command.__class__.__name__, command.__dict__)

class BeamControllerBase:
    def __init__(self, port, baudrate = 115200) -> None:
        self.port = port
        self.loop = asyncio.get_event_loop()
        self.baudrate = baudrate
        self.transport = None
        self.protocol = None

    async def open(self) -> None:
        self.transport, self.protocol = await serial_asyncio.create_serial_connection(
            self.loop,
            lambda: SerialProtocol(self),
            self.port,
            baudrate=self.baudrate
        )
        print("Opened serial port:", self.port)

    async def open_with_retry(self, delay=5):
        while True:
            try:
                await self.open()
                break
            except Exception as e:
                print(f"Failed to connect: {e}, retrying in {delay}s...")
                await asyncio.sleep(delay)


    def close(self) -> None:
        self.transport.close()

    def send_bytes(self, data: bytes) -> None:
        try:
            self.transport.write(data + b'\n')
        except Exception as e:
            print("Error sending bytes:", e)

        print("Sent Bytes:", data)

    def send_command(self, command: MessageBase) -> None:
        try:
            self.transport.write(command.encode() + b'\n')
        except Exception as e:
            print("Error sending command:", e)

        print("Sent Command:", repr(command))

class BeamController(BeamControllerBase):
    def __init__(self, port, baudrate=115200, phase_shifter_count=4, phase_shifter_resolution=4) -> None:
        super().__init__(port, baudrate)
        self.phase_shifter_count = phase_shifter_count
        self.phase_shifter_resolution = phase_shifter_resolution
        self.max_phase_shift = (1 << phase_shifter_resolution) - 1

    def set_phase_shifts(self, phase_shifters: list[int]) -> None:

        if len(phase_shifters) != self.phase_shifter_count:
            raise ValueError(f"Expected {self.phase_shifter_count} phase shifters, got {len(phase_shifters)}")
        if any(ps < 0 or ps > self.max_phase_shift for ps in phase_shifters):
            raise ValueError(f"Phase shifter values must be between 0 and {self.max_phase_shift}")

        cmd = CommandSetPhaseShift(phase_shifter_count=len(phase_shifters))
        cmd.phase_shifters = phase_shifters

        msg = Message()
        msg.load_command(cmd)
        self.send_command(msg)

