#include "eventlog.h"

#include <stdarg.h>
#include <Shlwapi.h>


#pragma comment( lib, "Shlwapi.lib" )
 
std::map<TSTRING, EventLog*> EventLog::OpenLogs;

std::auto_ptr<EventLog>
EventLog::InitialiseLog(
	const TSTRING filename,
	const TSTRING path,
	const EventLevel level,
	const int wait,
	const int maxqueue )
{
	TSTRING sanitised = SanitiseFileName( filename );

	if( OpenLogs.count( sanitised ) )
	{
		OpenLogs[sanitised]->references += 1;
	}
	else
	{
		OpenLogs[sanitised] = new EventLog( sanitised, path, level, wait );
	}

	return std::auto_ptr<EventLog>( OpenLogs[sanitised] );
}


TSTRING
EventLog::SanitiseFileName( const TSTRING& filename, bool fileTypeNeeded )
{
	TSTRING newFilename = filename;
	TSTRING::size_type ext;
	ext = filename.rfind( '.' );

	if( ext != TSTRING::npos )
	{
		if( filename.substr( ext ) == TEXT(".log") )
		{
			newFilename = filename.substr( 0, ext );
		}
		else if( fileTypeNeeded )
		{
			newFilename = filename + TEXT(".log");
		}
	}
	else if( fileTypeNeeded )
	{
		newFilename = filename + TEXT(".log");
	}

	return newFilename;
}


void
EventLog::SetEventLevel(const EventLevel level)
{
	eventLevel = level;
}


void
EventLog::Write(const EventLevel level, const TCHAR* const format, ...)
{
	if(level <= eventLevel)
	{
		va_list args;
		va_start( args, format );
		vWriteLog( format, level, args );
		va_end( args );
	}
}


void CALLBACK
EventLog::TimerFlush(void* params, BOOLEAN TimerOrWaitFired)
{
	EventLog* el = static_cast<EventLog*>( params );
	
	el->FlushQueue();

	/* clear this after so we don't requeue between the mutex */
	ResetEvent( el->requestedFlush );
}


EventLog::EventLog(
	const TSTRING filename,
	const TSTRING path,
	const EventLevel level,
	const int wait,
	const int maxqueue )
{
	TCHAR exeLocation[MAX_PATH];

	references = 1;

	queueSize = 0;
	SetMaxQueue( maxqueue );

	InitializeCriticalSection( &queueLock );
	InitializeCriticalSection( &fileLock );

	hFile = nullptr;
	SetEventLevel( level );

	filePath = path;
	if(filePath.compare( TEXT("Log") ) == 0)
	{
		HMODULE hm = NULL;

		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
							TEXT("InitialiseLog"),
							&hm);

		GetModuleFileName( hm, exeLocation, MAX_PATH );
		TCHAR* PathEnd = PathFindFileName( exeLocation );
		*PathEnd = '\0';

		filePath = TSTRING( exeLocation );
		filePath.append( TEXT("Log\\") );
	}
	fileName = filename;
	CreateDirectory( filePath.c_str(), NULL );

	requestedFlush = CreateEvent( NULL, TRUE, FALSE, NULL );

	ReopenFile();

	flushWait = wait;
	CreateTimerQueueTimer(
		&hFlushTimer,
		NULL,
		TimerFlush,
		(void*)this,
		INFINITE,
		0,
		WT_EXECUTELONGFUNCTION
	);
}


void
EventLog::CheckNewDay()
{
	SYSTEMTIME cur_time = {0};
	GetLocalTime(&cur_time);

	TCHAR dayChar = cur_time.wDayOfWeek + '0';

	/* get the day number before '.log' and compare */
	if( dayChar != fullFileName.at(fullFileName.rfind('.')-1) )
	{
		ReopenFile();
	}
}


void
EventLog::ReopenFile()
{
	bool append = false;

	SYSTEMTIME sys_time = {0};
	SYSTEMTIME cur_time = {0};
	WIN32_FILE_ATTRIBUTE_DATA file_attr = {0};

	GetLocalTime(&cur_time);

	TCHAR dayChar = cur_time.wDayOfWeek + '0';
	fullFileName = filePath + TEXT("/") + fileName + TEXT("-") + dayChar + TEXT(".log");

	EnterCriticalSection(&fileLock);

	if ( hFile != nullptr )
	{
		CloseHandle( hFile );
	}

	if ( GetFileAttributesEx( fullFileName.c_str(), GetFileExInfoStandard, &file_attr ) != 0 )
	{
		FileTimeToSystemTime(&file_attr.ftLastWriteTime, &sys_time);
		GetSystemTime(&cur_time);

		if( sys_time.wYear == cur_time.wYear && 
			sys_time.wMonth == cur_time.wMonth && 
			sys_time.wDay == cur_time.wDay )
		{
			append = true;
		}
		else
		{
			append = false;
		}
	}

	hFile = CreateFile(
		fullFileName.c_str(), 
		GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, 
		( append ) ? OPEN_ALWAYS : CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL
	);

	if ( append && hFile != INVALID_HANDLE_VALUE )
	{
		SetFilePointer(hFile, 0, NULL, FILE_END);
	}

	LeaveCriticalSection(&fileLock);
}


void
EventLog::SetMaxQueue(const int maxqueue)
{
	maxQueue = maxqueue;
}


TSTRING
EventLog::elToStr( EventLevel el )
{
	TCHAR* str;

	switch( el )
	{
		case EL_DEBUG:    str = TEXT("DEBUG");    break;
		case EL_INFO:     str = TEXT("INFO");     break;
		case EL_WARN:     str = TEXT("WARN");     break;
		case EL_ERROR:    str = TEXT("ERROR");    break;
		case EL_CRITICAL: str = TEXT("CRITICAL"); break;
		default:          str = TEXT("NONE");     break;
	}

	return TSTRING( str );
}


void
EventLog::vWriteLog( const TCHAR* const format, const EventLevel level, va_list args )
{
	int lineLength;
	std::vector<TCHAR> buf;

	SYSTEMTIME cur_time = {0};
	GetLocalTime( &cur_time );
	
	TCHAR header[32];
	wsprintf(
		header, 
		TEXT("%02d:%02d:%02d.%03d|%s|"), 
		cur_time.wHour,
		cur_time.wMinute,
		cur_time.wSecond,
		cur_time.wMilliseconds,
		elToStr(level).c_str()
	);

	TSTRING messageLine( header );
	messageLine += format;

	if ( messageLine.back() != '\n' )
	{
		messageLine += '\n';
	}

	lineLength = vsnprintf_t( NULL, 0, messageLine.c_str(), args );
	buf.resize( lineLength + 1 );

	vsprintf_t( &buf[0], messageLine.c_str(), args );

	EnterCriticalSection( &queueLock );
	Buffer.push( TSTRING( &buf[0] ) );

	bool request_state = ( WaitForSingleObject( requestedFlush, 0 ) == WAIT_OBJECT_0 );

	if ( hFlushTimer && !request_state )
	{
		/* instantly flush the log queue if it has reached its maximum value */
		if ( Buffer.size() >= maxQueue )
		{
			ChangeTimerQueueTimer( NULL, hFlushTimer, 0, flushWait );
		}
		/* start the timer to flush the queue after flushWait has been reached */
		else
		{
			ChangeTimerQueueTimer( NULL, hFlushTimer, flushWait, flushWait );
		}
		SetEvent( requestedFlush );
	}

	LeaveCriticalSection( &queueLock );
}


void
EventLog::FlushQueue()
{
	DWORD written;
	if( hFile && hFile != INVALID_HANDLE_VALUE )
	{
		EnterCriticalSection( &fileLock );
		CheckNewDay();
		
		EnterCriticalSection( &queueLock );
		while( !Buffer.empty() )
		{
			TSTRING& str = Buffer.front();
			WriteFile( hFile, str.c_str(), str.size() * sizeof(TCHAR), &written, NULL );
			Buffer.pop();
		}
		LeaveCriticalSection( &queueLock );
		LeaveCriticalSection( &fileLock );
	}
}


void
EventLog::CloseLog( EventLog* eventLog, const bool force )
{
	CloseLog( eventLog->fileName, force );
}


void
EventLog::CloseLog( const TSTRING filename, const bool force )
{
	TSTRING sanitised = SanitiseFileName( filename );
	std::map< TSTRING, EventLog* >::iterator sit;

	if ( OpenLogs.size() > 0 )
	{
		if( OpenLogs.count( sanitised ) )
		{
			sit = OpenLogs.find( sanitised );
			sit->second->references -= 1;
			if ( sit->second->references == 0 || force )
			{
				delete sit->second;
				OpenLogs.erase( sit );
			}
		}
	}
}


void
EventLog::FlushAll()
{
	std::map<TSTRING, EventLog*>::iterator sit;
	for ( sit = OpenLogs.begin(); sit != OpenLogs.end(); ++sit )
	{
		sit->second->FlushQueue();
	}
}


void
EventLog::CloseAll( const bool force )
{
	TSTRING::size_type i = 0;
	std::vector<TSTRING> filenames;

	std::map<TSTRING, EventLog*>::iterator sit;
	for ( sit = OpenLogs.begin(); sit != OpenLogs.end(); ++sit )
	{
		filenames.push_back( sit->second->fileName );
	}

	/* we need to do this because CloseLog will invalidate sit when called */
	for ( i = 0; i < filenames.size(); ++i )
	{
		CloseLog( filenames[i], true );
	}
}


EventLog::~EventLog()
{
	FlushQueue();

	if( hFlushTimer != nullptr )
	{
		DeleteTimerQueueTimer( NULL, hFlushTimer, NULL );
		hFlushTimer = nullptr;
		CloseHandle( requestedFlush );
	}

	if(hFile != nullptr)
	{
		CloseHandle( hFile );
	}

	DeleteCriticalSection( &queueLock );
	DeleteCriticalSection( &fileLock );
}