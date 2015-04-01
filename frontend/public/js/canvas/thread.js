var threads = [];
var pidToIndex = {};
var initTime = 0;

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

function setInitTime(time) { initTime = time; }

function pushThread(pid, name) {
	var thread = { 
		"pid": pid, 
		"name":name , 
		"index": threads.length, 
		"end": initTime,
		"state": TSTATE.v('INIT')
	};
	threads.push(thread);
	drawText(10, indexToY(thread.index) - 7, 15, thread.name);
	drawText(10, indexToY(thread.index) + 10, 12, thread.pid);
	pidToIndex[thread.pid] = thread.index;
}

function indexToY(index) { return index * 50 + 50; }
function timeToX(time) { return time - initTime + 80; }

function runTo(pid, time) {
	var thread = threads[pidToIndex[pid]];
	assert(time >= thread.end, "Time reverse");
	drawSolidLine(timeToX(thread.end), indexToY(thread.index), timeToX(time), indexToY(thread.index) );
	// console.log(timeToX(thread.end), indexToY(thread.index), timeToX(time), indexToY(thread.index) );
	thread.end = time;
}

function sleepTo(pid, time) {
	var thread = threads[pidToIndex[pid]];
	assert(time >= thread.end, "Time reverse");
	drawDotLine(timeToX(thread.end), indexToY(thread.index), timeToX(time), indexToY(thread.index) );
	// console.log(timeToX(thread.end), indexToY(thread.index), timeToX(time), indexToY(thread.index) );
	thread.end = time;
}

function timeOutTo(pid, time) {
	var thread = threads[pidToIndex[pid]];
	assert(time >= thread.end, "Time reverse");
	drawDotLine(timeToX(thread.end), indexToY(thread.index), timeToX(time), indexToY(thread.index) );
	// console.log(timeToX(thread.end), indexToY(thread.index), timeToX(time), indexToY(thread.index) );
	drawDownTriangle(timeToX(time), indexToY(thread.index) - 7);
	thread.end = time;
}

function wakeUp(fpid, tpid, time) {
	var fromThread = threads[pidToIndex[fpid]];
	var toThread = threads[pidToIndex[tpid]];
	drawArrow(timeToX(time), indexToY(fromThread.index), timeToX(time), indexToY(toThread.index) );
}
