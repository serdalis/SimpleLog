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


## TODO
Tests!
