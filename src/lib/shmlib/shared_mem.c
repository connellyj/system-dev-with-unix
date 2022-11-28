/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * shared_mem.c
 * Functions to connect, detach, and destroy shared memory segments.
 */

#include "shared_mem.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/shm.h>

#define OK 0
#define ERROR -1
#define MAXAT 4096

static int Index = 0;
static int Keys[MAXAT];
static int Ids[MAXAT];
static void** Addrs[MAXAT];

void* connect_shm(int key, int size)
{	
	const int id = shmget(key, size, IPC_CREAT | 0666);
	if(id < 0)
	{
		perror("Error getting shared memory segment: ");
		return NULL;
	}
	
	void* addr = shmat(id, 0, 0);
	if(*((int*)addr) < 0)
	{
		perror("Error getting address in shared memory segment: ");
		return NULL;
	}
	
	// store key, id, and addr information
	int i = 0;
	while(Keys[i] != key && i < Index)
	{
		i++;
	}
	if(Keys[i] == key)
	{
		// if key exists, add addr to end of array
		int next = 0;
		while(Addrs[Index][next] != NULL)
		{
			next++;
		}
		Addrs[Index][next] = addr;
	}
	else
	{
		// if key doesn't exist, create addr array and save key and id
		Addrs[Index] = malloc(sizeof(void*) * MAXAT);
		int j;
		for(j = 0; j < MAXAT; j++)
		{
			Addrs[Index][j] = NULL;
		}
		Addrs[Index][0] = addr;
		Keys[Index] = key;
		Ids[Index] = id;
		Index++;
	}
	
	return addr;
}

int detach_shm(void* addr)
{
	// REMOVE FROM TRACKING!!!
	return shmdt(addr);
}

int destroy_shm(int key)
{
	// find the relevant key
	int i = 0;
	while(Keys[i] != key && i < Index)
	{
		i++;
	}
	if(i == Index)
	{
		fprintf(stdout, "Key %d does not exist\n", key);
		return ERROR;
	}
	
	// detach segments and free internal array
	int j = 0;
	if(Addrs[i])
	{
		while(Addrs[i][j] != NULL)
		{
			const int status = detach_shm(Addrs[i][j]);
			j++;
			
			if(status != 0)
			{
				perror("Error detaching a segment: ");
			}
		}
		free(Addrs[i]);
	}
	
	// remove shared memory from the system
	return shmctl(Ids[i], IPC_RMID, NULL);
}
