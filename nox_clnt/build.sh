#!/bin/bash

checkrc()
{
    local rc=$1
    shift
    if [ $rc -eq 0 ]; then
        echo "$@ passed"
    else
        echo "$@ failed"
    fi

    return $rc
}

self=$(readlink -f $0)
rtt_path=$(dirname $self)
client_path=$rtt_path/rtt_client
lib_path=$client_path/lib
server_path=$rtt_path/radio_test_tool
build_path=$client_path/build

## build rtt server
cd $server_path
./build.sh
checkrc $? "build rtt server" || exit 1

## build rtt client
mkdir -p $build_path
cd $build_path
cp -rf $client_path/META-INF $client_path/testyeelight $build_path

cp $server_path/java/rtt.jar $server_path/java/librtt_interface.so $client_path/lib

libs="."
for lib_jar in $lib_path/*jar; do
    libs=$libs:$lib_jar
done

cd $build_path/testyeelight
javac -cp $libs *.java
checkrc $? "compile java source code" || exit 1
cd $build_path
rm -rf */*java
jar cvfm ../testYeelight.jar META-INF/MANIFEST.MF testyeelight/ META-INF/
checkrc $? "pack testYeelight.jar" || exit 1
