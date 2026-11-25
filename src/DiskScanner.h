#pragma once
#include <windows.h>
#include <string>
#include <vector>

class DiskScanner {
public:
    explicit DiskScanner(const std::wstring& drivePath);
    ~DiskScanner();

    bool open();
    void close();
    bool readSector(UINT64 lba, BYTE* buffer, DWORD sectorCount = 1);
    bool readBytes(UINT64 offset, BYTE* buffer, DWORD size);
    UINT64 getDriveSize() const;
    DWORD getSectorSize() const;
    bool isOpen() const;

private:
    std::wstring   m_drivePath;
    HANDLE         m_hDrive;
    DWORD          m_sectorSize;
    UINT64         m_driveSize;

    bool queryDriveInfo();
};
