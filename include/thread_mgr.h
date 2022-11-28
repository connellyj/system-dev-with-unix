/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * log_mgr.h
 * Header file for the thread_mgr library.
 */
 
#ifndef THREAD_MGR_H
#define THREAD_MGR_H

typedef int ThreadHandles;
typedef void *Funcptrs (void*);

/* 
 * Run the provided function on a thread
 * Returns a unique identifier for the thread
 */
ThreadHandles th_execute(Funcptrs);

/* 
 * Blocks until the provided thread terminates
 */
int th_wait(ThreadHandles);

/* 
 * Blocks until all threads terminate
 */
int th_wait_all(void);

/* 
 * Cancel the provided thread
 */
int th_kill(ThreadHandles);

/* 
 * Cancel all threads
 */
int th_kill_all(void);

/* 
 * When called by a thread, cleans up the calling threads resources in this library
 */
int th_exit(void);

#endif /* !THREAD_MGR_H */
