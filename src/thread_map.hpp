/*
 * This file is part of the lo2s software.
 * Linux OTF2 sampling
 *
 * Copyright (c) 2016,
 *    Technische Universitaet Dresden, Germany
 *
 * lo2s is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * lo2s is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with lo2s.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef X86_RECORD_OTF2_THREAD_MAP_HPP
#define X86_RECORD_OTF2_THREAD_MAP_HPP

#include "error.hpp"
#include "log.hpp"
#include "thread_monitor.hpp"
#include "process_info.hpp"

#include <otf2xx/writer/archive.hpp>

#include <boost/format.hpp>

#include <thread>
#include <unordered_map>
#include <unordered_set>

#include <fcntl.h>
#include <sys/stat.h>

namespace lo2s
{

class monitor;

class thread_map
{
public:
    thread_map(monitor& parent_monitor) : parent_monitor_(parent_monitor)
    {
    }

    ~thread_map();

    process_info& insert_process(pid_t pid, bool enable_on_exec);

    // Insert a thread and if needed it's process
    void insert(pid_t pid, pid_t tid, bool enable_on_exec);

    void disable(pid_t tid);
    void disable();

    thread_monitor& get_thread(pid_t tid);

    void try_join();
private:
    std::recursive_mutex mutex_;
    std::unordered_map<pid_t, process_info> processes_;
    std::unordered_map<pid_t, thread_monitor> threads_;
    monitor& parent_monitor_;
};
}

#endif // X86_RECORD_OTF2_THREAD_MAP_HPP