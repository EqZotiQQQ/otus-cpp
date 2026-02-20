#!/bin/sh
BINARY_HOME=./build/bin
INPUT=/otus_hws/hw12/assets/AB_NYC_2019.csv

cat ${INPUT} | ${BINARY_HOME}/mapper | sort -k1 | ${BINARY_HOME}/reducer > output

cat output