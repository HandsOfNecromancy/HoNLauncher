#include <iostream>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include <windows.h>
#elif __APPLE__
	#include <mach-o/dyld.h>
#elif __linux__
	#include <unistd.h>
#endif

std::string stripAfterLastSlash(const std::string& str)
{
#ifdef _WIN32
	size_t pos = str.find_last_of('\\');
#else
	size_t pos = str.find_last_of('/');
#endif
	if (pos != std::string::npos)
	{
		return str.substr(0, pos);
	}
	else
	{
		// No slash found, return the original string
		return str;
	}
}

std::string getExecutablePath()
{
	char path[1024];

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	GetModuleFileName(NULL, path, sizeof(path));
#elif __APPLE__
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) != 0)
		std::cout << "Buffer too small, need size: " << size;
#elif __linux__
	ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
	if (count == -1)
		std::cout << "Failed to get executable path.";
#endif

	return stripAfterLastSlash(path);
}
