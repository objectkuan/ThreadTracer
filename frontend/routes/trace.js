var express = require('express');
var router = express.Router();

/* POST set traced pids. */
router.post('/setpids', function(req, res, next) {

	var sys = require('sys');
	var exec = require('child_process').exec;

	var pids = JSON.parse(req.body.pids);
	var strPids = pids.join();
	var backendBinDir = "/home/hjq/playground/ttracer/backend/bin/"
	var cmd = "cd " + backendBinDir + " && ./set-trace-pids " + strPids;

	exec(cmd, 
		function puts(error, stdout, stderr) { 
			res.setHeader('Content-Type', 'application/json');
			res.end(JSON.stringify({ success: true }));
		}
	);
});

module.exports = router;
