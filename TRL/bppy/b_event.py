class BEvent:

    def __init__(self, name="", data={}):
        self.name = name
        self.data = data

    def __key(self):
        return tuple([self.name]) + tuple(str(self.data.items()))

    # TODO: double check
    def __hash__(self):
        return hash(self.__key())

    def __eq__(self, other):
        if isinstance(other, BEvent):
            # input_event - we ignore data and only consider the type
            if (self.__name()=='input_event' and other.__name()=='input_event'):
                return True
            # output_events - we only consider the action / not the score
            elif ((self.__name()=='output_event' and other.__name()=='output_event')
            or (self.__name()=='output_event_proxy' and other.__name()=='output_event_proxy')):
                return self.data['action'] == other.data['action']
            else:
                return self.__key() == other.__key()
        else:
            return False

    def __repr__(self):
        return "{}(name={},data={})".format(self.__class__.__name__, self.name, self.data)

    def __str__(self):
        return self.__repr__()

    def __name(self):
        return self.name

    def is_output_event(self):
        return self.__name()=='output_event_proxy'
        # return self.__name()=='output_event'

    def get_output_event_score(self):
        if(self.is_output_event()):
            return self.data['score']
        else:
            return None

    def get_output_event_action(self):
        if(self.is_output_event()):
            return self.data['action']
        else:
            return None



