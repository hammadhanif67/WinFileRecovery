#include "NTFSParser.h"
#include <cstring>
#include <iostream>

// MFT record signature
static const DWORD MFT_MAGIC = 0x454C4946; // "FILE"

NTFSParser::NTFSParser(DiskScanner& scanner)
    : m_scanner(scanner), m_bytesPerCluster(0),
      m_mftByteOffset(0), m_mftRecordSize(1024) {}

bool NTFSParser::parseBootSector() {
    BYTE buf[512] = {};
    if (!m_scanner.readSector(0, buf)) return false;

    memcpy(&m_boot, buf, sizeof(NTFSBootSector));

    // Validate NTFS signature
    if (strncmp(m_boot.oemId, "NTFS    ", 8) != 0) {
        std::wcerr << L"Not an NTFS volume.\n";
        return false;
    }

    m_bytesPerCluster = m_boot.bytesPerSector * m_boot.sectorsPerCluster;

    // MFT starts at mftLCN * bytesPerCluster
    m_mftByteOffset   = m_boot.mftLCN * m_bytesPerCluster;

    // MFT record size: if positive = clusters, if negative = 2^abs
    if (m_boot.clustersPerMFTRec < 0)
        m_mftRecordSize = 1u << (-m_boot.clustersPerMFTRec);
    else
        m_mftRecordSize = m_boot.clustersPerMFTRec * m_bytesPerCluster;

    return true;
}

std::vector<RecoveredFileInfo> NTFSParser::scanMFT(bool deepScan) {
    std::vector<RecoveredFileInfo> results;
    std::vector<BYTE> record(m_mftRecordSize);

    UINT64 offset = m_mftByteOffset;
    UINT64 limit  = deepScan ? m_scanner.getDriveSize() : offset + (50ULL * 1024 * 1024);

    while (offset < limit) {
        if (!m_scanner.readBytes(offset, record.data(), m_mftRecordSize)) break;

        DWORD magic = *reinterpret_cast<DWORD*>(record.data());
        if (magic == MFT_MAGIC) {
            RecoveredFileInfo info{};
            if (parseFileRecord(record.data(), info)) {
                info.mftOffset = offset;
                results.push_back(info);
            }
        }
        offset += m_mftRecordSize;
    }
    return results;
}

bool NTFSParser::isDeletedRecord(BYTE* record) {
    // Offset 22: flags. Bit 0 = in use. If 0 = deleted.
    WORD flags = *reinterpret_cast<WORD*>(record + 22);
    return (flags & 0x01) == 0;
}

bool NTFSParser::parseFileRecord(BYTE* record, RecoveredFileInfo& out) {
    out.isDeleted   = isDeletedRecord(record);
    out.mftOffset   = 0; // filled by caller
    out.isCorrupted = false;

    // Attribute offset at bytes 20-21
    WORD attrOffset = *reinterpret_cast<WORD*>(record + 20);
    BYTE* attr      = record + attrOffset;

    while (attr < record + m_mftRecordSize) {
        DWORD type = *reinterpret_cast<DWORD*>(attr);
        if (type == 0xFFFFFFFF) break; // End marker

        DWORD len = *reinterpret_cast<DWORD*>(attr + 4);
        if (len == 0) break;

        // 0x30 = $FILE_NAME attribute
        if (type == 0x30) {
            BYTE nonResident = attr[8];
            if (!nonResident) {
                WORD contentOff = *reinterpret_cast<WORD*>(attr + 20);
                BYTE* content   = attr + contentOff;
                // File name starts at offset 66 within $FILE_NAME
                BYTE nameLen    = content[64];
                WCHAR* name     = reinterpret_cast<WCHAR*>(content + 66);
                out.fileName    = std::wstring(name, nameLen);
            }
        }
        // 0x80 = $DATA attribute — get file size
        if (type == 0x80) {
            BYTE nonResident = attr[8];
            if (!nonResident) {
                WORD  contentOff = *reinterpret_cast<WORD*>(attr + 20);
                DWORD size       = *reinterpret_cast<DWORD*>(attr + 16);
                out.fileSize     = size;
            } else {
                out.fileSize = *reinterpret_cast<UINT64*>(attr + 48);
            }
        }
        attr += len;
    }
    return !out.fileName.empty();
}
