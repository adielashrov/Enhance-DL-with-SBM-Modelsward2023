from bppy.event_selection_strategy import EventSelectionStrategy
from bppy.b_event import BEvent
from bppy.event_set import EmptyEventSet
import random
from collections.abc import Iterable
import numpy as np
import copy

class OdnnEventSelectionStrategy(EventSelectionStrategy):

    def __init__(self):
        self.current_possible_output_events = []
        self.current_possible_output_events_scores = []

    def is_satisfied(self, event, statement):
        if isinstance(statement.get('request'), BEvent):
            if isinstance(statement.get('waitFor'), BEvent):
                return statement.get('request') == event or statement.get('waitFor') == event
            else:
                return statement.get('request') == event or statement.get('waitFor', EmptyEventSet()).__contains__(event)
        else:
            if isinstance(statement.get('waitFor'), BEvent):
                return statement.get('request', EmptyEventSet()).__contains__(event) or statement.get('waitFor') == event
            else:
                return statement.get('request', EmptyEventSet()).__contains__(event) or statement.get('waitFor', EmptyEventSet()).__contains__(event)

    def selectable_events(self, statements):
        possible_events = set()
        for statement in statements:
            if 'request' in statement:  # should be eligible for sets
                if isinstance(statement['request'], Iterable):
                    possible_events.update(statement['request'])
                elif isinstance(statement['request'], BEvent):
                    possible_events.add(statement['request'])
                else:
                    raise TypeError("request parameter should be BEvent or iterable")
        # for statement in statements:
        #     if 'block' in statement:
        #        if isinstance(statement.get('block'), BEvent):
        #            possible_events.discard(statement.get('block'))
        #        else:
        #            possible_events = {x for x in possible_events if x not in statement.get('block')}
        return possible_events

    def selected_event_is_blocked(self, selected_event, statements):
        for statement in statements:
            if 'block' in statement:
                if isinstance(statement.get('block'), BEvent):
                    if (selected_event == statement.get('block')):
                        return True
                else:
                    selected_event_is_blocked =  selected_event in statement.get('block')
                    return selected_event_is_blocked
        return False

    def keep_output_event_with_max_score(self, selectable_events):
        t_selectable_events = []
        max_score_output_event = None
        for possible_event in selectable_events:
            if(possible_event.is_output_event()):
                if((max_score_output_event is None) or
                        max_score_output_event.data['score'] < possible_event.data['score']):
                    max_score_output_event = possible_event
            else:
                t_selectable_events.append(possible_event)
        if max_score_output_event:
            t_selectable_events.append(max_score_output_event)
        return t_selectable_events

    # TODO: handle case when there are no more events - all events are blocked
    # TODO: Solution is not efficient - we sort every time
    def select_next_event(self, selectable_events, statements):
        t_selected_event = None
        t_selectable_events = []
        t_possible_output_events = []
        t_num_of_trials = 0
        for possible_event in selectable_events:
            if(possible_event.is_output_event()):
                t_possible_output_events.append(possible_event)
            else:
                t_selectable_events.append(possible_event)
        while (not t_selected_event):
            if(t_possible_output_events):
                t_possible_output_events.sort(
                    key=lambda output_event: output_event.get_output_event_score())
                t_possible_output_events_scores = [
                    output_event.get_output_event_score() for output_event in
                    t_possible_output_events]
                t_possible_output_events_scores /= np.array(t_possible_output_events_scores).sum() # override on known bug regarding precision
                # save output events and scores for re-draw if required.
                self.current_possible_output_events = t_possible_output_events
                self.current_possible_output_events_scores = t_possible_output_events_scores
                t_selected_event = np.random.choice(t_possible_output_events, p=t_possible_output_events_scores)
            else:
                t_selected_event = np.random.choice(t_selectable_events)
            if (self.selected_event_is_blocked(t_selected_event, statements)):
                # print(f"Selected event is blocked:{t_selected_event}")
                t_selected_event = None
                t_num_of_trials = t_num_of_trials + 1
        return t_selected_event

    def verify_there_are_output_events_to_select(self, override_event):
        if(len(self.current_possible_output_events) == 0):
            return False
        elif(len(self.current_possible_output_events) == 1 and self.current_possible_output_events.__contains__(override_event)):
            return False
        else:
            return True

    def get_current_possible_output_events_and_scores(self):
        events_copy = copy.deepcopy(self.current_possible_output_events)
        return events_copy

    def get_next_output_event(self, override_event):
        t_selected_event = None
        t_num_of_trials = 0
        if (self.verify_there_are_output_events_to_select(override_event)):
            while(not t_selected_event):
                t_selected_event = np.random.choice(self.current_possible_output_events,
                                                p=self.current_possible_output_events_scores)
                if (t_selected_event == override_event):
                    t_selected_event = None
                    t_num_of_trials = t_num_of_trials + 1
                elif(t_selected_event.get_output_event_score() <= override_event.get_output_event_score()):
                    t_selected_event = None
                    t_num_of_trials = t_num_of_trials + 1

        return t_selected_event

    def select(self, statements):
        selectable_events = self.selectable_events(statements)
        if selectable_events:
            selected_event = self.select_next_event(selectable_events, statements)
            # return random.choice(tuple(selectable_events))
            # selected_event = np.random.choice(self.keep_output_event_with_max_score(selectable_events))
            return selected_event
        else:
            return None