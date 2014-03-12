#!/bin/bash

# Mode C
python macros/getSys.py SR4a SMCwslep tot
sleep 0.5s
python macros/getSys.py SR4b SMCwslep tot
sleep 0.5s
python macros/getSys.py SR4c SMCwslep tot
sleep 0.5s

# Slepton Grid
python macros/getSys.py SR4a SparseDLiSlep tot
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep tot
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep tot
sleep 0.5s

# R Only
python macros/getSys.py SR4a SparseDLiSlep ROnly tot
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep ROnly tot
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep ROnly tot
sleep 0.5s

# L Only
python macros/getSys.py SR4a SparseDLiSlep LOnly tot
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep LOnly tot
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep LOnly tot
sleep 0.5s

# Selectron Grid
python macros/getSys.py SR4a SparseDLiSlep ee
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep ee
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep ee
sleep 0.5s

# R Only
python macros/getSys.py SR4a SparseDLiSlep ROnly ee
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep ROnly ee
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep ROnly ee
sleep 0.5s

# L Only
python macros/getSys.py SR4a SparseDLiSlep LOnly ee
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep LOnly ee
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep LOnly ee
sleep 0.5s

# Smuon Grid
python macros/getSys.py SR4a SparseDLiSlep mm
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep mm
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep mm
sleep 0.5s

# R Only
python macros/getSys.py SR4a SparseDLiSlep ROnly mm
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep ROnly mm
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep ROnly mm
sleep 0.5s

# L Only
python macros/getSys.py SR4a SparseDLiSlep LOnly mm
sleep 0.5s
python macros/getSys.py SR4b SparseDLiSlep LOnly mm
sleep 0.5s
python macros/getSys.py SR4c SparseDLiSlep LOnly mm
sleep 0.5s
