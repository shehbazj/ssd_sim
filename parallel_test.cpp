#include "page_md.cpp"
#include "block.cpp"
#include "page_group.cpp"
#include "ssd.cpp"
#include "l2p.cpp"
#include "coder.cpp"
#include <cstdlib>
#include <algorithm>
#include <gtest/gtest.h>
#include "arrays.hpp"

// Parallelization tests

TEST(ParallelTest, writeAndReadBlock)
{
	int num_blocks = 10;
	// block size is the number of pages inside the block.
	int block_size = 20;
	int page_size = 10;
	int ssd_cell_type = 3; // Multi-level cell

	ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);

	// NEW CODE-- does block level creation, block level write and read.

	block *b = new block(block_size, page_size , ssd_cell_type, 0, NO_WOM);

	int block_capacity_in_bytes = b->getPhysicalBlockSize();

	uint8_t *wbdata = new uint8_t [block_capacity_in_bytes]();
	uint8_t *rbdata = new uint8_t [block_capacity_in_bytes]();

	for (int i = 0 ; i < block_capacity_in_bytes ; i++) {
		wbdata[i] = 0xFF;
	}

	b->writeToBlock(wbdata, block_capacity_in_bytes);
	b->readFromBlock(rbdata, block_capacity_in_bytes);

	for (int i = 0 ; i < block_capacity_in_bytes ; i++) {
		// printf("i=%d w=%d r=%d\n", i, wbdata[i], rbdata[i]);
		ASSERT_EQ(wbdata[i], rbdata[i]);
	}

	delete []wbdata;
	delete []rbdata;
	delete b;
	delete mySSD;
}

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
