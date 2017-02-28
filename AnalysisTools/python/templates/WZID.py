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
        return 'userInt("IsWZLooseMuon") && ' \
               'pt() > 10 && abs(eta) < 2.4'
    def getWZMediumMuonID(self):
        return 'userInt("IsWZMediumMuon") && ' \
               'pt() > 10 && abs(eta) < 2.4'
    def getWZTightMuonID(self):
        return 'userInt("IsTightMuon") && ' \
               'pt() > 10 && abs(eta) < 2.4'
    def getWZLooseElectronID(self):
        return 'userInt("IsWWLoose") && ' \
               'pt() > 10 && abs(eta) < 2.5'
    def getPOGElectronIPCut(self):
        return '(? isEB ? userFloat("dxy") < 0.05 : userFloat("dxy") < 0.1)'#) && ' \
              # '(? isEB ? userFloat("dz") < 0.1 : userFloat("dz") < 0.2)'

