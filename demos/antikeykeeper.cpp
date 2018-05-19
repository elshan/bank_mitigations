// antikeykeeper.dll - demo
#include "stdafx.h"
#include <easyhook.h>
#include <string>
#include <iostream>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <ShellApi.h>
#include <fstream>

TCHAR path[256];
DWORD gFreqOffset = 0;

SHORT WINAPI GetAsyncKeyStateHook( _In_ int vKey )
{
	MessageBox(NULL, L"Keylogger blocked !!!", L"Anti-key-keeper", MB_OK);
	return 0;
}

HHOOK WINAPI SetWindowsHookExHook(_In_ int idHook, _In_ HOOKPROC lpfn, _In_ HINSTANCE hMod, _In_ DWORD dwThreadId)
{
	if (idHook == WH_KEYBOARD_LL)
	{
		MessageBox(NULL, L"Keylogger blocked !!!", L"Anti-key-keeper", MB_OK);
		return 0;
	}
	else
		return SetWindowsHookEx(idHook, lpfn, hMod, dwThreadId);
}

BOOL WINAPI GetCursorPosHook(_Out_ LPPOINT lpPoint)
{
	return 0;
}

// NOTE: EasyHook will be looking for this export to support DLL injection. If not found then
// DLL injection will fail.
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{

	if (inRemoteInfo->UserDataSize == sizeof(DWORD))
		gFreqOffset = *reinterpret_cast<DWORD *>(inRemoteInfo->UserData);

	// Perform hooking
	HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook
	HOOK_TRACE_INFO hHook2 = { NULL }; 
	HOOK_TRACE_INFO hHook3 = { NULL };



									  // Install the hook
	NTSTATUS result = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("user32")), "GetAsyncKeyState"),
		GetAsyncKeyStateHook,
		NULL,
		&hHook);
	if (FAILED(result))
	{
		std::wstring s(RtlGetLastErrorString());
		std::wcout << "Failed to install hook in antikeykeeper.dll: ";
		std::wcout << s;
	}
	else
	{
		std::wcout << "Anti-key-keeper hooked function GetAsyncKeyState()";
	}

	NTSTATUS result2 = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("kernel32")), "SetWindowsHookEx"),
		SetWindowsHookExHook,
		NULL,
		&hHook2);
	if (FAILED(result2))
	{
		std::wstring s(RtlGetLastErrorString());
		std::wcout << "Failed to install hook2 in antikeykeeper.dll: ";
		std::wcout << s;
	}
	else
	{
		std::wcout << "Anti-key-keeper hooked function SetWindowsHookEx()";
	}

	NTSTATUS result3 = LhInstallHook(
		GetProcAddress(GetModuleHandle(TEXT("user32")), "GetCursorPos"),
		GetCursorPosHook,
		NULL,
		&hHook3);
	if (FAILED(result3))
	{
		std::wstring s(RtlGetLastErrorString());
		std::wcout << "Failed to install hook3 in antikeykeeper.dll: ";
		std::wcout << s;
	}
	else
	{
		std::wcout << "Anti-key-keeper hooked function GetCursorPosHook()";
	}

	// If the threadId in the ACL is set to 0,
	// then internally EasyHook uses GetCurrentThreadId()
	ULONG ACLEntries[1] = { 0 };

	// Disable the hook for the provided threadIds, enable for all others
	LhSetExclusiveACL(ACLEntries, 1, &hHook);

	return;
}
