import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes

from UWVV.AnalysisTools.analysisFlowMaker import createFlow

from UWVV.Ntuplizer.helpers import parseChannels, expandChannelName
from UWVV.Ntuplizer.makeBranchSet import makeBranchSet
from UWVV.Ntuplizer.eventParams import makeEventParams
from UWVV.Ntuplizer.templates.triggerBranches import triggerBranches    

import os

process = cms.Process("Ntuple")

options = VarParsing.VarParsing('analysis')

options.inputFiles = '/store/mc/RunIIFall15MiniAODv2/GluGluHToZZTo4L_M2500_13TeV_powheg2_JHUgenV6_pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/60000/02C0EC1D-F3E4-E511-ADCA-AC162DA603B4.root'
options.outputFile = 'ntuplize.root'
options.maxEvents = -1

options.register('channels', "zz", 
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Channel to make ntuples for. May be comma-separated list and/or several presets like 'zz'")
options.register('globalTag', "", 
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Global tag. If empty (default), auto tag is chosen based on isMC")
options.register('isMC', 1,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "1 if simulation, 0 if data")
options.register('eCalib', 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "1 if electron energy corrections are desired")
options.register('muCalib', 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "1 if muon momentum corrections are desired")
options.register('isSync', 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "1 if this is for synchronization purposes")
options.register('skipEvents', 0,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.int,
                 "Number of events to skip (for debugging).")
options.register('lumiMask', '',
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.string,
                 "Lumi mask (for data only).")

options.parseArguments()


channels = parseChannels(options.channels)
zz = any(len(c) == 4 for c in channels)
zl = any(len(c) == 3 for c in channels)
z  = any(len(c) == 2 for c in channels)
l  = any(len(c) == 1 for c in channels)


from IgTools.IgProf.IgProfTrigger import igprof
process.load("IgTools.IgProf.IgProfTrigger")
process.igprofPath = cms.Path(process.igprof)
process.igprof.reportEventInterval     = cms.untracked.int32(250)
process.igprof.reportToFileAtBeginJob  = cms.untracked.string("|gzip -c>igprof.begin-job.gz")
process.igprof.reportToFileAtEvent = cms.untracked.string("|gzip -c>igprof.%I.%E.%L.%R.event.gz")
#process.schedule.append(process.igprofPath)


# Basic stuff for all jobs

process.load("Configuration.StandardSequences.GeometryRecoDB_cff")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag

if options.globalTag:
    gt = options.globalTag
elif options.isMC:
    gt = 'auto:run2_mc'
else:
    gt = 'auto:run2_data'
process.GlobalTag = GlobalTag(process.GlobalTag, gt)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.schedule = cms.Schedule()

process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles),
    skipEvents = cms.untracked.uint32(options.skipEvents),
    )

if options.lumiMask:
    lumiJSON = options.lumiMask
    if not os.path.exists(lumiJSON):
        raise IOError("Lumi mask file {} not found.".format(lumiJSON))
    lumiList = LumiList.LumiList(filename = lumiJSON)
    runs = lumiList.getRuns()
    lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
    lumisToProcess.extend(lumiList.getCMSSWString().split(','))
    process.source.lumisToProcess = lumisToProcess

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string(options.outputFile),
    )

process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents)
    )

#############################################################################
#    Make the analysis flow. It is assembled from a list of classes, each   #
#    of which adds related steps to the sequence.                           #
#############################################################################
FlowSteps = []

# everybody needs vertex cleaning
from UWVV.AnalysisTools.templates.VertexCleaning import VertexCleaning
FlowSteps.append(VertexCleaning)

# everybody needs basic lepton stuff
from UWVV.AnalysisTools.templates.ElectronBaseFlow import ElectronBaseFlow
FlowSteps.append(ElectronBaseFlow)
from UWVV.AnalysisTools.templates.RecomputeElectronID import RecomputeElectronID
FlowSteps.append(RecomputeElectronID)

from UWVV.AnalysisTools.templates.MuonBaseFlow import MuonBaseFlow
FlowSteps.append(MuonBaseFlow)

# jet energy corrections and basic preselection
from UWVV.AnalysisTools.templates.JetBaseFlow import JetBaseFlow
FlowSteps.append(JetBaseFlow)

# make final states
if zz:
    from UWVV.AnalysisTools.templates.ZZFinalStateBaseFlow import ZZFinalStateBaseFlow
    FlowSteps.append(ZZFinalStateBaseFlow)

    # HZZ discriminants and categorization
    from UWVV.AnalysisTools.templates.ZZClassification import ZZClassification
    FlowSteps.append(ZZClassification)

    from UWVV.AnalysisTools.templates.ZKinematicFitting import ZKinematicFitting
    FlowSteps.append(ZKinematicFitting)
    
elif zl or z:
    from UWVV.AnalysisTools.templates.ZPlusXBaseFlow import ZPlusXBaseFlow
    FlowSteps.append(ZPlusXBaseFlow)
    
# FSR and other ZZ/HZZ stuff
from UWVV.AnalysisTools.templates.ZZFlow import ZZFlow
FlowSteps.append(ZZFlow)

# Lepton calibrations
if options.eCalib:
    raise ValueError("Electron calibrations are not yet available in 80X")

    from UWVV.AnalysisTools.templates.ElectronCalibration import ElectronCalibration
    FlowSteps.append(ElectronCalibration)

if options.muCalib:
    raise ValueError("Muon calibrations are not yet available in 80X")

    from UWVV.AnalysisTools.templates.MuonCalibration import MuonCalibration
    FlowSteps.append(MuonCalibration)

# Turn all these into a single flow class
FlowClass = createFlow(*FlowSteps)
flow = FlowClass('flow', process, 
                 isMC=bool(options.isMC), isSync=bool(options.isSync))

### Set up tree makers

# meta info tree first
process.metaInfo = cms.EDAnalyzer(
    'MetaTreeGenerator',
    eventParams = makeEventParams(flow.finalTags()),
    )
process.treeSequence = cms.Sequence(process.metaInfo)

# no triggers in MC for now
if options.isMC:
    trgBranches = cms.PSet(
        trigNames=cms.vstring(),
        trigResultsSrc = cms.InputTag("TriggerResults", "", "HLT"),
        trigPrescaleSrc = cms.InputTag("patTrigger"),
        )
else:
    trgBranches = triggerBranches


# then the ntuples
for chan in channels:
    mod = cms.EDAnalyzer(
        'TreeGenerator{}'.format(expandChannelName(chan)),
        src = flow.finalObjTag(chan),
        branches = makeBranchSet(chan),
        eventParams = makeEventParams(flow.finalTags()),
        triggers = trgBranches,
        )

    setattr(process, chan, mod)
    process.treeSequence += mod

p = flow.getPath()
p += process.treeSequence

process.schedule.append(p)
