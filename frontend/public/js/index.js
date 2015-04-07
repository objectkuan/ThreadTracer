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
    	alert(data);
    });
}