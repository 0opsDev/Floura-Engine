#ifndef TASK_SCHEDULER_CLASS_H
#define TASK_SCHEDULER_CLASS_H

#include <iostream>
#include <thread>
#include <vector>
#include <utility>

class TaskScheduler {
public:

	static std::vector<std::thread> tempThreads;

	static void initialize();
	static void shutdown();

	static void StartTemporaryThread(void (*function)());

	static void update();

	static void JoinAll();

	static void WaitAll();

	static void killAll(); // force kill all threads also cleanup

private:

};

#endif
