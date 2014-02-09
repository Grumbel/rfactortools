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

  find "${DIR}" \
    -iname "*.gmt" \
    -exec \
      ./rfactordec -o -e -s 4b1dca9f960524e8 {} {} \;

  find "${DIR}" \
    -iname "*.mas" \
    -exec \
     ./rfactor-to-gsc2013-mas.sh {} \;
done

# EOF #
