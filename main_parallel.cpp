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
		boost::thread_group worker_threads;
		int NUM_THREADS = ii;
		// auto start_time = std::chrono::high_resolution_clock::now();
		// mySSD->read_from_disk(rbdata, ssd_capacity);
		for (int i = 0; i < NUM_THREADS; ++i) {
			worker_threads.create_thread(
				(boost::bind(&ssd::read_from_disk_threads, mySSD, rbdata, ssd_capacity, i, NUM_THREADS))
			);
		}
		worker_threads.join_all();
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

int simple_test() {
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
		// assert(wbdata[i] == rbdata[i]);
		0;
	}

	delete []wbdata;
	delete []rbdata;
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

	assert(NUM_THREADS > 0);
	assert(num_blocks > 0);

	// benchmark_threads();
	// block size is the number of pages inside the block.
	int block_size = 20; //20
	int page_size = 10; //10
	int ssd_cell_type = 3; // Multi-level cell

	int blocks_per_thread = num_blocks / NUM_THREADS;
	int block_capacity = block_size * page_size * ssd_cell_type; // bytes per block
	int capacity_per_thread = blocks_per_thread * block_capacity; // bytes per thread
	int ssd_capacity = num_blocks * block_capacity;
	vector<uint8_t *> write_buffer;
	uint8_t** read_buffer = new uint8_t*[NUM_THREADS];

	ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);
	
	uint8_t *wbdata = new uint8_t [ssd_capacity]();

	for (int i = 0 ; i < ssd_capacity ; i++) {
		wbdata[i] = i % 11; // Randomly write a byte modulo prime number
	}

	// Declare a read buffer with n blocks
	for (int i = 0 ; i < NUM_THREADS ; i++) {
		// write_buffer.push_back(new uint8_t [block_capacity]());
		read_buffer[i] = (new uint8_t [capacity_per_thread]());
		// for (int j = 0; j < block_capacity; j++) {
		// 	write_buffer[i][j] = j % 11;
		// }
	}

	// mySSD->write_to_disk(wbdata, ssd_capacity);
	mySSD->write_to_disk_threads(wbdata, block_capacity, 0, num_blocks);
	boost::thread_group worker_threads;
	auto start_time = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < NUM_THREADS; ++i) {
		worker_threads.create_thread(
			(boost::bind(&ssd::read_from_disk_threads, mySSD, read_buffer[i], capacity_per_thread, i, blocks_per_thread))
		);
	}
	worker_threads.join_all();
	auto end_time = std::chrono::high_resolution_clock::now();

	// Check if read buffer is correct
	for (int i = 0 ; i < NUM_THREADS ; i++) {
		for (int j = 0; j < capacity_per_thread; j++) {
			// printf("i=%d w=%d r=%d\n", i * capacity_per_thread + j, wbdata[i * capacity_per_thread + j], read_buffer[i][j]);
			assert(wbdata[i * capacity_per_thread + j] == read_buffer[i][j]);
		}
	}
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
	std::cout << "duration " << duration / 1000000.0 << endl;

	// Clean up
	for (int i = 0 ; i < NUM_THREADS ; i++) {
		// delete []write_buffer[i];
		delete []read_buffer[i];
	} 

	delete []wbdata;
	delete mySSD;
}

