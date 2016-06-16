#!/usr/bin/env python

'''

Script to set up everything needed for a big submission to the UW cluster and
write a bash script to do the actual submission.

Nate Woods, U. Wisconsin, based on:
    https://github.com/uwcms/FinalStateAnalysis/blob/miniAOD_dev_80X/Utilities/scripts/submit_job.py

'''


import argparse
import os
import sys
from socket import gethostname
import logging
import fnmatch

from UWVV.Utilities.dbsinterface import get_das_info


log = logging.getLogger("submitJobs")
logging.basicConfig(level=logging.INFO, stream=sys.stderr)


def writeFarmoutCommand(cfg, jobid, dataset, fullDataset, 
                        outdir='srm://cmssrm.hep.wisc.edu:8443/srm/v2/server?SFN=/hdfs/store/{}/{}/{}/',
                        **args):
    '''
    Write the actual farmout command.
    cfg (str): config file to run
    jobid (str): unique ID for this set of jobs
    dataset (str): short name for the dataset
    fullDataset (str): full dataset name with path
    outdir (str): where to put the output
    args: command line arguments
    '''
    user = os.environ['USER']

    if 'uwlogin' in gethostname():
        scratchDir = '/data'
    else:
        scratchDir = '/nfs_scratch'

    submitDir = os.path.join(scratchDir, user, jobid, dataset, 'submit')

    cmsRunArgs = list(args.get('cmsRunArgs', []))

    if os.path.exists(submitDir):
        log.warning("Submit directory for sample %s already exists, skipping", dataset)
        return "# Submit directory for sample {} already exists, skipping".format(dataset)

    log.info("Building submit files for sample %s", dataset)

    dagDir = '/' + os.path.join(*(submitDir.split('/')[:-1]+['dags', 'dag']))

    dasFilesCmd = 'file dataset={}'.format(fullDataset)
    dasFiles = get_das_info(dasFilesCmd)

    mkdirCmd = "mkdir -p {}inputs".format(dagDir)
    os.system(mkdirCmd)
    inputListFile = os.path.join(dagDir+"inputs", '{}_inputFiles.txt'.format(dataset))
    with open(inputListFile, 'w') as f:
        f.write('\n'.join(dasFiles))

    cmds = [
        'farmoutAnalysisJobs',
        '--infer-cmssw-path',
        '"--submit-dir={}"'.format(submitDir),
        '"--output-dag-file={}"'.format(dagDir),
        '"--output-dir={}"'.format(outdir.format(user=user, jobid=jobid, dataset=dataset)),
        '--input-files-per-job={}'.format(args.get('filesPerJob', 1)),
        '--input-file-list={}'.format(inputListFile),
        '--assume-input-files-exist',
        '--input-dir=/',
        '{}-{}'.format(jobid, dataset),
        cfg,
        "'inputFiles=$inputFileNames'",
        "'outputFile=$outputFileName'",
        ]

    if args.get('applyLumiMask', False):
        lumiMask = 'Cert_271036-274421_13TeV_PromptReco_Collisions16_JSON.txt' # 2.07/fb
        if args.get('lumiMaskJSON', ''):
            lumiMask = args.get('lumiMaskJSON')
        lumiMask = os.path.join('/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV', 
                                lumiMask)
        cmds.append('lumiMask={}'.format(lumiMask))

    cmds += cmsRunArgs

    command = '# Submit file for sample {}\n'.format(dataset)
    command += 'mkdir -p {}\n'.format(os.path.dirname(dagDir))
    command += ' '.join(cmds) + '\n'

    return command


def buildScript(cfg, jobid, scriptFile='',
                outdir='srm://cmssrm.hep.wisc.edu:8443/srm/v2/server?SFN=/hdfs/store/user/{}/{}/{}/',
                *samples, **args):
    '''
    Build script to submit jobs to ntuplize all requested samples.
    cfg (str): config file to run
    jobid (str): unique ID for this set of jobs
    scriptFile (str or None): file to write script to, or None for stderr
    outdir (str): where to put the output
    samples(strs): which datasets/samples to use
    args: command line arguments    
    '''
    lines = []

    campaign = args.get('campaign', '')
    dataEra = args.get('dataEra', '')

    # mc
    if campaign:
        assert not dataEra, "You can't specify a data era and a MC campaign tag."
        
        datasetStr = '/*/{}/MINIAODSIM'.format(campaign)
    # data
    else:
        assert dataEra, "You must specify a data era or an MC campaign tag."
        datasetStr = '/*/{}/MINIAOD'.format(dataEra)

    datasets = get_das_info(datasetStr)

    for dataset in datasets:
        name = dataset.split('/')[1]

        for pattern in samples:
            if fnmatch.fnmatchcase(name, pattern):
                lines.append(writeFarmoutCommand(cfg, jobid, name, dataset,
                                                 outdir, **args))
                break

    if not lines:
        log.warning("No datasets found matching {}".format(samples))

    if not scriptFile:
        for l in lines:
            print l
    else:
        with open(scriptFile, 'w') as f:
            for l in lines:
                f.write(l)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('jobid', type=str,
                        help='Unique identifier for set of jobs.')
    parser.add_argument('cfg', type=str,
                        help='CMS config file to run.')
    parser.add_argument('--applyLumiMask', action='store_true',
                        help='Pass the appropriate lumi-mask JSON (data only).')
    parser.add_argument('--lumiMaskJSON', type=str, default='',
                        help='Custom lumi mask JSON.')
    parser.add_argument('--campaign', type=str, default='',
                        help='MC generation campaign for samples (MC only).')
    parser.add_argument('--dataEra', type=str, default='',
                        help='Run over data.')
    parser.add_argument('--samples', nargs='+', type=str, required=False,
                        help='Samples to run. May contain wildcards.')
    parser.add_argument('--outdir', type=str, 
                        default='srm://cmssrm.hep.wisc.edu:8443/srm/v2/server?SFN=/hdfs/store/user/{user}/{jobid}/{dataset}/',
                        help = 'Where to put the output.  Default: %(default)s.')
    parser.add_argument('--filesPerJob', type=int, default=1,
                        help="Number of input files to pass to each job.")
    parser.add_argument('-o', type=str, dest="scriptFile", default="", required=False, 
                        help='File to write output bash script to (default: stdout).')
    parser.add_argument('cmsRunArgs', nargs=argparse.REMAINDER,
                        help="Arguments to cmsRun.")

    args = parser.parse_args()

    buildScript(args.cfg, args.jobid, args.scriptFile, args.outdir, 
                *args.samples, applyLumiMask=args.applyLumiMask, 
                lumiMaskJSON=args.lumiMaskJSON, campaign=args.campaign,
                dataEra=args.dataEra, filesPerJob=args.filesPerJob, 
                cmsRunArgs=args.cmsRunArgs)
                        
