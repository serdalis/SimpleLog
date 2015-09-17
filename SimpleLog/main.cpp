#include "eventlog.h"

#include <Windows.h>
#include <process.h>

/** 
 * auto_ptr is used so that when this log file pointer
 * goes out of scope it auto calls the destructor and flushes.
 **/
static std::auto_ptr<LogHandle> logFile;

void
makeLog1()
{
	std::auto_ptr<LogHandle> logFile1 = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Single Test Success %d."), 1 );
}

void
makeLog2()
{
	std::auto_ptr<LogHandle> logFile2 = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Single Test Success %d."), 2 );
}

void
makeLog3()
{
	std::auto_ptr<LogHandle> logFile3 = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Single Test Success %d."), 3 );
}

void
makeLog4()
{
	std::auto_ptr<LogHandle> logFile4 = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Single Test Success %d."), 4 );
}

unsigned __stdcall
MultiTest( void* params )
{
	std::auto_ptr<LogHandle> logFile  = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	int thread_num = *reinterpret_cast<int*>(params);

	/* add a random sleep to allow the threads to stop at non-sequential timings */
	Sleep( rand() % 1000 );

	logFile->Write( EL_CRITICAL, TEXT("Multi Test Success %d."), thread_num );

	return 0;
}


#define MAX_THREADS 10

int
main( int argc, char* argv[] )
{
	int error = 0;

	logFile = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->SetEventLevel( (EventLevel) EL_WARN );

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

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success.") );

	return error;
}