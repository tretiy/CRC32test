#pragma once

#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <limits>

using random_bytes_engine = std::independent_bits_engine<
    std::default_random_engine, CHAR_BIT, unsigned short>;
random_bytes_engine rbe;

class Block
{
    std::vector<unsigned char> data;
public:
    Block() = default;

    Block(size_t size)
        : data(size)
    {
        std::generate(data.begin(), data.end(), std::ref(rbe));
    }

    Block(const Block& other)
        : data{other.data.begin(), other.data.end()}
    {
    }

    const std::vector<unsigned char>& getData() const
    {
        return std::cref(data);
    }
};
