#!/bin/sh

bison -d regex.y
flex regex.l
gcc lex.yy.c regex.tab.c -ly -ll -o regex.out
