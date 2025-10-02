import ctypes
from commands import *

class MessageBase(ctypes.Structure):

    def serialize(self) -> bytes:
        return ctypes.string_at(ctypes.pointer(self), ctypes.sizeof(self))
    
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

    
def test():
    msg = Message()
    
    msg.command_id = 1
    msg.length = 4
    cmd = CommandSetPhaseShift(4)
    cmd.phase_shifters = [1, 2, 3, 4]
    msg.payload = ctypes.create_string_buffer(cmd.serialize(), Message.BUFFER_SIZE)

    data = msg.serialize()
    print(data)        

if __name__ == "__main__":
    test()