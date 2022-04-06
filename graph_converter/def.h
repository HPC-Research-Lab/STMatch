#pragma once

typedef uint32_t vidType;
typedef uint32_t labelType;
int previous_pos = -1;
int previous_len = 0;
#define LOG(...) {\
  char str[100];\
  if(/*std::cout.tellp() == -1*/true) {\
    /*for(int ITERATOR = 0; ITERATOR < previous_len; ITERATOR++) {\
      std::cout << "\b \b";\
    }*/\
    previous_len = snprintf(str, 100, __VA_ARGS__);\
    std::cout << str << "\n" << std::flush;\
  } else {\
    int current_pos = std::cout.tellp();\
    if(current_pos == previous_pos + previous_len) {\
      std::cout.seekp(previous_pos);\
      for(int ITERATOR = previous_pos; ITERATOR < current_pos; ITERATOR++) {\
        std::cout << " ";\
      }\
      std::cout.seekp(previous_pos);\
    } else {\
      previous_pos = std::cout.tellp();\
    }\
    previous_len = snprintf(str, 100, __VA_ARGS__);\
    std::cout << str << std::flush;\
  }\
}


#include <cassert>

// files
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// mmap
#include <sys/mman.h>

template<typename T>
class MemoryMappedFile {
  T *pointer;
  const uint64_t elements;
  const bool writeable;
  MemoryMappedFile(int fd, uint64_t bytes, bool write) :
        elements(bytes / sizeof(T)), writeable(write) {
    assert(bytes % sizeof(T) == 0);
    auto flags = writeable ? (PROT_READ|PROT_WRITE) : PROT_READ;
    pointer = (T*)mmap(nullptr, bytes, flags, MAP_SHARED, fd, 0);
		assert(pointer != MAP_FAILED);
  }
public:
  ~MemoryMappedFile() {
    int ret = munmap(pointer, elements * sizeof(T));
    if(ret != 0) perror("munmap");
  }
  static MemoryMappedFile Write(std::string fname, uint64_t elems) {
    int fd = open(fname.c_str(), O_CREAT|O_TRUNC|O_RDWR, 0666);
    assert(-1 != fd);
    size_t bytes = elems * sizeof(T);
    if(ftruncate(fd, bytes)) perror("ftruncate");
    MemoryMappedFile mmf(fd, bytes, true);
    if(close(fd)) perror("close");
    return mmf;
  }
  static MemoryMappedFile Read(std::string fname) {
		int fd = open(fname.c_str(), O_RDONLY, 0);
		assert(-1 != fd);
		struct stat sb;
		if(fstat(fd, &sb)) perror("fstat");
    size_t bytes = sb.st_size;
    MemoryMappedFile mmf(fd, bytes, false);
    if(close(fd)) perror("close");
    return mmf;
  }
  static MemoryMappedFile Modify(std::string fname) {
		int fd = open(fname.c_str(), O_RDWR, 0);
		assert(-1 != fd);
		struct stat sb;
		if(fstat(fd, &sb)) perror("fstat");
    size_t bytes = sb.st_size;
    MemoryMappedFile mmf(fd, bytes, true);
    if(close(fd)) perror("close");
    return mmf;
  }
  T *ptr(uint64_t idx=0) {
    assert(idx <= elements);
    return pointer + idx;
  }
  const T *ptr(uint64_t idx=0) const {
    assert(idx <= elements);
    return pointer + idx;
  }
  T& operator[](uint64_t idx) {
    assert(writeable);
    assert(idx < elements);
    return pointer[idx];
  }
  const T& operator[](uint64_t idx) const {
    assert(idx < elements);
    return pointer[idx];
  }
  uint64_t size() const { return elements; }
  void sync() {
    int ret = msync(pointer, elements * sizeof(T), MS_SYNC);
    if(ret != 0) perror("msync");
  }
  T *begin() { assert(writeable); return pointer; }
  T *end() { assert(writeable); return pointer + elements; }
  const T *begin() const { return pointer; }
  const T *end() const { return pointer + elements; }
  T fault_in() const {
    T temp = 0;
    uint64_t stride = getpagesize() / sizeof(T) / 2;
    for(uint64_t i = 0; i < elements; i += stride) {
      temp += pointer[i];
    }
    return temp;
  }
};

#include <chrono>
struct TimeStamp {
  std::chrono::high_resolution_clock::time_point t;
  TimeStamp() {
    t = std::chrono::high_resolution_clock::now();
  }
  double operator-(const TimeStamp &start) const {
    return std::chrono::duration<double, std::ratio<1,1>>(t - start.t).count();
  }
};
