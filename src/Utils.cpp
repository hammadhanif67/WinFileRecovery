#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <filesystem>

namespace Utils {

std::wstring formatFileSize(UINT64 bytes) {
    const wchar_t* units[] = { L"B", L"KB", L"MB", L"GB", L"TB" };
    int idx = 0;
    double size = static_cast<double>(bytes);
    while (size >= 1024.0 && idx < 4) {
        size /= 1024.0;
        ++idx;
    }
    std::wostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << L" " << units[idx];
    return oss.str();
}

std::wstring getTimestamp() {
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm tmBuf;
    localtime_s(&tmBuf, &time);

    wchar_t buf[64];
    wcsftime(buf, sizeof(buf) / sizeof(wchar_t), L"%Y-%m-%d_%H-%M-%S", &tmBuf);
    return std::wstring(buf);
}

void printProgress(UINT64 current, UINT64 total) {
    if (total == 0) return;
    double pct = (double)current / (double)total * 100.0;
    std::wcout << L"\r[PROGRESS] " << std::fixed << std::setprecision(1)
               << pct << L"% (" << formatFileSize(current) << L" / "
               << formatFileSize(total) << L")";
    if (current >= total) std::wcout << L"\n";
}

bool fileExists(const std::wstring& path) {
    return std::filesystem::exists(path);
}

std::wstring getLastErrorMessage() {
    DWORD err = GetLastError();
    LPWSTR msgBuf = nullptr;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&msgBuf, 0, nullptr
    );
    std::wstring msg = msgBuf ? msgBuf : L"Unknown error";
    if (msgBuf) LocalFree(msgBuf);
    return msg;
}

void hexDump(const BYTE* data, DWORD size, DWORD maxLines) {
    DWORD lines = std::min(size / 16 + 1, maxLines);
    for (DWORD i = 0; i < lines; ++i) {
        std::wcout << std::hex << std::setw(8) << std::setfill(L'0')
                   << (i * 16) << L"  ";
        for (DWORD j = 0; j < 16; ++j) {
            DWORD idx = i * 16 + j;
            if (idx < size)
                std::wcout << std::hex << std::setw(2) << std::setfill(L'0')
                           << (int)data[idx] << L" ";
            else
                std::wcout << L"   ";
        }
        std::wcout << L" |";
        for (DWORD j = 0; j < 16; ++j) {
            DWORD idx = i * 16 + j;
            if (idx < size) {
                wchar_t c = (data[idx] >= 32 && data[idx] < 127) ? (wchar_t)data[idx] : L'.';
                std::wcout << c;
            }
        }
        std::wcout << L"|\n";
    }
    std::wcout << std::dec;
}

} // namespace Utils
