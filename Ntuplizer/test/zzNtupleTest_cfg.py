import FWCore.ParameterSet.Config as cms

from UWVV.AnalysisTools.analysisFlowMaker import createFlow
from UWVV.AnalysisTools.templates.ElectronBaseFlow import ElectronBaseFlow 
from UWVV.AnalysisTools.templates.MuonBaseFlow import MuonBaseFlow
from UWVV.AnalysisTools.templates.ZZFinalStateBaseFlow import ZZFinalStateBaseFlow
from UWVV.Ntuplizer.makeBranchSet import makeBranchSet
from UWVV.Ntuplizer.eventParams import makeEventParams
    

process = cms.Process("TestNtuple")

process.load("Configuration.StandardSequences.GeometryRecoDB_cff")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc')

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.schedule = cms.Schedule()

process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring('/store/mc/RunIIFall15MiniAODv2/GluGluHToZZTo4L_M2500_13TeV_powheg2_JHUgenV6_pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/60000/02C0EC1D-F3E4-E511-ADCA-AC162DA603B4.root'),
    )

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("testNtuple.root"),
    )

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(1000))

LeptonFlowClass = createFlow(ElectronBaseFlow, MuonBaseFlow)
leptonFlow = LeptonFlowClass('leptonFlow', process)

FinalStateFlowClass = createFlow(ZZFinalStateBaseFlow)
finalStateFlow = FinalStateFlowClass('finalStateFlow', process,
                                     **leptonFlow.outputs[-1])

process.treeMakerEEEE = cms.EDAnalyzer(
    'TreeGeneratorEEEE',
    src = finalStateFlow.finalObjTag('zz4e'),
    branches = makeBranchSet('eeee'),
    eventParams = makeEventParams(finalStateFlow.finalTags()),
    )
process.treeMakerEEMuMu = cms.EDAnalyzer(
    'TreeGeneratorEEMuMu',
    src = finalStateFlow.finalObjTag('zz2e2m'),
    branches = makeBranchSet('eemm'),
    eventParams = makeEventParams(finalStateFlow.finalTags()),
    )
process.treeMakerMuMuMuMu = cms.EDAnalyzer(
    'TreeGeneratorMuMuMuMu',
    src = finalStateFlow.finalObjTag('zz4m'),
    branches = makeBranchSet('mmmm'),
    eventParams = makeEventParams(finalStateFlow.finalTags()),
    )

process.metaTreeMaker = cms.EDAnalyzer(
    'MetaTreeGenerator',
    eventParams = makeEventParams(finalStateFlow.finalTags()),
    )

process.makeTrees = cms.Path(
    process.treeMakerEEEE
    * process.treeMakerEEMuMu
    * process.treeMakerMuMuMuMu
    * process.metaTreeMaker
    )

process.schedule.append(process.makeTrees)
