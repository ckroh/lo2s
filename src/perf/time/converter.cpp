/*
 * This file is part of the lo2s software.
 * Linux OTF2 sampling
 *
 * Copyright (c) 2017,
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

#include <lo2s/perf/time/converter.hpp>

#include <lo2s/log.hpp>

namespace lo2s
{
namespace perf
{
namespace time
{
Converter::Converter()
{
    Reader reader;
    reader.read();

    if (reader.perf_time.time_since_epoch().count() == 0)
    {
        Log::error()
            << "Could not determine perf_time offset. Synchronization event was not triggered.";
        offset = otf2::chrono::duration(0);
        return;
    }
    offset = reader.local_time.time_since_epoch() - reader.perf_time.time_since_epoch();
    Log::debug() << "perf time offset: " << offset.count() << " ns ("
                 << reader.local_time.time_since_epoch().count() << " - "
                 << reader.perf_time.time_since_epoch().count() << ").";
}
}
}
}
