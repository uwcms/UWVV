# Modified from N. Smith, U. Wisconsin
# Usage examples:
# . crabSubmit.sh twoLepton-tranche4.txt | grep 'DYJets' | . /dev/stdin
# . crabSubmit.sh twoLepton-data.txt | . /dev/stdin
if [ $# -eq 0 ]; then
    echo "You need to specify a file containing your list of datasets"
    echo "    Usage: . crabSubmit.sh datasetList.txt"
    return 1
fi
scripts_path=$CMSSW_BASE/src/UWVV/Utilities/scripts
config_path=$scripts_path/CrabTemplates
config=$config_path/local.noweights.txt
if [[ $1 == *"AllLHEWeights"* ]]; then
  config=$config_path/local.allweights.txt
elif [[ $1 == *"NoLHEWeights"* ]]; then
  config=$config_path/local.noweights.txt
elif [[ $1 == *"LHEScaleWeights"* ]]; then
  config=$config_path/local.onlyscaleweights.txt
elif [[ $1 == *"Run2016H"* ]]; then
  config=$config_path/local.noweights.2016h.txt
fi 
(>&2 echo "Using config file $config")
cp $config local.txt
grep -v -e '^#' -e '^ *$' $1 | while read dataset
do
  # uncomment to record nevents
  # grep -q "${dataset}" nevents.txt && continue
  # echo $dataset $(das_client.py --format=json --query="dataset dataset=${dataset}" | grep -o "nevents[^,]*," -m 1)
  echo crab submit -c $scripts_path/crab.py Data.inputDataset=$dataset
done
