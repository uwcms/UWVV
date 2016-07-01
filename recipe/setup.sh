#!/usr/bin/bash


pushd $CMSSW_BASE/src

echo "Setting up electron ID code"
git clone https://github.com/Werbellin/RecoEgamma_8X RecoEgamma

echo "Setting up ZZ matrix element stuff"
git clone https://github.com/cms-analysis/HiggsAnalysis-ZZMatrixElement.git ZZMatrixElement

# Get a version we know works during the awkward MELA v1->v2 transition
pushd ZZMatrixElement
git checkout c6d45de674ee145a8a4d74ba99f3e446dd1d9987
popd

git clone -b segfaultFix git@github.com:nwoods/KinZfitter.git

popd