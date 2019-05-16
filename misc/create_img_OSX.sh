#!/bin/bash

echo "Removing old kernel.img..."
rm /Volumes/boot/kernel.img
echo "Copying new kernel.img..."
cp ../kernel.img /Volumes/boot/kernel.img
echo "Done"