#!/bin/bash -e

bin/halfToEdges "${1}"
bin/bsort -k 4 -r 8 "${1}.rev.bin"
bin/makeLists "${1}"
rm "${1}.bin" "${1}.rev.bin" "${1}.raw.degree.bin"
bin/sortLists "${1}"
bin/relabel "${1}"
