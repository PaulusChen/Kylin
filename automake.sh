#!/bin/sh

ProjectRootDir=`pwd`
buildDir="build"

./DevMode.sh

MakeBuildDir()
{
    currentDir=${buildDir}"/"${1}

    if [ ! -d ${currentDir} ]
    then
       mkdir -p ${currentDir}
    fi

    cd ${currentDir}
    cmake -DCMAKE_BUILD_TYPE=${1} ../../
    make
    cp vlss_deamon ${ProjectRootDir}/vlss_deamon_${1}
    cd ${ProjectRootDir}
}

if [ ${1}x = "clean"x ]
then
    echo "Removeing generated files..."
    rm -rf build vlss_deamon_*
    exit 0
fi

MakeBuildDir Debug
MakeBuildDir Release