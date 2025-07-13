#include "sdk/sdk.hpp"
#include <ImageHlp.h>
#include <Psapi.h>
#include <deque>
#include <sstream>
#include <fstream>
#include <format>
#include "shared/libs/discord/discord.h"
#include "game/helpers/cvars.hpp"
#pragma comment(lib, "imagehlp.lib")

void SendWebhook(const char* message) {
	const std::string webhook_url = "https://discord.com/api/webhooks/1385579999460982784/W1BvzN93VDltH5qEcg5RQr_CcRf9vR3WFpS0tlchjW24jkQlV7B2pr7skLEZLfqxPEwP";
	DiscordWebhook webhook(webhook_url);

	webhook.setContent("crashlog");
	webhook.setUsername("crashlog event");
	webhook.setAvatarUrl("https://cdn.discordapp.com/attachments/1382854518760280136/1383725203632422942/image.png?ex=684fd604&is=684e8484&hm=fed407ddb6d602dd2685e53f4e62ff369bdf8e67143452933d6c525076077c81&");

	DiscordEmbed embed("user crashed!", message);
	embed.setColor(0x00FF00);
	embed.setTimestamp();

	webhook.addEmbed(embed);

	webhook.execute();
}

constexpr int errc_noerr{ 0 };
constexpr int errc_sigs{ 1 };
constexpr int errc_ifaces{ 2 };
constexpr int errc_hasload{ 3 };
constexpr int errc_hasunload{ 4 };
constexpr int errc_hooks{ 5 };

struct Frame
{
	std::string m_sModule = "";
	uintptr_t m_uBase = 0;
	uintptr_t m_uAddress = 0;
	std::string m_sFile = "";
	unsigned int m_uLine = 0;
	std::string m_sName = "";
};

static std::deque<Frame> StackTrace(PCONTEXT context)
{
	HANDLE hProcess = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();

	if (!SymInitialize(hProcess, nullptr, TRUE))
		return {};

	SymSetOptions(SYMOPT_LOAD_LINES);

	STACKFRAME64 frame = {};
	frame.AddrPC.Offset = context->Rip;
	frame.AddrFrame.Offset = context->Rbp;
	frame.AddrStack.Offset = context->Rsp;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Mode = AddrModeFlat;

	std::deque<Frame> vTrace = {};
	while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, hProcess, hThread, &frame, context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
	{
		Frame tFrame = {};

		tFrame.m_uAddress = frame.AddrPC.Offset;

		if (auto hBase = HINSTANCE(SymGetModuleBase64(hProcess, frame.AddrPC.Offset)))
		{
			tFrame.m_uBase = uintptr_t(hBase);

			char buffer[MAX_PATH];
			if (GetModuleBaseName(hProcess, hBase, buffer, sizeof(buffer) / sizeof(char)))
				tFrame.m_sModule = buffer;
			else
				tFrame.m_sModule = std::format("{:#x}", tFrame.m_uBase);
		}

		{
			DWORD dwOffset = 0;
			IMAGEHLP_LINE64 line = {};
			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			if (SymGetLineFromAddr64(hProcess, frame.AddrPC.Offset, &dwOffset, &line))
			{
				tFrame.m_sFile = line.FileName;
				tFrame.m_uLine = line.LineNumber;
				auto find = tFrame.m_sFile.rfind("\\");
				if (find != std::string::npos)
					tFrame.m_sFile.replace(0, find + 1, "");
			}
		}

		{
			uintptr_t dwOffset = 0;
			char buf[sizeof(IMAGEHLP_SYMBOL64) + 255];
			auto symbol = PIMAGEHLP_SYMBOL64(buf);
			symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64) + 255;
			symbol->MaxNameLength = 254;
			if (SymGetSymFromAddr64(hProcess, frame.AddrPC.Offset, &dwOffset, symbol))
				tFrame.m_sName = symbol->Name;
		}

		vTrace.push_back(tFrame);
	}
	//if (!vTrace.empty())
	//	vTrace.pop_front();

	SymCleanup(hProcess);

	return vTrace;
}

std::string GetMotherboardSerial() {
	std::string result;

	// Try WMI first (more reliable)
	HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (SUCCEEDED(hres)) {
		// WMI code would go here, but for simplicity, we'll use registry
		CoUninitialize();
	}

	// Try multiple registry locations
	const char* keys[] = {
		"HARDWARE\\DESCRIPTION\\System\\BIOS",
		"SYSTEM\\CurrentControlSet\\Control\\SystemInformation"
	};

	const char* values[] = {
		"BaseBoardSerialNumber",
		"SystemSerialNumber",
		"SystemProductName"
	};

	HKEY hKey;
	char buffer[256] = { 0 };
	DWORD size = sizeof(buffer);

	for (const auto& key : keys) {
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			for (const auto& value : values) {
				size = sizeof(buffer);
				if (RegQueryValueExA(hKey, value, nullptr, nullptr, (LPBYTE)buffer, &size) == ERROR_SUCCESS) {
					if (strlen(buffer) > 0) {
						result = buffer;
						RegCloseKey(hKey);
						return result;
					}
				}
			}
			RegCloseKey(hKey);
		}
	}

	return result;
}

static LONG APIENTRY ExceptionFilter(PEXCEPTION_POINTERS ExceptionInfo) {
	static std::unordered_map<LPVOID, bool> mAddresses = {};
	static bool bException = false;

	if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION
		|| mAddresses.contains(ExceptionInfo->ExceptionRecord->ExceptionAddress)
		|| (bException && GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_RETURN) & 0x8000)) {
		return EXCEPTION_EXECUTE_HANDLER;
	}
	mAddresses[ExceptionInfo->ExceptionRecord->ExceptionAddress] = true;

	std::stringstream ssErrorStream;
	char username[256];
	DWORD username_len = sizeof(username);
	GetUserNameA(username, &username_len);
	ssErrorStream << std::format("with the username of {}\n", username);
	ssErrorStream << std::format("Motherboard: {}\n", GetMotherboardSerial());
	ssErrorStream << std::format("Error: {:#X}\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
	ssErrorStream << std::format("Address: {:#X}\n\n", reinterpret_cast<uintptr_t>(ExceptionInfo->ExceptionRecord->ExceptionAddress));
	ssErrorStream << std::format("RIP: {:#x}\n", ExceptionInfo->ContextRecord->Rip);
	ssErrorStream << std::format("RAX: {:#x}\n", ExceptionInfo->ContextRecord->Rax);
	ssErrorStream << std::format("RCX: {:#x}\n", ExceptionInfo->ContextRecord->Rcx);
	ssErrorStream << std::format("RDX: {:#x}\n", ExceptionInfo->ContextRecord->Rdx);
	ssErrorStream << std::format("RBX: {:#x}\n", ExceptionInfo->ContextRecord->Rbx);
	ssErrorStream << std::format("RSP: {:#x}\n", ExceptionInfo->ContextRecord->Rsp);
	ssErrorStream << std::format("RBP: {:#x}\n", ExceptionInfo->ContextRecord->Rbp);
	ssErrorStream << std::format("RSI: {:#x}\n", ExceptionInfo->ContextRecord->Rsi);
	ssErrorStream << std::format("RDI: {:#x}\n\n", ExceptionInfo->ContextRecord->Rdi);

	auto vTrace = StackTrace(ExceptionInfo->ContextRecord);
	if (!vTrace.empty()) {
		for (auto& tFrame : vTrace) {
			if (tFrame.m_uBase)
				ssErrorStream << std::format("{}+{:#x}", tFrame.m_sModule, tFrame.m_uAddress - tFrame.m_uBase);
			else
				ssErrorStream << std::format("{:#x}", tFrame.m_uAddress);
			if (!tFrame.m_sFile.empty())
				ssErrorStream << std::format(" ({} L{})", tFrame.m_sFile, tFrame.m_uLine);
			if (!tFrame.m_sName.empty())
				ssErrorStream << std::format(" ({})", tFrame.m_sName);
			ssErrorStream << "\n";
		}
		ssErrorStream << "\n";
	}

	ssErrorStream << "Built @ " __DATE__ ", " __TIME__ "\n";
	ssErrorStream << "Ctrl + C to copy. \n";
	if (bException)
		ssErrorStream << "Shift + Enter to skip repetitive exceptions. \n";
	bException = true;
	try {
		SendWebhook(ssErrorStream.str().c_str());
	}
	catch (...) {}

	MessageBoxA(0, ssErrorStream.str().c_str(), "exception", MB_OK | MB_ICONERROR);
	return EXCEPTION_EXECUTE_HANDLER;
}

void dump_error_string(const std::string& errors, const bool copyToClipboard)
{
	MessageBoxA(nullptr, errors.c_str(), "[DES]]", MB_OK | MB_ICONERROR);

	if (copyToClipboard)
	{
		const char* output = errors.c_str();
		const size_t len = errors.size() + 1; // + null terminator

		// Allocate movable memory
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		if (!hMem)
			return;

		// Lock the memory and copy the error string into it
		LPVOID pMem = GlobalLock(hMem);
		if (!pMem)
		{
			GlobalFree(hMem);
			return;
		}
		memcpy(pMem, output, len);
		GlobalUnlock(hMem);

		// Open the clipboard and set the data
		if (OpenClipboard(nullptr))
		{
			if (EmptyClipboard())
			{
				if (SetClipboardData(CF_TEXT, hMem) == nullptr)
					GlobalFree(hMem);
			}
			else
				GlobalFree(hMem);

			CloseClipboard();
		}
		else
			GlobalFree(hMem);
	}
}

int load()
{
	std::string errorSigs{}, errorIfaces{};
	bool errSigs = false, errIfaces = false;
	for (Sig *const s : getInsts<Sig>())
	{
		if (!s || !s->init()) {
			if (!errSigs)
				errSigs = true;

			errorSigs += std::format("{}\n", s->name());
		}
	}

	if (errSigs) {
		dump_error_string(errorSigs, true);
		return errc_sigs;
	}

	for (Iface *const i : getInsts<Iface>())
	{
		if (!i || !i->init()) {
			if (!errIfaces)
				errIfaces = true;

			errorIfaces += std::format("{}\n", i->name());
		}
	}

	if (errIfaces)
	{
		dump_error_string(errorIfaces, true);
		return errc_ifaces;
	}

	for (CvarInit* const l : getInsts<CvarInit>())
	{
		if (!l || !l->init()) {
			return errc_hasload;
		}
	}


	for (HasLoad *const l : getInsts<HasLoad>())
	{
		if (!l || !l->onLoad()) {
			return errc_hasload;
		}
	}

	if (MH_Initialize() != MH_OK) {
		return errc_hooks;
	}

	for (Hook *const h : getInsts<Hook>())
	{
		if (!h || !h->init()) {
			return errc_hooks;
		}
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		return errc_hooks;
	}

	return errc_noerr;
}

int unload()
{
	if (MH_Uninitialize() != MH_OK) {
		return errc_hooks;
	}

	for (HasUnload *const u : getInsts<HasUnload>())
	{
		if (!u || !u->onUnload()) {
			return errc_hasunload;
		}
	}

	return errc_noerr;
}

static PVOID pHandle;

DWORD WINAPI mainThread(LPVOID param)
{
	pHandle = AddVectoredExceptionHandler(1, ExceptionFilter);
	if (const int errc{ load() }) {
		MessageBoxA(0, std::format("failed to load ({})", errc).c_str(), "pipeline", 0);
		FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(param), EXIT_FAILURE);
	}

	while (!GetAsyncKeyState(VK_F11)) {
		Sleep(500);
	}

	if (const int errc{ unload() }) {
		MessageBoxA(0, std::format("failed to unload ({})", errc).c_str(), "pipeline", 0);
		FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(param), EXIT_FAILURE);
	}

	Sleep(500);
	RemoveVectoredExceptionHandler(pHandle);
	FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(param), EXIT_SUCCESS);
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
	if (reason != DLL_PROCESS_ATTACH) {
		return FALSE;
	}

	const HANDLE main_thread{ CreateThread(0, 0, mainThread, inst, 0, 0) };

	if (!main_thread) {
		return FALSE;
	}

	CloseHandle(main_thread);

	return TRUE;
}