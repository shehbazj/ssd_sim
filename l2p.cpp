#include "l2p.hpp"
#include <iostream>

// note that logical pages greater than physical page size can be mapped to the device.
// however, the number of logical pages cannot be greater than the physical page limit.

l2p :: l2p(unsigned long size, unsigned long np_per_block) {

	unsigned long i;
	for (i = 0 ; i < size ; i++) {
		l2pMap[i] = LONG_MAX;
		p2lMap[i] = LONG_MAX;
	}

	current_physical_page=0;
	num_pages_per_block=np_per_block;
	map_full=false;
	num_page_allocated=0;
	mapsize=size;

	if(size % np_per_block) {
		cerr << "Number of Pages " << size << " Number of Pages per Block " << np_per_block << endl;
		cerr << "Unaligned page numbers " << endl;
		assert(0);
	}
}

void l2p:: invalidateL2PMap(unsigned long lbn)
{
	unsigned long old_pbn;
	old_pbn = l2pMap[lbn];
	assert(p2lMap[old_pbn] != LONG_MAX);
	l2pMap[lbn] = LONG_MAX;
	p2lMap[old_pbn] = LONG_MAX;
	num_page_allocated--;
}

// overwrite leads to older PBN being marked for deletion.
int l2p:: setPBN(unsigned long lbn, unsigned long pbn)
{
	if (lbn > mapsize || pbn > mapsize) {
		return -1;
	}
	assert(p2lMap[pbn] == LONG_MAX);
	if(l2pMap[lbn] != LONG_MAX) {
		invalidateL2PMap(lbn);
	}
	l2pMap[lbn] = pbn;
	p2lMap[pbn] = lbn;
	num_page_allocated++;
}

unsigned long l2p:: getPBN(unsigned long lbn)
{
	if (l2pMap.find(lbn) != l2pMap.end())
		return l2pMap[lbn];
	return LONG_MAX;
}

unsigned long l2p:: getLBN(unsigned long pbn)
{
	if(p2lMap.find(pbn) != p2lMap.end())
		return p2lMap[pbn];
	return LONG_MAX;
}

unsigned long l2p:: getNextEmptyPage()
{
	if(getNumPageAllocated() == mapsize) {
		map_full = true;
		return LONG_MAX;
	}
	while(p2lMap[current_physical_page] != LONG_MAX) {
		current_physical_page = (current_physical_page + 1 ) % mapsize;
	}
	return current_physical_page;
}

unsigned long l2p:: map_lbns_to_pbns(unsigned long num_pages, vector <unsigned long> logical_page_numbers)
{
	unsigned long current_logical_page_no;
	unsigned long ret;
	int assignedPageCount=0;

	assert(num_pages == logical_page_numbers.size());

	// write for loop here that checks for each logical page being already mapped or not mapped.
	for (assignedPageCount = 0 ; assignedPageCount < num_pages ; assignedPageCount++)
	{
		current_logical_page_no = logical_page_numbers[assignedPageCount];
		// if already mapped, invalidate and remap.
		if(getPBN(logical_page_numbers[assignedPageCount]) != LONG_MAX) {
			invalidateL2PMap(current_logical_page_no);
		}
		// find empty physical page to map.

		ret = getNextEmptyPage();
		if(ret == LONG_MAX)
			return assignedPageCount;
		assert(p2lMap[current_physical_page] == LONG_MAX);
		p2lMap[current_physical_page] = current_logical_page_no;
		l2pMap[current_logical_page_no] = current_physical_page;
		num_page_allocated++;
	}
	return assignedPageCount;
}

unsigned long l2p :: getNumPageAllocated()
{
	return num_page_allocated;
}
