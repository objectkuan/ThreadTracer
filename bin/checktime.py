#!/usr/bin/python
import sys
import re

last = 0
pattern = re.compile(r".+\s(\d+\.[^:]+):.+")
for line in open(sys.argv[1]):
#for line in [ " 1.2:" ]:
	m = pattern.match(line)
	if m:
		time = m.groups()[0]
		time = time.replace(".", "")
		this = int(time)
		if this < last: 
			print "oops", this, last
		last = this
		print time
	else:
		print line
