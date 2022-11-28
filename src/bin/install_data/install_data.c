/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * install_data.c
 * Installs some data into shared memory.
 */

#include "log_mgr.h"
#include "shared_mem.h"

#include <fcntl.h>

#include <signal.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#define SIZE 20
#define KEY 2018
#define LENGTH 64

static int DoRestart = 0;

typedef struct Entry
{
	int is_valid;
	float x;
	float y;
} Entry;

void destroy(int arg)
{
	if(destroy_shm(KEY) != 0)
	{
		perror("Unable to destroy shared memory: ");
		log_event(WARNING, "Unable to destroy shared memory");
	}
	exit(0);
}

void restart(int arg)
{
	DoRestart = 1;
}

void zero(void* addr)
{
	Entry* entry;
	for(entry = (Entry*)addr; entry < (Entry*)addr + SIZE; entry++)
	{
		entry->is_valid = 0;
		entry->x = 0.0;
		entry->y = 0.0;
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stdout, "Requires path as first argument.\n");
		exit(1);
	}
	char* data_file = argv[1];
	
	struct sigaction action1;
	action1.sa_handler = restart;
	sigemptyset(&action1.sa_mask);
	action1.sa_flags = 0;
	sigaction(SIGHUP, &action1, NULL);
	
	struct sigaction action2;
	action2.sa_handler = destroy;
	sigemptyset(&action2.sa_mask);
	action2.sa_flags = 0;
	sigaction(SIGTERM, &action2, NULL);
	
	FILE* f = fopen(data_file, "r");
	if(f == NULL)
	{
		perror("Unable to read input file: ");
		log_event(FATAL, "Unable to read input file %s", data_file);
		exit(1);
	}
	
	void* addr = connect_shm(KEY, sizeof(Entry) * SIZE);
	if(!addr)
	{
		perror("Unable to connect shared memory: ");
		log_event(FATAL, "Unable to connect shared memory");
		exit(1);
	}
	
	// Initialize everything to 0
	zero(addr);
	
	char line[LENGTH];
	while(fgets(line, LENGTH, f) != NULL) 
	{
		char* token = strtok(line, " \t");
		const int index = atoi(token);
		if(index >= SIZE)
		{
			fprintf(stdout, "Unable to process line: %s", line);
			log_event(WARNING, "Unable to process line: %s", line);
			continue;
		}
		
		token = strtok(NULL, " \t");
		if(token == NULL)
		{
			fprintf(stdout, "Unable to process line: %s", line);
			log_event(WARNING, "Unable to process line: %s", line);
			continue;
		}
		const int x = atof(token);
		
		token = strtok(NULL, " \t");
		if(token == NULL)
		{
			fprintf(stdout, "Unable to process line: %s", line);
			log_event(WARNING, "Unable to process line: %s", line);
			continue;
		}
		const int y = atof(token);
		
		token = strtok(NULL, " \t");
		if(token == NULL)
		{
			fprintf(stdout, "Unable to process line: %s", line);
			log_event(WARNING, "Unable to process line: %s", line);
			continue;
		}
		const int time = atoi(token);
		
		// Fill in shared memory
		Entry* entry = (Entry*)addr;
		if(time < 0)
		{
			if(abs(time) > 0)
			{
				sleep(abs(time));
			}
			(entry + index)->is_valid = 0;
		}
		else
		{
			if(time > 0)
			{
				sleep(time);
			}
			(entry + index)->x = x;
			(entry + index)->y = y;
			(entry + index)->is_valid = 1;
		}
		
		if(DoRestart)
		{
			DoRestart = 0;
			zero(addr);
			if(fseek(f, 0, SEEK_SET) != 0)
			{
				perror("Error seeking to start of data file: ");
				log_event(WARNING, "Error seeking to start of data file");
			}
		}
	}
	
	destroy(0);
	
	if(fclose(f) != 0)
	{
		perror("Error closing data file: ");
		log_event(WARNING, "Error closing data file");
	}
}
