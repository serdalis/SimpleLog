
#ifndef _EVENTLOG_INCLUDED_H_
#define _EVENTLOG_INCLUDED_H_

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

#include "unicode_defines.h"


/**
 * Event Levels that can be used by the Log file.
 * Higher numbers are lower priority
 */
enum EventLevel
{
	EL_DEBUG = 5,
	EL_INFO  = 4,
	EL_WARN  = 3,
	EL_ERROR = 2,
	EL_CRIT  = 1
};

class LogHandle; /**< Forward delceration just for the header file */
typedef std::auto_ptr<LogHandle> LOGHANDLE; /**< Log handle will auto destroy out of context. */

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
	TSTRING fileType; /**< file type if an extension was entered */

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
		const TSTRING& filename = TEXT("EventLog"),
		const TSTRING& path = TEXT("Log"),
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
	 * parses the event level enum to human readable string.
	 * @param el event level enum value.
	 * @return human readable string of event level.
	 */
	TSTRING elToStr( EventLevel el ) const;

	/**
	 * Removes file extentions from the file name.
	 * @param filename name of the file to be sanitised.
	 * @return filename with extnsion stripped
	 */
	static TSTRING RemoveExtension( const TSTRING& filename );

public:

	/**
	 * Function to either create a new EventLog or return an existing one.
	 * @param filename name of the file without path.
	 * @param path path of the file.
	 * @param level minimum event level to pay attention to.
	 * @param wait time to wait between flushes.
	 * @param maxqueue max size of the message backlog.
	 */
	static LOGHANDLE InitialiseLog(
		const TSTRING& filename = TEXT("EventLog"),
		const TSTRING& path = TEXT("Log"),
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
	static void CloseLog ( EventLog* eventLog, const bool force = false );

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


/**
 * Purpose of this class is to provide allow event log to use an auto_ptr when being referenced.
 * This class will encapsulate the event log object and get destroyed when the auto_ptr falls out of scope.
 * Manually closing event logs should not be nessisary due to this.
 */
class LogHandle
{
	EventLog* eLog; /**< Pointer to the event log encapsulated in this log handle. */

public:
	/**
	 * Constructor.
	 * @param eventLog Event log pointer to encapsulate.
	 */
	LogHandle( EventLog* eventLog )
		: eLog( eventLog ) {};

	/**
	 * Sets the max queue size of the encapsulated event log.
	 * @param maxqueue Max Queue size to set.
	 */
	void SetMaxQueue( const int maxqueue )
	{
		if ( !eLog ) return;
		eLog->SetMaxQueue( maxqueue );
	}

	/**
	 * Sets the event level that the event log will log.
	 * @param level Minimum level that the event log will log.
	 */
	void SetEventLevel( const EventLevel level )
	{
		if ( !eLog ) return;
		eLog->SetEventLevel( level );
	}

	/**
	 * Writes a log entry to the log file.
	 * @param level Log Level of the entry.
	 * @param format Format string of the log entry.
	 * @... Variable arguments for the format string.
	 */
	void Write( const EventLevel level, const TCHAR* const format, ... )
	{
		if ( !eLog ) return;
		va_list args;
		va_start( args, format );
		eLog->vWriteLog( format, level, args );
		va_end( args );
	}

	/**
	 * Flushes the log file.
	 */
	void FlushQueue()
	{
		if ( !eLog ) return;
		eLog->FlushQueue();
	}

	/**
	 * Destructor flushes and deletes the log file.
	 */
	~LogHandle()
	{
		if ( !eLog ) return;
		eLog->FlushQueue();
		EventLog::CloseLog( eLog );
		eLog = nullptr;
	}
};

#ifndef DISABLE_LOGGING_

#define OPEN_LOG EventLog::InitialiseLog

#define FLUSH( log ) log->FlushQueue();
#define SET_MAX_QUEUE( log, max ) log->SetMaxQueue( max )
#define SET_LEVEL( log, level ) log->SetEventLevel( level )

#define LOG_CRIT( log, format, ... )  log->Write( EL_CRIT,  format, ##__VA_ARGS__ )
#define LOG_ERROR( log, format, ... ) log->Write( EL_ERROR, format, ##__VA_ARGS__ )
#define LOG_WARN( log, format, ... )  log->Write( EL_WARN,  format, ##__VA_ARGS__ )
#define LOG_DEBUG( log, format, ... ) log->Write( EL_DEBUG, format, ##__VA_ARGS__ )
#define LOG_INFO( log, format, ... )  log->Write( EL_INFO,  format, ##__VA_ARGS__ )

#define CLEANUP_LOGS() EventLog::FlushAll();

#else

#define OPEN_LOG(...) NULL

#define FLUSH( log ) NULL
#define SET_MAX_QUEUE( log, max ) NULL
#define SET_LEVEL( log, level ) NULL

#define LOG_CRIT( log, format, ... )  NULL
#define LOG_ERROR( log, format, ... ) NULL
#define LOG_WARN( log, format, ... )  NULL
#define LOG_DEBUG( log, format, ... ) NULL
#define LOG_INFO( log, format, ... )  NULL

#endif

#endif
