// CRC32_test.cpp : Defines the entry point for the application.
//

#include "CRC32_test.h"
#include "cxxopts.hpp"
#include <thread>
#include <list>
#include <chrono>
#include "Block.h"
#include "BlockManager.h"
#include "crc32c\crc32c.h"

uint32_t calculateCRC(const Block& block)
{
    return crc32c::Crc32c(block.getData().data(), block.getData().size());
}

void Generate_threadFunc(BlockManager& manager)
{
    while (!manager.generateFinished)
    {
        auto block = Block(manager.getBlockSize());
        manager.pushBlock(std::move(block));
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Calculate_threadFunc(BlockManager& manager)
{
    auto curBlockIdx = 0;
    while (curBlockIdx < manager.getBlocksCount())
    {
        if (manager.isBlockAvailable(curBlockIdx))
        {
            manager.setBlockCrc(curBlockIdx, calculateCRC(manager.getBlock(curBlockIdx)));
            ++curBlockIdx;
            std::this_thread::yield();
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        cxxopts::Options options("CRC32", "Multithreaded crc32 calculator");
        options.allow_unrecognised_options().add_options()
            ("g,generators", "Number of threads to generate blocks", cxxopts::value<int>())
            ("c,crccalculators", "Number of threads to calculate crc32", cxxopts::value<int>())
            ("s,blocksize", "Block size in bytes", cxxopts::value<size_t>())
            ("b,blockscount", "Number of blocks", cxxopts::value<size_t>());

        auto result = options.parse(argc, argv);
        if (!result["generators"].count()
            || !result["crccalculators"].count()
            || !result["blocksize"].count()
            || !result["blockscount"].count())
        {
            std::cout << options.help();
            return -1;
        }
        int generatorsCount = result["generators"].as<int>();
        int validatorsCount = result["crccalculators"].as<int>();
        size_t blockSize = result["blocksize"].as<size_t>();
        size_t blocksCount = result["blockscount"].as<size_t>();

        BlockManager manager(blockSize, blocksCount, validatorsCount);
        //gen threads
        std::list<std::thread> genthreads;
        for (auto i = 0; i < generatorsCount; ++i)
            genthreads.emplace_back(Generate_threadFunc, std::ref(manager));
        //calc threads
        std::list<std::thread> calcthreads;
        for (auto i = 0; i < validatorsCount; ++i)
            calcthreads.emplace_back(Calculate_threadFunc, std::ref(manager));

        auto joinThreads = [](auto&& threads)
        {
            std::for_each(threads.begin(), threads.end(), [](auto&& thread)
            {
                thread.join();
            });
        };
        joinThreads(genthreads);
        joinThreads(calcthreads);
    }
    catch (cxxopts::OptionException& ex)
    {
        std::cout << ex.what();
    }
    catch (...)
    {
        std::cout << "smth went wrong...\n";
    }
    std::cout << "Finished\n";
    return 0;
}
