#!/bin/sh

# 10 seconds play PRBS
./prbs 3840000 | ./aduplex both | ./first-stage | ./addup
