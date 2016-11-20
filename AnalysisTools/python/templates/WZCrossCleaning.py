from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms

class WZCrossCleaning(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.pop('isMC', True)
        super(WZCrossCleaning, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(WZCrossCleaning, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'selection':
            # Remove jets containing or near leptons passing loose IDs
            step.addCrossSelector(
                'j',
                '', # no further basic selection here
                e={
                    'deltaR' : 0.4,
                    'selection' : self.getWZLooseElectronID(),
                    },
                m={
                    'deltaR' : 0.4,
                    'selection' : self.getWZLooseMuonID(),
                    },
                )

            if self.isMC:
                step.addCrossSelector(
                    'j_jesUp',
                    '', # no further basic selection here
                    e={
                        'deltaR' : 0.4,
                        'selection' : self.getWZLooseElectronID(),
                        },
                    m={
                        'deltaR' : 0.4,
                        'selection' : self.getWZLooseMuonID(),
                        },
                    )
                step.addCrossSelector(
                    'j_jesDown',
                    '', # no further basic selection here
                    e={
                        'deltaR' : 0.4,
                        'selection' : self.getWZLooseElectronID(),
                        },
                    m={
                        'deltaR' : 0.4,
                        'selection' : self.getWZLooseMuonID(),
                        },
                    )
                step.addCrossSelector(
                    'j_jerUp',
                    '', # no further basic selection here
                    e={
                        'deltaR' : 0.4,
                        'selection' : self.getWZLooseElectronID(),
                        },
                    m={
                        'deltaR' : 0.4,
                        'selection' : self.getWZLooseMuonID(),
                      },
                    )
                step.addCrossSelector(
                    'j_jerDown',
                    '', # no further basic selection here
                    e={
                        'deltaR' : 0.4,
                        'selection' : self.getWZLooseElectronID(),
                        },
                    m={
                        'deltaR' : 0.4,
                        'selection' : self.getWZLooseMuonID(),
                      },
                    )

            step.addBasicSelector('j', 'abs(eta) < 2.4', newCollection="eta2p4")

        return step

