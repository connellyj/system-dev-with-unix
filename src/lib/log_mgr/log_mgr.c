/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * log_mgr.c
 * Provides functions to write logs to a file.
 * Log writing will work with multiple processes.
 */

#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "log_mgr.h"

#define ERROR -1
#define OK 0

static const char* LevelStrings[] = {"INFO", "WARNING", "FATAL"};
static const char* MonthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static int Logfile = -1;

// According to documentation, any write less than or equal to PIP_BUF bytes 
//     will not be interleaved with writes from other processes.
// https://pubs.opengroup.org/onlinepubs/9699919799/functions/write.html
static char Buffer[PIPE_BUF];

int fill_time(Levels l)
{
	time_t seconds;
	time(&seconds);
	const struct tm tm = *localtime(&seconds);
	const int status = sprintf(Buffer, "%s %02d %02d:%02d:%02d %s %d:%s:", MonthNames[tm.tm_mon], tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tzname[0], tm.tm_year + 1900,LevelStrings[l]);
	if(status < 0)
	{
		perror("Internal error writing time to buffer: ");
		return 0;
	}
	return strlen(Buffer);
}

void close_fd(const int to_close)
{
	if(to_close >= 0)
	{
		const int status = close(to_close); 
		if(status < 0)
		{
			perror("Error closing logfile: ");
		}
	}
}

int log_event(Levels l, const char* fmt, ...)
{
	if(Logfile < 0)
	{
		const int status = set_logfile(DefaultLogfile);
		if(status < 0)
		{
			perror("Error opening logfile: ");
			return ERROR;
		}
	}
	
	// Fill beginning of buffer with date, time, and level info
	const int offset = fill_time(l);
	
	va_list args;
	va_start(args, fmt);
	int status = vsprintf(Buffer + offset, fmt, args);
	va_end(args);
	
	if(status < 0)
	{
		perror("Internal error writing log to buffer: ");
		return ERROR;
	}
	
	strcat(Buffer, "\n");
	status = write(Logfile, Buffer, strlen(Buffer));
	if(status < 0)
	{
		perror("Error writing to logfile: ");
		return ERROR;
	}
	
	return OK;
}

int set_logfile(const char* logfile_name)
{
	const int old_fd = Logfile;
	if(logfile_name)
	{
		Logfile = open(logfile_name, O_CREAT | O_WRONLY | O_APPEND, 0600);
	}
	else
	{
		Logfile = open(DefaultLogfile, O_CREAT | O_WRONLY | O_APPEND, 0600);
	}
	
	if(Logfile < 0)
	{
		// Reset to previous log file on failure
		Logfile = old_fd;
		perror("Error opening logfile: ");
		return ERROR;
	}
	
	// Close previous log file
	close_fd(old_fd);
	
	return OK;
}

void close_logfile(void)
{
	const int fd = Logfile;
	Logfile = -1;
	close_fd(fd);
}
