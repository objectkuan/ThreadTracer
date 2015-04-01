function refreshProcesses() {
	$.get("/processes", function(data) {
		var procList = $("#process-list");
		procList.empty();
		for(var idx in data) {
			var process = data[idx];
			var item = '<li class="list-group-item" title="haha">' + process.app + '(' + process.pid + ')' + '</li>';
			procList.append(item);
		}
		updateCheckList();
	});
}