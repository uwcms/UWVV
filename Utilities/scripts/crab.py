# Modified from N. Smith, U. Wisconsin
from CRABClient.UserUtilities import config, getUsernameFromSiteDB
import ConfigParser
import os
import re
import subprocess
import sys
import datetime
import glob

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
print "Git status is %s" % gitDescription
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
configParams = [
    'isMC=%d' % isMC,
    "channels=%s" % localSettings.get("local", "channels"),
    "lheWeights=%s" % localSettings.get("local", "lheWeights"),
    "eCalib=%s" % localSettings.get("local", "eCalib"),
    "muCalib=%s" % localSettings.get("local", "muCalib"),
    "globalTag=%s" % (localSettings.get("local", "mcGlobalTag") if isMC else \
        localSettings.get("local", "dataGlobalTag")),
]
today = datetime.date.today().strftime("%d%b%Y")
campaign_name = localSettings.get("local", "campaign").replace("$DATE", today)
if isMC:
    config.General.requestName = '_'.join([campaign_name, primaryDS])
    # Check for extension dataset, force unique request name
    m = re.match(r".*(_ext[0-9]*)-", conditions)
    if m:
        config.General.requestName += m.groups()[0]
    config.Data.splitting = 'FileBased'
    config.Data.unitsPerJob = getUnitsPerJob(primaryDS)
else:
    # Since a PD will have several eras, add conditions to name to differentiate
    config.General.requestName = '_'.join([campaign_name, primaryDS, conditions])
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
config.JobType.inputFiles = ["../../../UWVV/data"]

config.Data.inputDBS = 'global'
config.Data.useParent = False
config.Data.publication = False
outdir = localSettings.get("local", "outLFNDirBase").replace(
    "$USER", getUsernameFromSiteDB()).replace("$DATE", today)
config.Data.outLFNDirBase = outdir 
config.Data.ignoreLocality = False

config.Site.storageSite = localSettings.get("local", "storageSite")
