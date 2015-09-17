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

 - EL_DEBUG
 - EL_INFO
 - EL_WARN
 - EL_ERROR
 - EL_CRITICAL

```C++
void
EventLog::Write( const EventLevel level, const TCHAR* const format, ... )


## TODO
Tests!
