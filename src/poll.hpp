/*
    Copyright (c) 2007-2016 Contributors as noted in the AUTHORS file

    This file is part of libzmq, the ZeroMQ core engine in C++.

    libzmq is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    As a special exception, the Contributors give you permission to link
    this library with independent modules to produce an executable,
    regardless of the license terms of these independent modules, and to
    copy and distribute the resulting executable under terms of your choice,
    provided that you also meet, for each linked independent module, the
    terms and conditions of the license of that module. An independent
    module is a module which is not derived from or based on this library.
    If you modify this library, you must extend this exception to your
    version of the library.

    libzmq is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ZMQ_POLL_HPP_INCLUDED__
#define __ZMQ_POLL_HPP_INCLUDED__

//  poller.hpp decides which polling mechanism to use.
#include "poller.hpp"
#if defined ZMQ_USE_POLL

#if !defined ZMQ_HAVE_WINDOWS
#include <poll.h>
#endif
#include <stddef.h>
#include <vector>

#include "ctx.hpp"
#include "fd.hpp"
#include "thread.hpp"
#include "poller_base.hpp"

namespace zmq
{
struct i_poll_events;

//  Implements socket polling mechanism using the POSIX.1-2001
//  poll() system call.

class poll_t : public poller_base_t
{
  public:
    typedef fd_t handle_t;

    poll_t (const ctx_t &ctx_);
    ~poll_t ();

    //  "poller" concept.
    handle_t add_fd (fd_t fd_, zmq::i_poll_events *events_);
    void rm_fd (handle_t handle_);
    void set_pollin (handle_t handle_);
    void reset_pollin (handle_t handle_);
    void set_pollout (handle_t handle_);
    void reset_pollout (handle_t handle_);
    void start ();
    void stop ();

    static int max_fds ();

  private:
    //  Main worker thread routine.
    static void worker_routine (void *arg_);

    //  Main event loop.
    void loop ();

    void cleanup_retired();

    // Reference to ZMQ context.
    const ctx_t &ctx;

    struct fd_entry_t
    {
        fd_t index;
        zmq::i_poll_events *events;
    };

    //  This table stores data for registered descriptors.
    typedef std::vector<fd_entry_t> fd_table_t;
    fd_table_t fd_table;

    //  Pollset to pass to the poll function.
    typedef std::vector<pollfd> pollset_t;
    pollset_t pollset;

    //  If true, there's at least one retired event source.
    bool retired;

    //  If true, thread is in the process of shutting down.
    bool stopping;

    //  Handle of the physical thread doing the I/O work.
    thread_t worker;

    poll_t (const poll_t &);
    const poll_t &operator= (const poll_t &);
};

typedef poll_t poller_t;
}

#endif

#endif
