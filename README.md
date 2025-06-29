# Multi-threaded Web Crawler

![C++](https://img.shields.io/badge/C++-11-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

This project implements a high-performance, multi-threaded web crawler in C++. Designed for efficiency and scalability, it leverages parallel processing to crawl websites rapidly while respecting web standards.

## Features

- 🚀 **Parallel crawling** with configurable thread pool
- 🔗 **URL deduplication** to prevent redundant requests
- 🔍 **HTML parsing** with gumbo-parser for accurate link extraction
- ⏱️ **Timeout handling** for unresponsive servers
- 📊 **Progress tracking** with real-time statistics
- ⚙️ **Customizable parameters** for crawl limits and timeouts
- 🤖 **robots.txt compliance** (basic implementation)

## Requirements

- C++11 compatible compiler (GCC, Clang, etc.)
- libcurl (for HTTP requests)
- gumbo-parser (for HTML parsing)
- pthreads (threading library)

### Dependencies Installation

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev libgumbo-dev
```

#### macOS (Homebrew)
```bash
brew install curl gumbo-parser
```

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/web-crawler.git
cd web-crawler
```

2. Build using Makefile:
```bash
make
```

This will compile the executable `crawler`.

### Makefile Targets
- `make` - Builds the project
- `make run` - Builds and runs the crawler
- `make clean` - Removes build artifacts
- `make install-deps` - Installs required dependencies (Ubuntu)

## Usage

Run the compiled executable:
```bash
./crawler
```

When prompted, enter a starting URL:
```
Enter URL to crawl: https://example.com
```

The crawler will:
1. Start from the specified URL
2. Use 8 worker threads by default
3. Crawl up to 100 unique URLs
4. Display real-time progress
5. Output statistics upon completion

## Configuration

Modify these constants in `crawler.cpp` for different behavior:

```cpp
const int MAX_CRAWL = 100;          // Maximum URLs to crawl
const int NUM_THREADS = 8;           // Worker thread count
const long CURL_TIMEOUT = 10L;       // Request timeout in seconds
const bool VERBOSE = true;           // Enable detailed logging
```

## Sample Output

```
[Thread 0x7f8ab7d37700] Crawling: https://example.com (1/100)
[Thread 0x7f8ab7536700] Crawling: https://example.com/about (2/100)
[Thread 0x7f8ab6d35700] Crawling: https://example.com/products (3/100)
...
Crawled 100 URLs
Discovered 342 unique URLs
Execution time: 12.84 seconds
```

## Implementation Details

### Architecture
- **Producer-Consumer Pattern**: URL queue managed by worker threads
- **Thread Synchronization**: Mutexes and condition variables for safe resource access
- **Memory Management**: RAII principles for resource cleanup

### Key Components
1. **URL Frontier**: Thread-safe queue for managing URLs to crawl
2. **HTML Parser**: Extracts links using gumbo-parser's robust DOM handling
3. **HTTP Client**: libcurl-based client with timeout and redirect handling
4. **Deduplication**: Concurrent hash set for tracking visited URLs

### Performance
- Processes 10-20 URLs per second (depending on server response times)
- Efficient memory usage through string interning
- Parallel I/O operations for maximum throughput

## Limitations

- Follows only absolute HTTP/HTTPS links
- Does not execute JavaScript (static content only)
- Limited support for modern authentication mechanisms
- Basic robots.txt implementation
- No sitemap.xml processing

## Future Enhancements

- [ ] Add domain restriction option
- [ ] Implement depth-limited crawling
- [ ] Add content-type filtering
- [ ] Develop proper robots.txt parser
- [ ] Create output file options (CSV, JSON)
- [ ] Add rate limiting to respect servers

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---
**Disclaimer**: Use this crawler responsibly and respect website terms of service. Always check robots.txt before crawling and avoid overloading servers.