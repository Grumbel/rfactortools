#!/bin/bash

set -e

mas2files="./MAS2Files.exe"
files2mas="./Files2MAS.exe"

for MASDIR in "$@"; do
  find "${MASDIR}" -type f -exec ./rfactordec -s 4b1dca9f960524e8 -e -o {} {} \;
  "${files2mas}" "${MASDIR}" "${MASDIR}.mas"
done

# EOF #
