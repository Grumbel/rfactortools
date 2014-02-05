#!/bin/bash

set -e

for DIR in "$@"; do
  find "${DIR}" \
    -iname "*.gdb" \
    -exec \
      sed -i "s/Filter Properties *=.*/Filter Properties = StockV8 \\*/" {} \;
done

# EOF #
