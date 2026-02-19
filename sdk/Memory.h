#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <tlhelp32.h>
#include <windows.h>


class Memory {
public:
  uint32_t process_id = 0;
  HANDLE process_handle = nullptr;

  Memory(const char *process_name) {
    process_id = get_process_id(process_name);
    if (process_id) {
      process_handle =
          OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE |
                          PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
                      FALSE, process_id);
    }
  }

  ~Memory() {
    if (process_handle) {
      CloseHandle(process_handle);
      process_handle = nullptr;
    }
  }

  bool is_valid() const { return process_handle != nullptr; }

  uintptr_t get_module_base(const char *module_name) {
    if (!process_id)
      return 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
    if (snapshot == INVALID_HANDLE_VALUE)
      return 0;

    MODULEENTRY32 entry{};
    entry.dwSize = sizeof(entry);

    uintptr_t base = 0;
    if (Module32First(snapshot, &entry)) {
      do {
        if (_stricmp(entry.szModule, module_name) == 0) {
          base = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
          break;
        }
      } while (Module32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return base;
  }

  template <typename T> bool read(uintptr_t address, T &out) {
    SIZE_T bytes_read = 0;
    if (!process_handle)
      return false;
    return ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(address),
                             &out, sizeof(T), &bytes_read) &&
           bytes_read == sizeof(T);
  }

  template <typename T> T read(uintptr_t address) {
    T buffer{};
    read(address, buffer);
    return buffer;
  }

  bool read_raw(uintptr_t address, void *buffer, size_t size) {
    SIZE_T bytes_read = 0;
    if (!process_handle)
      return false;
    return ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(address),
                             buffer, size, &bytes_read) &&
           bytes_read == size;
  }

  template <typename T> bool write(uintptr_t address, const T &value) {
    SIZE_T bytes_written = 0;
    if (!process_handle)
      return false;
    return WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(address),
                              &value, sizeof(T), &bytes_written) &&
           bytes_written == sizeof(T);
  }

private:
  uint32_t get_process_id(const char *process_name) {
    uint32_t pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
      return 0;

    PROCESSENTRY32 entry{};
    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry)) {
      do {
        if (_stricmp(entry.szExeFile, process_name) == 0) {
          pid = entry.th32ProcessID;
          break;
        }
      } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return pid;
  }
};

extern Memory *g_mem;
