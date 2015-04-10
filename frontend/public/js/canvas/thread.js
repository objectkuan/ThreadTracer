var threads = [];
var pidToIndex = {};
var filteredThreads = [];
var filteredPidToIndex = {};
var initTimeus = 0;
var usPerPixel = 50000;

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

function indexToY(index) { return index * 50 + 50; }
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
	}
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

function pushThread(pid, name) {
	if (hasThread(pid) || hasFilteredThread(pid)) return;
	var thread = { 
		"pid": pid, 
		"name": name , 
		"index": threads.length, 
		"end": initTimeus,
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
}

function wakeUp(fpid, tpid, timeus) {
	var fromThread = threads[pidToIndex[fpid]];
	var toThread = threads[pidToIndex[tpid]];
	if (toThread == undefined)
		return;
	if (fromThread != undefined && fromThread.pid == toThread.pid)
		return;
	
	if (timeus - toThread.end > 1000000) {
		if (fromThread == undefined)
			drawArrow(timeToX(timeus), indexToY(toThread.index) - 20, timeToX(timeus), indexToY(toThread.index) );
		else 
			drawArrow(timeToX(timeus), indexToY(fromThread.index), timeToX(timeus), indexToY(toThread.index) );
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