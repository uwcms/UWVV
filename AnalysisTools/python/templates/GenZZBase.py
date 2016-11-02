from UWVV.AnalysisTools.templates.ZPlusXBaseFlow import ZPlusXBaseFlow
from UWVV.Utilities.helpers import mapObjects, parseChannels

import FWCore.ParameterSet.Config as cms


class GenZZBase(ZPlusXBaseFlow):
    def __init__(self, *args, **kwargs):
        super(GenZZBase, self).__init__(*args, **kwargs)

    def makeAnalysisStep(self, stepName, **inputs):
        step = super(GenZZBase, self).makeAnalysisStep(stepName, **inputs)

        if stepName == 'initialStateCreation':

            for chan in parseChannels('zz'):
                z1Name = 'z{}1'.format(chan[0])
                z2Name = 'z{}{}'.format(chan[2], 2 if chan[0] == chan[2] else 1)
                mod = cms.EDProducer(
                    'PATCandViewShallowCloneCombiner',
                    decay = cms.string('{0} {1}'.format(step.getObjTagString(chan[:2]),
                                                        step.getObjTagString(chan[2:]))),
                    roles = cms.vstring(z1Name, z2Name),
                    cut = cms.string(('60. < daughter("{}").mass < 120. && '
                                      '60. < daughter("{}").mass < 120.').format(z1Name, z2Name)),
                    checkCharge = cms.bool(False),
                    setPdgId = cms.int32(25),
                    )

                step.addModule(chan+'GenProducer', mod, chan)

                cleaner = cms.EDProducer(
                    "GenZZCleaner",
                    src = step.getObjTag(chan),
                    l1PtCut = cms.double(20.),
                    l2PtCut = cms.double(10.),
                    l3PtCut = cms.double(5.),
                    l4PtCut = cms.double(5.),
                    etaCut = cms.double(2.5),
                    ossfMassCut = cms.double(4.),
                    z1MassMin = cms.double(60.),
                    z1MassMax = cms.double(120.),
                    z2MassMin = cms.double(60.),
                    z2MassMax = cms.double(120.),
                    )
                step.addModule(chan+'GenZZCleaner', cleaner, chan)

        if stepName == 'selection':
            # select and cross clean gen jets
            mod = cms.EDProducer(
                "GenJetCleaner",
                src=step.getObjTag('j'),
                preselection=cms.string('pt > 30. && abs(eta) < 4.7'),
                checkOverlaps = cms.PSet(
                    electrons = cms.PSet(
                        src=step.getObjTag('e'),
                        preselection=cms.string(''),
                        deltaR=cms.double(0.4),
                        ),
                    muons = cms.PSet(
                        src=step.getObjTag('m'),
                        preselection=cms.string(''),
                        deltaR=cms.double(0.4),
                        ),
                    ),
                finalCut = cms.string(''),
                )
            step.addModule('genJetCleaner', mod, 'j')

        return step
