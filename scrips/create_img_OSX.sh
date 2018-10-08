#!/bin/bash

echo "Generating jonOS.img base..."
cat ../misc/raspbian/x?? > ../jonOS.img
echo "Mounting image..."
hdiutil mount ../jonOS.img
echo "Removing old kernel.img..."
rm /Volumes/boot/kernel.img
echo "Copying new kernel.img..."
cp ../kernel.img /Volumes/boot/kernel.img
echo "Unmounting boot/ volumen..."
hdiutil unmount /Volumes/boot/