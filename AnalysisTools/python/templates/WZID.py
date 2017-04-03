from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms


class WZID(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(WZID, self).__init__(*args, **kwargs)
    
    def makeAnalysisStep(self, stepName, **inputs):
        step = super(WZID, self).makeAnalysisStep(stepName, **inputs)
        if stepName == 'embedding':
            self.addWZIDs(step)
        return step

    def addWZIDs(self, step):
        mod = cms.EDProducer(
            'ElectronWZIDEmbedder',
            src = step.getObjTag('e'),
            vertexSrc = step.getObjTag('v'),
            )
        step.addModule("ElectronWZIDEmbedder", mod, 'e')

    def getWZLooseMuonID(self):
        return 'userInt("isWZLooseMuon") && ' \
               'pt() > 10 && abs(eta) < 2.4'
    def getWZMediumMuonID(self):
        return 'userInt("isWZMediumMuon") && ' \
               'pt() > 10 && abs(eta) < 2.4'
    def getWZTightMuonID(self):
        return 'userInt("isTightMuon") && ' \
               'pt() > 10 && abs(eta) < 2.4'
    def getWZLooseElectronID(self):
        return 'userFloat("IsCBVIDVeto") && ' \
               'pt() > 10 && abs(eta) < 2.5'
    def getWZTightElectronID(self):
        return 'userFloat("IsCBVIDTight") && ' \
               'pt() > 10 && abs(eta) < 2.5'
    def getPOGElectronIPCut(self):
        return '(? isEB ? userFloat("dxy") < 0.05 : userFloat("dxy") < 0.1)'

