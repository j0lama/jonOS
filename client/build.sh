#!/bin/bash
make clean
make
python3 extractor.py
python3 sender.py 192.168.1.123 1234