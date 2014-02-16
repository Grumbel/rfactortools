#!/bin/bash

set -e

for DIR in "$@"; do
  find "${DIR}" \
    -iname "*.gdb" \
    -exec \
      sed -i "s/Filter Properties *=.*/Filter Properties = StockV8 \\*/" {} \;

  find "${DIR}" \
    -iname "*.veh" \
    -exec \
       sed -i 's/Classes="/Classes="reiza5, /' {} \;

  ./imgtool.py "$DIR"

  find "${DIR}" \
    -iname "*.gmt" \
    -exec \
      ./rfactorcrypt.py -e {} \;

  find "${DIR}" \
    -iname "*.mas" \
    -exec \
     ./rfactor-to-gsc2013-mas.sh {} \;
done

# EOF #
