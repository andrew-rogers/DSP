#!/bin/sh

# 10 second sine wave of 1kHz
./sin | ./int8 | ./aduplex

