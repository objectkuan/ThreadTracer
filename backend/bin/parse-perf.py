#!/usr/bin/python

import sys
import re
import operator

fin = sys.argv[1]
objs = sys.argv[2]
binfile = sys.argv[3]

# Parsing objects
valid_objs = ["func", "trace", "corr"]
objs = objs.split(',')
objs = [ val for val in objs if val in valid_objs ]

# Patterns
pat_event = r"(.*)\s(\d+)\s+(\d+)\.(\d+):\s*(\d+)\s[^:]+:"
pat_function = r"\s*([a-f0-9]*)\s([^\s]*)\s\((.*)\)"
pat_function = r"\s*([a-f0-9]+)\s(.*)\s\((.*)\)"

# Managing time
total_times = 0

start_time = sys.maxint
interval = 5000000
interval_stat = {}
last_times_5s = 0
last_period = 0

# Top of stack
skipping = False
interval_stat["func"] = []

# Call trace
current_trace = ""
interval_stat["trace"] = []

# Correlation
last_func = ""
interval_stat["corr"] = []

skipped_position_keywords = ["vmlinux", "libc", "/usr/bin/bash", "glib", "libdbus", "libgconf", "ld-"]
skipped_function_keywords = ["[unknown]"]

for line in open(fin):
	prog = re.compile(pat_event)
	mat = prog.match(line)
	if mat:
		tname = mat.group(1)
		tid = int( mat.group(2) )
		time_1 = int( mat.group(3) )
		time_2 = int( mat.group(4) )
		period = int( mat.group(5) )
		
		total_times = total_times + 1
		time = time_1 * 1000000 + time_2

		times_5s = time / interval
		if times_5s > last_times_5s:
			from_time = times_5s * interval
			to_time = from_time + interval
			interval_stat["func"].append( { "from": from_time, "to": to_time, "list": {} } )
			interval_stat["trace"].append({ "from": from_time, "to": to_time, "list": {} } )
			interval_stat["corr"].append( { "from": from_time, "to": to_time, "list": {} } )
			last_times_5s = times_5s
			last_period = last_period + 1

		# Top of stack
		skipping = False

		# Call trace
		current_trace = ""

		# Correlation
		last_func = ""

		continue
	
	prog = re.compile(pat_function)
	mat = prog.match(line)
	if mat:
		address = mat.group(1)
		function = mat.group(2)
		position = mat.group(3)

		#if not binfile in position:
		#	continue
		skipped = False
		for kw in skipped_position_keywords:
			if kw in position:
				skipped = True
				break
		for kw in skipped_function_keywords:
			if kw in function:
				skipped = True
				break
		if skipped:
			continue
		# Top of stack
		if not skipping:
			func_stat = interval_stat["func"][-1]["list"]
			if function in func_stat:
				func_stat[function] = func_stat[function] + 1
			else:
				func_stat[function] = 1

			skipping = True

		# Call trace
		current_trace = function + "::" + current_trace

		# Correlation
		if last_func.strip():
			calling = last_func + "::" + function
			corr_stat = interval_stat["corr"][-1]["list"]
			if calling in corr_stat:
				corr_stat[calling] = corr_stat[calling] + 1
			else:
				corr_stat[calling] = 1
		last_func = function
		continue

	if not line.strip():
		if len(current_trace.strip()) > 0:
			trace_stat = interval_stat["trace"][-1]["list"]
			if current_trace in trace_stat:
				trace_stat[current_trace] = trace_stat[current_trace] + 1
			else:
				trace_stat[current_trace] = 1

#print len(interval_stat["func"] )
#print len(interval_stat["corr"] )
#print len(interval_stat["trace"] )

#for item in interval_stat["func"]:
#	print "biu"
#	print item["from"], item["list"]
#exit()
#func_stat = interval_stat["func"][-1]["list"]
#corr_stat = interval_stat["corr"][-1]["list"]
#trace_stat = interval_stat["trace"][-1]["list"]

# Sorting and dumping
def print_func_stat(sorted_func_stat):
	counter = 0
	for item in sorted_func_stat:
		function = item[0]
		function_times = item[1]
		percentage = function_times * 100 / total_times
		counter = counter + 1
		print "{} {:16} {:8.2f}%".format(counter, function_times, percentage)
		print "\t", function
		if counter >= 10:
			break

def print_trace_stat(sorted_trace_stat):
	counter = 0
	for item in sorted_trace_stat:
		trace = item[0]
		trace_times = item[1]
		percentage = trace_times * 100 / total_times
		functions = trace.split("::")
		counter = counter + 1
		print "{} {:16} {:8.2f}% ".format(counter, trace_times, percentage)
		taken_out_functions = reversed(functions[:-1])
		for func in taken_out_functions:
			print "\t", func
		if counter >= 10:
			break

def print_corr_stat(sorted_corr_stat):
	counter = 0
	for item in sorted_corr_stat:
		calling = item[0]
		call_times = item[1]
		percentage = call_times * 100 / total_times
		functions = calling.split("::")
		counter = counter + 1
		print "{} {:16} {:8.2f}%".format(counter, call_times, percentage)
		print "\t", functions[0] + " <- " + functions[1]
		if counter >= 10:
			break

print_stat = { "func": print_func_stat, "trace": print_trace_stat, "corr": print_corr_stat }
for key, list_over_time in interval_stat.iteritems():
	if not key in objs:
		continue
	print "+=========", key, "=========="
	for item_over_time in enumerate(list_over_time):
		index = item_over_time[0]
		item = item_over_time[1]
		stat = item["list"]
		item["list"] = reversed(sorted(stat.items(),  key=operator.itemgetter(1)))
		sus = 1000000
		print "+--- {}.{} --- {}.{} ---".format(item["from"] / sus, item["from"] % sus, item["to"] / sus, item["to"] % sus)
		print_stat[key](item["list"])
		print "+--- end ---"
	print "\n\n\n\n\n\n"


