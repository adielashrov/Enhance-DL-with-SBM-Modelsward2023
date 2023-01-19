from copy import deepcopy
from copy import copy

def wait_for_external_event():
    print("Before yield...wait_for_external_event_g")
    external_event = yield "external_event"
    print("After yield...wait_for_external_event_g")
    return external_event

def wait_for_external_event_wrapper():
    # g = wait_for_external_event_g()
    # val = next(g)
    # return val
    pass

'''
# func - bThread code - generator - must contain yield
# 
'''
def b_thread(func, events_queue=None, deep_c=True):
    # What is *args? - empty tuple from where?
    def wrapper(*args):
        while True:
            selected_event = None
            if(deep_c):
                args_copy = deepcopy(args)
            else:
                args_copy = copy(args)

            if(events_queue is None):
                f = func(*args_copy)
            else: # sensor case
                f = func(*args_copy, events_queue)
            while True:
                try:
                    statement = f.send(selected_event)
                    selected_event = yield statement
                    if selected_event is None:
                        break
                except (KeyError, StopIteration):
                    selected_event = yield None
                    break
    return wrapper

