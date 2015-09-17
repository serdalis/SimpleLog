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

store the result in an `auto_ptr<EventLog>` with the scope of your choosing.

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

## TODO
Tests!
