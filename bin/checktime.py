#!/usr/bin/python

lines = [ i for i in open("data") ]

last = 0
for line in lines:
	this = int(line[10:21])
	if this < last:
		print "oops"
	last = this
