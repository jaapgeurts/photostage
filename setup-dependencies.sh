#!/bin/bash

#curl --http1.1 -L https://github.com/halide/Halide/releases/download/release_2018_02_15/halide-linux-64-gcc53-trunk-46d8e9e0cdae456489f1eddfd6d829956fc3c843.tgz --output /tmp/halide.tgz

#(cd PhotoStage/external; tar -zxf /tmp/halide.tgz)

#rm /tmp/halide.tgz

(cd PhotoStage/external; git clone https://github.com/darktable-org/rawspeed.git)



