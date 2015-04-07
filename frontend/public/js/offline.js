
$("document").ready(function() {


	if (window.File && window.FileReader && window.FileList && window.Blob) {
		// Great
		console.log("Greate with FILE APIs.");
	} else {
		alert("The File APIs are not fully supported in this browser");
		return;
	}

	function updateProgress(evt) {
		if(evt.lengthComputable) {
			var percentLoaded = Math.round((evt.loaded / evt.total) * 100);
			//if (percentLoaded < 100) {
				console.log(percentLoaded + "%");
			//}
		}
	}

	var threads = {};
	function handleFileSelect(evt) {
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
			pushFiltedThread(15765);

			var setInitTimeForFirst = true;
			for (var i = 3; i < lines.length; i++) {
				var line = lines[i];
				
				// Matching
				var argvs = line.split(' ');
				var timestamp = parseInt(argvs[0]);
				var tracedEvent = argvs[1];
				var pid = parseInt(argvs[2]);

				if (setInitTimeForFirst) {
					setInitTime(timestamp);
					setInitTimeForFirst = false;
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
						if (!hasThread(pid)) 
							continue;
						break;
					case "WAKEUP":
						var tpid = parseInt(argvs[3]);
						if (!hasThread(tpid)) 
							continue;
						break;
					default:
						console.log("Unknown evnet: " + line);
						break;
				}

				switch (tracedEvent) {
					case "CREATE_WAKEUP":
						var cpid = parseInt(argvs[3]);
						pushThread(cpid);
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
		};
		fr.onprogress = updateProgress;
		fr.readAsText(file);
	}

	$("#traced-file").change(handleFileSelect);
});