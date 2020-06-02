#!/bin/sh

rm -f ./repl.js

if echo $1 | grep test262; then
	cp test262/harness/assert.js ./repl.js
	cat test262/harness/sta.js >> ./repl.js
fi

cat $1 >> ./repl.js
../qjsc -c repl.js -o ../repl.c
#rm ./repl.js
