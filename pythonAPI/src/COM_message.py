import ctypes
from commands import *

class MessageBase(ctypes.Structure):

    def encode(self) -> bytes:
        return ctypes.string_at(ctypes.pointer(self), ctypes.sizeof(self))[:self.length + 2]
    
    def load(self, data: bytes) -> None:
        if data.__sizeof__ != ctypes.sizeof(self):
            pass
        ctypes.memmove(ctypes.addressof(self), data, ctypes.sizeof(self))
         

class Message(MessageBase):
    
    BUFFER_SIZE = 62
   
    _fields_ = [
        ("command_id", ctypes.c_uint8),
        ("length", ctypes.c_uint8),
        ("payload", ctypes.c_uint8 * BUFFER_SIZE)
    ]

    command_map = {
        0x21: CommandSetPhaseShift,
        0x22: CommandPhaseShifterLatch,
        0x30: CommandConfig,
        0xFF: CommandResponse
    }

    def load_command(self, command: CommandBase) -> None:
        self.command_id = command.command_id
        self.length = command.command_length
        serialized = command.serialize()
        if len(serialized) != command.command_length:
            raise ValueError("Serialized command length does not match command_length")
        if command.command_length > self.BUFFER_SIZE:
            raise ValueError("Command length exceeds buffer size")
        ctypes.memmove(self.payload, serialized, command.command_length)

    def get_command(self) -> CommandBase | None:
        command_class = self.command_map.get(self.command_id, None)
        if command_class is None:
            return None
        command_instance = command_class(self.length)
        command_instance.load(bytes(self.payload[:self.length]))
        return command_instance
    
    def __repr__(self) -> str:
        return f"Message(command_id={self.command_id}, length={self.length}, payload={list(self.payload[:self.length])})"


import unittest

class TestCOMMessage(unittest.TestCase):

    def test_message_serialization(self):
        msg = Message()
        cmd = CommandSetPhaseShift(phase_shifter_count=4)
        cmd.phase_shifters = [0, 1, 2, 3]
        msg.load_command(cmd)
        serialized_msg = msg.encode()
        print("Serialized Message:", list(serialized_msg))

    def test_message_loading(self):
        msg = Message()
        cmd = CommandSetPhaseShift(phase_shifter_count=4)
        cmd.phase_shifters = [0, 1, 2, 3]
        msg.load_command(cmd)
        serialized_msg = msg.encode()
        print("Serialized Message:", list(serialized_msg))
        new_msg = Message()
        new_msg.load(serialized_msg)
        loaded_cmd = new_msg.get_command()
        print("Loaded Command:", loaded_cmd.__class__.__name__, loaded_cmd.__dict__)

if __name__ == "__main__":
    unittest.main()