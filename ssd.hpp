#include <vector>
#include <stdint.h>
#include "block.hpp"

using namespace std;

class ssd {
	private:
		vector <block> block_array;
		unsigned long num_blocks_in_ssd;
		unsigned long size_of_block;
		int current_block_number;

		unsigned max_invalid_cell_threshold;
		unsigned max_invalid_page_threshold;
		unsigned max_invalid_block_threshold;
		void invokeGC();
	public:
		ssd(int num_blocks, int bs, int ps, int ssd_cell_type);
		~ssd();
		int write_to_disk(uint8_t *buf, int size);
		int read_from_disk(uint8_t *buf, int size);
};

