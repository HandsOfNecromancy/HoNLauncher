#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "launcher.h"
#include "configfile.h"
#include "getexepath.h"

#if defined(_WIN32)
#include <windows.h>
#define EXT ".exe"
#define CURDIR ".\\"
#elif defined(__APPLE__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#define EXT ""
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#define EXT ""
#endif

int vidmode = 0, userlang = 0;
char* commandline = nullptr;
bool skiplauncher = false;

void LoadSettings()
{
	userlang = 0;
	vidmode = 0;
	auto options = load_options();
	if (options.find("VidMode") != options.end())
		vidmode = atoi(options["VidMode"].data());
	if (options.find("Language") != options.end())
		userlang = atoi(options["Language"].data());
}

void AdjustSettings()
{
	if (std::strstr(commandline, "-nolauncher"))
		skiplauncher = true;
	if (std::strstr(commandline, "-vkdoom"))
	{
		vidmode = 0;
		skiplauncher = true;
	}
	if (std::strstr(commandline, "+set vid_preferbackend 1"))
	{
		vidmode = 2;
		skiplauncher = true;
	}
	if (std::strstr(commandline, "+set vid_preferbackend 2"))
	{
		vidmode = 1;
		skiplauncher = true;
	}
	if (std::strstr(commandline, "+set vid_preferbackend 0"))
	{
		vidmode = 3;
		skiplauncher = true;
	}
}

const char* GetProgram()
{
	switch(vidmode)
	{
	case 0:
		return "engine-vkdoom" EXT;
	default:
	case 1:
		return "engine-classic" EXT " +set vid_preferbackend 2";
	case 2:
		return "engine-classic" EXT " +set vid_preferbackend 1";
	case 3:
		return "engine-classic" EXT " +set vid_preferbackend 0";
	}
}

const char* GetLanguage()
{
	switch(userlang)
	{
	default:
	case 0:
		return "+set language eng";
	case 1:
		return "+set language fr";
	case 2:
		return "+set language de";
	case 3:
		return "+set language ru";
	case 4:
		return "+set language esm";
	case 5:
		return "+set language pt";
	}
}

void LaunchGame()
{
	std::string option1, option2;
	option1 = "     ";
	option2 = "     ";
	//option1 = itoa(userlang, (char*)option1.data(), 10);
	//option2 = itoa(vidmode, (char*)option2.data(), 10);
	snprintf(option1.data(), 5, "%d", userlang);
	snprintf(option2.data(), 5, "%d", vidmode);
	save_options(option1, option2);

#ifdef _WIN32
	char cmdLine[512];
	snprintf(cmdLine, sizeof(cmdLine), "%s %s %s", GetProgram(), GetLanguage(), commandline);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (SetCurrentDirectory(getExecutablePath().data()))
	{
		if (!CreateProcess(NULL,   // No module name (use command line)
			cmdLine,               // Command line
			NULL,                  // Process handle not inheritable
			NULL,                  // Thread handle not inheritable
			FALSE,                 // Set handle inheritance to FALSE
			0,                     // No creation flags
			NULL,                  // Use parent's environment block
			NULL,                  // Use parent's starting directory 
			&si,                   // Pointer to STARTUPINFO structure
			&pi)                   // Pointer to PROCESS_INFORMATION structure
			)
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return;
		}

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		printf("Failed to change to desired path.\n");
	}
#else // (__GNUC__ || __clang__)
	pid_t pid;
	char* argv[4];
	std::string full_path = getExecutablePath() + std::string("/") + std::string((char*)GetProgram());
	argv[0] = full_path.data();
	argv[1] = (char*)GetLanguage();
	argv[2] = commandline;
	argv[3] = NULL;

	//printf("%s\n", argv[0]);

	std::string desiredPath = getExecutablePath();
#ifdef __APPLE__
	desiredPath += "/../../..";
	//printf("%s\n", desiredPath.data());
#endif

	if (chdir(desiredPath.data()) == 0)
	{
#ifdef __linux__
		const char* ld_library_path = getenv("LD_LIBRARY_PATH");
		char new_ld_library_path[2048] = "";
		if (ld_library_path != NULL)
		{
			strcpy(new_ld_library_path, ld_library_path);
			strcat(new_ld_library_path, ":");
		}
		strcat(new_ld_library_path, getExecutablePath().data());
		if (setenv("LD_LIBRARY_PATH", new_ld_library_path, 1) != 0)
		{
			printf("Failed to set LD_LIBRARY_PATH\n");
		}
#endif
		pid = fork();
		if (pid == 0)
		{
			// This is the child process
			execvp(argv[0], argv);

			// execvp will only return if an error occurred.
			printf("An error occurred while launching the process.\n");
			return;
		}
		else if (pid < 0)
		{
			// The fork failed
			printf("An error occurred while forking.\n");
			return;
		}
		else
		{
			// parent process
			// at the moment, do nothing, just let the launcher close
		}
	}
	else
	{
		printf("Failed to change to desired path: %s\n", desiredPath.data());
	}
#endif
#if 0
	//printf("%s %s %s\n", GetProgram(), GetLanguage(), commandline);
	std::string launch(GetProgram());
	launch.append(" ");
	launch.append(GetLanguage());
	launch.append(" ");
	launch.append(commandline);
	system(launch.data());
#endif
}
