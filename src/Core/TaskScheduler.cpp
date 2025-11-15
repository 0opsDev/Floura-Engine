#include "TaskScheduler.h"

std::vector<std::thread> TaskScheduler::tempThreads;

void TaskScheduler::initialize()
{

}
void TaskScheduler::shutdown()
{

}

void TaskScheduler::StartTemporaryThread(void (*function)())
{
	std::thread new_thread(function);
	tempThreads.push_back(std::move(new_thread));
	//threads
	//tasks.push_back(newTask);
	//tasks.back().thread = std::thread(static_cast<void(*)()>(funtion));
}


void TaskScheduler::update()
{

}

void TaskScheduler::JoinAll()
{
	//for (size_t i = 0; i < taskThreads.size(); i++)
	//{
	//	if (taskThreads[i].thread.joinable())
	//	{
	//		taskThreads[i].thread.join();
	//	}
	//}
}

void TaskScheduler::WaitAll()
{
	//for (size_t i = 0; i < taskThreads.size(); i++)
	//{
	//	if (taskThreads[i].thread.joinable())
	//	{
	//		taskThreads[i].thread.join();
	//	}
	//}
}

void TaskScheduler::killAll()
{
	//for (size_t i = 0; i < taskThreads.size(); i++)
	//{
	//	if (taskThreads[i].thread.joinable())
	//	{
	//		taskThreads[i].thread.join();
	//	}
	//}
}