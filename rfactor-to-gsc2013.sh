#!/bin/bash

set -e

mas2files="./MAS2Files.exe"
files2mas="./Files2MAS.exe"

for MASFILE in "$@"; do
  TMPDIR="/tmp/rfactor-$(uuidgen)/"

  "${mas2files}" "${MASFILE}" "${TMPDIR}"
  find "${TMPDIR}" -type f -exec ./rfactordec -s 4b1dca9f960524e8 -e -o {} {} \;
  "${files2mas}" "${TMPDIR}" "${MASFILE}.new"
  rm -rfv "$TMPDIR"
done

# EOF #
