#!/bin/bash

dir=$(dirname $(realpath $0))
actual_date=$(date "+%Y-%m-%d-%H%M")
script_name=competitive
amxmodx_dir=cstrike/addons/amxmodx

mkdir -p $dir/$amxmodx_dir/scripting
mkdir $dir/$amxmodx_dir/plugins

cp -r $dir/configs $dir/$amxmodx_dir/
cp -r $dir/data $dir/$amxmodx_dir/
cp -r $dir/sprites $dir/cstrike/
cp -r $dir/pugconfig.cfg $dir/cstrike/

cd $dir/scripting
./amxxpc $script_name.sma

mv $script_name.amxx $dir/$amxmodx_dir/plugins/

cd $dir
zip -r build-$actual_date.zip cstrike
rm cstrike -r

echo "Compilado y comprimido!"

