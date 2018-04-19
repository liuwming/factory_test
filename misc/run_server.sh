#!/bin/bash
set -x

self=$(readlink -f $0)
rtt_path=$(dirname $self)
rtt_path=$(dirname $rtt_path)

server_path=$rtt_path/radio_test_tool
cd $server_path
sudo ./run.sh


