/* Julia Connelly
 * 605.614 System Development in the UNIX Environment
 * log_mgr.h
 * Header file for the log_mgr library.
 */
 
#ifndef LOG_MGR_H
#define LOG_MGR_H
 
/* 
 * Enumeration of the different log levels
 */
typedef enum {INFO, WARNING, FATAL} Levels;

/* 
 * Default logfile name used when none is set
 */
static const char* DefaultLogfile = "logfile";

/* 
 * Log the provided string formatted with the provided format at the given level in the set logfile
 * Supports logging from multiple processes at once
 */
int log_event(Levels l, const char* fmt, ...);

/* 
 * Set the name of the logfile used to log events
 */
int set_logfile(const char* logfile_name);

/* 
 * Close the set logfile
 * The set logfile name does not persist once the file has been closed
 */
void close_logfile(void);

#endif /* !LOG_MGR_H */
