#!/bin/bash

set -e

mas2files="./masunpack.py"
files2mas="./maspack.py"

for MASFILE in "$@"; do
  TMPDIR="/tmp/rfactor-$(uuidgen)/"

  "${mas2files}" "${MASFILE}" "${TMPDIR}"
  find "${TMPDIR}" -type f -exec ./rfactordec -s 4b1dca9f960524e8 -e -o {} {} \;
  "${files2mas}" "${TMPDIR}" "${MASFILE}"
  rm -rfv "$TMPDIR"
done

# EOF #
