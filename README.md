# SimpleLog
Windows Multi-Thread Safe Lite Logging

## Usage
Include the `eventlog.h` file into all `.cpp` files you want to run the event log in.

Call the EventLog initialisation function:

```C++
EventLog::InitialiseLog(
	const TSTRING filename,
	const TSTRING path,
	const EventLevel level,
	const int wait,
	const int maxqueue )
```

Store the result in an `auto_ptr<LogHandle>` with the scope of your choosing.
When the auto_ptr goes out of scope the log file's internal reference count will decrease.
If the internal reference count of the log file has reached 0 then the log will be flushed and closed.

Write to the log exactly like `printf` except with a logging level at the front:

Avaliable levels are:

```C++
enum EventLevel
{
	EL_DEBUG    = 5,
	EL_INFO     = 4,
	EL_WARN     = 3,
	EL_ERROR    = 2,
	EL_CRITICAL = 1
}

/* function call is: */
Write( const EventLevel level, const TCHAR* const format, ... )
```

## Basic Example

```C++
#include "eventlog.h"

int
main( int argc, char* argv[] )
{
	std::auto_ptr<LogHandle> logFile;
	logFile = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Lets log a Number! -- %d"), 42 );
	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success.") );

	return 0;
}
```

## Mutli-Threaded Example

```C++
#include "eventlog.h"

#include <Windows.h>
#include <process.h>

#define MAX_THREADS 10

unsigned __stdcall
MultiTest( void* params )
{
	std::auto_ptr<LogHandle> logFile  = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	int thread_num = *reinterpret_cast<int*>(params);

	logFile->Write( EL_CRITICAL, TEXT("Multi Test Success %d."), thread_num );

	return 0;
}

int
main( int argc, char* argv[] )
{
	std::auto_ptr<LogHandle> logFile = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	int i = 0;

	HANDLE threads[MAX_THREADS];
	int thread_nums[MAX_THREADS];

	for ( i = 0; i < MAX_THREADS; ++i )
	{
		thread_nums[i] = i;
		threads[i] = reinterpret_cast<HANDLE>(
					_beginthreadex(NULL, 0U, MultiTest, &thread_nums[i], 0U, NULL));
	}

	WaitForMultipleObjects( MAX_THREADS, threads, true, INFINITE );

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success.") );

	return 0;
}
```

## TODO
Tests!
