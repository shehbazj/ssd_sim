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

int main(int argc, char* argv[])
{
	assert(argc > 2);

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
	int block_capacity = block_size * page_size * ssd_cell_type;
	int ssd_capacity = num_blocks * block_size * page_size * ssd_cell_type;
	vector<uint8_t *> read_buffer;
	vector<uint8_t *> write_buffer;

	ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);
	
	uint8_t *wbdata = new uint8_t [ssd_capacity]();
	// uint8_t *rbdata = new uint8_t [ssd_capacity]();

	for (int i = 0 ; i < ssd_capacity ; i++) {
		wbdata[i] = i % 11; // Randomly write a byte modulo prime number
		printf("i=%d w=%d \n", i, wbdata[i]);
	}

	// Declare a read buffer with n blocks
	for (int i = 0 ; i < blocks_per_thread ; i++) {
		// write_buffer.push_back(new uint8_t [block_capacity]());
		read_buffer.push_back(new uint8_t [block_capacity]());
		// for (int j = 0; j < block_capacity; j++) {
		// 	write_buffer[i][j] = j % 11;
		// }
	}

	// mySSD->write_to_disk(wbdata, ssd_capacity);
	boost::thread_group worker_threads;
	auto start_time = std::chrono::high_resolution_clock::now();
	// mySSD->read_from_disk(rbdata, ssd_capacity);
	for (int i = 0; i < NUM_THREADS; ++i) {
		// boost::thread worker(boost::bind
		// 	(&ssd::read_from_disk_threads, mySSD, rbdata, ssd_capacity, i, 2)
		// );
		worker_threads.create_thread(
			(boost::bind(&ssd::read_from_disk_threads, mySSD, read_buffer[i], ssd_capacity, i, NUM_THREADS))
		);
		// worker.join();
	}
	worker_threads.join_all();
	auto end_time = std::chrono::high_resolution_clock::now();
	
	// for (int i = 0 ; i < ssd_capacity ; i++) {
	// 	printf("i=%d w=%d r=%d\n", i, wbdata[i], wbdata[i]);
	// 	assert(wbdata[i] == rbdata[i]);
	// }

	for (int i = 0 ; i < blocks_per_thread ; i++) {
		for (int j = 0; j < block_capacity; j++) {
			printf("i=%d w=%d r=%d\n", i, wbdata[i * block_capacity + j], read_buffer[i][j]);
			assert(wbdata[i * block_capacity + j] == read_buffer[i][j]);
		}
	}
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
	std::cout << "duration " << duration / 1000000.0 << endl;

	for (int i = 0 ; i < write_buffer.size() ; i++) {
		// delete []write_buffer[i];
		delete []read_buffer[i];
	} 

	delete []wbdata;
	// delete []rbdata;
	delete mySSD;
}

