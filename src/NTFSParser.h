#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "DiskScanner.h"

// NTFS Boot Sector (first 512 bytes of NTFS volume)
#pragma pack(push, 1)
struct NTFSBootSector {
    BYTE    jump[3];
    char    oemId[8];          // "NTFS    "
    WORD    bytesPerSector;
    BYTE    sectorsPerCluster;
    BYTE    reserved[7];
    BYTE    mediaDescriptor;
    WORD    reserved2;
    WORD    sectorsPerTrack;
    WORD    numHeads;
    DWORD   hiddenSectors;
    DWORD   reserved3;
    DWORD   reserved4;
    UINT64  totalSectors;
    UINT64  mftLCN;            // Logical Cluster Number of MFT
    UINT64  mftMirrLCN;
    INT32   clustersPerMFTRec; // Negative = power of 2
    INT32   clustersPerIdxBuf;
    UINT64  volumeSerialNumber;
    DWORD   checksum;
};
#pragma pack(pop)

struct RecoveredFileInfo {
    std::wstring  fileName;
    UINT64        mftOffset;   // Where in MFT this record is
    UINT64        fileSize;
    bool          isDeleted;
    bool          isCorrupted;
    UINT64        dataLCN;     // Starting cluster of data
};

class NTFSParser {
public:
    explicit NTFSParser(DiskScanner& scanner);
    bool     parseBootSector();
    std::vector<RecoveredFileInfo> scanMFT(bool deepScan = false);

private:
    DiskScanner&     m_scanner;
    NTFSBootSector   m_boot{};
    DWORD            m_bytesPerCluster;
    UINT64           m_mftByteOffset;
    DWORD            m_mftRecordSize;

    bool parseFileRecord(BYTE* record, RecoveredFileInfo& out);
    bool isDeletedRecord(BYTE* record);
};
