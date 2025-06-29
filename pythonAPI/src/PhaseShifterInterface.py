import Command

class PhaseShifterInterface:
    def __init__(self, serial_port):
        self.serial_port = serial_port

    def send_command(self, command):
        buffer = command.buffer()
        self.serial_port.write(buffer)
        
    def receive_command(self):
        response = self.serial_port.read(64)
        if not response:
            raise ValueError("No response received from the device")
        
        response_command = Command.Command(0, bytearray())
        response_command.recieve(response)
        print(f"Received command: {response_command}")
        return response_command
    
    def close(self):
        if self.serial_port.is_open:
            self.serial_port.close()
            print("Serial port closed.")
        else:
            print("Serial port is already closed.")

    def send_buffer(self):
        command = Command.Command(0x10, bytearray())
        self.send_command(command)
        response = self.receive_command()
        if response.command_id == 0xff:
            raise ValueError("Failed to send buffer")

    def set_buffer(self, buffer):
        command = Command.Command(0x11, buffer)
        self.send_command(command)
        response = self.receive_command()
        if response.command_id == 0xff:
            raise ValueError("Failed to set buffer")
        
    def get_buffer(self):
        command = Command.Command(0x12, bytearray())
        self.send_command(command)
        response = self.receive_command()
        if response.command_id == 0xff:
            raise ValueError("Failed to get buffer")
        return response.data
    
    def clear_buffer(self):
        command = Command.Command(0x14, bytearray())
        self.send_command(command)
        response = self.receive_command()
        if response.command_id == 0xff:
            raise ValueError("Failed to clear buffer")
        
    def echo(self, data):
        command = Command.Command(0x01, data)
        self.send_command(command)
        response = self.receive_command()
        if response.command_id == 0xff:
            raise ValueError("Echo command failed")
        return response.data
    
    def auto_latch(self, enable):
        command_data = bytearray([1]) if enable else bytearray([0])
        command = Command.Command(0x04, command_data)
        self.send_command(command)
        response = self.receive_command()
        if response.command_id == 0xff:
            raise ValueError("Failed to set auto latch")
        
    def set_phase_shifter_count(self, count):
        if not (0 <= count <= 16):
            raise ValueError("Phase shifter count must be between 0 and 255")
        command_data = bytearray([count])
        command = Command.Command(0x03, command_data)
        self.send_command(command)
        response = self.receive_command()
        if response.command_id == 0xff:
            raise ValueError("Failed to set phase shifter count")
        
