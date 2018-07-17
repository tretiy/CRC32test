#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <functional>

using random_bytes_engine = std::independent_bits_engine<
    std::default_random_engine, CHAR_BIT, unsigned short>;
        random_bytes_engine rbe;
class Block
{
    uint32_t crc32 = 0;
    std::vector<unsigned char> data;
    public:
    Block(int size):data(size)
    {
        std::generate(data.begin(),data.end(),std::ref(rbe));
    }
    Block(const Block& other) = default;
    
    const std::vector<unsigned char>& getData() const
    {
        return std::ref(data);
    }
    // return false if previous calculated crc is not equal current crc
    bool setCRC(uint32_t calcCrc)
    {
        if(crc32 !=0 && crc32 != calcCrc)
        {
            return false;
        }
        crc32 = calcCrc;
        return true;
    }
};