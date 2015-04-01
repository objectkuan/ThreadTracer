function runNautilus() {
	setInitTime(8810);

	pushThread(29306, "pool");
	pushThread(29411, "pool");
	pushThread(29580, "pool");
	pushThread(29197, "nautilus");

	runTo(29306, 8815);
	sleepTo(29306, 8938);
	runTo(29306, 10000);


	runTo(29411, 8996);
	sleepTo(29411, 9041);
	runTo(29411, 10000);


	runTo(29580, 9146);
	sleepTo(29580, 9301);
	runTo(29580, 9306);
	sleepTo(29580, 9351);
	runTo(29580, 9356);
	sleepTo(29580, 9371);
	runTo(29580, 10000);

	runTo(29197, 10000);

	wakeUp(29197, 29306, 8938);
	wakeUp(29197, 29411, 9041);
	wakeUp(29197, 29580, 9301);
	wakeUp(29197, 29580, 9351);
	wakeUp(29197, 29580, 9371);
}

function runChrome() {
	setInitTime(6300);

	pushThread(14335, "chrome");
	pushThread(14338, "Chrome_ChildIOT");

	runTo(14335, 6301);
	timeOutTo(14335, 6608);
	runTo(14335, 6608);
	timeOutTo(14335, 6609);
	runTo(14335, 6609);
	timeOutTo(14335, 6917);
	runTo(14335, 6917);
	sleepTo(14335, 6992);
	runTo(14335, 10000);

	runTo(14338, 10000);

	wakeUp(14338, 14335, 6992);
}