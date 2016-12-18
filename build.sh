#!/bin/sh
gcc -std=c99 -lm -lssl -lcrypto puzzle.c -o puzzle && cp puzzle /bin/puzzle
