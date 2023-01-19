# bppy imports
from bppy.b_thread import b_thread
from bppy.b_event import BEvent
from bppy.bprogram import BProgram
from bppy.print_b_program_runner_listener import PrintBProgramRunnerListener
from bppy.simple_event_selection_strategy import SimpleEventSelectionStrategy
from bppy.sync_statement import request, block, waitFor

from env.robotic_navigation import RoboticNavigation
import tensorflow as tf
import numpy as np
import os

def get_action(state, policy):
    #
    softmax_out = policy(state.reshape((1, -1)))
    selected_action = np.random.choice(3, p=softmax_out.numpy()[0])

    #
    deterministic_output = softmax_out.numpy()[0]
    deterministic_action = np.argmax(deterministic_output)

    #
    return selected_action

def main( env, policy_network, iterations=30 ):

	for ep in range(iterations):

		state = env.reset()

		while True:
			action = get_action( state, policy_network )
			state, _, done, info = env.step(action)
			if done: break

@b_thread
def hello():
    yield {request: BEvent("Hello")}

@b_thread
def world():
    while True:
        yield {request: BEvent("World"), block: BEvent("Hello")}

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    initial_list = [hello()]
    b_program = BProgram(bthreads=initial_list,
                            event_selection_strategy=SimpleEventSelectionStrategy(),
                            listener=PrintBProgramRunnerListener())
    b_program.run()

    path = os.getcwd()
    print("Currnet path", path)
    model_path = "models/RUL_s221_r1True_r2True_r5True_cl1-0-5_id221_ep46023_srate89.h5"
    policy_network = tf.keras.models.load_model(model_path)

    try:
        env = RoboticNavigation(editor_run=True, random_seed=0)
        success = main(env, policy_network, iterations=30)
        print(success)

    finally:
        env.close()
