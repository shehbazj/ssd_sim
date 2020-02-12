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
#include "coder.hpp"


using namespace std;

int main(int argc, char *argv[])
{
	int page_size = 4;
	int xlc = 4;
	int num_bytes = page_size * xlc;
	int i;

	uint8_t *wdata = new uint8_t[num_bytes]();
	uint8_t *rdata = new uint8_t[num_bytes]();

	enum wom_coding w = NO_WOM;

	for (i = 0 ; i < num_bytes ; i++)
	{
		wdata[i] = rand() % 255;
	}
	
	page_group *pg = new page_group(page_size, xlc, w);

	pg->write_page(wdata, num_bytes);
	// TODO: add code to visualize bits that were stored in PG.

	pg->read_page(rdata, num_bytes);

	for ( i = 0 ; i < num_bytes ; i++ ) {
		assert(wdata[i] == rdata[i]);
	}
}
