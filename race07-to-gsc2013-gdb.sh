#!/bin/bash

set -e

for DIR in "$@"; do
  find "${DIR}" -type f -iname "*.trk" -exec rename "s/.[Tt][Rr][Kk]\$/.scn/" {} \;
  
done

# EOF #
