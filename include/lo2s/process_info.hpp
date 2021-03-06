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

#pragma once

#include <lo2s/mmap.hpp>
#include <lo2s/address.hpp>
#include <lo2s/log.hpp>

#include <mutex>
#include <thread>

namespace lo2s
{
class ProcessInfo
{
public:
    ProcessInfo(pid_t pid, bool enable_on_exec) : pid_(pid), maps_(pid, !enable_on_exec)
    {
    }

    pid_t pid() const
    {
        return pid_;
    }

    void mmap(Address begin, Address end, Address pgoff, const std::string& dso_name)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (finalized_)
        {
            // FIXME happens with vampir
            Log::warn() << "Cannot insert new mappings into finalized process info";
            //throw std::runtime_error("Cannot insert new mappings into finalized process info");
        }
        maps_.mmap(begin, end, pgoff, dso_name);
    }

    const MemoryMap& maps()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            finalized_ = true;
        }
        return maps_;
    }

private:
    const pid_t pid_;
    std::mutex mutex_;
    bool finalized_ = false;
    MemoryMap maps_;
};
}
