#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

#include <pthread.h>
#include <stdlib.h>
#include <iostream>

class MutexLock {
public:
	MutexLock()
	{
		err = pthread_mutex_init(&mutex_lock, NULL);
		check_result("locker initial.", err);
	}

	~MutexLock()
	{
		err = pthread_mutex_destroy(&mutex_lock);
		check_result("locker destroy.", err);
	}

	void Lock()
	{
		err = pthread_mutex_lock(&mutex_lock);
		check_result("lock.", err);
	}

	void UnLock()
	{
		err = pthread_mutex_unlock(&mutex_lock);
		check_result("unlock.", err);
	}

private:
	void check_result(const char *prompt, int err)
	{
		if (err != 0) {
			std::cout << "Error on: " << prompt << "err = " << err
				<< std::endl;
			exit(1);
		}
		return;
	}

private:
	int err;
	pthread_mutex_t mutex_lock;
};

#endif /* end MUTEXLOCK_H */