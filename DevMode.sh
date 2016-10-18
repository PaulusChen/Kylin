#!/bin/sh

ln -s /usr/include ./SysLibInclude
ln -s /usr/local/include ./SysLocLibInclude
ln -s ../KylinLibs/ ./libs

cmake -DCMAKE_BUILD_TYPE=Debug .

cscope-indexer -r

