#include "ssd.hpp"
ssd :: ssd(int num_blocks, int bs, int ps, int ssd_cell_type)
{
	int num_pages = bs;
	num_blocks_in_ssd = num_blocks;
	size_of_block = bs;
	bytes_per_block = bs * ps * ssd_cell_type;

	current_block_number = 0;
	max_invalid_cell_threshold = 80;
	max_invalid_block_threshold = 80;
	max_invalid_page_threshold = 80;

	for (int i = 0 ; i < num_blocks ; i++) {
		block_array.push_back(new block(num_pages, ps ,ssd_cell_type, 0, NO_WOM));
		block_array[i]->setBlockNumber(i);
	}
}

/* buf is initialized by caller */
int ssd :: write_to_disk(uint8_t *buf, int size)
{
	// calculate number of LBN to be updated. (here).
	// check which of them are overwrites (shouldn't have lbn-pbn as LONG_MAX). (l2p.getPBN)
	// invalidate these pages. (block.InvalidatePage()). l2pMap.invalidatePage)

	// allocate new PBNs for all LBNs. (l2pMap.map_lbns_to_pbns)
	// A. get pbn of each new lbn (getPBN())

	// TODO if PBN is of a block marked for gc. request for additional pages.
	// go back to A. asking for new lbns = pbns marked for GC.

	// TODO beyond 4th write, read PBNs, check unprogrammable cells. check number
	// of additional physical pages to be assigned.
	// while goto A, asking for new lbns = unprogrammable cells / page_size.

	// get block on which write has to happen.
	// generate page vector from buf.
	// do write_to_block(block_no, vector_of_pages);

	// Check if caller has initialized buffer
	assert(buf != NULL);

	// Find number of blocks needed to write the buffer
	int needed_blocks = size / bytes_per_block;
	assert(needed_blocks <= num_blocks_in_ssd); 
	uint8_t *block_buf = buf;
	int total_bytes_written = 0;

	// uint8_t *rbdata = new uint8_t [size]();
	for (int i = 0; i < needed_blocks; i++) {
		int offset = block_array[i]->writeToBlock(block_buf, bytes_per_block);
		block_buf += offset;
		total_bytes_written += offset;
	} 

	return total_bytes_written;
}

/* buf is initialized by caller */
int ssd :: read_from_disk(uint8_t *buf, int size)
{
	// Check if caller has initialized buffer
	assert(buf != NULL);

	// Find number of blocks needed to write the buffer
	int needed_blocks = size / bytes_per_block;
	assert(needed_blocks <= num_blocks_in_ssd); 
	uint8_t *block_buf = buf;
	int total_bytes_read = 0;

	for (int i = 0; i < needed_blocks; i++) {
		int offset = block_array[i]->readFromBlock(block_buf, bytes_per_block);
		block_buf += offset;
		total_bytes_read += offset;
	} 

	return total_bytes_read;
}

/*
 * Takes in an integer to split up the buffer into n
 * Used for parallelizing reads
*/
int ssd :: read_from_disk_threads(uint8_t *buf, int block_size, int n, int blocks_per_thread)
{
	// Check if caller has initialized buffer
	assert(buf != NULL);
	assert(num_blocks_in_ssd >= n * blocks_per_thread);

	// Find number of blocks needed to write the buffer
	int total_bytes_read = 0;

	// Multithreading support
	int start_block = n * blocks_per_thread;
	int end_block = start_block + blocks_per_thread;
	// Start the read buffer from the start block bytes

	auto total_time = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = start_block; i < end_block; i++) {
		auto start_time = std::chrono::system_clock::now();
		int offset = block_array[i]->readFromBlock(buf, block_size);
		buf += offset;
		total_bytes_read += offset;
		auto end_time = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
		total_time += double(duration);
		// std::cout << "nth " << n << " block " << i << " Duration " << duration << endl;
	} 
	// auto end_time = std::chrono::high_resolution_clock::now();
	// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
	// std::cout << "Total Duration " << total_time << endl;

	return total_bytes_read;
}

ssd :: ~ssd()
{
	for (int i = 0 ; i < num_blocks_in_ssd ; i++) {
		delete block_array[i];
	}
}

void ssd :: invokeGC()
{
	// go through all blocks. arrange them in decreasing order of number of invalidated pages.
	// TODO: block.c get interface for getting number of invalid pages in each block.
	// select top X blocks for erasing. get number of valid pages in this block.
	// TODO: block.c get interface for getting vector of valid pages in block.
	// read all valid pages in a buffer.
	// TODO: get interface for reading page data from a block. getPageData(page_no);
	// call write_to_ssd() with buffer read, for all pages in current block. set block status to READY_TO_ERASE
	// set blockStatus to ERASING. reset all pages in block. make sure all pages are invalidated.
	// set blockStatus to EMPTY.
}
