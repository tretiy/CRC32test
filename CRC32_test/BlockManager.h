#pragma once

#include "Block.h"
#include <vector>
#include <mutex>
#include <atomic>

class BlockManager
{
    std::vector<Block> blocks;
    std::mutex writeMutex;
    int blckSize;
    size_t blckCount;
public:
    BlockManager(int blockSize, size_t blocksCount)
        :blocks(blocksCount, Block{blockSize}),
        blckSize{blockSize},
        blckCount{blocksCount}
    {
        blocks.clear();
    }
    Block getBlock(size_t index)
    {
        return blocks.at(index);
    }
    void pushBlock(const Block& block)
    {
        std::lock_guard<std::mutex> lock(writeMutex);
        blocks.push_back(block);
        if(blocks.size() >= blckCount)
        {
            generateFinished = true;
        }
    }

    int getBlockSize() const
    {
        return blckSize;
    }
    int getBlocksCount() const
    {
        return blckCount;
    }

    std::atomic<bool> generateFinished{false};
};