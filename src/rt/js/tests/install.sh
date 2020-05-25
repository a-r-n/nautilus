#!/bin/sh

cp $1 ./repl.js
../qjsc -c repl.js -o ../repl.c
rm ./repl.js
