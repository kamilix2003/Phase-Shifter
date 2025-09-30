
from collections.abc import Iterable
import ctypes

class message_base(ctypes.Structure):

    def serialize(self) -> bytes:
        return ctypes.string_at(ctypes.pointer(self), ctypes.sizeof(self))
    
    def load(self, data: bytes) -> None:
        if data.__sizeof__ != ctypes.sizeof(self):
            pass
        ctypes.memmove(ctypes.addressof(self), data, ctypes.sizeof(self))
    
    def __iter__(self):
        for field_name, _ in self._fields_: # type: ignore
            value = getattr(self, field_name)
            # Convert arrays to list for readability
            if isinstance(value, ctypes.Array):
                if field_name == "payload":
                    yield (field_name, list(value[:self.length]))
            else:
                yield (field_name, value)

    def __repr__(self):
        fields = ", ".join(f"{k}={v}" for k, v in self)
        return f"{self.__class__.__name__}({fields})"           

class command_set_phase_shift(message_base):
    COMMAND_ID = 0x01
    MAX_PHASE_SHIFTER_COUNT = 32
    
    def __init__(self, phase_shifter_count: int = 1):
        self.phase_shifter_count = phase_shifter_count
    
    _fields_ = [
        ("phase_shifter_values", ctypes.c_uint8 * MAX_PHASE_SHIFTER_COUNT)    
    ]
    

class command_phase_shifter_latch:
    COMMAND_ID = 0x02

    def __init__(self):
        pass

class command_config:
    COMMAND_ID = 0x03

    def __init__(self):
        pass

class command_response:
    COMMAND_ID = 0xff

    def __init__(self):
        pass


class message(message_base):
    
    BUFFER_SIZE = 62
    
    command_id_map = {
        0x01: command_set_phase_shift,
        0x02: command_phase_shifter_latch,
        0x03: command_config,
        0xff: command_response        
    }
    
    _fields_ = [
        ("command_id", ctypes.c_uint8),
        ("length", ctypes.c_uint8),
        ("payload", ctypes.c_uint8 * BUFFER_SIZE)
    ]
    
    def get_payload(self) -> bytes:
        return self.payload[:self.length]
    
def test():
    msg = message()
    
    msg.command_id = 1
    msg.length = 4
    msg.payload[0] = 3
    
    data = msg.serialize()
    msg.load(data)
    for name, value in msg:
        print(f"{name}: {value}")
        
    msg.get_payload()

    
if __name__ == "__main__":
    test()