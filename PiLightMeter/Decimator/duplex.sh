#!/bin/sh

# 10 seconds play noise
dd if=/dev/urandom bs=512 count=16000 | aplay -D plughw:1 -fS16_LE -c2 -r96000 &

# Record for 10 seconds
arecord -D plughw:1 -fS16_LE -c2 -r96000 -d10 -traw | ./first-stage | ./addup
