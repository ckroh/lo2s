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

#include <lo2s/ins_dec.hpp>

namespace lo2s
{
InsDec::InsDec() : id(ins_dec_new())
{
#ifdef ID_X86
    id_setPlatform(id, "X86_64");
#endif
    	
#ifdef ID_ARM64
    id_setPlatform(id, "ARM64");
#endif
    	
}

std::string InsDec::single_instruction(char* buf)
{
    auto len = strlen(buf);
    if (len == 0)
    {
        throw Error("empty instruction");
    }
    for (size_t i = 0; i < len; i++)
    {
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            break;
        }
    }
    return std::string(buf);
}

std::string InsDec::operator()(Address ip, std::istream& obj)
{
    std::streampos offset = ip.value();
    if (offset < 0)
    {
        throw Error("cannot read memory above 63bit");
    }

    constexpr size_t max_instr_size = 16;
    char buffer[max_instr_size];
    obj.seekg(offset);
    obj.read(buffer, max_instr_size);
    auto read_bytes = obj.gcount();
    if (read_bytes == 0)
    {
        throw Error("instruction pointer at end of file");
    }
   printf("buf size %d : %d\n", read_bytes, sizeof(buffer));
    id_decodeInstruction(id, (unsigned char*)buffer);
    auto res = id->id_insn[0].mnemonic;
//    auto code = r_asm_mdisassemble(r_asm_, (unsigned char*)buffer, read_bytes);
    auto ret = (std::string) res;
    return ret;
}

InsDecResolver::InsDecResolver(const std::string& filename) : obj_(filename)
{
    if (obj_.fail())
    {
        throw std::ios_base::failure("could not open library file.");
    }
    obj_.exceptions(std::ifstream::failbit | std::ifstream::badbit);
}

std::string InsDecResolver::instruction(Address ip)
{
    return InsDec::instance()(ip, obj_);
}
}
