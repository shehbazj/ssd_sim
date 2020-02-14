#include "ssd.hpp"
#include <cstdint>

int main()
{
	int num_blocks = 10;
	int block_size = 20;
	int page_size = 10;
	int ssd_cell_type = 3; // Multi-level cell

	int ssd_capacity_in_bytes = num_blocks * block_size * page_size * ssd_cell_type;

	uint8_t *wdata = new uint8_t [ssd_capacity_in_bytes]();
	uint8_t *rdata = new uint8_t [ssd_capacity_in_bytes]();

	for (int i = 0 ; i < ssd_capacity_in_bytes ; i++) {
		wdata[i] = 0xFF;
	}
	ssd *mySSD = new ssd(num_blocks, block_size, page_size, ssd_cell_type);

	// Try writing and reading from disk
	printf("Starting writes\n");
	mySSD->write_to_disk(wdata, ssd_capacity_in_bytes);
	printf("Finished writes\n");
	mySSD->read_from_disk(rdata, ssd_capacity_in_bytes);
	for (int i = 0 ; i < ssd_capacity_in_bytes ; i++) {
		if(wdata[i] != rdata[i]) {
			printf("i=%d w=%d r=%d\n", i, wdata[i], rdata[i]);
		}
	}

	delete []wdata;
	delete []rdata;
	delete mySSD;
}
