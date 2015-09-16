#include "eventlog.h"

static EventLog* logFile = nullptr; /**< LogFile used */

int
main( int argc, char* argv[] )
{
	int error = 0;

	logFile = EventLog::InitialiseLog( TEXT("SimpleLog_Test.log") );

	logFile->SetEventLevel( (EventLevel) EL_WARN );

	logFile->Write( EL_CRITICAL, TEXT("Logging Test Success.\n") );

	EventLog::CloseAll();

	return error;
}