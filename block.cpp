#include "block.hpp"
#include <cstring>

/* np number of page groups are created. 
	ps is size of each page, not of each page group.
	xlc is the cell type - slc, mlc, tlc or qlc
	bno is the block number (0-number of Blocks)
	womCode is one of the womCode types.
*/

block:: block(int np, int ps, int xlc, long bno, enum wom_coding womCode=NO_WOM, enum blockOverflowConfig b)
{
	num_pages = np;
	num_valid_pages = np*xlc;
	bstatus = EMPTY;
	block_number = bno;
	for (int i = 0 ; i < np ; i++) 
	{
		page_array.push_back(new page_group(ps, xlc, womCode));
	}
	C = new coder((enum wom_coding)womCode);
	boc=b;
	/*
	if (xlc == 3 && womCode >= WOM_2_4_CODE1) {
		printf("TLC Flash not supported for WOM 2,4 Codes,\
		 please change Flash Configuration or WOM Coding Scheme\n");
		assert(0);
	}
	else if (xlc == 4 && womCode < WOM_2_4_CODE1) {
		printf("QLC Flash not supported for WOM 2,3 Codes,\
		 please change Flash Configuration or WOM Coding Scheme\n");
		assert(0);
	}
	*/
}

block:: ~block()
{
	for(int i = 0; i < page_array.size() ; i++)
	{
		delete page_array[i];
	}
<<<<<<< HEAD
=======
	delete C;
>>>>>>> b19e4f1... Fix memory leak in block
}

enum blockStatus block:: getBlockStatus()
{
	return bstatus;
}

void block:: setBlockStatus(blockStatus b)
{
	bstatus = b;
}

/* 	
	caller initializes logical_pages_to_write. 
	caller gives different sequential ppns that can be written after looking at the L2P Map.

	writeToBlock will only write to pages continuously in ascending order.
	raise exception if that does not occur (do sanity test that there is uniform and strict increase of ppns).

	writeToBlock should be able to guarantee all writes that have been requested take place.

	assert if write does not take place successfully.
*/

int block:: writeToBlock(vector < logical_page * > logical_pages_to_write, vector <unsigned long>ppn)
{
	assert(num_pages < getNumValidPages());
	for(int i = 0 ; i < ppn.size() -1 ; i++)
	{
		assert(ppn[i] < ppn[i+1]);
	}
	for (int i = 0 ; i < logical_pages_to_write.size(); i++)
	{
		assert(logical_pages_to_write[i] != nullptr);
		writePage(ppn[i], logical_pages_to_write[i]);
	}
}

/*
	given a list of physical page numbers, this program returns a vector <logical_page>
	of pages. the physical page requested should be a valid physical page number i.e. it should
	be a page number belonging to the block from which read is being requested. 	

	Caller initializes vector of lplist.
	Caller deletes lplist.
*/

int block:: readFromBlock(vector <unsigned long> ppns, vector <logical_page *> lplist)
{
	int i;
	logical_page *l;
	int ret;

	if(lplist.size() == ppns.size()) {
		return -1;
	}
	for(i = 0 ; i < ppns.size() ; i++)
	{
		l = lplist[i];
		assert(l != nullptr);
		ret = readPage(i, l);
		if (ret == -1) {
			cout << "block " << block_number << " page " << i << " read failed" << endl; 
			return -1;
		}
	}
	return ppns.size();
}

void block:: eraseBlock()
{
	for (int i = 0 ; i < num_pages ; i++) {
		page_array[i]->erase_page();
	}
}

int block:: getNumValidPages() {
	return num_valid_pages;
}

int block:: setNumValidPages(int num_pages) {
	num_valid_pages = num_pages;
}

int block:: getNumPages() {
	return num_pages;
}

int block:: setNumPages(int NP) {
	num_pages = NP;
}
/*
int block:: getCurrentPageNumber() {
	return currentPageNumber;
}

int block:: setCurrentPageNumber(int CPN) {
	currentPageNumber = CPN;
}
*/
/* this should only return valid pages, that are programmed,
 * but are not empty.
*/
vector <long unsigned> block:: getAllValidWrittenPageNumbers() {
	vector <long unsigned> physical_page_numbers;
	for(int i = 0 ; i < num_pages ; i++) {
		if ( page_array[i]->isPageValid()  &&  !page_array[i]->isPageEmpty() ) {
			physical_page_numbers.push_back((block_number * num_pages) + i);
		}
	}
	return physical_page_numbers;
}

/* logical page needs to be initialized by caller. */
int block:: readPage(int pbn, logical_page *l) {
	if(l == NULL) {
		return -1;
	}
	if (pbn >= page_array.size()) {
		printf("requested physical page %d more than number of pages in block %lu\n", pbn, page_array.size());
		return -1;
	}
	if(l->getLogicalPageSize() > page_array[pbn]->getPageSize() * page_array[pbn]->getCellType()) {
		printf("read requested for %d is larger than physical page size %d\n", l->getLogicalPageSize() , page_array[pbn]->getPageSize() * page_array[pbn]-> getCellType());
		return -1;
	}
	return page_array[pbn]->read_page(l->getBuf(), page_array[pbn]->getPageSize() * page_array[pbn]->getCellType());
}

/* logical page needs to be initialized by caller. */
int block:: writePage(int pbn, logical_page *l) {
	if(l == NULL) {
		printf("write request with empty logical pages\n");
		return -1;
	}
	if(pbn >= page_array.size()) {
		printf("pbn %d greater than number of pages %lu\n", pbn, page_array.size());
		return -1;
	}
	if(l->getLogicalPageSize() > page_array[pbn]->getPageSize() * page_array[pbn]->getCellType()) {
		printf("Stray Write at pbn %d On Block with block size %d\n", pbn, int (page_array[pbn]->getPageSize() * page_array[pbn]->getCellType()));
		return -1;
	}
	return page_array[pbn]->write_page(l->getBuf(), page_array[pbn]->getPageSize() * page_array[pbn]->getCellType());
}

int block:: invalidatePage(int pbn) {
	page_array[pbn]->setPageValidStatus(false);
}

void block:: setBlockNumber(long bno) {
	block_number = bno;
}

int block :: getCodingScheme()
{
	return page_array[0]->getCellType();
}

int block :: getWomCode()
{
	return page_array[0]->getWomCode();
}

/*
	Take buf of size X MB. check wom Code. if wom code is
	NO_WOM 		-> convert buf to vector <logical_page *> size of the vector = (buf / num_pages)
				logical_buf size = blockSize.
	WOM_2_3 	-> convert buf to vector <logical_page *> size of the vector = (buf / num_pages) * 3/2
				logical_buf size = (blockSize * 2/ 3) 40MB data requires 60MB page size
	WOM_2_4_CODE1	-> convert buf to vector <logical_page *> size of the vector = (buf / num_pages) * 4/2
				logical_buf_size = (blockSize * 2/ 4) 30MB data requires 60MB page size.
	WOM_2_4_CODE2	-> convert buf to vector <logical_page *> size of the vector = (buf / num_pages) * 4/2
				logical_buf_size = (blockSize * 2/ 4) 30MB data requires 60MB page size.

		lpno = 0
		for each ppn:
			1. read prev_code_page in ppn.
			2. read logical_buf[lpno]. LPN
			3. bits_writable = encode(LPN, prev_code_page, new_code_page, lp_size);
			4. assert (bits_writable == lp_size * 8) // for now, we havent implemented gen_max++ writes.
			5. writePage(ppn, new_code_page);
		return -> lpno
*/

// TODO when this is extended to gen_max++ writes, add another parameter - vector <ppn offsets>
// that would contain list of different physical offsets that each of the logical buffers were written to.

/* returns the number of ip_write_block bytes that were written to disk */

int block :: writeToBlock(uint8_t *ip_write_block, int data_size)
{
	int xlc = getCodingScheme();
	int wom_code = getWomCode();
	int i;

	int pblock_size = getPhysicalBlockSize();
	int lblock_size = getLogicalBlockSize();
	int logical_page_size = lblock_size / getNumPages();
	int physical_page_size = pblock_size / getNumPages();

	if (data_size > lblock_size) {
		return -1;
	}

	logical_page *prev_code_page = new logical_page(physical_page_size);
	logical_page *new_code_page  = new logical_page(physical_page_size);
	uint8_t *write_page = new uint8_t[logical_page_size]();
	uint8_t *zeroed_page = new uint8_t[physical_page_size]();

	int lpno = 0, ppn;
	int bits_writable;
	int write_logical_buf_offset=0;
	int bytes_written=0;
	int ret;
	int prev_bits_written=0;
	int start_physical_page_offset=0;
	vector <bool> validBitmap(physical_page_size * 8 / C->getNumDataBits(),true);

	for (ppn = 0 ; ppn < getNumPages() ; ppn++)
	{
		ret = readPage(ppn, prev_code_page);
		if(ret != physical_page_size) {
			assert(ret == physical_page_size);
		}
		// copy block to be written from write_block to write_page.
	//	printf("page %d/%d offset %d/%d\n", ppn, getNumPages(), write_buf_offset , data_size);
		memcpy(write_page, ip_write_block + write_logical_buf_offset, logical_page_size);
		bits_writable = C->encode(write_page, prev_code_page->getBuf(), new_code_page->getBuf(), logical_page_size, validBitmap, start_physical_page_offset);
		if(bits_writable == physical_page_size * 8) {
			write_logical_buf_offset += logical_page_size;
		} else {
			write_logical_buf_offset += ((bits_writable * logical_page_size) / (physical_page_size * 8)) ;
			printf("bytes written %d expected %d data_size %d\n", bits_writable / 8, physical_page_size, data_size);
			assert(0);
		}

		writePage(ppn,new_code_page);
	
		new_code_page->setBuf(zeroed_page, physical_page_size);
		prev_code_page->setBuf(zeroed_page, physical_page_size);
		validBitmap.clear();
	}

	/*
	for (ppn = 0 ; ppn < getNumPages() ; ppn++)
	{
		ret = readPage(ppn, prev_code_page);
		if(ret != physical_page_size) {
			assert(ret == physical_page_size);
		}
		memcpy(write_page, ip_write_block + write_buf_offset, logical_page_size TODO 3 - prev_bits_writable + TODO 3 - startPhysicalPageOffset);

		// current assumption is that bits_writable returns the total number of bits that were encoded from write_page.

		bits_writable = C->encode(write_page, prev_code_page->getBuf(), new_code_page->getBuf(), logical_page_size, TODO 1.vector<bool>invalidBitmap);
										TODO add start physical page offset.

		// TODO change if statement:
	
		if(bits_writable == lps - pbw - so) {
			so = bits_writable;
			prev_bits_written = 0;
		}else {
			so = 0;
			prev_bits_written = bits_writable;
			ppn++;
		}
	
	//	if(bits_writable == physical_page_size * 8) {
	//		write_buf_offset += logical_page_size;
	//	} else {
	//		write_buf_offset += ((bits_writable);
	//		printf("bytes written %d expected %d\n", bits_writable / 8, physical_page_size);
	//		assert(0);
	//	}
		

		writePage(ppn,new_code_page, TODO 2. invalidBitmap);
	
		new_code_page->setBuf(zeroed_page, physical_page_size);
		prev_code_page->setBuf(zeroed_page, physical_page_size);
	
		prev_bits_writable = bits_writable;
	}

	TODO 0 Test - encode - number of bytes written smaller than prev_code_page, then copy remaining data from prev_code_page to new_code page.
	*/

	delete prev_code_page;
	prev_code_page=nullptr;
	delete new_code_page;
	new_code_page=nullptr;

	delete write_page;
	write_page=nullptr;
	delete zeroed_page;
	zeroed_page = nullptr;

	return write_logical_buf_offset;
}

/*
	user initializes data buffer. read each page, decode it. store decoded
	message in buf.
	return number of messages decoded. 
*/
int block :: readFromBlock(uint8_t *read_block, int read_block_size)
{
	/*
		go through all page_array.
		if page is valid, read page contents to coded_page.
		decode page contents to read_page.
		copy read_page to read_block.
	*/
	long unsigned pblock_size = getPhysicalBlockSize();
	long unsigned lblock_size = getLogicalBlockSize();

	int ppn;
	int i;
	int physical_page_size_bytes = pblock_size / getNumPages();
	int decoded_page_size_bytes = lblock_size / getNumPages();
	int bits_decoded=0, bytes_decoded=0;

	assert(pblock_size % getNumPages() == 0);
	assert(lblock_size % getNumPages() == 0);

	logical_page *coded_page = new logical_page(physical_page_size_bytes);
	assert(coded_page != nullptr);
	logical_page *decoded_page = new logical_page(decoded_page_size_bytes);
	assert(decoded_page != nullptr);
	logical_page *zeroed_page = new logical_page(physical_page_size_bytes);
	assert(zeroed_page != nullptr);
	int read_index = 0;

	for(ppn = 0 ; ppn < getNumPages() ; ppn++)
	{
		readPage(ppn, coded_page);
		bits_decoded = C->decode(decoded_page->getBuf(),coded_page->getBuf(), physical_page_size_bytes);
		bytes_decoded = bits_decoded / 8;
		if(read_index + bytes_decoded > read_block_size) {
			printf("read buffer shorter than decoded message, truncate decoded message r+b %d rbs %d\n", read_index + bytes_decoded , read_block_size);
			bytes_decoded = (read_block_size - read_index);
		}
		memcpy(read_block + read_index , decoded_page->getBuf(), bytes_decoded);
		read_index += bytes_decoded;
		if(bits_decoded != decoded_page_size_bytes * 8) {
			printf("short read\n");	
		}
		coded_page->setBuf(zeroed_page->getBuf(), physical_page_size_bytes);
		decoded_page->setBuf(zeroed_page->getBuf(), decoded_page_size_bytes);
	}

	if(coded_page!=nullptr) {
		delete coded_page;
		coded_page = nullptr;
	}
	if(decoded_page !=nullptr) {
		delete decoded_page;
		decoded_page = nullptr;
	}
	if(zeroed_page != nullptr) {
		delete zeroed_page;
		zeroed_page = nullptr;
	}
	return read_index;
}

// reads all physical block values as-is (without any form of encode/decode
// and stores the result in buf.

/* reads raw contents of the flash block in read_buffer */

int block :: readRawBlock(uint8_t *read_buffer)
{
	int lbsize = getLogicalBlockSize();
	int pbsize = getPhysicalBlockSize();

	int lpsize = lbsize / getNumPages();
	int ppsize = pbsize / getNumPages();

	logical_page *l = new logical_page(ppsize);
	uint8_t *zeropage = new uint8_t[ppsize]();

	int i;

	for (i = 0 ; i < getNumPages() ; i++)
	{
		readPage(i, l);
		memcpy(read_buffer + (i * ppsize) , l->getBuf() , ppsize);
		l->setBuf(zeropage, ppsize);
	}
	delete l;
	l = nullptr;
	return (i * ppsize);
}

void block:: getGenerations(int *generation_count)
{
	int pbsize = getPhysicalBlockSize();
	uint8_t *readBuf = new uint8_t[pbsize]();

	int ppsize = pbsize / getNumPages();
	int read_bytes = readRawBlock(readBuf);
	int byte_no, bit_no, code_bits, bit_off;
	int bit_offset;
	int bit;
	int i;
	int gen;

	for(bit_no = 0 ; bit_no < pbsize * 8 ;)
	{
		code_bits = 0;
		for(i = 0 ; i < C->getNumCodeBits() ; i++) {
			byte_no = bit_no / 8;
			bit_off = bit_no % 8;
			bit = readBuf[byte_no] >> (7-bit_off) & 0x01;
			code_bits = code_bits | (bit << (C->getNumCodeBits() - 1 - i));
			bit_no++;
		}
		gen = C->getGeneration(code_bits);
		assert(gen <= 4);
		generation_count[gen]++;
	}
	delete readBuf;
	readBuf = nullptr;
}

// returns size of physical block in bytes.
long unsigned block :: getPhysicalBlockSize()
{
	return getNumPages() * page_array[0]->getPageSize() * page_array[0]->getCellType();
}

// returns size of physical block in bytes.
long unsigned block :: getLogicalBlockSize()
{
	int codeType = getWomCode();
	int data_size=1, code_size=1;
	switch (codeType) {
		case 1:
			data_size = 1; code_size=1;
			break;
		case 2:
			data_size = 2; code_size=3;
			break;
		case 3:
			data_size = 2; code_size=4;
			break;
		case 4:
			data_size = 2; code_size=4;
			break;
		default:
			data_size = 1;
			code_size = 1;
	}
	if( ((getNumPages() * page_array[0]->getPageSize() * page_array[0]->getCellType()) *data_size) % code_size )
	{
		cout << "cannot create logical page with this configuration" << endl;
		return 0;
	}
	return ((getNumPages() * page_array[0]->getPageSize() * page_array[0]->getCellType())*data_size) / code_size;
}

void block :: resetBlockWithoutErase()
{
	int i;
	for (i = 0 ; i < page_array.size() ; i++) {
		page_array[i]->resetPageWithoutErase();
	}
}

logical_page :: logical_page (uint32_t bs)
{
	buf = new uint8_t[bs]();
	byte_size = bs;
}

logical_page :: ~logical_page()
{
	if(buf != nullptr)
		delete []buf;
	buf = nullptr;
}

uint8_t logical_page :: getLogicalPageSize()
{
	return byte_size;
}

uint8_t *logical_page :: getBuf()
{
	assert(buf!= nullptr);
	return buf;
}

void logical_page :: setBuf(uint8_t *data, int buf_size)
{
	assert(buf_size == byte_size);
	assert(data != NULL);
	for (int i=0; i< buf_size; i++)
	{
		memcpy(buf, data, buf_size);
	}
}
