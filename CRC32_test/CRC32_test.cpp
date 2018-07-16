// CRC32_test.cpp : Defines the entry point for the application.
//

#include "CRC32_test.h"
#include <cxxopts.hpp>
#include <thread>
#include <list>
#include "Block.h"
#include "BlockManager.h"
#include "crc32c\crc32c.h"

uint32_t calculateCRC(const Block& block)
{
    return crc32c::Crc32c(block.data.data(), block.data.size());
}

void GenerateBlocks_threadFunc(BlockManager& manager)
{
    while(!manager.generateFinished)
    {
        manager.pushBlock(Block(manager.getBlockSize()));
        std::this_thread::yield();
    }
}

void Calculate_threadFunc(BlockManager& manager)
{
    auto thread_id = std::this_thread::get_id();
    auto curBlockIdx = 0;
    
    while(curBlockIdx < manager.getBlocksCount())
    {
        auto crc32 = calculateCRC(manager.getBlock(curBlockIdx));

        ++curBlockIdx;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        cxxopts::Options options("CRC32", "Multithreaded crc32 calculator");
        options.allow_unrecognised_options().add_options()
        ("g,generators", "Number of threads to generate blocks",cxxopts::value<int>())
        ("c,crccalculators", "Number of threads to calculate crc32", cxxopts::value<int>())
        ("s,blocksize", "block size in bytes", cxxopts::value<int>())
        ("b,blockscount", "Number of blocks", cxxopts::value<size_t>());

        auto result = options.parse(argc, argv);
        if(  !result["generators"].count() 
                || !result["crccalculators"].count() 
                || !result["blocksize"].count()
                || !result["blockscount"].count())
        {
            std::cout<<options.help();
            return -1;
        }
        int generatorsCount = result["generators"].as<int>();
        int validatorsCount = result["crccalculators"].as<int>();
        int blockSize = result["blocksize"].as<int>();
        size_t blocksCount = result["blockscount"].as<size_t>();

        BlockManager manager(blockSize, blocksCount);
        std::list<std::thread> genthreads;
        for(auto i=0;i<generatorsCount;++i)
            genthreads.emplace_back(GenerateBlocks_threadFunc, std::ref(manager));
        std::for_each(genthreads.begin(), genthreads.end(), [](auto&& thread)
        {
            thread.join();
        });
    }
    catch(cxxopts::OptionException& ex)
    {
        std::cout<< ex.what();
    }
    int i;
    std::cout<<"finished\n";
    std::cin>>i;
	return 0;
}
