#!/bin/bash
gcc -c function.c
python3 extractor.py
python3 sender.py
rm function.o