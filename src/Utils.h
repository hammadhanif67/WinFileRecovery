#pragma once
#include <windows.h>
#include <string>
#include <vector>

namespace Utils {

std::wstring formatFileSize(UINT64 bytes);

std::wstring getTimestamp();

void printProgress(UINT64 current, UINT64 total);

bool fileExists(const std::wstring& path);

std::wstring getLastErrorMessage();

void hexDump(const BYTE* data, DWORD size, DWORD maxLines = 16);

} // namespace Utils
