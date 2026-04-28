# WinFileRecovery

Windows File Recovery Software — recovers damaged, deleted, or corrupted files on Windows systems using raw disk-level access via Windows low-level APIs.

## Features

- Scan NTFS file systems at the raw disk level
- Recover deleted files before they are overwritten
- Repair corrupted file headers (JPEG, PNG, PDF, ZIP, DOCX, etc.)
- Deep scan mode for severely damaged drives
- File signature validation and magic byte detection
- Progress reporting and hex dump utilities

## System Requirements

| Component    | Requirement                          |
|-------------|--------------------------------------|
| OS          | Windows 10 / 11 (64-bit)            |
| Compiler    | Visual Studio 2022 / MinGW-w64      |
| C++ Standard| C++17 or higher                      |
| Privileges  | Administrator (required for disk access) |
| RAM         | Minimum 512 MB                       |
| Disk Space  | 50 MB for installation               |

## Project Structure

```
WinFileRecovery/
├── src/
│   ├── main.cpp              // Entry point
│   ├── DiskScanner.cpp/.h    // Raw disk reader
│   ├── NTFSParser.cpp/.h     // NTFS structure parser
│   ├── FileRecovery.cpp/.h   // Recovery logic
│   ├── FileRepair.cpp/.h     // Corruption repair
│   └── Utils.cpp/.h          // Helpers
├── include/
│   └── WinTypes.h            // Windows type aliases
├── build/                    // Compiled output
├── output/                   // Recovered files
└── CMakeLists.txt            // Build config
```

## Architecture

```
┌─────────────────────────────────────┐
│  UI Layer (main.cpp)                │  User interaction, argument parsing
├─────────────────────────────────────┤
│  Recovery Layer (FileRecovery)      │  Orchestrates scan and recovery
├─────────────────────────────────────┤
│  Parser Layer (NTFSParser)          │  Reads MFT, boot sector, file records
├─────────────────────────────────────┤
│  Disk Layer (DiskScanner)           │  Raw sector I/O via WinAPI
├─────────────────────────────────────┤
│  OS Layer (Windows API)             │  DeviceIoControl, CreateFile, ReadFile
└─────────────────────────────────────┘
```

## Build Instructions

### Using CMake (Command Line)

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### Using Visual Studio 2022

1. Open Visual Studio 2022
2. Create New Project > Empty C++ Project
3. Add all `.cpp` files from `src/` to the project
4. Project Properties > Configuration Properties > General > C++ Language Standard = **C++17**
5. Project Properties > Linker > Input > Additional Dependencies: add `shell32.lib`
6. Build > Build Solution (Ctrl + Shift + B)

## Usage

> **IMPORTANT:** This program accesses raw disk sectors. Windows requires Administrator privileges. Always right-click Command Prompt > **Run as Administrator**.

```
WinFileRecovery.exe <drive> [options]
```

### Command Examples

| Command | Description |
|---------|-------------|
| `WinFileRecovery.exe C:` | Quick scan of C: drive |
| `WinFileRecovery.exe D: --deep` | Deep scan of D: drive |
| `WinFileRecovery.exe C: --repair` | Scan and attempt repair |
| `WinFileRecovery.exe D: --deep --repair` | Full deep scan with repair |

### Output

Recovered files are saved to the `output\` folder in the same directory as the executable. Each file retains its original name. Corrupted files may be partial — open with a hex editor to inspect.

## Testing

### Safe Testing — Use a Virtual Machine

**NEVER** test on your main drive. Always use a VM or a USB drive:

1. Install VirtualBox or VMware
2. Create a Windows 10/11 VM
3. Add a small virtual disk (1 GB is enough for testing)
4. Format it as NTFS inside the VM
5. Copy some test files to it
6. Delete some files
7. Run your recovery tool on that virtual disk

### Test Cases

| Test Case | Expected Result |
|-----------|-----------------|
| Scan NTFS drive — no deletions | Zero results returned |
| Delete a .txt file, run scan | File appears in results |
| Recover deleted .txt | File saved to `output\` with correct content |
| Scan FAT32 drive | Boot sector parse fails gracefully with error message |
| Run without admin | Error: "Run as Administrator" shown |
| Pass invalid drive letter | Error: "Cannot open drive" shown |

### Verifying Recovery

- Compare recovered file size with original
- Open in the appropriate application (Notepad for `.txt`, etc.)
- Use `fc /b original.txt recovered.txt` to compare bytes
- For images — open and visually verify

## Key Concepts

### NTFS Terms

| Term | Meaning |
|------|---------|
| MFT (Master File Table) | Database of every file on NTFS |
| LCN (Logical Cluster Number) | Address of a cluster on disk |
| File Record | One entry in the MFT — describes one file |
| $FILE_NAME (0x30) | MFT attribute containing the file name |
| $DATA (0x80) | MFT attribute containing file data or pointer to data |
| Resident attribute | Small data stored directly inside the MFT record |
| Non-resident attribute | Large data stored in clusters — MFT has a pointer |

### Windows API Used

| API Function | Purpose |
|-------------|---------|
| `CreateFileW()` | Open drive as a raw file handle |
| `ReadFile()` | Read bytes from the drive |
| `SetFilePointerEx()` | Seek to a byte offset on the drive |
| `DeviceIoControl()` | Query disk geometry (sector size, drive size) |
| `IsUserAnAdmin()` | Check if running as Administrator |
| `CloseHandle()` | Release the drive handle when done |

## Known Limitations

| Limitation | How to Fix Later |
|-----------|------------------|
| Only reads NTFS | Add FAT32 parser in NTFSParser equivalent |
| Non-resident $DATA not fully handled | Parse data runs (runlist) from MFT |
| No GUI | Add Windows Forms or Qt frontend |
| No file preview | Read first bytes and detect file type by magic bytes |
| Repair is basic | Add per-extension header repair (JPEG, PNG, DOCX, etc.) |

## License

This project is provided for educational purposes. Use at your own risk.
