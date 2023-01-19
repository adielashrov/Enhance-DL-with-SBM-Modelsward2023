from bppy import *
from bppy.b_event import BEvent
from bppy.b_thread import b_thread
from bppy.bprogram import BProgram
from bppy.print_b_program_runner_listener import PrintBProgramRunnerListener
from bppy.simple_event_selection_strategy import SimpleEventSelectionStrategy
from bppy.sync_statement import request

@b_thread
def hello():
    yield {request: BEvent("Hello")}

@b_thread
def world():
    yield {request: BEvent("World")}


if __name__ == "__main__":
    b_program = BProgram(bthreads=[hello(), world()],
                         event_selection_strategy=SimpleEventSelectionStrategy(),
                         listener=PrintBProgramRunnerListener())
    b_program.run()
