function refreshProcesses() {
	$.get("/processes", function(data) {
		var procList = $("#process-list");
		procList.empty();
		for(var idx in data) {
			var process = data[idx];
			var item = '<li class="list-group-item" pid="' + process.pid + '" title="' + process.cmd + '">' + process.app + '(' + process.pid + ')' + '</li>';
			procList.append(item);
		}
		updateCheckList();
	});
}

function filterProcesses() {
	var keywords = $("#process-key-words").val();
	$.get("/processes", {keywords: keywords}, function(data) {
		var procList = $("#process-list");
		procList.empty();
		for(var idx in data) {
			var process = data[idx];
			var item = '<li class="list-group-item" pid="' + process.pid + '" title="' + process.cmd + '">' + process.app + '(' + process.pid + ')' + '</li>';
			procList.append(item);
		}
		updateCheckList();
	});
}

function startTrace() {
    var pids = [], counter = 0;
    $("#process-list li.active").each(function(idx, li) {
		pids.push($(li).attr("pid"));
    });
    $.post("/trace/setpids", { pids: JSON.stringify(pids) }, function(data) {
    	if (data.error) {
    		alert(data.message);
    	}
    });
}

function runCommand() {
	var cmd = $("#run-command").val();
	var cmdOutput = $("#command-output");
	cmdOutput.html();
	$.post("/trace/runCommand", {cmd: cmd}, function(data) {
		if (data.error) {
			cmdOutput.html("<p> <font color='red'> [ERROR] " + data.message + "</font> </p>");
			return;
		} else {
			// Clear all cache
			resetThreads();

			// Set up the init pids
			console.log(data.pids);
			for (var idx in data.pids) {
				var pid = data.pids[idx];
				pushThread(pid);
				console.log("Bind pid " + pid);
			}
			initInteraction();
			setUpdateGraphTimer();
		}
	});
}

var setUpdateGraphPointer = 0;
var setInitTimeForFirst = true;
var getTracesInterval;
function setUpdateGraphTimer() {

	getTracesInterval = window.setInterval(function() {
		$.post("/trace/getTraces", {wantToGet: setUpdateGraphPointer}, 
			function(data) {
				var cmdOutput = $("#command-output");
				var events = data.split("\n");
				var comingTraceFile = parseInt(events[0]);

				if (comingTraceFile < 0) {
					// Process terminated
					window.clearInterval(getTracesInterval);
					console.log("Process terminated");
					return;
				}

				setUpdateGraphPointer += comingTraceFile;
				console.log("ComingTraceFile" + comingTraceFile);

				for (var i = 1; i < events.length; i++) {
					var line = events[i];

					processEventLine(line, setInitTimeForFirst);
					setInitTimeForFirst = false;
				}
			}
		);
	}, 3000);
}

$("document").ready(function() {


	if (window.File && window.FileReader && window.FileList && window.Blob) {
		// Great
		console.log("Greate with FILE APIs.");
	} else {
		alert("The File APIs are not fully supported in this browser");
		return;
	}
});