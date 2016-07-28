from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase


class NoCombinedStates(AnalysisFlowBase):
    '''
    A class that prevents composite intermediate/final states.
    '''
    def __init__(self, *args, **kwargs):
        super(NoCombinedStates, self).__init__(*args, **kwargs)

    def listSteps(self):
        '''
        Remove any steps that have "intermediateState" or "finalState" in them
        (case is ignored).
        '''
        steps = super(NoCombinedStates, self).listSteps()

        newSteps = []
        for s in steps:
            if 'intermediatestate' not in s.lower() and 'finalstate' not in s.lower():
                newSteps.append(s)

        return newSteps
