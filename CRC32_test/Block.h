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
    public:
    std::vector<unsigned char> data;

    Block(int size):data(size)
    {
        std::generate(data.begin(),data.end(),std::ref(rbe));
    }
};