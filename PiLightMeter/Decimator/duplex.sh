#!/bin/sh

# 10 seconds play PRBS
./prbs 3840000 | aplay -D plughw:1 -fS16_LE -c2 -r96000 &

# Record for 10 seconds
./acapture 3840000 | ./first-stage | ./addup
