#!/bin/bash

set -eu

seed=$(date "+%Y%m%d")
date=$(date "+%Y-%m-%d")

title=$(./levelname.py $seed)

echo "Seed: $seed"
rm -f slige.wad megawad.wad megawad.zip
./tools/slige.amd64 -seed $seed -config SLIGE.CFG -doom2 -levels 32 -outfile slige.wad
./tools/bsp.amd64 -blockmap comp slige.wad -o megawad.wad

sed "s/__TITLE__/$title/; s/__DATE__/$date/" < megawad.txt.templ > megawad.txt

zip megawad.zip megawad.wad megawad.txt
chmod a+r megawad.zip
exit 0

