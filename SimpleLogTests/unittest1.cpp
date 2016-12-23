#include <SDKDDKVer.h>
#include "CppUnitTest.h"

#include "..\SimpleLog\eventlog.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SimpleLogTests
{		
	TEST_CLASS(EventLogTest)
	{
	public:
		
		TEST_METHOD(RemoveExtensionTest)
		{
			Assert::AreEqual( 
				EventLog::RemoveExtension( TEXT("apples.log") ).c_str(),
				TEXT( "apples" ), LINE_INFO() );
		}

	};
}