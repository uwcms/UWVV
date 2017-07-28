from UWVV.AnalysisTools.AnalysisFlowBase import AnalysisFlowBase

import FWCore.ParameterSet.Config as cms
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

class JetBaseFlow(AnalysisFlowBase):
    def __init__(self, *args, **kwargs):
        if not hasattr(self, 'isMC'):
            self.isMC = kwargs.pop('isMC', True)
        super(JetBaseFlow, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(JetBaseFlow, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'preliminary':
            # Pileup veto
            # This puts the IDs in the event stream, not an updated
            # jet collection
            self.process.load("RecoJets.JetProducers.PileupJetID_cfi")
            self.process.pileupJetIdUpdated = self.process.pileupJetId.clone(
                jets = step.getObjTag('j'),
                inputIsCorrected = True,
                applyJec = True,
                vertexes = step.getObjTag('v'),
                )
            step.addModule('pileupJetIdUpdated',
                           self.process.pileupJetIdUpdated,
                           'puID', puID='fullId')

            # Jet energy corrections
            corrections = ['L1FastJet', 'L2Relative', 'L3Absolute',]
            if not self.isMC:
                corrections.append('L2L3Residual')
            updateJetCollection(
                self.process,
                jetSource = step.getObjTag('j'),
                labelName = 'UpdatedJEC',
                jetCorrections = ('AK4PFchs', cms.vstring(corrections), 'None'),
                )

            # Store PU ID in jet collection as a userInt
            self.process.updatedPatJetsUpdatedJEC.userData.userInts.src += [step.getObjTagString('puID')]

            self.process.jecSequence = cms.Sequence(
                self.process.patJetCorrFactorsUpdatedJEC
                * self.process.updatedPatJetsUpdatedJEC
                )
            step.addModule('jecSequence',
                           self.process.jecSequence,
                           'j')

            if self.isMC:
                # shift corrections up and down for systematics
                jesShifts = cms.EDProducer(
                    "PATJetEnergyScaleShifter",
                    src = step.getObjTag('j'),
                    )
                step.addModule('jesShifts', jesShifts, 'j_jesUp', 'j_jesDown',
                               j_jesUp='jesUp', j_jesDown='jesDown')

            jetIDEmbedding = cms.EDProducer(
                "PATJetIDEmbedder",
                src = step.getObjTag('j'),
                )
            step.addModule('jetIDEmbedding', jetIDEmbedding, 'j')

            if self.isMC:
                jetIDEmbedding_jesUp = cms.EDProducer(
                    "PATJetIDEmbedder",
                    src = step.getObjTag('j_jesUp'),
                    )
                step.addModule('jetIDEmbeddingJESUp', jetIDEmbedding_jesUp, 'j_jesUp')
                jetIDEmbedding_jesDown = cms.EDProducer(
                    "PATJetIDEmbedder",
                    src = step.getObjTag('j_jesDown'),
                    )
                step.addModule('jetIDEmbeddingJESDown', jetIDEmbedding_jesDown, 'j_jesDown')


                jetSmearing = cms.EDProducer(
                    "PATJetSmearing",
                    src = step.getObjTag('j'),
                    rhoSrc = cms.InputTag("fixedGridRhoFastjetAll"),
                    systematics = cms.bool(True),
                    )
                step.addModule("jetSmearing", jetSmearing, 'j', 'j_jerUp',
                               'j_jerDown', j_jerUp='jerUp', j_jerDown='jerDown')

                jetSmearing_jesUp = jetSmearing.clone(src = step.getObjTag('j_jesUp'),
                                                      systematics = cms.bool(False))
                step.addModule("jetSmearingJESUp", jetSmearing_jesUp, 'j_jesUp')
                jetSmearing_jesDown = jetSmearing.clone(src = step.getObjTag('j_jesDown'),
                                                      systematics = cms.bool(False))
                step.addModule("jetSmearingJESDown", jetSmearing_jesDown, 'j_jesDown')

                # need to re-sort now that we're calibrated
                jSort_jesUp = cms.EDProducer(
                    "PATJetCollectionSorter",
                    src = step.getObjTag('j_jesUp'),
                    function = cms.string('pt'),
                    )
                step.addModule('jetSortingJESUp', jSort_jesUp, 'j_jesUp')

                jSort_jesDn = cms.EDProducer(
                    "PATJetCollectionSorter",
                    src = step.getObjTag('j_jesDown'),
                    function = cms.string('pt'),
                    )
                step.addModule('jetSortingJESDn', jSort_jesDn, 'j_jesDown')

                jSort_jerUp = cms.EDProducer(
                    "PATJetCollectionSorter",
                    src = step.getObjTag('j_jerUp'),
                    function = cms.string('pt'),
                    )
                step.addModule('jetSortingJERUp', jSort_jerUp, 'j_jerUp')

                jSort_jerDn = cms.EDProducer(
                    "PATJetCollectionSorter",
                    src = step.getObjTag('j_jerDown'),
                    function = cms.string('pt'),
                    )
                step.addModule('jetSortingJERDn', jSort_jerDn, 'j_jerDown')

            # need to re-sort now that we're calibrated
            jSort = cms.EDProducer(
                "PATJetCollectionSorter",
                src = step.getObjTag('j'),
                function = cms.string('pt'),
                )
            step.addModule('jetSorting', jSort, 'j')

        if stepName == 'preselection':
            # For now, we're not using the PU ID, but we'll store it in the
            # ntuples later
            selectionString = ('pt > 30. && abs(eta) < 4.7 && '
                               'userFloat("idLoose") > 0.5')

            # # use medium PU ID
            # # PU IDs are stored as a userInt where the first three digits are
            # # tight, medium, and loose PUID decisions (going right to left)
            # selectionString = ('pt>30. && abs(eta) < 4.7 && '
            #                    'userFloat("idLoose") > 0.5 && '
            #                    'userInt("{}") >= 6').format(step.getObjTagString('puID'))

            step.addBasicSelector('j', selectionString)
            if self.isMC:
                step.addBasicSelector('j_jesUp', selectionString)
                step.addBasicSelector('j_jesDown', selectionString)
                step.addBasicSelector('j_jerUp', selectionString)
                step.addBasicSelector('j_jerDown', selectionString)

        return step






