#!/bin/bash -e

#make labels and the new label form
if test $# -ne 1
then
  echo "usage: ./convert.sh <dir containing snap.txt>" 1>&2
  exit 1
fi

dir="${1}"
fname="${dir}/snap.txt"

make

#:<<END
if ! test -f "${fname}"
then
  for webgraph in ${dir}/*.properties
  do
    echo
    date
    printf "\nWEBGRAPH TO SNAP\n"
    java -cp $(echo ~/common/approxg/webgraph/*.jar|tr " " ":") it.unimi.dsi.webgraph.ArcListASCIIGraph -g BVGraph "${webgraph//.properties/}" "${fname}" #|| true
  done
fi
#END

echo
date
printf "\nSNAP TO BIN\n"
bin/snapToBin "${fname}"
#rm "${fname}"

# NOTE fname.bin and fname.rev.bin are BIG-ENDIAN

echo
date
printf "\nSORT REVERSE\n"
#bin/bsort -k 4 -r 8 -v "${fname}.rev.bin"

echo
date
printf "\nMAKE LISTS\n"
#bin/makeLists "${fname}"
rm "${fname}.bin" "${fname}.rev.bin" "${fname}.raw.degree.bin"

echo
date
printf "\nCOMPACTIFY\n"
#bin/compactify "${fname}"
rm "${fname}.raw.edge.bin" "${fname}.raw.vertex.bin"

bin/labelize "${fname}"

echo
date
printf "\nCOMPACTIFYLABEL\n"
bin/compactifylabel "${fname}"



echo
date
printf "\nHALVE\n"
#bin/halve "${fname}"

echo
date
printf "\nRELABEL\n"
#bin/relabel "${fname}"

