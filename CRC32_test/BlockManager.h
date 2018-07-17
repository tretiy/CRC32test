#pragma once

#include "Block.h"
#include <vector>
#include <mutex>
#include <atomic>

class BlockManager
{
    std::vector<Block> blocks;
    std::mutex addBlockMutex;
    std::mutex setCrcMutex;
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
    const Block& getBlock(size_t index)
    {
        std::lock_guard<std::mutex> lock(addBlockMutex);
        return blocks.at(index);
    }
    void pushBlock(const Block& block)
    {
        std::lock_guard<std::mutex> lock(addBlockMutex);
        if(blocks.size() >= blckCount)
        {
            generateFinished = true;
        }
        blocks.push_back(block);

    }
    void setBlockCrc(size_t index, uint32_t calcCrc)
    {
        std::lock_guard<std::mutex> lock(setCrcMutex);
         std::lock_guard<std::mutex> lock2(addBlockMutex);

        if(false == blocks[index].setCRC(calcCrc))
        {
            std::cout << "crc32 mismatch! blockid = " << index << " \n";
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