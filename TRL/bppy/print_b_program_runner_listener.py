from bppy.b_program_runner_listener import BProgramRunnerListener

class PrintBProgramRunnerListener(BProgramRunnerListener):

    def starting(self, b_program):
        # print("STARTED")
        pass
    def started(self, b_program):
        pass

    def super_step_done(self, b_program):
        pass

    def ended(self, b_program):
        # print("ENDED")
        pass

    def assertion_failed(self, b_program):
        pass

    def b_thread_added(self, b_program):
        pass

    def b_thread_removed(self, b_program):
        pass

    def b_thread_done(self, b_program):
        pass

    def event_selected(self, b_program, event):
        # print(event)
        pass

    def halted(self, b_program):
        pass

    def __init__(self):
        super().__init__()


