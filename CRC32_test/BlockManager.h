#pragma once

#include "Block.h"
#include <deque>
#include <map>
#include <set>
#include <mutex>
#include <atomic>
#include <fstream>

class BlockManager
{
    struct Crc
    {
        uint32_t crc = 0;
        size_t count = 0;
    };

    std::deque<Block> blocks;
    std::map<size_t, Crc> crcValues;
    std::set<size_t> badBlocksIdxs;
    std::vector<size_t> processedBlocks;
    std::mutex insertMutex;
    size_t blckSize = 0;
    size_t blckCount = 0;
    size_t calcThreads = 0;
    size_t availableBlocks = 0;
    size_t offset = 0;
    size_t removeBlocksCount = 10000;
    std::basic_fstream<unsigned char> badBlocks;
    unsigned int badBlockEmulation = 0;
public:
    std::atomic<bool> generateFinished{false};

    BlockManager(size_t blockSize, size_t blocksCount, size_t crcThreadsCount)
        : blckSize{blockSize},
          blckCount{blocksCount},
          calcThreads{crcThreadsCount}
    {
        badBlocks.open("bad_blocks.bin", std::ios::binary | std::ios::out | std::ios::trunc);
        processedBlocks.reserve(removeBlocksCount);
    }

    ~BlockManager()
    {
        if (badBlocks.is_open())
            badBlocks.close();
    }

    const Block& getBlock(size_t index)
    {
        std::lock_guard<std::mutex> lock(insertMutex);
        return blocks[index - offset];
    }

    void pushBlock(Block&& block)
    {
        std::lock_guard<std::mutex> lock(insertMutex);
        if (availableBlocks == blckCount)
        {
            generateFinished = true;
        }
        blocks.push_back(std::move(block));
        ++availableBlocks;
    }

    void DumpBlock(size_t index)
    {
        badBlocks.write(blocks[index - offset].getData().data(), blckSize);
    }

    void CheckProcessed(size_t index)
    {
        //all blocks calculated crc => free block related data
        if (crcValues[index].count == calcThreads)
        {
            processedBlocks.push_back(index);
            //check if can delete some blocks data
            if (processedBlocks.size() >= removeBlocksCount)
            {
                std::sort(processedBlocks.begin(), processedBlocks.end());
                size_t idxToRemove = processedBlocks[0];
                if ((offset == 0 && idxToRemove == offset) || offset && idxToRemove == offset + 1)
                {
                    --idxToRemove;
                    for (auto& idx : processedBlocks)
                    {
                        if (idx == idxToRemove + 1)
                            idxToRemove = idx;
                        else
                            break;
                    }
                    if (idxToRemove > offset)
                    {
                        blocks.erase(blocks.begin(), blocks.begin() + idxToRemove - offset);
                        for (auto it = crcValues.begin(); it != crcValues.end();)
                        {
                            if (it->first <= idxToRemove)
                                it = crcValues.erase(it);
                            else
                                ++it;
                        }
                        for (auto it = badBlocksIdxs.begin(); it != badBlocksIdxs.end();)
                        {
                            if (*it <= idxToRemove)
                                it = badBlocksIdxs.erase(it);
                            else
                                ++it;
                        }
                        processedBlocks.erase(std::remove_if(processedBlocks.begin(), processedBlocks.end(),
                                                             [&idxToRemove](auto& val)
                                                             {
                                                                 return val <= idxToRemove;
                                                             }), processedBlocks.end());
                        offset = idxToRemove;
                    }
                }
            }
        }
    }

    void setBlockCrc(size_t index, uint32_t calcCrc)
    {
        std::lock_guard<std::mutex> lock(insertMutex);

        if (crcValues.find(index) == crcValues.end())
        {
            crcValues[index] = {calcCrc, 0};
        }
        else if ((crcValues[index].crc != calcCrc && badBlocksIdxs.count(index) == 0) 
          || (index && badBlockEmulation && index % badBlockEmulation == 0))
        {
            DumpBlock(index);
            badBlocksIdxs.insert(index);
            std::cout << "crc32 mismatch! blockid = " << index << " \n";
        }
        crcValues[index].count += 1;
        CheckProcessed(index);
    }

    int getBlockSize() const
    {
        return blckSize;
    }

    int getBlocksCount() const
    {
        return blckCount;
    }

    bool isBlockAvailable(size_t idx) const
    {
        return idx < availableBlocks;
    }
    
    void setBadBlockEmulation(unsigned int eachNth)
    {
        badBlockEmulation = eachNth;
    }
};
