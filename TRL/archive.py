'''
@b_thread
def Sensor_v2():
    wait_for_external_event_g = wait_for_external_event()
    next(wait_for_external_event_g)  # First time
    while True:
        state = next(wait_for_external_event_g)
        data = {"state": state}
        lastEv = yield {request: BEvent("input_event", data)}
        print(f"Sensor - input_event: {lastEv}")

@b_thread
def Sensor():
    while True:
        if env.is_current_state_updated():
            state = env.get_current_state()
            env.set_state_ready_to_read(False)
            data = {"state": state}
            lastEv = yield {request: BEvent("input_event", data)}
            # print(f"Sensor - last Event: {lastEv}")
        else:
            yield {waitFor:BEvent("env_updated")}
'''

'''
@b_thread
def guard_back_and_forth():
    overrideLeft = False
    overrideRight = False
    # Actions - 0 - forward, 1 - right, 2 - left
    waitforEvList = [BEvent("env_action_proxy",{'action':0}),
                     BEvent("env_action_proxy",{'action':1}),
                     BEvent("env_action_proxy",{'action':2})]
    while True:
        lastEv = yield {waitFor: waitforEvList}
        print(f"guard_back_and_forth lastEv: {lastEv}")
        requested_events = []
        if lastEv.data['action'] == 0: # last event is forward
            overrideLeft = False
            overrideRight = False
            requested_events.append(BEvent("env_action",{'action':0}))
        elif lastEv.data['action'] == 1: # 1 == right
            if overrideRight:
                requested_events.append(BEvent("env_action", {'action': 0}))
                requested_events.append(BEvent("env_action", {'action': 2}))
            else:
                overrideLeft = True
                requested_events.append(BEvent("env_action", {'action': 1}))
        else: # 2 == left
            if overrideLeft:
                requested_events.append(BEvent("env_action", {'action': 0}))
                requested_events.append(BEvent("env_action", {'action': 1}))
            else:
                overrideRight = True
                requested_events.append(BEvent("env_action", {'action': 2}))
        lastEv = yield {request: requested_events}
        if lastEv.data['action'] == 0:  # lastEvent is forward
            overrideLeft = False
            overrideRight = False
        elif lastEv.data['action'] == 1: # lastEvent is right
            overrideLeft = True
        else:                           # lastEvent is left
            overrideRight = True
        requested_events.clear()
'''



'''
@b_thread
def Guard_colliade_into_obstacle_blocking_v2():
    all_output_events = [BEvent("output_event", {'action': 0}),
                         BEvent("output_event", {'action': 1}),
                         BEvent("output_event", {'action': 2})]
    blocked_events = []
    print("Enter guard thread...")
    while True:
        lastEv = yield {waitFor: BEvent("input_event"), block: blocked_events}
        blocked_events.clear()
        state = lastEv.data['state']
        if(is_obstacle_ahead(state)):
            print("guard_colliade_into_obstable: blocking move_forward")
            blocked_events.append(BEvent("output_event", {'action': 0}))

'''