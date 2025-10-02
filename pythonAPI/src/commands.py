import ctypes

class CommandBase:
    pass

class CommandSetPhaseShift(CommandBase):

    def __init__(self, phase_shifter_count: int = 0):
        self.phase_shifter_count: int = phase_shifter_count
        self.phase_shifters: list[int] = [0] * phase_shifter_count
        
    def serialize(self) -> bytes:
        return bytes(self.phase_shifters)
    
    def load(self, data: bytes) -> None:
        if len(data) != self.phase_shifter_count:
            raise ValueError("Data length does not match phase shifter count")
        self.phase_shifters = list(data)


class CommandPhaseShifterLatch(CommandBase):

    def __init__(self):
        self.reserved: int = 0
        self.latch: int = 0

    def serialize(self) -> bytes:
        out = bytes(1)
        out |= (self.latch & 0x01)
        return out
    
    def load(self, data: bytes) -> None:
        if len(data) != 1:
            raise ValueError("Data length must be 1 byte")
        self.latch = data[0] & 0x01

class CommandConfig(CommandBase):

    def __init__(self):
        self.reserved: int = 0
        self.enable: int = 0

    def serialize(self) -> bytes:
        out = bytes()
        out[0] |= (self.reserved & 0x7F) << 1
        out[0] |= (self.enable & 0x01)
        return out

    def load(self, data: bytes) -> None:
        if len(data) != 1:
            raise ValueError("Data length must be 1 byte")
        self.reserved = (data[0] >> 1) & 0x7F
        self.enable = data[0] & 0x01

class CommandResponse(CommandBase):

    def __init__(self):
        self.status: int = 0
        self.error_code: int = 0

    def serialize(self) -> bytes:
        out = bytes(2)
        out[0] = self.status & 0xFF
        out[1] = self.error_code & 0xFF
        return out

    def load(self, data: bytes) -> None:
        if len(data) != 2:
            raise ValueError("Data length must be 2 bytes")
        self.status = data[0]
        self.error_code = data[1]


def test():
    set_phase = CommandSetPhaseShift(4)
    set_phase.phase_shifters = [10, 20, 30, 40]
    data = set_phase.serialize()
    print(data)
    set_phase_loaded = CommandSetPhaseShift(4)
    set_phase_loaded.load(data)
    print(set_phase_loaded.phase_shifters)

    latch = CommandPhaseShifterLatch()
    latch.latch = 1
    data = latch.serialize()
    print(data)
    latch_loaded = CommandPhaseShifterLatch()
    latch_loaded.load(data)
    print(latch_loaded.latch)



if __name__ == "__main__":
    test()