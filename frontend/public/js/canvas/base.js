
function assert(condition, message) {
    if (!condition) {
        message = message || "Assertion failed";
        if (typeof Error !== "undefined") {
            throw new Error(message);
        }
        throw message; // Fallback
    }
}

/**
 * Math functions
 */
function toRadian(degree) {
	return degree * Math.PI / 180;
}

function sin(degree) { return Math.sin(toRadian(degree)); }
function cos(degree) { return Math.cos(toRadian(degree)); }
function tan(degree) { return Math.tan(toRadian(degree)); }


/**
 * Drawing functions
 */
function drawSolidLine(fromX, fromY, toX, toY) {
	var c = document.getElementById("myCanvas");
	var ctx = c.getContext("2d");

    ctx.save();

	ctx.beginPath();
	ctx.setLineDash([]);
	ctx.lineWidth = 5;
    ctx.strokeStyle = '#0d0';
	ctx.moveTo(fromX, fromY);
	ctx.lineTo(toX, toY);
	ctx.stroke();

    ctx.restore();
}

function drawDotLine(fromX, fromY, toX, toY) {
	var c = document.getElementById("myCanvas");
	var ctx = c.getContext("2d");
    ctx.save();

	ctx.beginPath();
	ctx.lineWidth = 1;
    ctx.strokeStyle = '#d00';
	ctx.setLineDash([1, 1]);
	ctx.moveTo(fromX, fromY);
	ctx.lineTo(toX, toY);
	ctx.stroke();

    ctx.restore();
}

function drawUpTriangle(centerX, centerY) {
	var c = document.getElementById("myCanvas");
	var ctx = c.getContext("2d");
	var l = 5;

    ctx.save();

    ctx.setLineDash([1, 0]);
	ctx.beginPath();
        ctx.fillStyle = '#000';
        ctx.moveTo(centerX, centerY - l / cos(30));
        ctx.lineTo(centerX - l, centerY + l * tan(30));
        ctx.lineTo(centerX + l, centerY + l * tan(30));
    ctx.closePath();
    ctx.fill();

    ctx.restore();
}

function drawDownTriangle(centerX, centerY) {
	var c = document.getElementById("myCanvas");
	var ctx = c.getContext("2d");
	var l = 5;

    ctx.save();
    
    ctx.setLineDash([1, 0]);
	ctx.beginPath();
        ctx.fillStyle = '#000';
        ctx.moveTo(centerX, centerY + l / cos(30));
        ctx.lineTo(centerX - l, centerY - l * tan(30));
        ctx.lineTo(centerX + l, centerY - l * tan(30));
    ctx.closePath();
    ctx.fill();

    ctx.restore();
}

function drawArrow(fromX, fromY, toX, toY, color) {
	var c = document.getElementById("myCanvas");
	var ctx = c.getContext("2d");

    var headlen = 12;   // length of head in pixels
    var angle = Math.atan2(toY - fromY,  toX - fromX);
    var arrowAngle = toRadian(20);

    ctx.save();
    ctx.strokeStyle = (color == undefined ? '#44d' : color);
    // first part
    ctx.beginPath();
    ctx.setLineDash([]);
    ctx.moveTo(fromX, fromY);
    ctx.lineTo(toX, toY);
    ctx.stroke();
    // second part
    ctx.beginPath();
    ctx.setLineDash([]);
    ctx.moveTo(toX, toY);
    ctx.lineTo(toX - headlen * Math.cos(angle - arrowAngle), toY - headlen * Math.sin(angle - arrowAngle));
    ctx.moveTo(toX, toY);
    ctx.lineTo(toX - headlen * Math.cos(angle + arrowAngle), toY - headlen * Math.sin(angle + arrowAngle));
    ctx.stroke();
    // 
    ctx.restore();          // this will, in fact, restore strokeStyle
}

function drawText(x, y, size, text) {
	var c = document.getElementById("myCanvas");
	var ctx = c.getContext("2d");
	ctx.font = size + "px Arial";
	ctx.fillText(text, x, y);
}

function clearCanvas() {
    var c = document.getElementById("myCanvas");
    var ctx = c.getContext("2d");
    ctx.clearRect(0, 0, c.width, c.height);
    // console.log("Clear canvas", c.width, c.height);
}

function backupCanvas() {
    var c = document.getElementById("myCanvas");
    var ctx = c.getContext("2d");

    var backUpCanvas = document.getElementById('backupCanvas');
    var backUpContex = backUpCanvas.getContext('2d');
    backUpContex.clearRect(0, 0, backUpCanvas.width, backUpCanvas.height);

    backUpContex.drawImage(c, 0, 0, c.width, c.height);
    // console.log("Backup Canvas", c.width, c.height, backUpCanvas.width, backUpCanvas.height);
}

function restoreCanvas() {
    var c = document.getElementById("myCanvas");
    var ctx = c.getContext("2d");
    var backUpCanvas = document.getElementById('backupCanvas');
    clearCanvas();
    // console.log("Restore canvas", c.width, c.height);
    ctx.drawImage(backUpCanvas, 0, 0, c.width, c.height);
    // console.log("Restore Canvas", c.width, c.height, backUpCanvas.width, backUpCanvas.height);
}

function drawRectangle(left, top, width, height) {
    var c = document.getElementById("myCanvas");
    var ctx = c.getContext("2d");

    ctx.save();

    ctx.globalAlpha = 0.2;
    ctx.fillStyle = '#112';
    ctx.fillRect(left, top, width, height);
    ctx.globalAlpha = 1;

    ctx.restore();
}

function drawTallRectangle(left, right) {
    var c = document.getElementById("myCanvas");
    drawRectangle(left, 0, right - left, c.height);
}

function addClickListener(func) {
    var c = document.getElementById("myCanvas");
    c.addEventListener("click", function(evt) {
        func(evt.layerX, evt.layerY);
    }, false);
}