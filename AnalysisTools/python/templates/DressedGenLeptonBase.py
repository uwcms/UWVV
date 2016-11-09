from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms


class GenLeptonBase(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        self.flag = kwargs.pop('leptonStatusFlag', 'fromHardProcessFinalState')
        super(GenLeptonBase, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(GenLeptonBase, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'selection':
            promptPhotonsMod = cms.EDFilter("GenParticleSelector",
                src = cms.InputTag(genParticlesLabel),
                cut = cms.string("pdgId = 22 && statusFlags().isPrompt() && status() == 1")
            )
            step.addModule('promptPhotonsMod', genEMod, 'g')
            
            genEMod = cms.EDFilter(
                "GenParticleSelector",
                src = step.getObjTag('e'),
                cut = cms.string("abs(pdgId) == 11 && {}".format(self.flag)),
                )
            step.addModule('genSelectionE', genEMod, 'e')
            
            dressedGenEMod = cms.EDProducer("DressedGenParticlesProducer",
                baseCollection = step.getObjTag('e'),
                associates = step.getObjTag('g'),
                dRmax = cms.untracked.double(0.1)
            )
            step.addModule('dressedGenEMod', genEMod, 'e')

            genMuMod = cms.EDFilter(
                "GenParticleSelector",
                src = step.getObjTag('m'),
                cut = cms.string("abs(pdgId) == 13 && {}".format(self.flag)),
                )
            step.addModule('genSelectionMu', genMuMod, 'm')
            
            dressedGenMuMod = cms.EDProducer("DressedGenParticlesProducer",
                baseCollection = step.getObjTag('e'),
                associates = step.getObjTag('g'),
                dRmax = cms.untracked.double(0.1)
            )
            step.addModule('dressedGenMuMod', genEMod, 'm')

        return step


