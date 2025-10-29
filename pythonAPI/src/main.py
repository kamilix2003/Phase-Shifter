import asyncio
from beam_controller import BeamController
from COM_message import Message
from commands import *
import cmd


async def main():
    bc: BeamController = BeamController(port="COM14", baudrate=115200, phase_shifter_count=4, phase_shifter_resolution=4)
    await bc.open_with_retry()

    await asyncio.sleep(1)  # Wait for connection to establish

    # Example command usage
    bc.set_phase_shifts([0, 1, 2, 3])

    await asyncio.sleep(1)  # Wait for connection to establish

    try:
        bc.set_phase_shifts([0, 1, 2, 17])
    except ValueError as e:
        print("Error:", e)


if __name__ == "__main__":
    asyncio.run(main())