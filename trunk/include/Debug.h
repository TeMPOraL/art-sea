#ifndef __ART_SEA_DEBUG_H__
#define __ART_SEA_DEBUG_H__

#include <cassert>
#include <sstream>
#include <errno.h>
#include <cstring>
#include <stdexcept>

#include <OgreLogManager.h>
#include <OgreLog.h>

//Use following defines to log artSea-specific log messages to Ogre default logger.
//Those defines introduce a common log message format of:
//[Message type char] Message
//It can be used for batch log message parsing.
//In the future this concept might be replaced with some sort of subclassing or other type
//of Ogre tweaking.
#define ARTSEA_LOG CDebugTools::getLog()->stream() << "[I] "
#define ARTSEA_DEBUG_LOG CDebugTools::getLog()->stream() << "[D] "
#define ARTSEA_WARNING_LOG CDebugTools::getLog()->stream() << "[W] "
#define ARTSEA_ERROR_LOG CDebugTools::getLog()->stream() << "[E] "
#define ARTSEA_FATAL_LOG CDebugTools::getLog()->stream() << "[F] "

//some useful debugging macros:

//do an instruction or a block of instructions in debug mode only
#ifndef NDEBUG
	#define ARTSEA_DEBUG(instruction) instruction
	#define ARTSEA_DEBUG_BLOCK_START {
	#define ARTSEA_DEBUG_BLOCK_END }
#else
	#define ARTSEA_DEBUG(instruction)
	#define ARTSEA_DEBUG_BLOCK_START if(0){
	#define ARTSEA_DEBUG_BLOCK_END }
#endif


//assertions
#define ARTSEA_COMPILE_ASSERT(expression) typedef char __ARTSEA_COMPILE_ASSERT_FAILED__[(expression)?1:-1]
#define ARTSEA_LOW_LEVEL_ASSERT(expr,descr) assert((expr) && (descr))

#ifndef ARTSEA_NOASSERT
	#define ARTSEA_ASSERT(expr,descr) if(!(expr)) { CDebugTools::assertFailed(#expr,descr,__LINE__,__FILE__); }
#else
	#define ARTSEA_ASSERT(expr,descr)
#endif

//guarding
#ifndef ARTSEA_NOGUARD
	#define ARTSEA_GUARD(functionName) {static const char __ARTSEA_GUARDED_FUNCTION_NAME__[] = #functionName ; try{
	#define ARTSEA_UNGUARD } catch(...) { CDebugTools::addCallStackEntry(__ARTSEA_GUARDED_FUNCTION_NAME__); throw; } }
#else
	#define ARTSEA_GUARD(functionName) {
	#define ARTSEA_UNGUARD }
#endif

//second set of macros for performance-critical locations
#ifndef ARTSEA_NOGUARD_SLOW
	#define ARTSEA_GUARD_SLOW(functionName) {static const char __ARTSEA_GUARDED_FUNCTION_NAME__[] = #functionName ; try{
	#define ARTSEA_UNGUARD_SLOW } catch(...) { CDebugTools::addCallStackEntry(__ARTSEA_GUARDED_FUNCTION_NAME__); throw; } }
#else
	#define ARTSEA_GUARD_SLOW(functionName) {
	#define ARTSEA_UNGUARD_SLOW }
#endif

//checking return values (int only)
#ifndef ARTSEA_NCHECK_RETVALS
	#define ARTSEA_CHECK(line,expected) { int res = line; if(res != (expected)) { CDebugTools::abort(__LINE__, __FILE__, #line, #expected, expected, res);} }
#else
	#define ARTSEA_CHECK(line,expected) { line; }
#endif

//Processing code

class CDebugTools
{
private:
	static std::stringstream callStack;
	static Ogre::Log* logTarget;	//Ogre log that will be the target of our logging
public:
	static void assertFailed(const char* exprString, const char* description, unsigned int line, const char* fileName)
	{
		std::stringstream strBuff;
		strBuff << "ASSERTION FAILED!" << std::endl;
		strBuff << "Failed when evaluating: " << exprString << std::endl;
		strBuff << "File: " << fileName << "; line: " << line << std::endl;
		strBuff << "Description: " << description << std::endl;
		//strBuff << "C Runtime Library ERRNO: " << errno << " - " << std::strerror(errno);
		//ERRNO moved to ViewTools

		throw std::logic_error(strBuff.str());
	}
	
	static void addCallStackEntry(const char* what)
	{
		callStack << what << std::endl;
	}
	
	static std::string getCallStack()
	{
		return callStack.str();
	}
	
	static void abort(unsigned int line, const char* fileName, const char* expression, const char* expected, int expectedValue, int result)
	{
		std::stringstream strBuff;
		strBuff << "ERROR!" << std::endl << expression << ": expected: " << expected << " (" << expectedValue << "), got: " << result << std::endl;
		strBuff << "File: " << fileName << "; line: " << line << std::endl;
		//strBuff << "C Runtime Library ERRNO: " << errno << " - " << std::strerror(errno);
		//ERRNO moved to ViewTools
		
		throw std::runtime_error(strBuff.str());
	}

	static void displayException(const char* type, const char* message);

	static void createLog(const char* name, bool bAsDefault = false)
	{
		logTarget = Ogre::LogManager::getSingleton().createLog(name, bAsDefault);
	}

	static Ogre::Log* getLog()
	{
		if(logTarget == NULL)
		{
			return Ogre::LogManager::getSingleton().getDefaultLog();
		}
		return logTarget;
	}
};


#endif //__ART_SEA_DEBUG_H__