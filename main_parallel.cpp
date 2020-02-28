#include "ssd.hpp"
#include <cstdint>
#include <boost/thread.hpp>
#include <chrono>

/* 
 * Runs SSD read and writes with 1 to NUM_TRIALS threads
 */
int benchmark_threads() {
	int NUM_TRIALS = 10;
	for (int ii = 1; ii < NUM_TRIALS; ii++){
		int num_blocks = 500;
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
		auto start_time = std::chrono::high_resolution_clock::now();
		// mySSD->read_from_disk(rbdata, ssd_capacity);
		for (int i = 0; i < NUM_THREADS; ++i) {
			worker_threads.create_thread(
				(boost::bind(&ssd::read_from_disk_threads, mySSD, rbdata, ssd_capacity, i, NUM_THREADS))
			);
		}
		worker_threads.join_all();
		auto end_time = std::chrono::high_resolution_clock::now();
		
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
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
			std::cout << "Threads: " << ii << " " << duration << endl;
		}

		delete []wbdata;
		delete []rbdata;
		delete mySSD;
	}
}

int main()
{
	benchmark_threads();
	// int num_blocks = 10;
	// // block size is the number of pages inside the block.
	// int block_size = 20;
	// int page_size = 10;
	// int ssd_cell_type = 3; // Multi-level cell
	// int ssd_capacity = num_blocks * block_size * page_size * ssd_cell_type;

	// ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);

	// uint8_t *wbdata = new uint8_t [ssd_capacity]();
	// uint8_t *rbdata = new uint8_t [ssd_capacity]();

	// for (int i = 0 ; i < ssd_capacity ; i++) {
	// 	wbdata[i] = i % 11; // Randomly write a byte modulo prime number
	// }

	// mySSD->write_to_disk(wbdata, ssd_capacity);
	// boost::thread_group worker_threads;
	// int NUM_THREADS = 2;
	// auto start_time = std::chrono::high_resolution_clock::now();
	// // mySSD->read_from_disk(rbdata, ssd_capacity);
	// for (int i = 0; i < NUM_THREADS; ++i) {
	// 	// boost::thread worker(boost::bind
	// 	// 	(&ssd::read_from_disk_threads, mySSD, rbdata, ssd_capacity, i, 2)
	// 	// );
	// 	worker_threads.create_thread(
	// 		(boost::bind(&ssd::read_from_disk_threads, mySSD, rbdata, ssd_capacity, i, NUM_THREADS))
	// 	);
	// 	// worker.join();
	// }
	// worker_threads.join_all();
	// auto end_time = std::chrono::high_resolution_clock::now();
	
	// bool check = true;
	// for (int i = 0 ; i < ssd_capacity ; i++) {
	// 	// printf("i=%d w=%d r=%d\n", i, wbdata[i], rbdata[i]);
	// 	assert(wbdata[i] == rbdata[i]);
	// }

	// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time).count();
	// std::cout << "duration " << duration << endl;

	// delete []wbdata;
	// delete []rbdata;
	// delete mySSD;
}

