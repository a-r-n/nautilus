#!/bin/sh

cp test262/harness/assert.js ./repl.js
cat test262/harness/sta.js >> ./repl.js

cat $1 >> ./repl.js
../qjsc -c repl.js -o ../repl.c
#rm ./repl.js
