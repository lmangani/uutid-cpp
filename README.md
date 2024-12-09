# Heade-only library to generate TSID _(Time-Sortable Unique Identifiers)_
[![C++ CI](https://github.com/lmangani/uutid-cpp/actions/workflows/main.yml/badge.svg)](https://github.com/lmangani/uutid-cpp/actions/workflows/main.yml)

uutid-cpp is a header-only C++11 library that generates time-sortable unique identifiers. These IDs are similar to UUIDs but maintain time-ordering properties, making them perfect for distributed systems where chronological ordering is important.

> The uutid-cpp library is a very loose port of the [https://github.com/flowstack/go-uutid] library

## Features

- Header-only library - just include and use
- Time-sortable IDs
- UUID compatibility
- C++11 compatible
- Thread-safe
- No external dependencies for core functionality
- Timestamp extraction from IDs
- Multiple string format outputs (Hex, UUID)

## Requirements

- C++11 compatible compiler
- CMake 3.14+ (for building tests)

## Installation

As this is a header-only library, you have two options:

### Option 1: Direct inclusion
Simply copy `uutid.hpp` into your project and include it:

```cpp
#include "uutid.hpp"
```

### Option 2: CMake integration
Add as a subdirectory in your CMake project:

```cmake
add_subdirectory(uutid)
target_link_libraries(your_target PRIVATE uutid)
```

## Usage

### Basic Usage

```cpp
#include "uutid.hpp"
#include <iostream>

int main() {
    // Generate a new ID
    auto id = UUTID::new_id();
    
    // Convert to string formats
    std::cout << "Hex format: " << id.to_string() << std::endl;
    std::cout << "UUID format: " << id.to_uuid_string() << std::endl;
    
    // Get the timestamp from an ID
    auto timestamp = id.time();
}
```

### Custom Timestamp

```cpp
auto custom_time = std::chrono::system_clock::now();
auto id = UUTID::new_with_time(custom_time);
```

### Parse from String

```cpp
// From hex string
auto id1 = UUTID::from_string("550e8400e29b41d4a716446655440000");

// From UUID format
auto id2 = UUTID::from_string("550e8400-e29b-41d4-a716-446655440000");
```

### Set Version

```cpp
UUTID::set_version(5);  // Default is 4
```

## Building and Testing

```bash
mkdir build
cd build
cmake ..
cmake --build .
ctest
```

## Technical Details

UUTIDs are 16-byte (128-bit) identifiers with the following structure:

- First 4 bytes: Unix timestamp (seconds)
- Next 4 bytes: Nanoseconds portion of timestamp with version
- Last 8 bytes: Random data with variant bits

This structure ensures that:
1. IDs are chronologically sortable
2. IDs are unique even when generated in the same nanosecond
3. IDs can be converted to standard UUID format

## License

MIT License

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
