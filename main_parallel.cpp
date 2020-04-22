#include "ssd.hpp"
#include <cstdint>
#include <boost/thread.hpp>
#include <chrono>

/* 
 * Runs SSD read and writes with 1 to NUM_TRIALS threads
 */
int benchmark_threads() {
	int NUM_TRIALS = 8;
	int num_blocks = 1200;
	for (int ii = 1; ii < NUM_TRIALS && num_blocks % ii == 0; ii++){
		// block size is the number of pages inside the block.
		int block_size = 20;
		int page_size = 10;
		int ssd_cell_type = 3; // Multi-level cell
		int ssd_capacity = num_blocks * block_size * page_size * ssd_cell_type;

		ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);

		uint8_t *wbdata = new uint8_t [ssd_capacity]();
		uint8_t *rbdata = new uint8_t [ssd_capacity]();

		for (int i = 0 ; i < ssd_capacity ; i++) {
			wbdata[i] = i % 11; // Randomly write a byte modulo prime number
		}

		mySSD->write_to_disk(wbdata, ssd_capacity);
		boost::thread_group read_threads;
		int NUM_THREADS = ii;
		// auto start_time = std::chrono::high_resolution_clock::now();
		// mySSD->read_from_disk(rbdata, ssd_capacity);
		for (int i = 0; i < NUM_THREADS; ++i) {
			read_threads.create_thread(
				(boost::bind(&ssd::read_from_disk_threads, mySSD, rbdata, ssd_capacity, i, NUM_THREADS))
			);
		}
		read_threads.join_all();
		// auto end_time = std::chrono::high_resolution_clock::now();
		
		bool check = true;
		for (int i = 0 ; i < ssd_capacity ; i++) {
			// printf("i=%d w=%d r=%d\n", i, wbdata[i], rbdata[i]);
			// assert(wbdata[i] == rbdata[i]);
			if (wbdata[i] != rbdata[i]) {
				check = false;
				break;
			}
		}

		if (check) {
			// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
			// std::cout << "Threads: " << ii << " " << duration << endl;
			0;
		}

		delete []wbdata;
		delete []rbdata;
		delete mySSD;
	}
}

// Test for single threaded reads and writes
int single_thread_test() {
	int num_blocks = 10;
	// block size is the number of pages inside the block.
	int block_size = 20;
	int page_size = 10;
	int ssd_cell_type = 3; // Multi-level cell
	int ssd_capacity = num_blocks * block_size * page_size * ssd_cell_type;

	ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);

	uint8_t *wbdata = new uint8_t [ssd_capacity]();
	uint8_t *rbdata = new uint8_t [ssd_capacity]();

	for (int i = 0 ; i < ssd_capacity ; i++) {
		wbdata[i] = 0xFF;
	}

	// mySSD->write_to_disk(wbdata, ssd_capacity);
	mySSD->read_from_disk(rbdata, ssd_capacity);

	for (int i = 0 ; i < ssd_capacity ; i++) {
		// printf("i=%d w=%d r=%d\n", i, wbdata[i], rbdata[i]);
		assert(wbdata[i] == rbdata[i]);
	}

	delete []wbdata;
	delete []rbdata;
	delete mySSD;
}

int multithread_test(int num_threads, int num_blocks) {
	assert(num_threads > 0);
	assert(num_blocks > 0);

	cout << "Threads : " << num_threads << " Blocks: " << num_blocks << endl;

	int block_size = 20; //20 # of pages in block
	int page_size = 10; //10 # of cells in page
	int ssd_cell_type = 3; // Multi-level cell

	int blocks_per_thread = num_blocks / num_threads;
	int block_capacity = block_size * page_size * ssd_cell_type; // bytes per block
	int capacity_per_thread = blocks_per_thread * block_capacity; // bytes per thread

	uint8_t** write_buffer = new uint8_t*[num_threads];
	uint8_t** read_buffer = new uint8_t*[num_threads];

	ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);
	

	// Declare a read buffer with n blocks
	for (int i = 0 ; i < num_threads ; i++) {
		// write_buffer.push_back(new uint8_t [block_capacity]());
		read_buffer[i] = (new uint8_t [capacity_per_thread]());
		write_buffer[i] = (new uint8_t [capacity_per_thread]());
		for (int j = 0; j < capacity_per_thread; j++) {
			write_buffer[i][j] = j % 11;
			// printf("i=%d w=%d\n", i , write_buffer[i][j]);
		}
	}

	// Multithreaded writes to ssd
	boost::thread_group write_threads;
	auto write_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < num_threads; ++i) {
		write_threads.create_thread(
			(boost::bind(&ssd::write_to_disk_threads, mySSD, write_buffer[i], capacity_per_thread, i, blocks_per_thread))
		);
	}
	write_threads.join_all();
	auto write_time_end = std::chrono::high_resolution_clock::now();
	auto duration_write = std::chrono::duration_cast<std::chrono::microseconds>(write_time_end-write_time).count();
	std::cout << "write duration (sec): " << duration_write / 1000000.0 << endl;

	// Multithreaded reads to ssd
	boost::thread_group read_threads;
	auto read_start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < num_threads; ++i) {
		read_threads.create_thread(
			(boost::bind(&ssd::read_from_disk_threads, mySSD, read_buffer[i], capacity_per_thread, i, blocks_per_thread))
		);
	}
	read_threads.join_all();
	auto end_time = std::chrono::high_resolution_clock::now();

	// Check if read buffer is correct
	for (int i = 0 ; i < num_threads ; i++) {
		for (int j = 0; j < capacity_per_thread; j++) {
			// printf("i=%d w=%d r=%d\n", i , write_buffer[i][j], read_buffer[i][j] );
			assert(write_buffer[i][j] == read_buffer[i][j]);
		}
	}
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-read_start_time).count();
	std::cout << "read duration (sec): " << duration / 1000000.0 << endl;

	// Clean up
	for (int i = 0 ; i < num_threads ; i++) {
		delete []write_buffer[i];
		delete []read_buffer[i];
	} 

	delete []write_buffer;
	delete []read_buffer;
	delete mySSD;
}

int main(int argc, char* argv[])
{
	if (argc <= 2) {
		cout << "Usage: ./main_parallel [NUM_THREADS] [NUM_BLOCKS]" << endl;
		return 1;
	}

	int NUM_THREADS = atoi(argv[1]);
	int num_blocks = atoi(argv[2]);

	vector<int> threads = {1, 2, 4, 5, 10, 20, 50, 100};

	for (auto i : threads) {
		multithread_test(i, num_blocks);
	}
	
	return 0;
}

