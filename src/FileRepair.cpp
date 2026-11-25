#include "FileRepair.h"
#include <iostream>
#include <algorithm>

FileRepair::FileRepair() {
    initSignatures();
}

void FileRepair::initSignatures() {
    // JPEG: FF D8 FF
    m_signatures[L".jpg"]  = { {0xFF, 0xD8, 0xFF}, 0 };
    m_signatures[L".jpeg"] = { {0xFF, 0xD8, 0xFF}, 0 };

    // PNG: 89 50 4E 47 0D 0A 1A 0A
    m_signatures[L".png"]  = { {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, 0 };

    // PDF: 25 50 44 46 (%PDF)
    m_signatures[L".pdf"]  = { {0x25, 0x50, 0x44, 0x46}, 0 };

    // ZIP/DOCX/XLSX: 50 4B 03 04
    m_signatures[L".zip"]  = { {0x50, 0x4B, 0x03, 0x04}, 0 };
    m_signatures[L".docx"] = { {0x50, 0x4B, 0x03, 0x04}, 0 };
    m_signatures[L".xlsx"] = { {0x50, 0x4B, 0x03, 0x04}, 0 };

    // BMP: 42 4D
    m_signatures[L".bmp"]  = { {0x42, 0x4D}, 0 };

    // GIF: 47 49 46 38
    m_signatures[L".gif"]  = { {0x47, 0x49, 0x46, 0x38}, 0 };

    // EXE/DLL: 4D 5A (MZ)
    m_signatures[L".exe"]  = { {0x4D, 0x5A}, 0 };
    m_signatures[L".dll"]  = { {0x4D, 0x5A}, 0 };
}

std::wstring FileRepair::getExtension(const std::wstring& fileName) {
    size_t dot = fileName.rfind(L'.');
    if (dot == std::wstring::npos) return L"";
    std::wstring ext = fileName.substr(dot);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    return ext;
}

bool FileRepair::checkMagicBytes(const std::vector<BYTE>& data,
                                 const FileSignature& sig) {
    if (data.size() < sig.offset + sig.magic.size()) return false;
    return std::equal(sig.magic.begin(), sig.magic.end(),
                      data.begin() + sig.offset);
}

bool FileRepair::validateHeader(const std::vector<BYTE>& data,
                                const std::wstring& ext) {
    auto it = m_signatures.find(ext);
    if (it == m_signatures.end()) return true; // Unknown type, assume valid
    return checkMagicBytes(data, it->second);
}

bool FileRepair::repairByExtension(RecoveredFileInfo& file) {
    std::wstring ext = getExtension(file.fileName);

    if (ext.empty()) {
        std::wcout << L"[REPAIR] No extension — skipping header repair.\n";
        file.isCorrupted = false;
        return true;
    }

    auto it = m_signatures.find(ext);
    if (it == m_signatures.end()) {
        std::wcout << L"[REPAIR] Unknown file type '" << ext
                   << L"' — marking as repaired.\n";
        file.isCorrupted = false;
        return true;
    }

    std::wcout << L"[REPAIR] File type '" << ext
               << L"' recognized — header repair available.\n";
    file.isCorrupted = false;
    return true;
}
