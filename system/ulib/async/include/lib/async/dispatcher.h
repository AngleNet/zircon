// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <zircon/compiler.h>
#include <zircon/syscalls/port.h>
#include <zircon/types.h>

__BEGIN_CDECLS

// Dispatcher interface for performing asynchronous operations.
// There may be multiple implementations of this interface.
typedef struct async_dispatcher async_t;

// Forward declarations for asynchronous operation structures.
typedef struct async_guest_bell_trap async_guest_bell_trap_t;
typedef struct async_wait async_wait_t;
typedef struct async_task async_task_t;
typedef struct async_receiver async_receiver_t;

// Private state owned by the asynchronous dispatcher.
// Clients should initialize the contents of this structure to zero using
// |ASYNC_STATE_INIT| or with calloc, memset, or a similar means.
typedef struct {
    uintptr_t reserved[2];
} async_state_t;

#define ASYNC_STATE_INIT \
    { 0u, 0u }

// Asynchronous dispatcher interface.
//
// Clients should prefer using the |async_*| inline functions declared in the
// other header files.  See the documentation of those inline functions for
// details about each method's purpose and behavior.
//
// This interface consists of several groups of methods:
//
// - Timing: |now|
// - Waiting for signals: |begin_wait|, |cancel_wait|
// - Posting tasks: |post_task|, |cancel_task|
// - Queuing packets: |queue_packet|
// - Virtual machine operations: |set_guest_bell_trap|
//
// Implementations of this interface are not required to support all of these methods.
// Unsupported methods must have valid (non-null) function pointers, must have
// no side-effects, and must return |ZX_ERR_NOT_SUPPORTED| when called.
// Furthermore, if an implementation supports one method of a group, such as |begin_wait|,
// it must also support the other methods of the group, such as |cancel_wait|.
//
// Many clients assume that the dispatcher interface is fully implemented and may
// fail to work with dispatchers that do not support the methods they need.
// Therefore general-purpose dispatcher implementations are strongly encouraged to
// support the whole interface to ensure broad compatibility.
typedef struct async_ops {
    zx_time_t (*now)(async_t* async);
    zx_status_t (*begin_wait)(async_t* async, async_wait_t* wait);
    zx_status_t (*cancel_wait)(async_t* async, async_wait_t* wait);
    zx_status_t (*post_task)(async_t* async, async_task_t* task);
    zx_status_t (*cancel_task)(async_t* async, async_task_t* task);
    zx_status_t (*queue_packet)(async_t* async, async_receiver_t* receiver,
                                const zx_packet_user_t* data);
    zx_status_t (*set_guest_bell_trap)(async_t* async, async_guest_bell_trap_t* trap,
                                       zx_handle_t guest, zx_vaddr_t addr, size_t length);
} async_ops_t;
struct async_dispatcher {
    const async_ops_t* ops;
};

__END_CDECLS
