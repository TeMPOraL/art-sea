#include <sstream>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "Debug.h"

//================================================================
// Static variable definitions
//================================================================
std::stringstream CDebugTools::callStack;
Ogre::Log* CDebugTools::logTarget = NULL;

//================================================================
// Function implementations
//================================================================
void CDebugTools::displayException(const char* type, const char* message)
{
	std::stringstream buffer;
	buffer << message << std::endl << std::endl;
	buffer << "Call stack:" << std::endl << getCallStack() << std::endl;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	MessageBox( NULL, buffer.str().c_str(), type, MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
	std::cerr << type << std::endl << std::endl << buffer.str() << std::flush;
#endif

	ARTSEA_FATAL_LOG << buffer.str().c_str();
}