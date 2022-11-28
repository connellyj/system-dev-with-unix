/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * thread_mgr.c
 * Provides functions to spawn and manage threads.
 * Limited to 50 threads.
 */

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log_mgr.h"
#include "thread_mgr.h"

#define THD_ERROR -1
#define THD_OK 0
#define MAX_THREADS 50
#define TERMINATED 0
#define RUNNING 1
#define EXITED 2
#define KILLED 3

static const char* Exited = "exited";
static const char* Killed = "killed";
static const char* Terminated = "terminated";
static const char* Running = "running";

static pthread_t Handles[MAX_THREADS];
static char* Names[MAX_THREADS];
static int Status[MAX_THREADS];
static int NumThreads = 0;

void print_threads(int arg)
{
	fprintf(stdout, "\n");
	int i;
	for(i = 0; i < NumThreads; ++i)
	{
		fprintf(stdout, "%d: %s, %s\n", i + 1, Names[i], Status[i] ? Running : Terminated);
	}
}

int valid_handle(ThreadHandles handle)
{
	return handle > 0 && handle <= MAX_THREADS && Status[handle - 1];
}

void kill_all(int arg)
{
	log_event(INFO, "thread_mgr: Killing all threads");
	fprintf(stdout, "thread_mgr: Killing all threads\n");
	th_kill_all();
}

ThreadHandles th_execute(Funcptrs func)
{
	if(NumThreads == 0)
	{
		struct sigaction action;
		action.sa_handler = print_threads;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;
		sigaction(SIGINT, &action, NULL);
		
		action.sa_handler = kill_all;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;
		sigaction(SIGQUIT, &action, NULL);
	}
	
	const ThreadHandles handle = NumThreads + 1;
	NumThreads++;
	if(handle > MAX_THREADS)
	{
		log_event(FATAL, "thread_mgr: Max number of threads already started");
		fprintf(stderr, "thread_mgr: Max number of threads already started\n");
		return THD_ERROR;
	}
	
	char* name = malloc(16 * sizeof(char));
	sprintf(name, "Thread%d", handle);
	Names[handle - 1] = name;
	
	pthread_t thread;
	log_event(INFO, "thread_mgr: Starting %s", name);
	int status = pthread_create(&thread, NULL, func, (void*) name);
	if(status != 0)
	{
		log_event(FATAL, "thread_mgr: Error starting %s: %s", name, strerror(errno));
		fprintf(stderr, "thread_mgr: Error starting %s: %s\n", name, strerror(errno));
		return THD_ERROR;
	}
	
	Handles[handle - 1] = thread;
	Status[handle - 1] = RUNNING;
	
	return handle;
}

int th_wait(ThreadHandles handle)
{
	if(!valid_handle(handle))
	{
		log_event(FATAL, "thread_mgr: Invalid thread handle: %d", handle);
		fprintf(stderr, "thread_mgr: Invalid thread handle: %d\n", handle);
		return THD_ERROR;
	}
	
	pthread_t thread = Handles[handle - 1];
	log_event(INFO, "thread_mgr: Joining %s", Names[handle - 1]);
	const int status = pthread_join(thread, NULL);
	if(status != 0)
	{
		log_event(FATAL, "thread_mgr: Error joining with thread %s: %s", Names[handle - 1], strerror(errno));
		fprintf(stderr, "thread_mgr: Error joining with thread %s: %s\n", Names[handle - 1], strerror(errno));
	}
	
	Status[handle - 1] = TERMINATED;
	free(Names[handle - 1]);
	
	return THD_OK;
}

int th_wait_all(void)
{
	if(NumThreads == 0)
	{
		log_event(FATAL, "thread_mgr: No threads currently managed");
		fprintf(stderr, "thread_mgr: No threads currently managed\n");
		return THD_ERROR;
	}
	
	int i;
	for(i = 1; i <= NumThreads; ++i)
	{
		const int status = th_wait(i);
		if(status != 0)
		{
			log_event(FATAL, "thread_mgr: Error waiting for thread %s", Names[i - 1]);
			fprintf(stderr, "thread_mgr: Error waiting for thread %s\n", Names[i - 1]);
			return THD_ERROR;
		}
	}
	
	return THD_OK;
}

int th_kill(ThreadHandles handle)
{
	if(!valid_handle(handle))
	{
		log_event(FATAL, "thread_mgr: Invalid thread handle: %d", handle);
		fprintf(stderr, "thread_mgr: Invalid thread handle: %d\n", handle);
		return THD_ERROR;
	}
	
	pthread_t thread = Handles[handle - 1];
	log_event(INFO, "thread_mgr: Cancelling %s", Names[handle - 1]);
	int status = pthread_cancel(thread);
	if(status != 0)
	{
		log_event(WARNING, "thread_mgr: Error cancelling thread %s: %s", Names[handle - 1], strerror(errno));
		fprintf(stderr, "thread_mgr: Error cancelling thread %s: %s\n", Names[handle - 1], strerror(errno));
	}
	
	Status[handle - 1] = KILLED;
	
	return THD_OK;
}

int th_kill_all(void)
{
	if(NumThreads == 0)
	{
		log_event(FATAL, "thread_mgr: No threads currently managed");
		fprintf(stderr, "thread_mgr: No threads currently managed\n");
		return THD_ERROR;
	}
	
	int i;
	for(i = 1; i <= NumThreads; ++i)
	{
		const int status = th_kill(i);
		if(status != 0)
		{
			log_event(WARNING, "thread_mgr: Error killing thread %s", Names[i - 1]);
			fprintf(stderr, "thread_mgr: Error killing thread %s\n", Names[i - 1]);
		}
	}
	
	return THD_OK;
}

int th_exit(void)
{
	const pthread_t id = pthread_self();
	int i = 0;
	while(pthread_equal(id, Handles[i]) != 0 && i < NumThreads)
	{
		i++;
	}
	
	Status[i] = EXITED;
	pthread_exit(NULL);
	
	return THD_ERROR;
}
