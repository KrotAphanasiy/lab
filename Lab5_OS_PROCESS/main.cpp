#include <iostream>
#include "windows.h"
#include <tchar.h>
#include <random>
using namespace std;

int main(int argc, _TCHAR* argv[]) {

	LPWSTR procName = GetCommandLine();
	int messageCounter = 0;

	HANDLE startingEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("START_SENDING_MESSAGES"));
	HANDLE parentReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("PARENT_READY"));

	string finishedEventName = (const char*)procName;
	finishedEventName.erase(finishedEventName.end());
	finishedEventName.append("_FINISHED");
	HANDLE finished = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LPWSTR)finishedEventName.c_str());

	string messageSentEventName = (const char*)procName;
	messageSentEventName.erase(messageSentEventName.end());
	messageSentEventName.append("_SENT_MESSAGE");
	HANDLE messageSent = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LPWSTR)messageSentEventName.c_str());

	random_device rd;
	mt19937 mersenne(rd());

	WaitForSingleObject(startingEvent, INFINITE);

	int sleepTime = 0;
	string toPut;
	toPut = (const char*)procName;
	int bytesSent;
	while (messageCounter < 10) {

		WaitForSingleObject(parentReady, INFINITE);

		cout << toPut << " " << messageCounter << endl;
		SetEvent(messageSent);
		messageCounter++;
		sleepTime = 1000 + mersenne() % 4000;
		Sleep(sleepTime);
	}

	SetEvent(finished);
	return 0;
}