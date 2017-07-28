#!/bin/bash
####
####    Set up a virtual environment with Rootpy, IPython, and Jupyter.
####    Installs things the first time it runs, just activates the virtualenv
####    thereafter.
####
####    Rootpy: Pythonic wrappers for PyROOT
####    IPython: Nicer interactive Python (already in CMS software but broken)
####    Jupyter: IPython notebooks
####    BRIL Work Suite: tools for luminosity (etc.)
####
####    Author: Nate Woods, U. Wisconsin
####


if [ "$CMSSW_BASE" == "" ]; then
    echo "Please do this only in a CMSSW environment. Did you cmsenv?"
    return 2
fi

if [ ! -d "$CMSSW_BASE"/src/UWVV/recipe/virtualenv ]; then
    echo "UWVV doesn't look like it's set up in the usual way. Aborting."
    return 2
fi

if [ -z "$(ls -A $CMSSW_BASE/src/UWVV/recipe/virtualenv)" ]; then
    echo "It looks like the virtualenv package is empty. Did you forget --recursive?"
    echo "If you did, fix it with "
    echo "    git submodule update --init --recursive"
    return 2
fi

export vpython="$CMSSW_BASE"/src/UWVV/recipe/vpython

if [ ! -d "$vpython" ]; then
    python "$CMSSW_BASE"/src/UWVV/recipe/virtualenv/virtualenv.py "$vpython"

    source "$vpython"/bin/activate

    pip install -U pip
    pip install -U rootpy
    pip install -U ipython
    pip install -U jupyter
    pip install brilws
else
    source "$vpython"/bin/activate
fi

export PYTHONPATH="$vpython"/lib/python2.7/site-packages/:"$PYTHONPATH"
