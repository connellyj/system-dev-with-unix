/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * shared_mem.h
 * Header file for the shmlib library.
 */
 
#ifndef SHMLIB_H
#define SHMLIB_H
 
/* 
 * Attaches shared memory segment identified by the given key of the provided size
 * Returns NULL on failure
 */
void* connect_shm(int key, int size);

/* 
 * Detaches the shared memory segment attached to the process via the provided address
 */
int detach_shm(void* addr);

/* 
 * Detaches all shared memory segments from the segment identified by the provided key
 * Deletes the shared memory segment
 */
int destroy_shm(int key);

#endif /* !SHMLIB_H */
