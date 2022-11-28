/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * monitor_shm.c
 * Prints out information about data in shared memory
 */

#include "log_mgr.h"
#include "shared_mem.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define TIME 30
#define SIZE 20
#define KEY 2018
#define LENGTH 64

typedef struct Entry
{
	int is_valid;
	float x;
	float y;
} Entry;

int main(int argc, char *argv[])
{
	int time = TIME;
	if(argc > 1)
	{
		time = atoi(argv[1]);
	}
	
	void* addr = connect_shm(KEY, sizeof(Entry) * SIZE);
	if(!addr)
	{
		perror("Unable to connect shared memory: ");
		log_event(FATAL, "Unable to connect shared memory");
		exit(1);
	}
	
	int t;
	for(t = 0; t < time; t++)
	{
		int count = 0;
		int x_count = 0;
		int y_count = 0;
		
		Entry* entry;
		for(entry = (Entry*)addr; entry < (Entry*)addr + SIZE; entry++)
		{
			if(entry->is_valid)
			{
				count++;
				x_count += entry->x;
				y_count += entry->y;
			}
		}
		
		float x = 0;
		float y = 0;
		if(count > 0) 
		{
			x = x_count * 1.0 / count;
			y = y_count * 1.0 / count;
		}
		fprintf(stdout, "%d: %d active, x=%f, y=%f\n", t, count, x, y);
		
		sleep(1);
	}
	
	if(detach_shm(addr) != 0)
	{
		perror("Unable to detach shared memory: ");
		log_event(WARNING, "Unable to detach shared memory");
	}
}
