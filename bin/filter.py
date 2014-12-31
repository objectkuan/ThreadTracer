#!/usr/bin/python

import sys
import re

fin = sys.argv[1]
tids = map(int, sys.argv[2:])

fp = open(fin, "r")
fout = open("out.txt", "w")
lines = [ line for line in fp ]
for line in lines[0:11]:
    fout.write(line)

shown = False
for line in lines[11:]:
    m = re.match( r'\s+.+-(?P<pid>\d+).+\n' , line)
    if m:
        _pid = int(m.group("pid"))
        shown = (_pid == tids[0] or _pid == tids[1] or _pid == tids[2])

    if shown:
        fout.write(line)

fp.close()
fout.close()
