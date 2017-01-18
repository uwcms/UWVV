# Modified from N. Smith, U. Wisconsin
from CRABClient.UserUtilities import config
import ConfigParser
import os
import re
import subprocess
import sys

settingsFile = "local.txt"
if not os.path.exists(settingsFile):
    print "Please copy local.template.txt to local.txt and edit as appropriate"
    exit()
localSettings = ConfigParser.ConfigParser()
localSettings.read(settingsFile)

gitDescription = subprocess.check_output(["git", "describe", "--always"]).strip()
gitStatus = subprocess.check_output(["git", "status", "--porcelain", "-uno"])
if gitStatus != "":
    print "\033[33mWARNING: git status is dirty!\033[0m"
    print gitStatus
    gitDescription += "*"
print gitDescription
# We have to hack our way around how crab parses command line arguments :<
dataset = 'dummy'
for arg in sys.argv:
    if 'Data.inputDataset=' in arg:
        dataset = arg.split('=')[1]
if dataset == 'dummy':
    raise Exception("Must pass dataset argument as Data.inputDataset=...")

(_, primaryDS, conditions, dataTier) = dataset.split('/')
if dataTier == 'MINIAOD':
    isMC = 0
elif dataTier == 'MINIAODSIM':
    isMC = 1
else:
    raise Exception("Dataset malformed? Couldn't deduce isMC parameter")

def getUnitsPerJob(ds):
    if isMC == 0:
        # Data is split by lumisection
        # The difference is due to trigger rates
        if 'Double' in ds:
            return 200
        elif 'MuonEG' in ds:
            return 400
        else:
            return 200
    else:
        return 10

config = config()
config.Data.inputDataset = dataset
config.Data.outputDatasetTag = conditions
configParams = ['isMC=%d' % isMC]#, 'crabDataset='+dataset, 'gitversion="%s"' % gitDescription]
configParams += ["channels=wz",
    "eCalib=1",
    "muCalib=1",
    "globalTag=%s" % ("80X_mcRun2_asymptotic_2016_TrancheIV_v7" if isMC else \
        "80X_dataRun2_2016SeptRepro_v6"),
]

if isMC:
    config.General.requestName = '_'.join([localSettings.get("local", "campaign"), primaryDS])
    # Check for extension dataset, force unique request name
    m = re.match(r".*(_ext[0-9]*)-", conditions)
    if m:
        config.General.requestName += m.groups()[0]
    config.Data.splitting = 'FileBased'
    config.Data.unitsPerJob = getUnitsPerJob(primaryDS)
else:
    # Since a PD will have several eras, add conditions to name to differentiate
    config.General.requestName = '_'.join([localSettings.get("local", "campaign"), primaryDS, conditions])
    config.Data.lumiMask ='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt'
    config.Data.splitting = 'LumiBased'
    config.Data.unitsPerJob = getUnitsPerJob(primaryDS)

config.JobType.pyCfgParams = configParams

# Things that don't change with dataset
config.General.workArea = '.'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'ANALYSIS'
config.JobType.psetName = '../../Ntuplizer/test/ntuplize_cfg.py'
config.JobType.numCores = 1

config.Data.inputDBS = 'global'
config.Data.useParent = False
config.Data.publication = False
config.Data.outLFNDirBase = localSettings.get("local", "outLFNDirBase").replace("$USER", os.environ["USER"])
config.Data.ignoreLocality = False

config.Site.storageSite = localSettings.get("local", "storageSite")
