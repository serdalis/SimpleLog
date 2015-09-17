#include "eventlog.h"

/** 
 * auto_ptr is used so that when this log file pointer
 * goes out of scope it auto calls the destructor and flushes.
 **/
static std::auto_ptr<EventLog> logFile;

int
main( int argc, char* argv[] )
{
	int error = 0;

	logFile = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->SetEventLevel( (EventLevel) EL_WARN );

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success.") );

	return error;
}