import ctypes

class CommandBase:
    def __init__(self, command_id: int = 0xff, command_length: int = 0):
        self.command_id: int = command_id
        self.command_length: int = command_length

    def serialize(self) -> bytes:
        raise NotImplementedError("Serialize method must be implemented in subclasses")
    
    def load(self, data: bytes) -> None:
        raise NotImplementedError("Load method must be implemented in subclasses")

class CommandSetPhaseShift(CommandBase):

    def __init__(self, phase_shifter_count: int = 0):
        super().__init__(command_id=0x21, command_length=phase_shifter_count)
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
        super().__init__(command_id=0x22, command_length=1)
        self.reserved: int = 0

    def serialize(self) -> bytes:
        out = bytes(1)
        return out
    
    def load(self, data: bytes) -> None:
        if len(data) != 1:
            raise ValueError("Data length must be 1 byte")

class CommandConfig(CommandBase):

    def __init__(self):
        super().__init__(command_id=0x30, command_length=1)
        self.reserved: int = 0
        self.enable_auto_latch: int = 0

    def serialize(self) -> bytes:
        out: list[int] = list(bytes(1))
        out[0] |= (self.reserved & 0x7F) << 1
        out[0] |= (self.enable_auto_latch & 0x01)
        return bytes(out)

    def load(self, data: bytes) -> None:
        if len(data) != 1:
            raise ValueError("Data length must be 1 byte")
        temp = list(data)
        self.reserved = (temp[0] >> 1) & 0x7F
        self.enable_auto_latch = temp[0] & 0x01

class CommandResponse(CommandBase):

    def __init__(self):
        super().__init__(command_id=0xFF, command_length=2)
        self.status: int = 0
        self.error_code: int = 0

    def serialize(self) -> bytes:
        out: list[int] = list(bytes(2))
        out[0] = self.status & 0xFF
        out[1] = self.error_code & 0xFF
        return bytes(out)

    def load(self, data: bytes) -> None:
        if len(data) != 2:
            raise ValueError("Data length must be 2 bytes")
        temp = list(data)
        self.status = temp[0]
        self.error_code = temp[1]


def test():
    set_phase = CommandSetPhaseShift(4)
    set_phase.phase_shifters = [10, 20, 30, 40]
    data = set_phase.serialize()
    print(data)
    set_phase_loaded = CommandSetPhaseShift(4)
    set_phase_loaded.load(data)
    print(set_phase_loaded.phase_shifters)

    latch = CommandPhaseShifterLatch()
    data = latch.serialize()
    print(data)
    latch_loaded = CommandPhaseShifterLatch()
    latch_loaded.load(data)

    config = CommandConfig()
    config.enable_auto_latch = 1
    data = config.serialize()
    print(data)
    config_loaded = CommandConfig()
    config_loaded.load(data)
    print(config_loaded.enable_auto_latch)

    response = CommandResponse()
    response.status = 1
    response.error_code = 2
    data = response.serialize()
    print(data)
    response_loaded = CommandResponse()
    response_loaded.load(data)
    print(response_loaded.status, response_loaded.error_code)



if __name__ == "__main__":
    test()