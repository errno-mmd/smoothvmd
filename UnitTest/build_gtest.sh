#!/bin/bash

gtestdir=/usr/src/googletest/googletest

mkdir gtestlib
cd gtestlib
cmake $gtestdir && make
