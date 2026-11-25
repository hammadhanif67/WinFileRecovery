#pragma once
#include "NTFSParser.h"
#include <string>
#include <vector>
#include <unordered_map>

class FileRepair {
public:
    FileRepair();

    bool repairByExtension(RecoveredFileInfo& file);
    bool validateHeader(const std::vector<BYTE>& data, const std::wstring& ext);

    static std::wstring getExtension(const std::wstring& fileName);

private:
    struct FileSignature {
        std::vector<BYTE> magic;
        DWORD             offset;
    };

    std::unordered_map<std::wstring, FileSignature> m_signatures;

    void initSignatures();
    bool checkMagicBytes(const std::vector<BYTE>& data, const FileSignature& sig);
};
