/*
 * Copyright (C) 2005-2016 Christoph Rupp (chris@crupp.de).
 * All Rights Reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See the file COPYING for License information.
 */

/*
 * @exception_safe: strong
 * @thread_safe: no
 */

#ifndef UPS_DEVICE_INMEM_H
#define UPS_DEVICE_INMEM_H

#include "0root/root.h"

// Always verify that a file of level N does not include headers > N!
#include "1mem/mem.h"
#include "2device/device.h"
#include "2page/page.h"

#ifndef UPS_ROOT_H
#  error "root.h was not included"
#endif

namespace upscaledb {

/*
 * an In-Memory device
 */ 
class InMemoryDevice : public Device {
    struct State {
      // flag whether this device was "opened" or is uninitialized
      bool is_open;

      // the allocated bytes
      uint64_t allocated_size;
    };

  public:
    // constructor
    InMemoryDevice(const EnvironmentConfiguration &config)
      : Device(config) {
      State state;
      state.is_open = false;
      state.allocated_size = 0;
      std::swap(m_state, state);
    }

    // Create a new device
    virtual void create() {
      m_state.is_open = true;
    }

    // opens an existing device 
    virtual void open() {
      ups_assert(!"can't open an in-memory-device");
      throw Exception(UPS_NOT_IMPLEMENTED);
    }

    // returns true if the device is open 
    virtual bool is_open() {
      return (m_state.is_open);
    }

    // closes the device 
    virtual void close() {
      ups_assert(m_state.is_open);
      m_state.is_open = false;
    }

    // flushes the device 
    virtual void flush() {
    }

    // truncate/resize the device 
    virtual void truncate(uint64_t newsize) {
    }

    // get the current file/storage size 
    virtual uint64_t file_size() {
      ups_assert(!"this operation is not possible for in-memory-databases");
      throw Exception(UPS_NOT_IMPLEMENTED);
    }

    // seek position in a file 
    virtual void seek(uint64_t offset, int whence) {
      ups_assert(!"can't seek in an in-memory-device");
      throw Exception(UPS_NOT_IMPLEMENTED);
    }

    // tell the position in a file 
    virtual uint64_t tell() {
      ups_assert(!"can't tell in an in-memory-device");
      throw Exception(UPS_NOT_IMPLEMENTED);
    }

    // reads from the device; this function does not use mmap 
    virtual void read(uint64_t offset, void *buffer, size_t len) {
      ups_assert(!"operation is not possible for in-memory-databases");
      throw Exception(UPS_NOT_IMPLEMENTED);
    }

    // writes to the device 
    virtual void write(uint64_t offset, void *buffer, size_t len) {
    }

    // reads a page from the device 
    virtual void read_page(Page *page, uint64_t address) {
      ups_assert(!"operation is not possible for in-memory-databases");
      throw Exception(UPS_NOT_IMPLEMENTED);
    }

    // allocate storage from this device; this function
    // will *NOT* use mmap.  
    virtual uint64_t alloc(size_t size) {
      if (m_state.allocated_size + size > m_config.file_size_limit_bytes)
        throw Exception(UPS_LIMITS_REACHED);

      uint64_t retval = (uint64_t)Memory::allocate<uint8_t>(size);
      m_state.allocated_size += size;
      return (retval);
    }

    // allocate storage for a page from this device 
    virtual void alloc_page(Page *page) {
      ups_assert(page->get_data() == 0);

      size_t page_size = m_config.page_size_bytes;
      if (m_state.allocated_size + page_size > m_config.file_size_limit_bytes)
        throw Exception(UPS_LIMITS_REACHED);

      uint8_t *p = Memory::allocate<uint8_t>(page_size);
      page->assign_allocated_buffer(p, (uint64_t)PTR_TO_U64(p));

      m_state.allocated_size += page_size;
    }

    // frees a page on the device; plays counterpoint to @ref alloc_page 
    virtual void free_page(Page *page) {
      page->free_buffer();

      ups_assert(m_state.allocated_size >= m_config.page_size_bytes);
      m_state.allocated_size -= m_config.page_size_bytes;
    }

    // Returns true if the specified range is in mapped memory
    virtual bool is_mapped(uint64_t file_offset, size_t size) const {
      return (false);
    }

    // Removes unused space at the end of the file
    virtual void reclaim_space() {
    }

    // releases a chunk of memory previously allocated with alloc()
    void release(void *ptr, size_t size) {
      Memory::release(ptr);
      ups_assert(m_state.allocated_size >= size);
      m_state.allocated_size -= size;
    }

  private:
    State m_state;
};

} // namespace upscaledb

#endif /* UPS_DEVICE_INMEM_H */
