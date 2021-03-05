# Ghost_imaging_parser
c++ app for parsing pulsed TimePix 3 csv data into TOT matrices


compile with:
g++ ghost_imaging_parser.cpp -o gip.out

use with:
./gip.out -f filename.csv

This should produce frames_filename.txt


Use GI_parser.py library to load the frames_filename.txt file into python

Example shown in GIP_test.ipynb




.csv data is available here: https://drive.google.com/drive/folders/1FtosbDEydRCcJLZoqe4Fi9XfIuBWffzh?usp=sharing
