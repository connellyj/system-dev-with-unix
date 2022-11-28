/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * my_fortune.c
 * Runs the fortune program at random intervals
 */

#include <aio.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <sys/stat.h>

#include <unistd.h>

#define MINTIME 1
#define MAXTIME 8
#define MAXLINE 200

static char* Dev4Loc = "/home/jcn/unix_class/fortune/fortune";
static char* AbsarokaLoc = "/home/jcn/unix_class/fortune_absaroka/fortune";
static char* Fortune = "echo 'hello world'";

struct aiocb Aio;
static char Input[2];

static void aio_done(union sigval s)
{
	if(Input[0] == 'q' && Input[1] == '\n')
	{
		exit(0);
	}
	if(aio_read(&Aio) != 0)
	{
		perror("Unable to do asynchronous read: ");
		exit(1);
	}
}

void run(const char* cmd)
{
	while(1)
	{
		FILE* f = popen(cmd, "r");
		if(!f)
		{
			perror("Error running fortune program: ");
			exit(1);
		}
		else
		{
			char line[MAXLINE];
			while(fgets(line, MAXLINE, f) != NULL) 
			{
				// convert to uppercase
				int i;
				for (i = 0; i < MAXLINE && line[i] != '\0'; i++)
				{
					if(line[i] >= 'a' && line[i] <= 'z')
					{
						line[i] -= 32;
					}
				}
				
				// output to stdout
				fprintf(stdout, "%s\n", line);
			}
		}
		
		// Wait for 1-8 seconds
		const int waitTime = (rand() % (MAXTIME - MINTIME + 1)) + MINTIME;
		fflush(stdout);
		sleep(waitTime);
	}
}

int main(int argc, char *argv[])
{
	// Get hostname to determine which command to run
	const int len = 20;
	char hostname[len];
	gethostname(hostname, len);
	if(strcmp(hostname, "dev4") == 0)
	{
		Fortune = Dev4Loc;
	}
	else if(strcmp(hostname, "absaroka.apl.jhu.edu") == 0)
	{
		Fortune = AbsarokaLoc;
	}
	
	// Set up asynchronous IO and first read
	Aio.aio_fildes = STDIN_FILENO;
	Aio.aio_offset = 0;
	Aio.aio_buf = (void*)Input;
	Aio.aio_nbytes = sizeof(char) * 2;
	Aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
	Aio.aio_sigevent.sigev_notify_function = aio_done;
	if(aio_read(&Aio) != 0)
	{
		perror("Unable to do asynchronous read: ");
		exit(1);
	}
	
	run(Fortune);
}
