var express = require('express');
var router = express.Router();

/* GET users listing. */
router.get('/', function(req, res, next) {
	var sys = require('sys')
	var exec = require('child_process').exec;
	exec("ps aux | grep ^`logname`", 
		function puts(error, stdout, stderr) { 
			var lines = stdout.split("\n");
			var processes = [];
			for(var idx in lines) {
				var line = lines[idx];
				if (!line.trim()) continue;
				var blocks = line.split(/\s+/);
				var pid = blocks[1];
				var appBlocks = blocks[10].split("\/");
				var app = appBlocks[appBlocks.length - 1];
				processes.push(
					{
						"pid": pid,
						"app": app,
						"cmd": line.substr(65)
					}
				);
			}
			res.setHeader('Content-Type', 'application/json');
			res.end(JSON.stringify(processes));
		}
	);
});

module.exports = router;
