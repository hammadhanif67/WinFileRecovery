#include <windows.h>
#include <iostream>
#include <string>
#include "DiskScanner.h"
#include "FileRecovery.h"

void printUsage() {
    std::wcout << L"Usage: WinFileRecovery.exe <drive> [options]\n";
    std::wcout << L"  Example: WinFileRecovery.exe C:\\\n";
    std::wcout << L"  --deep   Deep scan (slower, more results)\n";
    std::wcout << L"  --repair Also attempt file repair\n";
}

int wmain(int argc, wchar_t* argv[]) {
    // Must run as Administrator
    if (!IsUserAnAdmin()) {
        std::wcerr << L"[ERROR] Run as Administrator.\n";
        return 1;
    }

    if (argc < 2) { printUsage(); return 1; }

    std::wstring drive = argv[1];
    bool deepScan = false, doRepair = false;

    for (int i = 2; i < argc; ++i) {
        if (wcscmp(argv[i], L"--deep") == 0)   deepScan = true;
        if (wcscmp(argv[i], L"--repair") == 0) doRepair = true;
    }

    DiskScanner scanner(drive);
    if (!scanner.open()) {
        std::wcerr << L"[ERROR] Cannot open drive: " << drive << L"\n";
        return 1;
    }

    FileRecovery recovery(scanner);
    auto files = recovery.scan(deepScan);

    std::wcout << L"[INFO] Found " << files.size() << L" recoverable files.\n";

    for (auto& f : files) {
        if (doRepair) recovery.repairFile(f);
        recovery.recoverFile(f, L"output\\");
    }

    std::wcout << L"[DONE] Recovery complete.\n";
    return 0;
}
