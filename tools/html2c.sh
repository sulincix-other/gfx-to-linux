#!/bin/sh -e
set -o pipefail
in=$(realpath "$1")
name=${in/.*/}
name=${name/*\//}
echo "$name"
out="$2"
echo "const char* $name = \" \\" > "$out"
cat "$in" | sed "s/\\\\/\\\\\\\\/g" | sed "s|\"|\\\\\"|g" | sed "s/$/\\\\n\\\\/g" >> "$out"
echo "\";" >> "$out"
