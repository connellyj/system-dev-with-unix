/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * install_and_monitor.c
 * Reads and monitors some data on multiple threads.
 */

#include "log_mgr.h"
#include "thread_mgr.h"

#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#define SIZE 20
#define KEY 2018
#define LENGTH 64

typedef struct Entry
{
	int is_valid;
	float x;
	float y;
} Entry;

static FILE* DataFile;
static int Time = 30;

static Entry Data[20];

void* install_data(void* arg)
{
	// Initialize everything to 0
	Entry* entry;
	for(entry = Data; entry < Data + SIZE; entry++)
	{
		entry->is_valid = 0;
		entry->x = 0.0;
		entry->y = 0.0;
	}
	
	char line[LENGTH];
	while(fgets(line, LENGTH, DataFile) != NULL) 
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
		
		if(time < 0)
		{
			if(abs(time) > 0)
			{
				sleep(abs(time));
			}
			(Data + index)->is_valid = 0;
		}
		else
		{
			if(time > 0)
			{
				sleep(time);
			}
			(Data + index)->x = x;
			(Data + index)->y = y;
			(Data + index)->is_valid = 1;
		}
	}
	
	th_exit();
}

void* monitor(void* ptr)
{
	int t;
	for(t = 0; t < Time; t++)
	{
		int count = 0;
		int x_count = 0;
		int y_count = 0;
		
		Entry* entry;
		for(entry = Data; entry < Data + SIZE; entry++)
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
	
	th_exit();
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		fprintf(stdout, "Requires path as first argument.\n");
		exit(1);
	}
	if(argc > 2)
	{
		Time = atoi(argv[2]);
	}
	
	char* data_file = argv[1];
	DataFile = fopen(data_file, "r");
	if(DataFile == NULL)
	{
		perror("Unable to read input file: ");
		log_event(FATAL, "Unable to read input file %s", data_file);
		exit(1);
	}
	
	const int monitor_id = th_execute(monitor);
	if(monitor_id < 0)
	{
		fprintf(stdout, "Error starting monitor thread\n");
		log_event(FATAL, "Error starting monitor thread");
		exit(1);
	}
	
	const int install_id = th_execute(install_data);
	if(install_id < 0)
	{
		fprintf(stdout, "Error starting install data thread\n");
		log_event(FATAL, "Error starting install data thread");
		exit(1);
	}
	
	// only wait for install thread, since the monitor thread will have nothing 
	// to monitor if install is done
	const int status = th_wait(install_id);
	if(status != 0)
	{
		fprintf(stdout, "Error waiting for install thread\n");
		log_event(FATAL, "Error waiting for install thread");
		exit(1);
	}
	
	sleep(3); // wait for a few more data points
	
	if(fclose(DataFile) != 0)
	{
		perror("Error closing data file: ");
		log_event(WARNING, "Error closing data file");
	}
}
