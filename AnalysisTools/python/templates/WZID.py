from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms


class WZID(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(WZID, self).__init__(*args, **kwargs)
    
    def makeAnalysisStep(self, stepName, **inputs):
        step = super(WZID, self).makeAnalysisStep(stepName, **inputs)
        if stepName == 'embedding':
            self.addWWLooseID(step)
        return step

    def addWWLooseID(self, step):
        mod = cms.EDProducer(
            'ElectronWWIdEmbedder',
            src = step.getObjTag('e'),
            vertexSrc = step.getObjTag('v'),
            )
        step.addModule("ElectronWWIDEmbedder", mod, 'e')

    def getWZLooseMuonID(self):
        return 'userInt("isWZLooseMuon") && ' \
               'pt() > 10 &&' \
               '((pfIsolationR04().sumChargedHadronPt' \
               '+ max(0., pfIsolationR04().sumNeutralHadronEt' \
               '+ pfIsolationR04().sumPhotonEt' \
               '- 0.5*pfIsolationR04().sumPUPt))' \
               '/pt() < 0.25)'

    def getWZMediumMuonID(self):
        return 'userInt("isWZMediumMuon") && ' \
               'pt() > 10 &&' \
               '((pfIsolationR04().sumChargedHadronPt' \
               '+ max(0., pfIsolationR04().sumNeutralHadronEt' \
               '+ pfIsolationR04().sumPhotonEt' \
               '- 0.5*pfIsolationR04().sumPUPt))' \
               '/pt() < 0.15)'

    def getWZTightMuonID(self):
        return 'userInt("isTightMuon") && ' \
               'pt() > 10 &&' \
               '((pfIsolationR04().sumChargedHadronPt' \
               '+ max(0., pfIsolationR04().sumNeutralHadronEt' \
               '+ pfIsolationR04().sumPhotonEt' \
               '- 0.5*pfIsolationR04().sumPUPt))' \
               '/pt() < 0.15)'
    
    def getWZLooseElectronID(self):
        return 'userInt("isWWLoose")'
