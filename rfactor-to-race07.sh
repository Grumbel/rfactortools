#!/bin/bash

set -e

mas2files="./MAS2Files.exe"

for MASFILE in "$@"; do
  OUTDIR="${MASFILE%.*}"

  "${mas2files}" "${MASFILE}" "${OUTDIR}"
done

# EOF #
