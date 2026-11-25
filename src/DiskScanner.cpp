#include "DiskScanner.h"
#include <iostream>
#include <stdexcept>

DiskScanner::DiskScanner(const std::wstring& drivePath)
    : m_drivePath(drivePath), m_hDrive(INVALID_HANDLE_VALUE),
      m_sectorSize(512), m_driveSize(0) {}

DiskScanner::~DiskScanner() { close(); }

bool DiskScanner::open() {
    // Build path e.g. \\.\C:
    std::wstring path = L"\\\\.\\" + m_drivePath;
    if (path.back() == L':') path = path.substr(0, path.size());

    m_hDrive = CreateFileW(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING,   // Bypass cache for raw access
        nullptr
    );

    if (m_hDrive == INVALID_HANDLE_VALUE) {
        std::wcerr << L"CreateFileW failed: " << GetLastError() << L"\n";
        return false;
    }
    return queryDriveInfo();
}

bool DiskScanner::queryDriveInfo() {
    DISK_GEOMETRY_EX geo{};
    DWORD bytesRet = 0;
    if (!DeviceIoControl(m_hDrive, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
        nullptr, 0, &geo, sizeof(geo), &bytesRet, nullptr)) {
        std::wcerr << L"IOCTL failed: " << GetLastError() << L"\n";
        return false;
    }
    m_sectorSize = geo.Geometry.BytesPerSector;
    m_driveSize  = geo.DiskSize.QuadPart;
    return true;
}

bool DiskScanner::readSector(UINT64 lba, BYTE* buffer, DWORD count) {
    UINT64 offset = lba * m_sectorSize;
    LARGE_INTEGER li;
    li.QuadPart = offset;
    if (!SetFilePointerEx(m_hDrive, li, nullptr, FILE_BEGIN))
        return false;

    DWORD bytesRead = 0;
    DWORD toRead    = count * m_sectorSize;
    return ReadFile(m_hDrive, buffer, toRead, &bytesRead, nullptr)
           && bytesRead == toRead;
}

bool DiskScanner::readBytes(UINT64 offset, BYTE* buffer, DWORD size) {
    // Align to sector boundary
    UINT64 alignedOff = (offset / m_sectorSize) * m_sectorSize;
    DWORD  delta      = (DWORD)(offset - alignedOff);
    DWORD  sectors    = (delta + size + m_sectorSize - 1) / m_sectorSize;

    std::vector<BYTE> tmp(sectors * m_sectorSize);
    UINT64 lba = alignedOff / m_sectorSize;
    if (!readSector(lba, tmp.data(), sectors)) return false;

    memcpy(buffer, tmp.data() + delta, size);
    return true;
}

void DiskScanner::close() {
    if (m_hDrive != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hDrive);
        m_hDrive = INVALID_HANDLE_VALUE;
    }
}

UINT64 DiskScanner::getDriveSize() const { return m_driveSize; }
DWORD  DiskScanner::getSectorSize() const { return m_sectorSize; }
bool   DiskScanner::isOpen()        const { return m_hDrive != INVALID_HANDLE_VALUE; }
