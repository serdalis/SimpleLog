#include "eventlog.h"

/** 
 * auto_ptr is used so that when this log file pointer
 * goes out of scope it auto calls the destructor and flushes.
 **/
static std::auto_ptr<LogHandle> logFile;

void
makeLog1()
{
	std::auto_ptr<LogHandle> logFile1 = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success %d."), 1 );
}

void
makeLog2()
{
	std::auto_ptr<LogHandle> logFile2 = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success %d."), 2 );
}

void
makeLog3()
{
	std::auto_ptr<LogHandle> logFile3 = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success %d."), 3 );
}

void
makeLog4()
{
	std::auto_ptr<LogHandle> logFile4 = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success %d."), 4 );
}


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

	

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success.") );

	return error;
}