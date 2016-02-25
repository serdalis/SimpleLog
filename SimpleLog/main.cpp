#include "eventlog.h"

#include <Windows.h>
#include <process.h>

/** 
 * auto_ptr is used so that when this log file pointer
 * goes out of scope it auto calls the destructor and flushes.
 **/
static LOGHANDLE logFile;

void
makeLog1()
{
	LOGHANDLE logFile1 = OPEN_LOG( TEXT("SimpleLog_Test.log") );

	LOG_CRIT( logFile, TEXT("Single Test Success %d."), 1 );
}

void
makeLog2()
{
	LOGHANDLE logFile2 = OPEN_LOG( TEXT("SimpleLog_Test.log") );

	LOG_CRIT( logFile, TEXT("Single Test Success %d."), 2 );
}

void
makeLog3()
{
	LOGHANDLE logFile3 = OPEN_LOG( TEXT("SimpleLog_Test.log") );

	LOG_CRIT( logFile, TEXT("Single Test Success %d."), 3 );
}

void
makeLog4()
{
	LOGHANDLE logFile4 = OPEN_LOG( TEXT("SimpleLog_Test.log") );

	LOG_CRIT( logFile, TEXT("Single Test Success %d."), 4 );
}

unsigned __stdcall
MultiTest( void* params )
{
	LOGHANDLE logFile = OPEN_LOG( TEXT("SimpleLog_Test.log") );

	int thread_num = *reinterpret_cast<int*>(params);

	/* add a random sleep to allow the threads to stop at non-sequential timings */
	Sleep( rand() % 1000 );

	LOG_CRIT( logFile, TEXT("Multi Test Success %d."), thread_num );

	return 0;
}


#define MAX_THREADS 10

int
main( int argc, char* argv[] )
{
	int error = 0;

	logFile = OPEN_LOG( TEXT("SimpleLog_Test.log") );

	SET_LEVEL( logFile, (EventLevel) EL_WARN );

	makeLog1();
	makeLog2();
	makeLog3();
	makeLog4();

	
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

	CLEANUP_LOGS();

	return error;
}