# bppy imports
import os
import logging
from bppy.b_thread import b_thread, wait_for_external_event
from bppy.b_event import BEvent
from bppy.bprogram import BProgram
from bppy.print_b_program_runner_listener import PrintBProgramRunnerListener
from bppy.simple_event_selection_strategy import SimpleEventSelectionStrategy
from bppy.odnn_event_selection_strategy import OdnnEventSelectionStrategy
from bppy.sync_statement import request, block, waitFor, not_synced
from env.robotic_navigation import RoboticNavigation
import tensorflow as tf
import numpy as np
import csv
from datetime import datetime

# Fixate the random seed
np.random.seed(0)

# Global variables
PROXY_PATTERN = True
upper_step_limit = 1500
iterations=30
FWD_DIR = 0.5
FWD_DIR_TOLERANCE = 0.1
MINIMAL_CLEARANCE = 0.30
MINIMAL_FWD_CLEARANCE = 0.35

env = None
policy_network = None

# Statistics global variables.
num_of_experiments = 100
status = ""
global_model_name = ""
global_override_enabled = False
num_of_steps = 0
num_of_steps_with_override = 0
num_of_overrides = 0
num_of_collisions = 0
num_of_collisions_with_override = 0

# Dictionary from model name to statistics
stats_per_model = {}

def get_action(state, policy):
    #
    softmax_out = policy(state.reshape((1, -1)))
    selected_action = np.random.choice(3, p=softmax_out.numpy()[0])

    #
    deterministic_output = softmax_out.numpy()[0]
    deterministic_action = np.argmax(deterministic_output)

    #
    #return selected_action
    return deterministic_action

def get_all_possible_output_events(state, policy,proxy=False):
    output_events = []
    softmax_out = policy(state.reshape((1, -1)))
    all_possible_scores = softmax_out.numpy()[0]
    output_event_type = "output_event_proxy" if proxy else "output_event"
    for index, score in enumerate(all_possible_scores):
        data = {"action": index, "score": all_possible_scores[index]}
        output_events.append(BEvent(output_event_type, data))
    return output_events

def get_determinstic_output_event(state, policy):
    softmax_out = policy(state.reshape((1, -1)))
    deterministic_output = softmax_out.numpy()[0]
    deterministic_action = np.argmax(deterministic_output)
    data = {"action": deterministic_action,"score": max(deterministic_output) }
    return BEvent("output_event", data)

def get_random_output_event(state, policy):
    softmax_out = policy(state.reshape((1, -1)))
    random_output = np.random.choice(3, p=softmax_out.numpy()[0])
    random_output_score = softmax_out.numpy()[0][random_output]
    data = {"action": random_output,"score": random_output_score }
    return BEvent("output_event", data)

def get_all_output_events_from_proxy():
    pass

def get_direction_from_event(BEvent):
    action = BEvent.data['action']
    if action == 0:
        return "Forward"
    elif action == 1:
        return "Left"
    elif action == 2:
        return "Right"
    else:
        return "Direction unknown"

def main( env, policy_network, iterations=30 ):
    for ep in range(iterations):
        state = env.reset()
        while True:
            action = get_action( state, policy_network )
            state, _, done, info = env.step(action)
            if done:
                print("Solved the maze! episode:", ep)
                break

@b_thread
def Actuator():
    global num_of_steps
    global status
    num_of_steps = 0
    status = ""
    # Actions - 0 - forward, 1 - right, 2 - left
    all_output_events = [BEvent("output_event",{'action':0}),
                          BEvent("output_event",{'action':1}),
                          BEvent("output_event",{'action':2})]
    while True:
        output_event = yield {waitFor: all_output_events}
        action = output_event.data['action']
        t_state, _, done, info = env.step(action)
        num_of_steps = num_of_steps + 1
        if done:
            print("Done, no more actions to perform on the environment")
            if info["time_out"]:
                status = "time_out"
                print(f'Timeout, num_of_steps: {num_of_steps}')
            elif info["goal_reached"]:
                status = "goal_reached"
                print(f'Solved the maze, num_of_steps: {num_of_steps}')
            elif info["collision"]:
                status = "collision"
                print(f'Collided into obstacle, num_of_steps: {num_of_steps}')
            else:
                status = "unknown"
                print(f'Unknown termination, num_of_steps: {num_of_steps}')
            break

@b_thread
def ODNN_no_proxy():
    while True:
        lastEv = yield {waitFor: BEvent("input_event")}
        state = lastEv.data['state']
        all_possible_output_events = get_all_possible_output_events(state, policy_network)
        lastEv = yield {request: all_possible_output_events}

@b_thread
def ODNN_with_proxy():
    while True:
        lastEv = yield {waitFor: BEvent("input_event")}
        state = lastEv.data['state']
        all_possible_output_events = get_all_possible_output_events(state, policy_network,proxy=True)
        lastEv = yield {request: all_possible_output_events}

def Sensor_v3(event_stack):
    while True:
        if(len(event_stack) > 0):
            input_event = event_stack.pop()
            data = {"state": input_event}
            lastEv = yield {request: BEvent("input_event", data)}
            # print(f"Sensor requested: {lastEv}")
        else:
            yield {not_synced: 'Not_synced'}

def request_same_output_event(outputEventProxy, requested_events):
    t_action = outputEventProxy.data['action']
    requested_events.append(
        BEvent("output_event", {'action': t_action}))

def Guard_avoid_turning_when_clear(override_enabled=False):
    global num_of_overrides
    requested_events = []
    waitforEvList = [BEvent("output_event_proxy", {'action': 0}), # 0 - forward
                     BEvent("output_event_proxy", {'action': 1}), # 1 - left
                     BEvent("output_event_proxy", {'action': 2})] # 2 - right
    while True:
        inputEvent = yield {waitFor: BEvent("input_event")}
        state = inputEvent.data['state']
        outputEventProxy = yield {waitFor: waitforEvList}
        t_dir = state[-2]
        # ANY EVENT[front lidar is clear] and ANY EVENT[target diretction is straight ahead]
        if(override_enabled and (state[3] > MINIMAL_FWD_CLEARANCE and state[2] > MINIMAL_CLEARANCE and
            state[4] > MINIMAL_CLEARANCE) and (abs(FWD_DIR - t_dir) < FWD_DIR_TOLERANCE)):
            if((outputEventProxy.data['action'] == 1) or (outputEventProxy.data['action'] == 2)):
                # print(f"Guard_avoid_turning_when_clear -> overriding {get_direction_from_event(outputEventProxy)}")
                num_of_overrides = num_of_overrides + 1
                requested_events.append(BEvent("output_event", {'action': 0}))
            else:
                request_same_output_event(outputEventProxy, requested_events)
        else:
            request_same_output_event(outputEventProxy, requested_events)
        lastEv = yield {request: requested_events}
        requested_events.clear()

def is_obstacle_ahead(state):
    ans = False
    distance_from_obstacle = 0.22 # lower bound - 0.185 - not enough - 0.3 too large
    if(state[3] <= distance_from_obstacle or
            state[2] <= distance_from_obstacle or state[4] <= distance_from_obstacle):
        print(f"Found obstacle near - front, left, right...{state}")
        ans = True
    # elif(state[3] < 0.3 and state[4] < 0.3):
    #   ans = True
    # elif(state[2] < 0.3 or state[4] < 0.3):
    #   ans = True
    return ans

def reverse_output_event_direction(output_event):
    if(get_direction_from_event(output_event) == "Left"):
        return BEvent("output_event_proxy", {'action': 2})
    elif(get_direction_from_event(output_event) == "Right"):
        return BEvent("output_event_proxy", {'action': 1})
    else:
        print("Tried to reverse direction for Forward event")

def Guard_take_conservative_action(odnnEventSelectionStrategy, override_enabled=True):
    global num_of_overrides
    all_output_events = [BEvent("output_event_proxy", {'action': 0}),
                         BEvent("output_event_proxy", {'action': 1}),
                         BEvent("output_event_proxy", {'action': 2})]
    requested_events = []
    while True:
        t_action = None
        lastInputEvent = yield {waitFor: BEvent("input_event")}
        state = lastInputEvent.data['state']
        outputEventProxy = yield {waitFor: all_output_events}
        if override_enabled:
            if outputEventProxy.get_output_event_score() < 0.2:
                all_possible_output_events = odnnEventSelectionStrategy.get_current_possible_output_events_and_scores()
                next_output_event = odnnEventSelectionStrategy.get_next_output_event(outputEventProxy)
                # print( f"The overriden event: {next_output_event}" )
                num_of_overrides = num_of_overrides + 1
                t_action = next_output_event.data['action']
            else:
                t_action = outputEventProxy.data['action']
        else:
            t_action = outputEventProxy.data['action']

        requested_events.append(
            BEvent("output_event", {'action': t_action}))
        lastEv = yield {request: requested_events}
        # print(f"after request in Guard_take_conservative_action: {lastEv}")
        requested_events.clear()


def Guard_take_conservative_action_2(override_enabled=True):
    global num_of_overrides
    all_output_events = [BEvent("output_event_proxy", {'action': 0}),
                         BEvent("output_event_proxy", {'action': 1}),
                         BEvent("output_event_proxy", {'action': 2})]
    while True:
        lastInputEvent = yield {waitFor: BEvent("input_event")}
        # print(f"Guard_0, {lastInputEvent}")
        outputEventProxy = yield {waitFor: all_output_events}
        # print(f"Guard_0, {outputEventProxy}")
        if override_enabled:
            while outputEventProxy.get_output_event_score() < 0.2:
                # print(f"Guard re-evaluating outputEventProxy, "\
                #      f"{outputEventProxy.get_output_event_score}")
                yield {request: lastInputEvent}
                # print(f"Guard_loop, {i} {lastInputEvent}")
                outputEventProxy = yield {waitFor: all_output_events}
                # print(f"Guard_loop, {i} {outputEventProxy}")
                num_of_overrides = num_of_overrides + 1
        t_action = outputEventProxy.data['action']
        last_output_event = yield {request: BEvent("output_event", {'action': t_action})}
        # print(f"Guard_1, actual output event., {last_output_event}")

def Guard_colliade_into_obstacle_blocking_with_proxy(odnnEventSelectionStrategy, override_enabled=True):
    global num_of_overrides
    all_output_events = [BEvent("output_event_proxy", {'action': 0}),
                         BEvent("output_event_proxy", {'action': 1}),
                         BEvent("output_event_proxy", {'action': 2})]
    requested_events = []
    while True:
        lastInputEvent = yield {waitFor: BEvent("input_event")}
        state = lastInputEvent.data['state']
        outputEventProxy = yield {waitFor: all_output_events}
        if(override_enabled and is_obstacle_ahead(state)):
            # print("guard_colliade_into_obstable: obstacle_ahead")
            if (outputEventProxy.data['action'] == 0): # we are moving forward
                print("guard_colliade_into_obstable: overriding move_forward")
                outputEvent = odnnEventSelectionStrategy.get_next_output_event(outputEventProxy)
                if(outputEvent.get_output_event_score() < 0.3):
                    print(f"Reverse direction: {get_direction_from_event(outputEvent)}"
                          f", {outputEvent.get_output_event_score}")
                    outputEvent = reverse_output_event_direction(outputEvent)

                requested_events.append(BEvent("output_event", {'action': outputEvent.data['action']}))
                num_of_overrides = num_of_overrides + 1
            else:
                requested_events.append(
                    BEvent("output_event", {'action': outputEventProxy.data['action']}))
        else:
            t_action = outputEventProxy.data['action']
            requested_events.append(BEvent("output_event", {'action': t_action}))

        lastEv = yield {request: requested_events}
        # print(f"after request in Guard_colliade_into_obstacle_blocking_with_proxy: {lastEv}")
        requested_events.clear()

@b_thread
def Guard_colliade_into_obstacle_blocking():
    all_output_events = [BEvent("output_event", {'action': 0}),
                         BEvent("output_event", {'action': 1}),
                         BEvent("output_event", {'action': 2})]
    blocked_events = []
    while True:
        lastEv = yield {waitFor: BEvent("input_event")}
        state = lastEv.data['state']
        blockForward = False
        if(is_obstacle_ahead(state)):
            print("guard_colliade_into_obstable: blocking move_forward")
            blocked_events.append(BEvent("output_event", {'action': 0}))
            blockForward = True
        lastEv = yield {waitFor: all_output_events, block: blocked_events} # pattern here is similar to EnsureJobExits
        blocked_events.clear()

@b_thread
def Guard_as_channel_for_proxy():
    waitforEvList = [BEvent("output_event_proxy", {'action': 0}),
                     BEvent("output_event_proxy", {'action': 1}),
                     BEvent("output_event_proxy", {'action': 2})]
    requested_events = []
    print("Enter Guard_as_channel_for_proxy")
    while True:
        lastEv = yield {waitFor: waitforEvList}
        print(f"Guard_as_channel_for_proxy: lastEv{lastEv}")
        t_action = lastEv.data['action']
        requested_events.append(BEvent("output_event", {'action': t_action}))
        lastEv = yield {request: requested_events}
        requested_events.clear()

# Raz comment
@b_thread
def Guard_colliade_into_obstacle():
    waitforEvList = [BEvent("output_event",{'action':0}),
                     BEvent("output_event",{'action':1}),
                     BEvent("output_event",{'action':2})]
    requested_events = []
    while True:
        lastEv = yield {waitFor: BEvent("input_event")}
        state = lastEv.data['state']
        blockForward = False
        if(state[3] < 0.3 and state[4] < 0.3):
            blockForward = True
        lastEv = yield {waitFor: waitforEvList}
        if blockForward:
            if lastEv.data['action'] == 0:  # last event is forward
                requested_events.append(BEvent("env_action", {'action': 2})) # turn left
                requested_events.append(BEvent("env_action", {'action': 1}))  # turn right
            else:
                t_action = lastEv.data['action']
                requested_events.append(BEvent("env_action", {'action': t_action}))
        else:
            t_action = lastEv.data['action']
            requested_events.append(BEvent("env_action", {'action': t_action}))
        lastEv = yield {request: requested_events}
        requested_events.clear()


#TODO: Reduce to one variable only.
def load_policy(model_choice=1, model_path=""):
    if (not model_path):
        if (model_choice==1): # Semi Simple model - collides into walls
            model_path = "models/STD_id486_ep1047_no_rules_srate43.h5"
        elif (model_choice==2): # Confused model
            model_path = "models/RUL_s191_r1False_r2False_r5False_cl1_id191_ep2001_srate63.h5"
        else: # default value - focused model
            model_path = "models/RUL_s221_r1True_r2True_r5True_cl1-0-5_id221_ep46023_srate89.h5"
    policy_network = tf.keras.models.load_model(model_path)
    return policy_network

def open_csv_file(models_folder):
    # datetime object containing current date and time
    now = datetime.now()
    dt_string = now.strftime("%d_%m_%Y_%H_%M_%S")
    file_name = models_folder + '_' + dt_string + '.csv'
    file = open(file_name, 'w', newline='')
    return file

def write_statistics(file_writer, model_name):
    row_as_array = []
    row_as_array.append(model_name)
    row_as_array.append(num_of_experiments)
    row_as_array.append(stats_per_model[model_name]["time_out"])
    row_as_array.append(stats_per_model[model_name]["time_out_with_override"])
    row_as_array.append(stats_per_model[model_name]["num_of_solved"])
    row_as_array.append(stats_per_model[model_name]["num_of_steps"])
    row_as_array.append(stats_per_model[model_name]["num_of_solved_with_override"])
    row_as_array.append(stats_per_model[model_name]["num_of_steps_with_override"])
    row_as_array.append(stats_per_model[model_name]["num_of_overrides"])
    row_as_array.append(stats_per_model[model_name]["num_of_collisions"])
    row_as_array.append(stats_per_model[model_name]["num_of_collisions_with_override"])
    row_as_array.append(stats_per_model[model_name]["unknown_termination"])
    row_as_array.append(stats_per_model[model_name]["unknown_termination_with_override"])
    row_as_array.append(stats_per_model[model_name]["avg_num_steps_to_solve"])
    row_as_array.append(stats_per_model[model_name]["avg_num_steps_to_solve_with_override"])

    file_writer.writerow(row_as_array)

def extract_success_rate(model_name):
    success_rate_index = model_name.index('srate')
    success_rate = model_name[success_rate_index+5: success_rate_index+7]
    return success_rate

def init_override_enabled_conf(range_size):
    override_enabled_conf = [False for i in range(10)]
    override_enabled_conf.extend([True for i in range(range_size)])
    return override_enabled_conf

def update_dictionary( model_name, field_name, inc_value=1 ):
    if (stats_per_model[model_name].get(field_name) is None):
        stats_per_model[model_name][field_name] = 0
    stats_per_model[model_name][field_name] = \
        stats_per_model[model_name][field_name] + inc_value

def init_statistics( model_name ):
    global num_of_steps
    global status
    global global_model_name
    global global_override_enabled
    global num_of_steps_with_override
    global num_of_overrides
    global num_of_collisions
    global num_of_collisions_with_override
    global stats_per_model
    status = ""
    global_model_name = model_name
    global_override_enabled = False
    num_of_steps = 0
    num_of_steps_with_override = 0
    num_of_overrides = 0
    num_of_collisions = 0
    num_of_collisions_with_override = 0
    stats_per_model[model_name] = {
        "num_of_experiments": num_of_experiments}
    stats_per_model[model_name]["time_out"] = 0
    stats_per_model[model_name]["time_out_with_override"] = 0
    stats_per_model[model_name]["num_of_solved"] = 0
    stats_per_model[model_name]["num_of_steps"] = 0
    stats_per_model[model_name]["num_of_solved_with_override"] = 0
    stats_per_model[model_name]["num_of_steps_with_override"] = 0
    stats_per_model[model_name]["num_of_overrides"] = 0
    stats_per_model[model_name]["num_of_collisions"] = 0
    stats_per_model[model_name]["num_of_collisions_with_override"] = 0
    stats_per_model[model_name]["unknown_termination"] = 0
    stats_per_model[model_name]["unknown_termination_with_override"] = 0
    stats_per_model[model_name]["avg_num_steps_to_solve"] = 0
    stats_per_model[model_name]["avg_num_steps_to_solve_with_override"] = 0

def log_statistics():
    if status == "time_out":
        if not global_override_enabled:
            update_dictionary(global_model_name, "time_out")
        else:
            update_dictionary(global_model_name, "time_out_with_override")
    elif status == "goal_reached":
        if not global_override_enabled:
            update_dictionary(global_model_name, "num_of_solved")
            update_dictionary(global_model_name, "num_of_steps", num_of_steps)
        else:
            update_dictionary(global_model_name, "num_of_solved_with_override")
            update_dictionary(global_model_name, "num_of_steps_with_override", num_of_steps)
            update_dictionary(global_model_name, "num_of_overrides",
                              num_of_overrides)
    elif status == "collision":
        if not global_override_enabled:
            update_dictionary(global_model_name, "num_of_collisions")
        else:
            update_dictionary(global_model_name, "num_of_collisions_with_override")
    else:
        if not global_override_enabled:
            update_dictionary(global_model_name, "unknown_termination")
        else:
            update_dictionary(global_model_name, "unknown_termination_with_override")

def summarize_stats( model_name ):
    avg_num_steps_to_solve = 0
    avg_num_steps_to_solve_with_override = 0
    if(stats_per_model[model_name].get(
        "num_of_steps") is not None and stats_per_model[model_name].get("num_of_solved") is not None):
        if(stats_per_model[model_name].get("num_of_solved") != 0):
            stats_per_model[model_name]["avg_num_steps_to_solve"] = stats_per_model[model_name].get("num_of_steps") \
                                     / stats_per_model[model_name].get("num_of_solved")
    if( stats_per_model[model_name].get("num_of_steps_with_override") is not None and \
            stats_per_model[model_name].get("num_of_solved_with_override") is not None):
        if(stats_per_model[model_name].get("num_of_solved_with_override") != 0):
            stats_per_model[model_name][
                "avg_num_steps_to_solve_with_override"] = stats_per_model[model_name].get(
            "num_of_steps_with_override") / stats_per_model[model_name].get("num_of_solved_with_override")


if __name__ == "__main__":
    try:
        # models_dir_path = "models/models_rule_5"
        # models_dir_path = "models/local_models"
        # models_dir_path = "models/models_total_batch_1/RUL_s18_r1False_r2False_r5False_cl1_20220603_192745/models"
        # models_dir_path = "models/gradual_models_no_rules"
        models_dir_path = "models/test_models"

        csv_file = open_csv_file(models_dir_path)
        file_writer = csv.writer(csv_file)
        headers = ["model", "num_of_experiments", "time_out",
                   "time_out_with_override", "num_of_solved",
                   "num_of_steps", "num_of_solved_with_override",
                   "num_of_steps_with_override", "num_of_overrides",
                   "num_of_collisions", "num_of_collisions_with_override",
                   "unknown_termination", "unknown_termination_with_override",
                   "avg_num_steps_to_solve", "avg_num_steps_to_solve_with_override"]
        file_writer.writerow(headers)

        override_enabled_conf = [False, True]
        #env = RoboticNavigation(step_limit=upper_step_limit, editor_run=False,
        #                        random_seed=0)
        env = RoboticNavigation(step_limit=upper_step_limit, editor_run=True,
                                random_seed=0)
        events_q = []
        initial_list = []
        odnnEventSelectionStrategy = OdnnEventSelectionStrategy()
        Sensor_v3 = b_thread(Sensor_v3, events_queue=events_q)
        # Initialize generator
        Guard_take_conservative_action = b_thread(Guard_take_conservative_action, deep_c=False)

        Guard_take_conservative_action_2 = b_thread(
            Guard_take_conservative_action_2, deep_c=False)

        list_of_models = os.listdir(models_dir_path)


        for model_name in list_of_models:
            init_statistics( model_name=model_name )

            for override_enabled in override_enabled_conf:
                global_override_enabled = override_enabled
                for i in range(num_of_experiments):
                    num_of_overrides = 0
                    events_q.clear()
                    initial_list.clear()
                    sensor_thread = Sensor_v3()
                    actuator_thread = Actuator()
                    # guard_take_conservative_action - v1
                    guard_take_conservative_action = Guard_take_conservative_action(odnnEventSelectionStrategy, override_enabled)

                    # guard_take_conservative_action - v2
                    # guard_take_conservative_action = Guard_take_conservative_action_2(override_enabled)
                    odnn_thread = ODNN_with_proxy()
                    initial_list = [actuator_thread, odnn_thread, guard_take_conservative_action]
                    sensors_list = [sensor_thread]
                    model_path = models_dir_path + "/" + model_name
                    if os.path.isfile(model_path):
                        env.reset()
                        policy_network = load_policy(model_path=model_path)
                        print(f"Model: {model_name}\n",
                              f"override_enabled:{override_enabled}")
                        b_program = BProgram(bthreads=initial_list, sensors=sensors_list,
                                             event_selection_strategy=odnnEventSelectionStrategy,
                                             listener=PrintBProgramRunnerListener(), environment=env, events_queue=events_q)
                        b_program.run()
                        log_statistics()


            summarize_stats(model_name=model_name)
            write_statistics(file_writer, model_name)
    finally:
        csv_file.close()
        env.close()
