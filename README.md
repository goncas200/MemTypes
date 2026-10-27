# MemTypes

A lightweight, cross-platform, header-only C++ library designed for building **arbitrary-precision data types** and customized **low-level memory management** utilities.

## Overview

**MemTypes** provides optimized abstractions for high-performance memory alignment, custom allocations via direct OS kernel calls, and custom-defined integer structures ranging from 128-bit blocks up to a massive 1-Gigabyte big integer implementation.

## Key Features

### 1. Advanced Memory Management
* **Page Alignment:** `alloc_mem` automatically calculates page boundaries (aligned to 4096 bytes) and adds magic number validation headers (`0x4D454D5459504553`) to prevent memory corruption.
* **Direct OS Kernel Allocation:** Bypasses `malloc` by invoking native Windows Virtual APIs (`VirtualAlloc`/`VirtualFree`) on Windows and direct system calls (`sys_mmap`/`sys_munmap`) via inline assembly on Linux x86_64.
* **Optimized Operations:** Includes custom vector and memory copy (`my_memcpy`) routines built for raw speed.

### 2. Custom Extended Types
* **`TYPE8b`**: Standard 64-bit unsigned alias (`unsigned long long`).
* **`TYPE16b`**: A mathematical 128-bit integer type with full arithmetic operators (`+`, `-`, `+=`, `-=`) and comparison logic (`<`, `>`, `==`, etc.).
* **`TYPE32b`**: A 256-bit integer structure utilizing safe carry-over propagation logic.
* **`TYPE1Gb`**: An ultra-large arbitrary data structure allocating a sequential 1-Gigabyte storage block in virtual memory, perfect for intensive numeric processing.
* **`vector<T>`**: Custom dynamic array allocation template that automatically adjusts paging sizing strategies depending on whether elements are larger or smaller than 128 bytes.

## Project Structure

```text
├── MemTypes.hpp       # Core library header containing types and assembly allocations
├── main.cpp           # Demo setup file
├── MemTypes.slnx      # Visual Studio Solution
└── LICENSE.txt        # GPL-3.0 License
```

## Getting Started

### Prerequisites

* C++17 or C++20 compliant compiler.
* Linux x86_64 or Microsoft Windows environment.

### Quick Example

```cpp
#include "MemTypes.hpp"
#include <iostream>

int main() {
    // 1. Safe Paged Memory Allocation (Raw System Level)
    sizetype num_bytes = 1024;
    void* my_ptr = alloc_mem(num_bytes);
    
    if (my_ptr != nullptr) {
        std::cout << "Allocated page-aligned memory block successfully.\n";
        free_mem(my_ptr);
    }

    // 2. Working with Extended Bit Types (Windows environment target)
    #ifdef _WIN32
    TYPE16b large_num(5000);
    large_num += 15000;
    
    // Dynamic Custom Vector tracking
    vector<TYPE16b> my_list;
    my_list.add(large_num);
    #endif

    return 0;
}
```

## License

Distributed under the **GPL-3.0 License**. See `LICENSE.txt` for details.
