#pragma once
#include "DiskScanner.h"
#include "NTFSParser.h"
#include <string>
#include <vector>

class FileRecovery {
public:
    explicit FileRecovery(DiskScanner& scanner);

    std::vector<RecoveredFileInfo> scan(bool deepScan = false);
    bool recoverFile(const RecoveredFileInfo& file, const std::wstring& outDir);
    bool repairFile(RecoveredFileInfo& file);

private:
    DiskScanner& m_scanner;
    NTFSParser   m_parser;

    bool writeRawData(const std::wstring& path, UINT64 offset, UINT64 size);
    std::wstring sanitizeName(const std::wstring& name);
};
