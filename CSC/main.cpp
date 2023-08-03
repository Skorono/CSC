#include "pch.h"
#include <tchar.h>
#include <iostream>
#include <Windows.h>
#include "TlHelp32.h"

#define ExceptionFlag 1
#define localPlayer 0x004035C0
#define health 0x5c

void ListProcessModules(DWORD processID);
HANDLE GetWindowProcess(const LPCWSTR WinName, const DWORD Permissions);
DWORD FindProcessModule(const DWORD dwProcessId, const char* szModuleName);
char* lpcwstrToChar(const std::wstring& wstr);


int main() {
	DWORD ClientDLL;
	DWORD ProcID;
	int Player;
	int PlayerHP;

	setlocale(LC_ALL, "Russian");
	
	HANDLE procH = GetWindowProcess(L"Counter-Strike", PROCESS_ALL_ACCESS);
	ProcID = GetProcessId(procH);
	if (procH) {
		std::cout << "Connection Success" << std::endl;
	}
	
	ListProcessModules(ProcID);
	ClientDLL = FindProcessModule(ProcID, "client_s.dll");
	if (ClientDLL) {
		ReadProcessMemory(procH, (LPCVOID)(ClientDLL + localPlayer), &Player, sizeof(Player), NULL);
		ReadProcessMemory(procH, (LPCVOID)(Player + health), &PlayerHP, sizeof(PlayerHP), NULL);
		std::cout << PlayerHP;
	}
}

HANDLE GetWindowProcess(const LPCWSTR WinName, const DWORD Permissions) {
	DWORD pId;
	HANDLE pH;
	HWND hwin;

	hwin = FindWindow(0, WinName);
	if (!hwin) {
		std::cout << "Окно не найдено";
		return NULL;
	}

	GetWindowThreadProcessId(hwin, &pId);
	pH = OpenProcess(Permissions, FALSE, pId);
	if (!pH) {
		std::cout << "Процесс не открывается";
		return NULL;
	}
	return pH;
}

DWORD FindProcessModule(const DWORD dwProcessId, const char* szModuleName) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (!hSnap) {
		return ExceptionFlag;
	}

	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);

	DWORD dwReturn = 0;

	if (Module32First(hSnap, &me32)) {
		while (Module32Next(hSnap, &me32)) {
			if (lstrcmpi(me32.szModule, (LPCWSTR)szModuleName) == 0) {
				dwReturn = (DWORD)me32.modBaseAddr;
				break;
			}
		}
	}
	CloseHandle(hSnap);
	return dwReturn;
}

void ListProcessModules(DWORD processID)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
	if (!hSnap) {
		return;
	}

	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);

	DWORD dwReturn = 0;

	if (Module32First(hSnap, &me32)) {
		while (Module32Next(hSnap, &me32)) {
			std::cout << lpcwstrToChar(me32.szModule) << std::endl;
			}
	}
	CloseHandle(hSnap);
}

char* lpcwstrToChar(const std::wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	char* cstr = new char[strTo.length() + 1];
	strcpy(cstr, strTo.c_str());
	return cstr;
}
