from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase
from UWVV.AnalysisTools.templates.ZPlusXBaseFlow import ZPlusXBaseFlow
from UWVV.AnalysisTools.templates.ZZFinalStateBaseFlow import ZZFinalStateBaseFlow

import FWCore.ParameterSet.Config as cms


class ZZFSR(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        super(ZZFSR, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(ZZFSR, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'embedding':
            leptonFSREmbedder = cms.EDProducer(
                "PATObjectFSREmbedder",
                muSrc = step.getObjTag('m'),
                eSrc = step.getObjTag('e'),
                candSrc = step.getObjTag('pfCands'),
                phoSelection = cms.string("pt > 2 && abs(eta) < 2.4"),
                nIsoSelection = cms.string("pt > 0.5"),
                chIsoSelection = cms.string("pt > 0.2"),
                eSelection = cms.string('userFloat("%s") > 0.5'%self.getZZIDLabel()),
                muSelection = cms.string('userFloat("%s") > 0.5'%self.getZZIDLabel()),
                fsrLabel = cms.string(self.getFSRLabel()),
                etPower = cms.double(2.),
                maxDR = cms.double(0.5),
                isoDR = cms.double(0.3),
                nIsoVetoDR = cms.double(0.01),
                chIsoVetoDR = cms.double(0.0001),
                relIsoCut = cms.double(1.8),
                eMuCrossCleaningDR = cms.double(0.05),
                )
            step.addModule('fsrEmbedder', leptonFSREmbedder, 'e', 'm')

        if stepName == 'selection':
            jetFSRCleaner = cms.EDProducer(
                'PATJetFSRCleaner',
                src = step.getObjTag('j'),
                srcE = step.getObjTag('e'),
                srcMu = step.getObjTag('m'),
                fsrLabel = cms.string(self.getFSRLabel()),
                fsrElecSelection = cms.string('userFloat("%sTight") > 0.5 && userFloat("%s") > 0.5'%(self.getZZIDLabel(), self.getZZIsoLabel())),
                fsrMuonSelection = cms.string('userFloat("%sTight") > 0.5 && userFloat("%s") > 0.5'%(self.getZZIDLabel(), self.getZZIsoLabel())),
                )
            step.addModule('jetFSRCleaner', jetFSRCleaner, 'j')

        if stepName == 'intermediateStateEmbedding':
            if isinstance(self, ZPlusXBaseFlow):
                zeFSREmbedder = cms.EDProducer(
                    'PATElectronCompositeUserCandPromoter',
                    src = step.getObjTag('ee'),
                    label = cms.string(self.getFSRLabel()),
                    )
                zmFSREmbedder = cms.EDProducer(
                    'PATMuonCompositeUserCandPromoter',
                    src = step.getObjTag('mm'),
                    label = cms.string(self.getFSRLabel()),
                    )

                step.addModule('zeFSREmbedder', zeFSREmbedder, 'ee')
                step.addModule('zmFSREmbedder', zmFSREmbedder, 'mm')

        if stepName == 'finalStateEmbedding':
            if isinstance(self, ZZFinalStateBaseFlow):
                zz4eFSREmbedder = cms.EDProducer(
                    'PATCompositeCompositeUserCandPromoter',
                    src = step.getObjTag('eeee'),
                    label = cms.string(self.getFSRLabel()),
                    )
                zz2e2mFSREmbedder = cms.EDProducer(
                    'PATCompositeCompositeUserCandPromoter',
                    src = step.getObjTag('eemm'),
                    label = cms.string(self.getFSRLabel()),
                    )
                zz4mFSREmbedder = cms.EDProducer(
                    'PATCompositeCompositeUserCandPromoter',
                    src = step.getObjTag('mmmm'),
                    label = cms.string(self.getFSRLabel()),
                    )
                step.addModule('zz4eFSREmbedder', zz4eFSREmbedder, 'eeee')
                step.addModule('zz2e2mFSREmbedder', zz2e2mFSREmbedder, 'eemm')
                step.addModule('zz4mFSREmbedder', zz4mFSREmbedder, 'mmmm')

        return step

    def getFSRLabel(self):
        return "fsr"
