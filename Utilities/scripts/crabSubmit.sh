# Modified from N. Smith, U. Wisconsin
# Usage examples:
# . submit.sh twoLepton-tranche4.txt | grep 'DYJets' | . /dev/stdin
# . submit.sh twoLepton-data.txt | . /dev/stdin
if [[ $1 == *"AllLHEWeights"* ]]; then
  config=CrabTemplates/local.allweights.txt
elif [[ $1 == *"NoLHEWeights"* ]]; then
  config=CrabTemplates/local.noweights.txt
else
  config=CrabTemplates/local.onlyscaleweights.txt
fi 
(>&2 echo "Using config file $config")
cp $config local.txt
grep -v -e '^#' -e '^ *$' $1 | while read dataset
do
  # uncomment to record nevents
  # grep -q "${dataset}" nevents.txt && continue
  # echo $dataset $(das_client.py --format=json --query="dataset dataset=${dataset}" | grep -o "nevents[^,]*," -m 1)
  echo crab submit -c crab.py Data.inputDataset=$dataset
done
