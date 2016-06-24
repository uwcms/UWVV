#!/usr/bin/bash


pushd $CMSSW_BASE/src

echo "Setting up electron ID code"
git clone https://github.com/Werbellin/RecoEgamma_8X RecoEgamma

echo "Setting up ZZ matrix element stuff"
git clone https://github.com/cms-analysis/HiggsAnalysis-ZZMatrixElement.git ZZMatrixElement
pushd ZZMatrixElement
git checkout -b from-master master
popd

popd