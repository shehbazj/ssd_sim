#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "page_md.hpp"
#include "page_group.hpp"
#include "block.hpp"

/* @Input:
	num_blocks_in_ssd
	size_of_block 
	num_pages_in_block
	input_binary (1 large binary image, smaller than SSD).
	encode (0/1 - 0 data written as is, 1 - data encoded using WOM EC, default 1)
	num_write_cycles (number of times binary image would be written, default 300)
	num_cells (this will determine number of pages dedicated to one write operation).

   @Data Structures:
	Dynamically allocated 2D array (num_blocks_in_ssd * num_pages_in_block X page_size)
	OOB array 2D array (num_blocks_in_ssd * num_pages_in_block X oob_size)
*/

// PAGE

using namespace std;

// SSD - Contains Block

int main(int argc, char *argv[])
{
	// create page object..
	// create page_metadata object.. set generation, set valid, get num_valid_cells, set num_valid_cells.
	// create block_oob object... this will create page metadata objects array. set 4 pages generation, validity directly through this object by indexing a page_group.
	// a page_group is a cluster of 3-4 pages based on TLC or QLC flash type.
	// block_oob == encapsulates ==> page_metadata.
	// block == > encapsulates => block_oob, page.
	// ssd ==> encapsulates ==> block. 
	
	int page_size = 4;
	int xlc = 4;

	page_group page_group_obj(page_size, xlc, NO_WOM);

	uint8_t data_read;
	uint8_t i;
	for (i = 0 ; i < 15; i++) {
		data_read = page_group_obj.writeToCell(i);
		printf("%d\n",data_read);
	}
}
