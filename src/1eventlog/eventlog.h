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
 * Event logging
 *
 * @exception_safe: nothrow
 * @thread_safe: yes
 */
 
#ifndef UPS_EVENTLOG_H
#define UPS_EVENTLOG_H

#include "0root/root.h"

#include <stdio.h>

// Always verify that a file of level N does not include headers > N!

#ifndef UPS_ROOT_H
#  error "root.h was not included"
#endif

namespace upscaledb {

namespace EventLog {

#ifdef UPS_ENABLE_EVENT_LOGGING

// Locks the EventLog; used by the helper macros below
extern void
lock();

// Unlocks the EventLog; used by the helper macros below
extern void
unlock();

// Creates an event log, overwriting any existing file. The filename
// will be <filename>.elog
extern void
create(const char *filename);

// Opens an existing event log. The filename will be <filename>.elog
extern void
open(const char *filename);

// Closes an existing event log.
extern void
close(const char *filename);

// Appends a printf-formatted string to the log
extern void
append(const char *filename, const char *tag, const char *format, ...);

// Converts a binary string to an escaped C literal
extern const char *
escape(const void *data, size_t size);

// A few helper macros
#  define EVENTLOG_CREATE(f)    do {                            \
                                  EventLog::lock();             \
                                  EventLog::create(f);          \
                                  EventLog::unlock();           \
                                } while (0)

#  define EVENTLOG_OPEN(f)      do {                            \
                                  EventLog::lock();             \
                                  EventLog::open(f);            \
                                  EventLog::unlock();           \
                                } while (0)

#  define EVENTLOG_CLOSE(f)     do {                            \
                                  EventLog::lock();             \
                                  EventLog::close(f);           \
                                  EventLog::unlock();           \
                                } while (0)

#  define EVENTLOG_APPEND(x)    do {                            \
                                  EventLog::lock();             \
                                  EventLog::append x;           \
                                  EventLog::unlock();           \
                                } while (0)

#else /* !UPS_ENABLE_EVENT_LOGGING */

#  define EVENTLOG_CREATE(x)    (void)0
#  define EVENTLOG_OPEN(x)      (void)0
#  define EVENTLOG_APPEND(x)    (void)0

inline const char *
escape(const void *data, size_t size)
{
  return (0);
}

#endif /* UPS_ENABLE_EVENT_LOGGING */

} // namespace EventLog

} // namespace upscaledb

#endif /* UPS_EVENTLOG_H */

