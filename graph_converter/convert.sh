#!/bin/bash -e

if test $# -ne 1
then
  echo "usage: ./convert.sh <dir containing snap.txt>" 1>&2
  exit 1
fi

dir="${1}"
fname="${dir}/snap.txt"

make

:<<END
# to convert from webgraph, download it from http://webgraph.di.unimi.it/ and set the path below
webgraph_path=""
if ! test -f "${fname}"
then
  for webgraph in ${dir}/*.properties
  do
    echo
    date
    printf "\nWEBGRAPH TO SNAP\n"
    java -cp $(echo "${webgraph_path}"/*.jar|tr " " ":") it.unimi.dsi.webgraph.ArcListASCIIGraph -g BVGraph "${webgraph//.properties/}" "${fname}"
  done
fi
END

echo
date
printf "\nSNAP TO BIN\n"
bin/snapToBin "${fname}"
rm "${fname}"

# NOTE fname.bin and fname.rev.bin are BIG-ENDIAN

echo
date
printf "\nSORT REVERSE\n"
bin/bsort -k 4 -r 8 -v "${fname}.rev.bin"

echo
date
printf "\nMAKE LISTS\n"
bin/makeLists "${fname}"
rm "${fname}.bin" "${fname}.rev.bin" "${fname}.raw.degree.bin"

echo
date
printf "\nCOMPACTIFY\n"
bin/compactify "${fname}"
rm "${fname}.raw.edge.bin" "${fname}.raw.vertex.bin"

echo
date
printf "\nHALVE\n"
bin/halve "${fname}"

echo
date
printf "\nLABELIZE\n"
bin/labelize "${fname}"

#echo
#date
#printf "\nRELABEL\n"
#bin/relabel "${fname}"

