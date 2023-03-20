from importlib import import_module
from inspect import getmembers, isfunction
from itertools import tee

from z3 import *

class BProgram:

    def __init__(self, bthreads=None, sensors=None, source_name=None, event_selection_strategy=None, listener=None, environment=None,
                 steps_limit=None, events_queue=None):
        self.source_name = source_name
        self.bthreads = bthreads
        self.sensors = sensors
        self.event_selection_strategy = event_selection_strategy
        self.listener = listener
        self.env = environment
        self.events_queue = events_queue
        self.steps_limit = steps_limit
        self.current_number_of_steps = 0
        self.variables = None
        self.tickets = None

    def setup(self):
        if self.source_name:
            self.bthreads = [o[1]() for o in getmembers(import_module(self.source_name)) if
                             isfunction(o[1]) and o[1].__module__ == self.source_name]

            self.variables = dict([o for o in getmembers(import_module(self.source_name)) if
                                   isinstance(o[1], ExprRef) or isinstance(o[1], list)])

        self.tickets = [{'bt': bt} for bt in self.bthreads]
        self.tickets_sensors = [{'bt': sensor} for sensor in self.sensors]
        self.extract_external_event()
        self.advance_sensors(None)
        self.advance_bthreads(None)

    def extract_external_event(self):
        if (self.env.is_current_state_updated()):
            input_state = self.env.get_current_state()
            self.events_queue.append(input_state)

    def sensor_is_not_synchronized(self, ticket):
        if 'not_synced' in ticket:
            return True
        return False

    def external_events_in_queue(self):
        return len(self.events_queue) > 0

    def advance_sensors(self, event):
        # No need to check that event is not blocked -
        # assumption - sensors do not block events, only request
        if(self.sensors):
            for ticket in self.tickets_sensors:
                try:
                    bt = ticket['bt']
                    ll = None
                    if (self.sensor_is_not_synchronized(ticket)):
                            if(self.external_events_in_queue()):
                                ll = bt.send(None)
                    else:
                        if event is None or self.event_selection_strategy.is_satisfied(
                                event, ticket):
                            ll = bt.send(event)
                    if ll is None:
                        continue
                    ticket.clear()
                    ticket.update(ll)
                    ticket.update({'bt': bt})
                except (KeyError, StopIteration):
                    pass

    # TODO: Meaningful names
    def advance_bthreads(self, event):
        for l in self.tickets:
            if event is None or self.event_selection_strategy.is_satisfied(event, l):
                try:
                    bt = l['bt']
                    l.clear()
                    ll = bt.send(event)
                    if ll is None:
                        continue
                    l.update(ll)
                    l.update({'bt': bt})
                except (KeyError, StopIteration):
                    pass

    def unify_tickets(self):
        unified_tickets =  self.tickets + self.tickets_sensors
        return unified_tickets

    def next_event(self):
        tickets = self.unify_tickets()
        return self.event_selection_strategy.select(tickets)

    def reached_number_of_steps_limit(self):
        if(self.steps_limit is None):
            return False
        else:
            ans = self.current_number_of_steps <  self.steps_limit
            return ans

    def run(self):
        if self.listener:
            self.listener.starting(b_program=self)

        print("BProgram.run -> Before setup...")
        self.setup()
        print("BProgram.run -> Finished setup...")
        # Main loop
        interrupted = False
        while not interrupted and not(self.reached_number_of_steps_limit()):
            self.extract_external_event()
            if(self.external_events_in_queue()):
                self.advance_sensors(None)
            event = self.next_event()
            # Finish the program if no event is selected
            if event is None:
                break
            if self.listener:
                interrupted = self.listener.event_selected(b_program=self, event=event)
            self.advance_sensors(event)
            self.advance_bthreads(event)
            self.current_number_of_steps = self.current_number_of_steps + 1

        if self.listener:
            self.listener.ended(b_program=self)
