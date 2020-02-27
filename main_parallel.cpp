#include "ssd.hpp"
#include <cstdint>
#include <boost/thread.hpp>
#include <chrono>

int main()
{
	int num_blocks = 10;
	// block size is the number of pages inside the block.
	int block_size = 20;
	int page_size = 10;
	int ssd_cell_type = 3; // Multi-level cell
	int ssd_capacity = num_blocks * block_size * page_size * ssd_cell_type;

	ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);

	// NEW CODE-- does block level creation, block level write and read.

	block *b = new block(block_size, page_size , ssd_cell_type, 0, NO_WOM);

	printf("physical block size = %lu\n", b->getPhysicalBlockSize());
	printf("logical block size = %lu\n", b->getLogicalBlockSize());

	int block_capacity_in_bytes = b->getPhysicalBlockSize();

	uint8_t *wbdata = new uint8_t [block_capacity_in_bytes]();
	uint8_t *rbdata = new uint8_t [block_capacity_in_bytes]();

	for (int i = 0 ; i < block_capacity_in_bytes ; i++) {
		wbdata[i] = 0xFF;
	}

	// b->writeToBlock(wbdata, block_capacity_in_bytes);
	mySSD->write_to_disk(wbdata, ssd_capacity);
	auto start_time = std::chrono::high_resolution_clock::now();
	// b->readFromBlock(rbdata, block_capacity_in_bytes);
	mySSD->read_from_disk(rbdata, ssd_capacity);
	// for (int i = 0; i < 2; ++i)
    // {
	// 	boost::thread z(mySSD->read_from_disk(rbdata, block_capacity_in_bytes));
	// 	z.join();
    // }
	auto end_time = std::chrono::high_resolution_clock::now();

	for (int i = 0 ; i < block_capacity_in_bytes ; i++) {
		// printf("i=%d w=%d r=%d\n", i, wbdata[i], rbdata[i]);
		// assert(wbdata[i] == rbdata[i]);
	}

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
    std::cout << duration;

	delete []wbdata;
	delete []rbdata;
	delete b;
	delete mySSD;
}
