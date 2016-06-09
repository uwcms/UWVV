from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class ZZCrossCleaning(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(ZZCrossCleaning, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZZCrossCleaning, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'selection':
            # remove electrons near tight muons
            step.addCrossSelector(
                'e', 
                '', # no further basic selection here
                m={
                    'deltaR' : 0.05,
                    'selection' : 'userFloat("{}Tight") > 0.5'.format(self.getZZIDLabel()),
                    }
                )
            
            # remove jets close to tight, isolated electrons and muons
            step.addCrossSelector(
                'j',
                '', # no further basic selection here
                e={
                    'deltaR' : 0.4,
                    'selection' : ('userFloat("{}Tight") > 0.5 && '
                                   'userFloat("{}") > 0.5').format(self.getZZIDLabel(),
                                                                   self.getZZIsoLabel()),
                    },
                m={
                    'deltaR' : 0.4,
                    'selection' : ('userFloat("{}Tight") > 0.5 && '
                                   'userFloat("{}") > 0.5').format(self.getZZIDLabel(),
                                                                   self.getZZIsoLabel()),
                    },
                )

        return step
    





