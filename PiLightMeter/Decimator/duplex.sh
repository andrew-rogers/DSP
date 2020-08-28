#!/bin/sh

# 10 seconds play PRBS
./prbs 3840000 | ./aplay &

# Record for 10 seconds
./acapture 3840000 | ./first-stage | ./addup
