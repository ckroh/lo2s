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

#include <lo2s/error.hpp>
#include <lo2s/log.hpp>
#include <lo2s/address.hpp>

#include <cstdint>

extern "C" {
#include <ins_dec.h>

#include <unistd.h>
}

namespace lo2s
{

class InsDec
{
public:
    class Error : public std::runtime_error
    {
    public:
        Error(const std::string& what) : std::runtime_error(what)
        {
        }
    };

    InsDec();

    static std::string single_instruction(char* buf);
    std::string operator()(Address ip, std::istream& obj);

    static InsDec& instance()
    {
        static InsDec r;
        return r;
    }

private:
    ins_decAPI* id;
};

class InsDecResolver
{
public:
    InsDecResolver(const std::string& filename);

    std::string instruction(Address ip);

private:
    std::ifstream obj_;
};
}
