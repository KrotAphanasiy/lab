#include <iostream>
#include <tchar.h>
#include <string>
#include <vector>
#include <io.h>
#include <Fcntl.h>
#include "windows.h"

using namespace std;

int main(int argc, _TCHAR* argv[]) {
	vector<string> childNames;
	vector<PHANDLE> pipesWrite;
	vector<PHANDLE> pipesRead;
	vector<FILE*> readPipesFiles;
	vector<PROCESS_INFORMATION> childInfo;
	vector<STARTUPINFO> startupInfo;
	vector<BOOL> childRuns;
	SECURITY_ATTRIBUTES pipeAttributes = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

	HANDLE* childFinished = NULL;
	HANDLE* childSentMessage = NULL;


	int childCount = 0;
	cout << "Enter wished child processes count: ";
	cin >> childCount;
	HANDLE startingEvent = CreateEvent(NULL, TRUE, FALSE, _T("START_SENDING_MESSAGES"));
	HANDLE readyToRecieve = CreateEvent(NULL, TRUE, FALSE, _T("PARENT_READY"));

	
	childNames.resize(childCount);
	pipesWrite.resize(childCount);
	pipesRead.resize(childCount);
	readPipesFiles.resize(childCount);
	childInfo.resize(childCount);
	startupInfo.resize(childCount);
	childRuns.resize(childCount);
	childFinished = new HANDLE[childCount];
	childSentMessage = new HANDLE[childCount];

	string eventName;

	for (int counter = 0; counter < childCount; counter++){
		cout << "Enter child`s name: ";

		pipesRead[counter] = new HANDLE;
		pipesWrite[counter] = new HANDLE;
		
		CreatePipe(pipesRead[counter], pipesWrite[counter], &pipeAttributes, 0);

		cin >> childNames[counter];

		eventName = childNames[counter] + "_FINISHED";
		string nameBuf1 = eventName;
		childFinished[counter] = CreateEvent(NULL, TRUE, FALSE, (LPWSTR)nameBuf1.c_str());

		eventName = childNames[counter] + "_SENT_MESSAGE";
		string nameBuf2 = eventName;
		childSentMessage[counter] = CreateEvent(NULL, TRUE, FALSE, (LPWSTR)nameBuf2.c_str());

		startupInfo[counter].dwFlags = STARTF_USESTDHANDLES;
		startupInfo[counter].hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startupInfo[counter].hStdOutput = *pipesWrite[counter]; 
		childRuns[counter] = CreateProcess(_T("C:\\Users\\Михаил\\source\\repos\\Lab5_OS\\Debug\\Lab5_OS_PROCESS.exe"), (LPWSTR)(childNames[counter].c_str()), 
			NULL, NULL, TRUE, NULL, NULL, NULL, &startupInfo[counter], &childInfo[counter]);

		CloseHandle(*pipesWrite[counter]);

		if (!childRuns[counter]) {
			cout << "Not able to start process " << childNames[counter] << endl;
			LPDWORD exitcode = new DWORD;
			GetExitCodeProcess(childInfo[counter].hProcess, exitcode);
			cout << exitcode;
			break;
		}
	}

	SetEvent(startingEvent);
	int whoIsActive;
	char ans[32];
	int numbytes;
	bool read;

	while (WaitForMultipleObjects(childCount, childFinished, TRUE, 0) == WAIT_TIMEOUT) {
		SetEvent(readyToRecieve);
		whoIsActive = WaitForMultipleObjects(childCount, childSentMessage, FALSE, 10000);
		ResetEvent(readyToRecieve);
		if (whoIsActive == WAIT_TIMEOUT) {
			break;
		}
		ResetEvent(childSentMessage[whoIsActive]);

		for (int counter = 0; counter < childCount; counter++) {
			if (whoIsActive == counter) {
				char ans[256];
				int numbytes;
				read = ReadFile((HANDLE)*pipesRead[counter], ans, 255 * sizeof(char), (LPDWORD)&numbytes, NULL);
				ans[numbytes] = '\0';
				if (read) {
					cout << ans;
				}
				else {
					cout << "Read failed! " << GetLastError();
				}
				break;
			}
		}
	}



	return 0;
}