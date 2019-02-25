#!/bin/bash
make clean
make
python3 extractor.py
python3 sender.py $1