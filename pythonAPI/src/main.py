import cmd
from beam_controller import BeamController
from COM_message import Message
from commands import *
from time import sleep
import asyncio

class BeamCLI(cmd.Cmd):
    intro = "Welcome to the Beam Controller CLI. Type help or ? to list commands.\n"
    prompt = "(beam) "

    def __init__(self, beam_controller: BeamController):
        super().__init__()
        self.beam_controller = beam_controller
        self.loop = asyncio.new_event_loop()
        self.listen_task = asyncio.create_task(self.beam_controller.listen_on_port())

    def do_set_phase(self, arg):
        "Set phase shifters: set_phase <phase1> <phase2> ..."
        phases = list(map(int, arg.split()))
        cmd = CommandSetPhaseShift(phase_shifter_count=len(phases))
        cmd.phase_shifters = phases
        msg = Message()
        msg.load_command(cmd)
        self.beam_controller.send_command(msg)

    # def do_get_status(self, arg):
    #     "Get status of the beam controller"
    #     cmd = CommandGetStatus()
    #     msg = Message()
    #     msg.load_command(cmd)
    #     self.beam_controller.send_command(msg)

    def do_exit(self, arg):
        "Exit the CLI"
        print("Exiting...")
        self.listen_task.cancel()
        self.beam_controller.close()
        return True

    def do_EOF(self, arg):
        return self.do_exit(arg)

def main():
    bc: BeamController = BeamController(port="COM10")
    cli = BeamCLI(bc)
    cli.cmdloop()


if __name__ == "__main__":
    main()