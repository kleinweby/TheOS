#!/bin/bash

# Find base dir
pushd `dirname $0` > /dev/null
BASEDIR=`pwd`
popd > /dev/null

pushd "$BASEDIR"
pushd "CoreSystem"
rake CoreSystem.framework libCoreSystem-kernel.a || exit 1
popd
pushd "Kernel"
rake || exit 1
popd
popd
