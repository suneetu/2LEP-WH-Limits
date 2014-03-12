#!/bin/bash

# ModeC
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","all","SMCwslep","NoSys","",5)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","all","SMCwslep","NoSys","",5)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","all","SMCwslep","NoSys","",5)'

# Slepton
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","all","SparseDLiSlep","NoSys","",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","all","SparseDLiSlep","NoSys","",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","all","SparseDLiSlep","NoSys","",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","ee","SparseDLiSlep","NoSys","",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","ee","SparseDLiSlep","NoSys","",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","ee","SparseDLiSlep","NoSys","",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","mm","SparseDLiSlep","NoSys","",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","mm","SparseDLiSlep","NoSys","",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","mm","SparseDLiSlep","NoSys","",14)'

# Slepton RH
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","all","SparseDLiSlep","NoSys","ROnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","all","SparseDLiSlep","NoSys","ROnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","all","SparseDLiSlep","NoSys","ROnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","ee","SparseDLiSlep","NoSys","ROnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","ee","SparseDLiSlep","NoSys","ROnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","ee","SparseDLiSlep","NoSys","ROnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","mm","SparseDLiSlep","NoSys","ROnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","mm","SparseDLiSlep","NoSys","ROnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","mm","SparseDLiSlep","NoSys","ROnly",14)'

# Slepton LH
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","all","SparseDLiSlep","NoSys","LOnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","all","SparseDLiSlep","NoSys","LOnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","all","SparseDLiSlep","NoSys","LOnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","ee","SparseDLiSlep","NoSys","LOnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","ee","SparseDLiSlep","NoSys","LOnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","ee","SparseDLiSlep","NoSys","LOnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4a","mm","SparseDLiSlep","NoSys","LOnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4b","mm","SparseDLiSlep","NoSys","LOnly",14)'
root -l -b -q 'macros/makeUpperLimitFiles.C("SR4c","mm","SparseDLiSlep","NoSys","LOnly",14)'