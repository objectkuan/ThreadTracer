function runNautilus() {
	setInitTime(88100000);

	pushThread(29306, "pool");
	pushThread(29411, "pool");
	pushThread(29580, "pool");
	pushThread(29197, "nautilus");

	runTo(29306, 88150000);
	wakeUp(29197, 29306, 89380000);
	sleepTo(29306, 89380000);
	runTo(29306, 100000000);


	runTo(29411, 89960000);
	wakeUp(29197, 29411, 90410000);
	sleepTo(29411, 90410000);
	runTo(29411, 100000000);


	runTo(29580, 91460000);
	wakeUp(29197, 29580, 93010000);
	sleepTo(29580, 93010000);
	runTo(29580, 93060000);
	wakeUp(29197, 29580, 93510000);
	sleepTo(29580, 93510000);
	runTo(29580, 93560000);
	wakeUp(29197, 29580, 93710000);
	sleepTo(29580, 93710000);
	runTo(29580, 100000000);

	runTo(29197, 100000000);

	backupCanvas();
}

function runChrome() {
	setInitTime(63000000);

	pushThread(14335, "chrome");
	pushThread(14338, "Chrome_ChildIOT");

	runTo(14335, 63010000);
	timeOutTo(14335, 66080000);
	runTo(14335, 66080000);
	timeOutTo(14335, 66090000);
	runTo(14335, 66090000);
	timeOutTo(14335, 69170000);
	runTo(14335, 69170000);
	wakeUp(14338, 14335, 69920000);
	sleepTo(14335, 69920000);
	runTo(14335, 100000000);

	runTo(14338, 100000000);

	backupCanvas();

}



// Paper data
function runSlowUI() {
	setInitTime(63000000);

	pushThread(14335, "UIThread");

	runTo(14335, 64010000);
	sleepTo(14335, 78080000);
	runTo(14335, 79080000);
	
	runTo(14338, 100000000);

	backupCanvas();

}

function runSimleDep() {
	setInitTime(63000000);

	pushThread(2342, "UIThread");
	pushThread(2343, "Thread 1");
	pushThread(2344, "Thread 2");

	runTo(2342, 64010000);

	runTo(2343, 67010000);

	runTo(2344, 70010000);
	sleepTo(2344, 77080000);
	wakeUp(2344, 2343, 77380000);
	sleepTo(2343, 77580000);
	wakeUp(2343, 2342, 77880000);

	sleepTo(2342, 78080000);

	runTo(2344, 100000000);
	runTo(2342, 100000000);
	runTo(2343, 100000000);


	backupCanvas();

}

function runTimeout() {
	setInitTime(63000000);

	pushThread(14553, "Thread 1");
	pushThread(14554, "Thread 2");

	runTo(14553, 64010000);
	runTo(14554, 64010000);

	timeOutTo(14553, 69010000);
	timeOutTo(14553, 74010000);
	
	wakeUp(14554, 14553, 75010000);
	sleepTo(14553, 75020000);

	runTo(14554, 100000000);
	runTo(14553, 100000000);


	backupCanvas();

}

function runOnlyRun() {
	setInitTime(63000000);

	pushThread(8301, "Thread 1");
	runTo(8301, 100000000);

	backupCanvas();

}