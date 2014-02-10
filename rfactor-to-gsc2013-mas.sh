#!/bin/bash

set -e

mas2files="./masunpack.py"
files2mas="./maspack.py"

for MASFILE in "$@"; do
  TMPDIR="/tmp/rfactor-$(uuidgen)/"

  "${mas2files}" "${MASFILE}" "${TMPDIR}"
  find "${TMPDIR}" -type f -exec ./rfactorcrypt.py -e {} {} \;
  "${files2mas}" "${TMPDIR}" "${MASFILE}"
  rm -rfv "$TMPDIR"
done

# EOF #
