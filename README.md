# SimpleLog
Windows Multi-Thread Safe Lite Logging

## Usage
To disable logging use the `DISABLE_LOGGING_` macro define.

Include the `eventlog.h` file into all `.cpp` files you want to run the event log in.

Call the EventLog initialisation function:

```C++
OPEN_LOG(
	const TSTRING filename,
	const TSTRING path,
	const EventLevel level,
	const int wait,
	const int maxqueue )
```

Store the result in an `auto_ptr<LogHandle>` with the scope of your choosing.
When the auto_ptr goes out of scope the log file's internal reference count will decrease.
If the internal reference count of the log file has reached 0 then the log will be flushed and closed.

Write to the log exactly like `printf` except with a logging level at the front.

## Basic Example

```C++
#include "eventlog.h"

int
main( int argc, char* argv[] )
{
	LOGHANDLE logFile = OPEN_LOG( TEXT("SimpleLog_Test.log") );

	LOG_CRIT( logFile, TEXT("Lets log a Number! -- %d"), 42 );
	LOG_CRIT( logFile, TEXT("Logging Test Success.") );

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
	LOGHANDLE logFile = OPEN_LOG( TEXT("SimpleLog_Test.log") );

	int thread_num = *reinterpret_cast<int*>(params);

	LOG_CRIT( logFile, TEXT("Multi Test Success %d."), thread_num );

	return 0;
}

int
main( int argc, char* argv[] )
{
	LOGHANDLE logFile = OPEN_LOG( TEXT("SimpleLog_Test.log") );

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

	LOG_CRIT( logFile, TEXT("Logging Test Success.") );

	return 0;
}
```

## TODO
Tests!
