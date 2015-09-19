// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>

namespace leveldb {

class Slice {
 public:
  // Create an empty slice.
  Slice() : data_(""), size_(0) { }   //默认构造函数

  // Create a slice that refers to d[0,n-1].
  Slice(const char* d, size_t n) : data_(d), size_(n) { }  //用给定的大小和地址创建一个Slice

  // Create a slice that refers to the contents of "s"
  Slice(const std::string& s) : data_(s.data()), size_(s.size()) { } //用一个string创建一个slice

  // Create a slice that refers to s[0,strlen(s)-1]
  Slice(const char* s) : data_(s), size_(strlen(s)) { }  //用C类型的字符串创建一个Slice

  // Return a pointer to the beginning of the referenced data
  const char* data() const { return data_; }             //返回Slice的起始地址

  // Return the length (in bytes) of the referenced data
  size_t size() const { return size_; }                 //返回Slice的大小

  // Return true iff the length of the referenced data is zero
  bool empty() const { return size_ == 0; }   //检查Slice是否为空

  // Return the ith byte in the referenced data.
  // REQUIRES: n < size()
  char operator[](size_t n) const {
    assert(n < size());            
    return data_[n];
  }          //返回Slice第n位的数据

  // Change this slice to refer to an empty array
  void clear() { data_ = ""; size_ = 0; }  //清楚Slice的数据
	
  // Drop the first "n" bytes from this slice.
  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }  //删除Slice的前n个字节的数据

  // Return a string that contains the copy of the referenced data.
  std::string ToString() const { return std::string(data_, size_); }  //将Slice中的数据以字符串返回
	
  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int compare(const Slice& b) const;  //比较函数

  // Return true iff "x" is a prefix of "*this"
  bool starts_with(const Slice& x) const {
    return ((size_ >= x.size_) &&
            (memcmp(data_, x.data_, x.size_) == 0));
  }  //判断传入的x是不是当前Slice的前缀

 private:
  const char* data_;    //指向一个存储
  size_t size_;         //指向这段存储的大小 

  // Intentionally copyable
};

inline bool operator==(const Slice& x, const Slice& y) {
  return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}  //重载的==运算符

inline bool operator!=(const Slice& x, const Slice& y) {
  return !(x == y);
}  //重载的!=运算符

inline int Slice::compare(const Slice& b) const {
  const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);
  if (r == 0) {
    if (size_ < b.size_) r = -1;
    else if (size_ > b.size_) r = +1;
  }
  return r;
}  //Slice的compare函数定义，按字符比较

}  // namespace leveldb


#endif  // STORAGE_LEVELDB_INCLUDE_SLICE_H_
