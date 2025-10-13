from serial import Serial
from COM_message import MessageBase, Message
from commands import *
import asyncio

class BeamController:
    def __init__(self, port) -> None:
        self.serial = Serial(port, baudrate=115200, timeout=10)

    def close(self) -> None:
        self.serial.close()

    def send_bytes(self, data: bytes) -> None:
        self.serial.write(data + b'\n')

    def send_command(self, command: MessageBase) -> None:
        self.serial.write(command.encode() + b'\n')

    def receive_response(self) -> bytes:
        out = self.serial.read(64)
        return out
    
    async def listen_on_port(self):
        byte_response: bytes = self.receive_response()
        if byte_response:
            msg = Message()
            msg.load(byte_response)
            try: 
                cmd = msg.get_command()
                if isinstance(cmd, CommandResponse):
                    print("Response Code:", cmd.response_code)
                if cmd:
                    print("Command:", cmd.__class__.__name__, cmd.__dict__)
            except Exception as e:
                print("Error loading command:", e)