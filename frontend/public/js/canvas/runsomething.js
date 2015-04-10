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