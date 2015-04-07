var threads = [];
var pidToIndex = {};
var filteredThreads = [];
var filteredPidToIndex = {};
var initTimeus = 0;

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

function indexToY(index) { return index * 50 + 50; }
function timeToX(timeus) { return (timeus - initTimeus) / 50000 + 130; }


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