var express = require('express');
var router = express.Router();

var backendBinDir = "/home/hjq/playground/ttracer/backend/bin/";
var tracingDir = "/tmp/trace_result/";

/* POST set traced pids. */
router.post('/setpids', function(req, res, next) {

	var sys = require('sys');
	var exec = require('child_process').exec;

	var pids = JSON.parse(req.body.pids);
	var strPids = pids.join();
	var cmd = "cd " + backendBinDir + " && ./forfront/set-trace-pids " + strPids;

	exec(cmd, 
		function puts(error, stdout, stderr) { 
			res.setHeader('Content-Type', 'application/json');
			res.end(JSON.stringify({ success: true }));
		}
	);
});

/* POST set traced pids. */
router.post('/runCommand', function(req, res, next) {

	var sys = require('sys');
	var exec = require('child_process').exec;

	var runcmd = req.body.cmd;
	var cmd = "cd " + backendBinDir + " && DISPLAY=:0 ./trcmd " + runcmd;
	console.log(cmd);

	var fs= require('fs');
	var spawn = require('child_process').spawn;
	var outLogPath = tracingDir + "/out.log";
	var errLogPath = tracingDir + "/err.log";
	var outLog = fs.openSync(outLogPath, "w");
	var errLog = fs.openSync(errLogPath, "w");

	spawn("bash", ["-c", cmd], {
		stdio: ['ignore', outLog, errLog],
		detached: true
	}).unref();

	console.log("Executed");
	setTimeout(
		function() {
			// Executed after 1 second
			// Now read the error file
			console.log("Executed after 1 second");
			fs.readFile(errLogPath, 'utf8', function(err, data) {
				// Executed after file read
				// Now check whether error
				console.log("Executed after file read");
				if (data.trim() == "ALREADYTRACING") {
					res.setHeader('Content-Type', 'application/json');
					res.end(JSON.stringify({ error: true, message: "Already tracing." }));
					return;
				}

				// Check pid
				var getPIDCmd = "cd " + backendBinDir + " && ./forfront/cat-trace-pids";
				console.log(getPIDCmd);

				exec(getPIDCmd, 
					function puts(error, stdout, stderr) {
						if (error) {
							res.setHeader('Content-Type', 'application/json');
							res.end(JSON.stringify({ error: true, message: stderr }));
							return;
						}

						res.setHeader('Content-Type', 'application/json');
						res.end(JSON.stringify({ pids: eval(stdout) }));
						return;
					}
				);
			});
		}, 1000);
});

/* POST set traced pids. */
router.post('/getTraces', function(req, res, next) {

	var sys = require('sys');
	var exec = require('child_process').exec;

	var wantToGet = req.body.wantToGet;
	console.log(backendBinDir);
	var cmd = "cd " + backendBinDir + " && ./forfront/biu " + wantToGet;

	exec(cmd, 
		function puts(error, stdout, stderr) {
			res.setHeader('Content-Type', 'text/plain');
			res.end(stdout);
		}
	);
});

module.exports = router;
