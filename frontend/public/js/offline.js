$("document").ready(function() {


	if (window.File && window.FileReader && window.FileList && window.Blob) {
		// Great
		console.log("Greate with FILE APIs.");
	} else {
		alert("The File APIs are not fully supported in this browser");
		return;
	}



	var threads = {};
	function tracedFileChanged(evt) {
		var file = evt.target.files[0];
		var fr = new FileReader();
		fr.onload = function() {
			var fileContent = this.result;
			var lines = fileContent.split('\n');

			// Clear all cache
			resetThreads();

			// Set up the init pids
			var strPids = lines[2];
			var pids = eval(strPids);
			for (var i = 0; i < pids.length; i++) {
				var pid = parseInt(pids[i]);
				if (pid > 0) pushThread(pid);
			}
			//pushFiltedThread(15765);

			var setInitTimeForFirst = true;
			for (var i = 3; i < lines.length; i++) {
				var line = lines[i];

				processEventLine(line, setInitTimeForFirst);
				setInitTimeForFirst = false;
			}
			backupCanvas();
		};
		fr.onprogress = function (evt) {
			if(evt.lengthComputable) {
				var percentLoaded = Math.round((evt.loaded / evt.total) * 100);
				//if (percentLoaded < 100) {
					console.log(percentLoaded + "%");
				//}
			}
		};
		fr.readAsText(file);
	}

	$("#traced-file").change(tracedFileChanged);


	function perfFileChanged(evt) {
		var file = evt.target.files[0];
		var fr = new FileReader();
		fr.onload = function() {
			var fileContent = this.result;
			var lines = fileContent.split('\n');

			var statType;	// Type: trace? corr? func?
			var periodStat; // Store a time period, and with a sorted list of stat.
			var itemOverTime = {};
			var itemInPeriod = {};
			for (var i = 0; i < lines.length; i++) {
				var line = lines[i];
				if (line.trim().length < 1) 
					continue;
				
				if (line[0] == '+') {
					// Spliter
					if (line[1] == '=') {
						// Output mode spliter
						statType = line.replace(/\=|\+|\s/gi, "");
						initPerfStat(statType);
					} else if(line[1] == '-') {
						// Time spliter
						var compoments = line.split("---");
						var from = compoments[1];
						var to = compoments[2];

						if (from.trim() == "end") {
							pushPerfStat(statType, itemOverTime);
							continue;
						}
						
						itemOverTime = {
							"from": parseInt(from),
							"to": parseInt(to),
							"itemsInPeriod": []
						};
					}
					continue;
				}

				if(line[0] >= '0' && line[0] <= '9') {
					// Item header
					itemInPeriod = {
						"head": line,
						"contents": []
					};
					itemOverTime.itemsInPeriod.push(itemInPeriod);
					continue;
				}

				// Content lines
				itemInPeriod.contents.push(line);
			}

			tryDump();
		};
		fr.onprogress = function (evt) {
			if(evt.lengthComputable) {
				var percentLoaded = Math.round((evt.loaded / evt.total) * 100);
				//if (percentLoaded < 100) {
					console.log("Perf file: " + percentLoaded + "%");
				//}
			}
		};
		fr.readAsText(file);
	}

	$("#perf-file").change(perfFileChanged);

	$("#save-canvas").click(
		function() {
			var c = document.getElementById("myCanvas");
			window.open(c.toDataURL("image/jpeg"));
		}
	);
	initInteraction();
});