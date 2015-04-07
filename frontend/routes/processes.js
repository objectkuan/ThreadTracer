var express = require('express');
var router = express.Router();

/* GET process listing. */
router.get('/', function(req, res, next) {
	var sys = require('sys');
	var exec = require('child_process').exec;

	var cmd = "ps aux | grep ^`logname`";
	var keywords = req.query.keywords;
	if(keywords) {
		cmd = cmd + " | grep " + keywords;
	}
	cmd = cmd + " | grep -v grep";

	exec(cmd, 
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
				var cmd = line.substr(65);
				if (keywords && cmd.indexOf(keywords) < 0) continue;
				processes.push(
					{
						"pid": pid,
						"app": app,
						"cmd": cmd
					}
				);
			}
			res.setHeader('Content-Type', 'application/json');
			res.end(JSON.stringify(processes));
		}
	);
});

module.exports = router;
