
#ifndef _EVENTLOG_H_
#define _EVENTLOG_H_

/**
 * @author Ricky Neil
 * @file eventlog.h
 * File containing the EventLog class and enums needed for its operation.
 */

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <string>
#include <queue>
#include <deque>
#include <map>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

#ifdef _UNICODE

#define vsprintf_t  _vswprintf
#define vsnprintf_t _vsnwprintf
typedef std::wstring TSTRING;

#else

#define vsprintf_t  vsprintf
#define vsnprintf_t vsnprintf
typedef std::string TSTRING;

#endif /* _UNICODE */


/**
 * Event Levels that can be used by the Log file.
 * Higher numbers are lower priority
 */
enum EventLevel
{
	EL_DEBUG    = 5,
	EL_INFO     = 4,
	EL_WARN     = 3,
	EL_ERROR    = 2,
	EL_CRITICAL = 1
};


/**
 * Multithread aware Event Reporting Class.
 * Designed to provide global event logging to a program.\n
 * This class allows only one instance of a log file to be open.\n
 * Events will be flushed after the event queue max size has been reached or the flush timeout has occured.
 */
class EventLog
{
protected:
	static std::map<TSTRING, EventLog*> OpenLogs; /**< Dictionary of open log files available to all EventLog */

	int references; /**< number of times this class is referenced currently */

	HANDLE requestedFlush; /**< event to record if a flush has been requested */
	int flushWait;         /**< time to wait between flushes */

	int queueSize;         /**< size of the current message queue */
	unsigned int maxQueue; /**< when queueSize is above this value log will be flushed */
	std::queue<TSTRING> Buffer; /**< buffer to hold queued up log messages waiting to be flushed */

	EventLevel eventLevel; /**< maximum event level to pay attention to */

	TSTRING filePath; /**< path of the file to write to */
	TSTRING fileName; /**< name of the file to write to */

	TSTRING fullFileName; /**< path, filename and index of the file to write to */

	HANDLE hFile;        /**< handle to the log file */
	HANDLE hFlushTimer;  /**< Timer which will cause a flush on timeout */

	CRITICAL_SECTION queueLock; /**< message queue read / write lock */
	CRITICAL_SECTION fileLock;  /**< file write lock */

protected:
	/**
	 * Constructor
	 * @param filename name of the file without path.
	 * @param path path of the file.
	 * @param level minimum event level to pay attention to.
	 * @param wait time to wait between flushes.
	 * @param maxqueue max size of the message backlog.
	 */
	EventLog(
		const TSTRING filename = TEXT("EventLog"),
		const TSTRING path = TEXT("Log"),
		const EventLevel level = EL_WARN,
		const int wait = 5000,
		const int maxqueue = 20
	);

	/**
	 * Timer Proc for the flush timer.
	 * @param params parameters to pass to this function.
	 * @param TimerOrWaitFired whether this was filed from a timer or a wait event.
	 */
	static void CALLBACK TimerFlush( void* params, BOOLEAN TimerOrWaitFired );
	
	/**
	 * Reopens the log file with a new day index, filename or path
	 */
	void ReopenFile();
	
	/**
	 * Checks to see if we are now in a new day,
	 * will reopen the file if we are
	 */
	void CheckNewDay();
	
	/**
	 * Internal write function with va_args.
	 * @param format message format.
	 * @param level level of the event to be logged.
	 * @param args va_args list of arguments to log.
	 */
	void vWriteLog(
		const TCHAR* const format,
		const EventLevel level,
		va_list args
	);

	/**
	 * parses the event level enum to human readable string.
	 * @param el event level enum value.
	 * @return human readable string of event level.
	 */
	TSTRING elToStr( EventLevel el );

	/**
	 * allows both "file.log" and "file" to be valid file names
	 * @param filename name of the file to be sanitised.
	 * @param fileTypeNeeded whether we want to return the filename with .log at the end or not.
	 * @return filename with either .log added or stripped depending on fileTypeNeeded
	 */
	static TSTRING SanitiseFileName( const TSTRING& filename, bool fileTypeNeeded = false );

public:

	/**
	 * Function to either create a new EventLog or return an existing one.
	 * @param filename name of the file without path.
	 * @param path path of the file.
	 * @param level minimum event level to pay attention to.
	 * @param wait time to wait between flushes.
	 * @param maxqueue max size of the message backlog.
	 */
	static std::auto_ptr<EventLog> InitialiseLog(
		const TSTRING filename = TEXT("EventLog"),
		const TSTRING path = TEXT("Log"),
		const EventLevel level = EL_WARN,
		const int wait = 5000,
		const int maxqueue = 20
	);

	/**
	 * Flush all log files.
	 */
	static void FlushAll();

	/**
	 * Close all log files.
	 * @param force close the log file even if there are still references to it.
	 */
	static void CloseAll( const bool force = false );

	/**
	 * Close a log file using an EventLog pointer.
	 * @param eventLog pointer to the EventLog to close.
	 * @param force force the log to close even if there are still references to it.
	 */
	static void CloseLog (EventLog* eventLog, const bool force = false );

	/**
	 * Close a log file using a file name.
	 * @param filename name of the file the EventLog is hooked into.
	 * @param force force the log to close even if there are still references to it.
	 */
	static void CloseLog( const TSTRING filename = TEXT("EventLog"), const bool force = false );

public:

	/**
	 * Set the max queue size before flushing.
	 * @param maxqueue new maximum size to set.
	 */
	void SetMaxQueue( const int maxqueue );
	
	/**
	 * Set the maximum event level to pay attention to.
	 * @param level new maximum level to pay attentiuon to.
	 */
	void SetEventLevel( const EventLevel level );

	/**
	 * Add a log message to the queue to be written.
	 * @param level level of the message to be written.
	 * @param format message format.
	 * @param ... arguments to insert into the format.
	 */
	void Write( const EventLevel level, const TCHAR* const format, ... );

	/**
	 * Flush the log queue instantly
	 */
	void FlushQueue();

	/**
	 * Virtual destructor for EventLog.
	 */
	virtual ~EventLog();
};

#endif
