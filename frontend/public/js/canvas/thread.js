var threads = [];
var pidToIndex = {};
var filteredThreads = [];
var filteredPidToIndex = {};
var initTimeus = 0;
var usPerPixel = 50000;
var threadHeight = 30;

var TSTATE = (function() {
     var private = {
     	'INIT': 0,
        'SLEEPING': 1,
        'RUNNABLE': 2.
     };

     return {
        v: function(name) { return private[name]; }
    };
})();
var LEFTMOST = 130;

function indexToY(index) { return index * 40 + 50; }
function timeToX(timeus) { return (timeus - initTimeus) / usPerPixel + LEFTMOST; }


function resetThreads() {
	clearCanvas();
	threads = [];
	filteredThreads = [];
	pidToIndex = {};
	filteredPidToIndex = {};
	initTimeus = 0;
}

function setInitTime(time) { 
	initTimeus = time;
	for (var i = 0; i < threads.length; i++) {
		threads[i].end = initTimeus;
		threads[i].timeout = false;
	}
	clearCanvas();
	console.log("Set init time ", initTimeus);
}

function hasThread(pid) {
	return (pid in pidToIndex);
}

function hasFilteredThread(pid) {
	return (pid in filteredPidToIndex);
}

function pushFiltedThread(pid) {
	if (hasFilteredThread(pid)) return;
	filteredPidToIndex[pid] = filteredThreads.length;
	filteredThreads.push(pid);
}

function pushThread(pid, name, time) {
	if (hasThread(pid) || hasFilteredThread(pid)) return;
	if (!time) time = initTimeus;
	var thread = { 
		"pid": pid, 
		"name": name , 
		"index": threads.length, 
		"end": time,
		"timeout": false,
		"state": TSTATE.v('INIT'),
		"lastEvent": ""
	};
	threads.push(thread);
	if(thread.name) drawText(10, indexToY(thread.index) - 7, 15, thread.name);
	drawText(10, indexToY(thread.index) + 10, 12, thread.pid);
	pidToIndex[thread.pid] = thread.index;
}

function nameThread(pid, name, timeus) {
	var thread = threads[pidToIndex[pid]];
	if(thread == undefined) return;
	if(thread.name) return;
	thread.name = name;
	drawText(10, indexToY(thread.index) - 7, 15, thread.name);
}

function runTo(pid, timeus) {
	var thread = threads[pidToIndex[pid]];
	if(thread == undefined) return;
	assert(timeus >= thread.end, "Time reverse");
	drawSolidLine(timeToX(thread.end), indexToY(thread.index), timeToX(timeus), indexToY(thread.index) );
	// console.log(timeToX(thread.end), indexToY(thread.index), timeToX(timeus), indexToY(thread.index) );
	thread.end = timeus;
	thread.timeout = false;
}

function sleepTo(pid, timeus) {
	var thread = threads[pidToIndex[pid]];
	if(thread == undefined) return;
	assert(timeus >= thread.end, "Time reverse");
	drawDotLine(timeToX(thread.end), indexToY(thread.index), timeToX(timeus), indexToY(thread.index) );
	// console.log(timeToX(thread.end), indexToY(thread.index), timeToX(timeus), indexToY(thread.index) );
	thread.end = timeus;
}

function timeOutTo(pid, timeus) {
	var thread = threads[pidToIndex[pid]];
	if(thread == undefined) return;
	assert(timeus >= thread.end, "Time reverse");
	drawDotLine(timeToX(thread.end), indexToY(thread.index), timeToX(timeus), indexToY(thread.index) );
	// console.log(timeToX(thread.end), indexToY(thread.index), timeToX(timeus), indexToY(thread.index) );
	drawDownTriangle(timeToX(timeus), indexToY(thread.index) - 7);
	thread.end = timeus;
	thread.timeout = true;
}

function wakeUp(fpid, tpid, timeus) {
	var fromThread = threads[pidToIndex[fpid]];
	var toThread = threads[pidToIndex[tpid]];
	if (toThread == undefined)
		return;
	if (fromThread != undefined && fromThread.pid == toThread.pid)
		return;
	
	if (timeus - toThread.end > 1000000 || toThread.timeout) {
		if (fromThread == undefined)
			drawArrow(timeToX(timeus), indexToY(toThread.index) - 20, timeToX(timeus), indexToY(toThread.index) );
		else 
			drawArrow(timeToX(timeus), indexToY(fromThread.index), timeToX(timeus), indexToY(toThread.index) );
		toThread.timeout = false;
	}
}

function wakeUpNew(fpid, tpid, timeus) {
	var fromThread = threads[pidToIndex[fpid]];
	var toThread = threads[pidToIndex[tpid]];
	if (toThread == undefined) 
		return;
	drawArrow(timeToX(timeus), indexToY(fromThread.index), timeToX(timeus), indexToY(toThread.index), '#332');
}

function exitAt(timeus) {

}

function setThreadLastEvent(pid, event) {
	var thread = threads[pidToIndex[pid]];
	if(thread == undefined) return;
	thread.lastEvent = event;
}

function getThreadLastEvent(pid) {
	var thread = threads[pidToIndex[pid]];
	if(thread == undefined) return null;
	return thread.lastEvent;
}


// Perf data
var perfStat = {};

function initPerfStat(statType) {
	perfStat[statType] = [];
}

function pushPerfStat(statType, statItem) {
	perfStat[statType].push(statItem);
}

function tryDump() {
	console.log(perfStat);
}

// Interaction
function initInteraction() {
	addClickListener(function(x, y) {
		console.log(x, y);
		
		var _x = x - LEFTMOST;
		if (_x < 0) return;
		var pixelPerPeriod = 5000000 / usPerPixel;
		var period = Math.floor(_x / pixelPerPeriod);
		console.log("Period: " + period);
		var maskLeft = period * pixelPerPeriod + LEFTMOST;
		var maskRight = maskLeft + pixelPerPeriod;

		// Draw shadow
		restoreCanvas();
		drawTallRectangle(maskLeft, maskRight);

		// Draw Text
		var statTypes = ["func", "corr", "trace"];
		for (var index in statTypes) {
			var statType = statTypes[index];
			if (statType in perfStat) {
				console.log("Playing with " + statType);
				listOverTime = perfStat[statType];
				for (var i in listOverTime) {
					var itemOverTime = listOverTime[i];
					var fromTimeX = timeToX(itemOverTime.from * 1000000);
					var toTimeX = timeToX(itemOverTime.to * 1000000);
					console.log(itemOverTime.from, fromTimeX, x, toTimeX);
					if (x > fromTimeX && x < toTimeX) {
						var itemsInPeriod = itemOverTime.itemsInPeriod;
						var content = "";
						for (var j in itemsInPeriod) {
							var itemInPeriod = itemsInPeriod[j];
							content = content + itemInPeriod.head + "\n";
							for (var k in itemInPeriod.contents) {
								content = content + itemInPeriod.contents[k] + "\n";
							}
						}
						$("#text-" + statType).val(content);
						break;
					}
				}
			}
		}
	});
}

function processEventLine(line, setInitTimeForFirst) {
		// Matching
		var argvs = line.split(' ');
		var timestamp = parseInt(argvs[0]);
		var tracedEvent = argvs[1];
		var pid = parseInt(argvs[2]);

		if (setInitTimeForFirst) {
			setInitTime(Math.floor(timestamp / 5000000) * 5000000);
		}

		// Skip the nonsense threads
		switch (tracedEvent) {
			case "CREATE_WAKEUP":
			case "ENTER_FUTEX":
			case "EXIT_FUTEX":
			case "ENTER_POLL":
			case "EXIT_POLL":
			case "SLEEP":
			case "EXIT":
			case "NAME_THREAD":
			case "EXIT_FUTEX_TIMEOUT":
				if (!hasThread(pid)) 
					return;
				break;
			case "WAKEUP":
				var tpid = parseInt(argvs[3]);
				if (!hasThread(tpid)) 
					return;
				break;
			default:
				console.log("Unknown evnet: " + line);
				break;
		}

		switch (tracedEvent) {
			case "CREATE_WAKEUP":
				var cpid = parseInt(argvs[3]);
				pushThread(cpid, "", timestamp);
				wakeUpNew(pid, cpid, timestamp);
				break;
			case "ENTER_FUTEX":
				var futex = argvs[3];
				runTo(pid, timestamp);
				setThreadLastEvent(pid, tracedEvent);
				break;
			case "EXIT_FUTEX":
				sleepTo(pid, timestamp);
				break;
			case "EXIT_FUTEX_TIMEOUT":
				timeOutTo(pid, timestamp);
				break;
			case "ENTER_POLL":
				var poll = argvs[3];
				runTo(pid, timestamp);
				setThreadLastEvent(pid, tracedEvent);
				break;
			case "EXIT_POLL":
				sleepTo(pid, timestamp);
				break;
			case "WAKEUP":
				var tpid = parseInt(argvs[3]);
				var lastThreadEvent = getThreadLastEvent(tpid);
				if (lastThreadEvent == "ENTER_FUTEX" || lastThreadEvent == "ENTER_POLL") {
					wakeUp(pid, tpid, timestamp);
					sleepTo(tpid, timestamp);
				} else if (lastThreadEvent == "SLEEP") {
					wakeUp(pid, tpid, timestamp);
				}
				break;
			case "SLEEP":
				runTo(pid, timestamp);
				setThreadLastEvent(pid, tracedEvent);
				break;
			case "EXIT":
				exitAt(pid, timestamp);
				break;
			case "NAME_THREAD":
				var threadName = argvs[3];
				nameThread(pid, threadName, timestamp);
				break;
			default:
				console.log("Unknown evnet: " + line);
				break;
		}

	}