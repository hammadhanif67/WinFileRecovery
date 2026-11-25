#include "FileRecovery.h"
#include "FileRepair.h"
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

FileRecovery::FileRecovery(DiskScanner& scanner)
    : m_scanner(scanner), m_parser(scanner) {}

std::vector<RecoveredFileInfo> FileRecovery::scan(bool deepScan) {
    if (!m_parser.parseBootSector()) {
        std::wcerr << L"[ERROR] Boot sector parse failed.\n";
        return {};
    }
    auto files = m_parser.scanMFT(deepScan);
    // Filter: only deleted or corrupted
    std::vector<RecoveredFileInfo> result;
    for (auto& f : files)
        if (f.isDeleted || f.isCorrupted)
            result.push_back(f);
    return result;
}

bool FileRecovery::recoverFile(const RecoveredFileInfo& file,
                               const std::wstring& outDir) {
    fs::create_directories(outDir);
    std::wstring safe = sanitizeName(file.fileName);
    std::wstring dest = outDir + safe;

    std::wcout << L"[RECOVERING] " << safe << L" (" << file.fileSize << L" bytes)\n";

    return writeRawData(dest, file.dataLCN, file.fileSize);
}

bool FileRecovery::writeRawData(const std::wstring& path,
                                UINT64 offset, UINT64 size) {
    const DWORD CHUNK = 65536; // 64 KB chunks
    std::vector<BYTE> buf(CHUNK);
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;

    UINT64 written = 0;
    while (written < size) {
        DWORD toRead = (DWORD)std::min((UINT64)CHUNK, size - written);
        if (!m_scanner.readBytes(offset + written, buf.data(), toRead)) break;
        out.write(reinterpret_cast<char*>(buf.data()), toRead);
        written += toRead;
    }
    return written == size;
}

bool FileRecovery::repairFile(RecoveredFileInfo& file) {
    std::wcout << L"[REPAIR] Attempting repair: " << file.fileName << L"\n";
    FileRepair repair;
    return repair.repairByExtension(file);
}

std::wstring FileRecovery::sanitizeName(const std::wstring& name) {
    std::wstring result = name;
    for (auto& ch : result)
        if (ch == L'<' || ch == L'>' || ch == L':' ||
            ch == L'"' || ch == L'/' || ch == L'\\' ||
            ch == L'|' || ch == L'?' || ch == L'*')
            ch = L'_';
    return result;
}
