
import phase_shifter_api as ps

if __name__ == "__main__":
    # Create an instance of the PhaseShifter class
    phase_shifter = ps.PhaseShifter(port = "COM25",
                                    baudrate = 115200,
                                    timeout = 1,
                                    phase_shifter_count= 4,
                                    bit_resolution= 4)
    
    phase_shifter.set_latch(False)  # Ensure latch is low before transmitting data

    # Send a list of phase values to the phase shifter
    phase_shifter.set_phase([0x4, 0x8, 0xF, 0xA])

    # Transmit the phase values to the phase shifter over SPI on MCU side
    phase_shifter.transmit()

    # Toggle Latch pin to update the phase shifter with the new values
    phase_shifter.set_latch(True)
    phase_shifter.set_latch(False)

    # Close the connection to the phase shifter
    phase_shifter.close()


