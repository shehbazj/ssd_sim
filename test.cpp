#include "page_md.cpp"
#include "block.cpp"
#include "page_group.cpp"
#include "ssd.cpp"
#include "l2p.cpp"
#include "coder.cpp"
#include <cstdlib>
#include <algorithm>

#include <gtest/gtest.h>

#include "arrays.hpp"

// writes first cell of a QLC Flash
TEST(CellTest, WriteOneCell)
{
	page_group page_group_obj(4, 4);
	uint8_t i;
	uint8_t cell_value_read_from_disk;
	uint8_t cell_value_written_to_disk;
	int dataWritten;

	cell_value_written_to_disk = 0x0A;
	dataWritten = page_group_obj.writeToCell(cell_value_written_to_disk);
	ASSERT_EQ(cell_value_written_to_disk, dataWritten);
}

// Reads from empty flash should be 0.
TEST(CellTest, EmptyReadCells)
{
	page_group QLCPage(4, 4);
	uint8_t i;
	uint8_t dataWritten;
	
	for (i = 0 ; i <= 15 ; i++) {
		ASSERT_EQ(QLCPage.readFromCell(i), 0);
	}
}

// writes first 16 cells of a QLC Flash
// read 16 cells should be same as data written.
TEST(CellTest, Write16Cells)
{
	page_group QLCPage(4, 4);
	uint8_t i;
	int data_written;
	
	for (i = 0 ; i <= 15 ; i++) {
		data_written = QLCPage.writeToCell(i);
		ASSERT_EQ(QLCPage.readFromCell(i), data_written);
	}
}

// writes all 10 bytes of page X 8 cells per page of QLC flash.
// reading back 80 Cells should give the same value written.
TEST(CellTest, WriteEntirePageGroup)
{
	int xlc = 4;
	int page_size = 10;

	page_group QLCPage(page_size, xlc);

	uint8_t i;
	int data_written;
	int data_read;
	uint32_t cell;
	
	for(cell = 0 ; cell < page_size * 8 ; cell++) {
		data_written = QLCPage.writeToCell(0x0A);
		ASSERT_EQ(data_written, 0x0A);
	}

	for(cell = 0 ; cell < page_size * 8 ; cell++) {
		data_read = QLCPage.readFromCell(cell);
		ASSERT_EQ(data_read , 0x0A);
	}
}

// checks for illegal values written to XLC Cell.
TEST(CellTest, InvalidValueWriteTest)
{
	page_group QLC_Page(4, 4);
	page_group TLC_Page(4, 3);
	uint8_t i;
	uint8_t cell_value_read_from_disk;
	int data_written;

	// QLC page 0x0A is VALID
	data_written = QLC_Page.writeToCell(0x0A);
	ASSERT_EQ(data_written,0x0A);

	// QLC page 0x10 is INVALID
	data_written = QLC_Page.writeToCell(0x10);
	ASSERT_EQ(data_written,-1);


	// TLC page 0x08 is INVALID
	data_written = TLC_Page.writeToCell(0x08);
	ASSERT_EQ(data_written,-1);
	
	// TLC page 0x07 is VALID
	data_written = TLC_Page.writeToCell(0x07);
	ASSERT_EQ(data_written,0x07);
}

// Fills Entire Page Group. Checks if Page group is still writable. 
// Checks page status.
TEST(CellTest, FullPageWriteTest)
{
	int xlc = 4;
	int page_size = 10;

	page_group QLCPage(page_size, xlc);

	uint8_t i;
	int data_written;
	int data_read;
	uint32_t cell;

	for(cell = 0 ; cell < page_size * 8 ; cell++) {
		data_written = QLCPage.writeToCell(0x0A);
		ASSERT_EQ(data_written, 0x0A);
	}

	// Write to full page.
	data_written = QLCPage.writeToCell(0x0A);
	ASSERT_EQ(data_written, -1);
}

// Fills Entire Page Group. Checks if Page group is still writable. 
// Checks page status.
TEST(CellTest, WriteAfterErase)
{
	int xlc = 4;
	int page_size = 10;

	page_group QLCPage(page_size, xlc);

	uint8_t i;
	int data_written;
	int data_read;
	uint32_t cell;

	for(cell = 0 ; cell < page_size * 8 ; cell++) {
		data_written = QLCPage.writeToCell(0x0A);
		ASSERT_EQ(data_written, 0x0A);
	}

	// Write to full page.
	data_written = QLCPage.writeToCell(0x0A);
	ASSERT_EQ(data_written, -1);

	QLCPage.erase_page();

	for(cell = 0 ; cell < page_size * 8 ; cell++) {
		data_written = QLCPage.writeToCell(0x0A);
		ASSERT_EQ(data_written, 0x0A);
	}
	
	// Write to full page.
	data_written = QLCPage.writeToCell(0x0A);
	ASSERT_EQ(data_written, -1);
}

TEST(L2PTest, createEmptyMap)
{
	l2p myMap(20, 5);
	unsigned long invalidPBN, i;
	for(i = 0 ; i < 21 ; i++) {
		invalidPBN = myMap.getPBN(i);
		ASSERT_EQ(invalidPBN, LONG_MAX);
	}
}

// basic read write.
TEST(L2PTest, writeReadEntireMap)
{
	l2p myMap(20, 5);
	vector <unsigned long> logical_page_numbers;
	int i;

	for(i = 0 ; i < 20 ; i++) {
		logical_page_numbers.push_back(i);
	}
	myMap.map_lbns_to_pbns(20, logical_page_numbers);
//	for(i = 0 ; i < 20 ; i++) {
//		ASSERT_EQ(i,myMap.getPBN(i));
//	}
}

TEST(L2PTest, writeMultipleTimes)
{
	l2p myMap(20, 5);
	vector <unsigned long> logical_page_numbers;
	int i;
	unsigned long num_pages_written;

	for(i = 0 ; i < 10 ; i++) {
		logical_page_numbers.push_back(i);
	}		
	num_pages_written = myMap.map_lbns_to_pbns(10, logical_page_numbers);
	ASSERT_EQ(num_pages_written, 10);
	num_pages_written = myMap.map_lbns_to_pbns(10, logical_page_numbers);
	ASSERT_EQ(num_pages_written, 10);
	for(i = 0 ; i < 10 ; i++) {
		ASSERT_EQ(10+i,myMap.getPBN(i));
	}
}


TEST(L2PTest, fillSSDReWriteSSD)
{
	l2p myMap(20, 5);
	vector <unsigned long> logical_page_numbers;
	int i;
	unsigned long num_pages_written;

	for(i = 0 ; i < 20 ; i++) {
		logical_page_numbers.push_back(i);
	}		
	num_pages_written = myMap.map_lbns_to_pbns(20, logical_page_numbers);
	ASSERT_EQ(num_pages_written, 20);
	num_pages_written = myMap.map_lbns_to_pbns(20, logical_page_numbers);
	ASSERT_EQ(num_pages_written, 20);
	for(i = 0 ; i < 20 ; i++) {
		ASSERT_EQ(i,myMap.getPBN(i));
	}
}

TEST(L2PTest, reWriteMultipleTimes)
{
	l2p myMap(20, 5);
	vector <unsigned long> logical_page_numbers;
	int i;
	unsigned long num_pages_written;

	for(i = 0 ; i < 20 ; i++) {
		logical_page_numbers.push_back(i);
	}
	num_pages_written = myMap.map_lbns_to_pbns(15, vector <unsigned long> (logical_page_numbers.begin() , logical_page_numbers.begin() + 15));
	ASSERT_EQ(num_pages_written, 15);
	num_pages_written = myMap.map_lbns_to_pbns(10, vector <unsigned long> (logical_page_numbers.begin() , logical_page_numbers.begin() + 10));
	ASSERT_EQ(num_pages_written, 10);
	num_pages_written = myMap.map_lbns_to_pbns(5, vector <unsigned long> (logical_page_numbers.begin() , logical_page_numbers.begin() + 5));
	ASSERT_EQ(num_pages_written, 5);
	for(i = 0 ; i < 5 ; i++) {
		ASSERT_EQ(i+5,myMap.getPBN(i));	// [LBN 0-4 gets mapped to PBN 5-9]
		ASSERT_EQ(myMap.getLBN(i+5),i);
		ASSERT_EQ(i,myMap.getPBN(i+5)); // [LBN 5-9 gets mapped to PBN 0-4]
		ASSERT_EQ(myMap.getLBN(i),i+5);
		ASSERT_EQ(i+10,myMap.getPBN(i+10)); // [LBN 10-14 gets mapped to PBN 10-14]
		ASSERT_EQ(myMap.getLBN(i+10),i+10);
	}
}

TEST(L2PTest, mapMoreThanCapacity)
{
	l2p myMap(20, 5);
	vector <unsigned long> logical_page_numbers;
	int i;
	unsigned long num_pages_written;

	logical_page_numbers.push_back(20);
	num_pages_written = myMap.map_lbns_to_pbns(1, logical_page_numbers);
	ASSERT_EQ(num_pages_written, 1);
	ASSERT_EQ(0,myMap.getPBN(20)); // errored lbn
}

// Add page level testing.

TEST(PageTest, writeReadFullPage)
{
	int page_size=50;
	int xlc_type=3;

	//class block b(10, 50, 3, 0);
	uint8_t wbuf[page_size * xlc_type], rbuf[page_size * xlc_type];
	uint8_t x=0xFF;

	int i;
	long unsigned rval, wval;
	
	for(i = 0 ; i < page_size * xlc_type ; i++) {
		wbuf[i]=x;
	}

	page_group page_group_obj(page_size,xlc_type);
	wval = page_group_obj.write_page(wbuf,page_size*xlc_type);
	ASSERT_EQ(wval, page_size*xlc_type);

	rval = page_group_obj.read_page(rbuf,page_size*xlc_type);
	for(i = 0 ; i < page_size *xlc_type ; i++) {
		ASSERT_EQ(rbuf[i],wbuf[i]);
	}
}

// BLOCK Level Tests

TEST(BlockTest, getSetBlockStatus)
{
	class block b(10, 4, 3, 0);
	b.setBlockStatus(EMPTY);
	ASSERT_EQ(b.getBlockStatus(), EMPTY);
}

/*
      int getNumValidPages();
                int setNumValidPages(int num_pages);
*/
//TEST: get number of pages. number of pages should be same as the ones created in block.

TEST(BlockTest, getPages)
{
	class block b(10, 4, 3, 0);
	ASSERT_EQ(30,b.getNumValidPages());
}

/*
                int getNumPages();
                int setNumPages(int num_pages);
// TEST: write to a page. check current page number.
*/


TEST(BlockTest, TLCwriteCheckPageNumber)
{
	int num_pages = 10;
	int page_size = 50;
	int xlc_type = 3;
	int block_number = 0;
	int ret = 0;

	class block b(num_pages, page_size, xlc_type, block_number);
	logical_page *lpw = new logical_page(page_size * xlc_type);
	logical_page *lpr = new logical_page(page_size * xlc_type);
	uint8_t data[page_size * xlc_type];
	uint8_t rdata[page_size * xlc_type];

	for (int i = 0 ; i < page_size * xlc_type ; i++)
	{
		data[i] = 0xFF;
	}
	lpw->setBuf(data, page_size * xlc_type);
	//page_group pgo(page_size, xlc_type);

	ret = b.writePage(0,lpw);
	ASSERT_EQ(ret, page_size * xlc_type);

	ret = b.readPage (0,lpr);
	ASSERT_EQ(ret, page_size * xlc_type);

	EXPECT_TRUE( 0 == std::memcmp(lpw->getBuf(), lpr->getBuf(), lpw->getLogicalPageSize()));
}

TEST(BlockTest, QLCwriteCheckPageNumber)
{
	int num_pages = 10;
	int page_size = 50;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;

	class block b(num_pages, page_size, xlc_type, block_number);
	logical_page *lpw = new logical_page(page_size * xlc_type);
	logical_page *lpr = new logical_page(page_size * xlc_type);
	uint8_t data[page_size * xlc_type];
	uint8_t rdata[page_size * xlc_type];

	for (int i = 0 ; i < page_size * xlc_type ; i++)
	{
		data[i] = 0xFF;
	}
	lpw->setBuf(data, page_size * xlc_type);
//	page_group pgo(page_size, xlc_type);

	ret = b.writePage(0,lpw);
	ASSERT_EQ(ret, page_size * xlc_type);

	ret = b.readPage (0,lpr);
	ASSERT_EQ(ret, page_size * xlc_type);

	EXPECT_TRUE( 0 == std::memcmp(lpw->getBuf(), lpr->getBuf(), lpw->getLogicalPageSize()));
}

/*
We write to alternate pages in the block, then get the different pages that were written to the block.
The page numbers that were written are returned.
*/

TEST(BlockTest , getValidSequence)
{
	int num_pages = 10;
	int page_size = 50;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i, j;
	vector <long unsigned > pbno;

	class block b(num_pages, page_size, xlc_type, block_number);
	uint8_t data [page_size * xlc_type];
	uint8_t rdata[page_size * xlc_type];

	logical_page *lpw = new logical_page(page_size * xlc_type);
	logical_page *lpr = new logical_page(page_size * xlc_type);

	for (i = 0 ; i < page_size * xlc_type ; i++)
	{
		data[i] = 0xFF;
	}
	lpw->setBuf(data, page_size * xlc_type);

	for(i = 0 ; i < 10 ; i+=2) {
		ret = b.writePage(i, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
	}
	pbno = b.getAllValidWrittenPageNumbers();

	for(j = 0, i =0; j < pbno.size() ; j++, i+=2)
	{
		ASSERT_EQ(i, pbno[j]);
	}
}

// TEST: write to all pages, read from all pages.
TEST(BlockTest , QLCwriteAndReadAllPages)
{
	int num_pages = 45;
	int page_size = 10;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}

		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
	}

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		logical_page *lpr = new logical_page(page_size * xlc_type);
		b.readPage(data_byte, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(data_byte, lpr->getBuf()[i]);
		}
		delete lpr;
	}
}

TEST(BlockTest , TLCwriteAndReadAllPages)
{
	int num_pages = 45;
	int page_size = 10;
	int xlc_type = 3;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}

		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
	}

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		logical_page *lpr = new logical_page(page_size * xlc_type);
		b.readPage(data_byte, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(data_byte, lpr->getBuf()[i]);
		}
		delete lpr;
	}
}



//NOTE: This is commented out because it takes too much time to run!!!

/*
TEST(BlockTest , BigBlockWriteRead)
{
	int num_pages = 4096;
	int page_size = 4096;
	int xlc_type = 3;
	int block_number = 0;
	int ret = 0;
	int i;
	uint32_t page_index;

	class block b(num_pages, page_size, xlc_type, block_number);

	cout << "THIS TEST TAKES A WHILE TO RUN...(~2 mins) PLEASE BE PATIENT... " << endl;
	for (page_index = 0; page_index < num_pages; page_index++)
	{
		uint8_t *data = new uint8_t[page_size * xlc_type]();
		if(data == NULL) {
			printf("%s():Out of Memory\n", __func__);
			return;
		}
		logical_page *lpw = new logical_page(page_size * xlc_type);
		if(lpw == NULL) {
			printf("%s():Out of memory\n", __func__);
			return;
		}
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = page_index % UINT8_MAX;
		}

		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(page_index, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
		delete data;
	}

	for (page_index = 0; page_index < num_pages; page_index++)
	{
		uint8_t *rdata = new uint8_t[page_size * xlc_type]();
		if(rdata == NULL) {
			printf("%s():Out of Memory\n", __func__);
			return;
		}
		logical_page *lpr = new logical_page(page_size * xlc_type);
		if(lpr == NULL) {
			printf("%s():Out of memory\n", __func__);
			return;
		}
		b.readPage(page_index, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(page_index % UINT8_MAX, lpr->getBuf()[i]);
		}
		delete lpr;
		delete rdata;
	}
}
*/


TEST(BlockTest , SmallBlockWriteRead)
{
	int num_pages = 1;
	int page_size = 1;
	int xlc_type = 3;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
	}

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		logical_page *lpr = new logical_page(page_size * xlc_type);
		b.readPage(data_byte, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(data_byte, lpr->getBuf()[i]);
		}
		delete lpr;
	}
}

// TEST: write to page. write to page again. read from page.
// First write to the page should suceed.
// Second write to the page should fail.
// Read from the page should still suceed.
TEST(BlockTest , OverWritePageReadPage)
{
	int num_pages = 15;
	int page_size = 3;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, -1);
		delete lpw;
	}

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		logical_page *lpr = new logical_page(page_size * xlc_type);
		b.readPage(data_byte, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(data_byte, lpr->getBuf()[i]);
		}
		delete lpr;
	}
}

// TEST: write to pages more than the block size.
TEST(BlockTest , ExceedWrites)
{
	int num_pages = 15;
	int page_size = 3;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
	}

	// writing more than BSize of data!!
	uint8_t data  [page_size * xlc_type];
	logical_page *lpw = new logical_page(page_size * xlc_type);
	for (i = 0; i < page_size * xlc_type; i++)
	{
		data[i] = data_byte;
	}
	lpw->setBuf(data, page_size * xlc_type);
	ret = b.writePage(data_byte, lpw);
	ASSERT_EQ(ret, -1);

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		logical_page *lpr = new logical_page(page_size * xlc_type);
		b.readPage(data_byte, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(data_byte, lpr->getBuf()[i]);
		}
		delete lpr;
	}
}

// TEST: read from pages more than block size.
TEST(BlockTest , ExceedReads)
{
	int num_pages = 15;
	int page_size = 3;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
	}

	uint8_t rdata [page_size * xlc_type];
	logical_page *lpr = new logical_page(page_size * xlc_type);
	ret = b.readPage(data_byte, lpr);
	ASSERT_EQ(ret, -1);
}

// TEST: write empty logical blocks.
TEST(BlockTest , WriteReadEmptyLogicalBlocks)
{
	int num_pages = 2;
	int page_size = 3;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	// write with empty logical blocks.
	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
//		logical_page *lpw = new logical_page(page_size * xlc_type);
		logical_page *lpw = NULL;
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
//		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, -1);
		delete lpw;
	}

	// write with non-empty logical blocks,
	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
	}
	// read with empty logical blocks.
	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		//logical_page *lpr = new logical_page(page_size * xlc_type);
		logical_page *lpr = NULL;
		ret = b.readPage(data_byte, lpr);
		ASSERT_EQ(ret, -1);
		delete lpr;
	}
}

// TEST: negative numbered pbn.
// returns -1 on failure.

TEST(BlockTest , WriteReadNegativePBN)
{
	int num_pages = 2;
	int page_size = 3;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(-1, lpw);
		ASSERT_EQ(ret, -1);
		delete lpw;
	}
		
	uint8_t data  [page_size * xlc_type];
	logical_page *lpw = new logical_page(page_size * xlc_type);
	for (i = 0; i < page_size * xlc_type; i++)
	{
		data[i] = data_byte;
	}
	lpw->setBuf(data, page_size * xlc_type);
	ret = b.writePage(0, lpw);
	ASSERT_EQ(ret, page_size * xlc_type);
	delete lpw;

	uint8_t rdata [page_size * xlc_type];
	logical_page *lpr = new logical_page(page_size * xlc_type);
	ret = b.readPage(-1, lpr);
	ASSERT_EQ(ret, -1);
}

// TEST: read empty page, no data.
// This test creates an empty block and does not write any data to it.
// Next it reads data from a random page from the block. 
// The resultant data should be 0.
TEST(BlockTest , readEmptyBlock)
{
	int num_pages = 2;
	int page_size = 3;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	uint8_t rdata [page_size * xlc_type];
	logical_page *lpr = new logical_page(page_size * xlc_type);
	ret = b.readPage(0, lpr);
	ASSERT_EQ(ret, page_size * xlc_type);
	for( i = 0 ; i < page_size * xlc_type ; i++) {
		ASSERT_EQ(lpr->getBuf()[i],0);		
	}
}

// TEST: read invalidated page.
TEST(BlockTest , readInvalidatedPage)
{
	int num_pages = 2;
	int page_size = 3;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	uint8_t rdata [page_size * xlc_type];

	b.invalidatePage(0);
	logical_page *lpr = new logical_page(page_size * xlc_type);
	ret = b.readPage(0, lpr);
	ASSERT_EQ(ret, -1);
}

// TEST: read page with invalid pbn number.
TEST(BlockTest , readInvalidPBNNumber)
{
	int num_pages = 2;
	int page_size = 3;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	uint8_t rdata [page_size * xlc_type];

	logical_page *lpr = new logical_page(page_size * xlc_type);
	ret = b.readPage(2, lpr);
	ASSERT_EQ(ret, -1);
}

// TEST: write to block, read from block, erase block, read from block, write to block again, read from block, 
// erase block again, read from block.

//                void eraseBlock();

TEST(BlockTest, writeReadEraseReadWriteReadBlock)
{
	int num_pages = 15;
	int page_size = 40;
	int xlc_type = 4;
	int block_number = 0;
	int ret = 0;
	int i;
	uint8_t data_byte;

	class block b(num_pages, page_size, xlc_type, block_number);

	// 1. WRITE
	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
	}

	// 2. READ (should be readable)
	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		logical_page *lpr = new logical_page(page_size * xlc_type);
		b.readPage(data_byte, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(data_byte, lpr->getBuf()[i]);
		}
		delete lpr;
	}

	// 3. ERASE
	b.eraseBlock();

	// 4. READ (should return 0)
	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		logical_page *lpr = new logical_page(page_size * xlc_type);
		b.readPage(data_byte, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(0, lpr->getBuf()[i]);
		}
		delete lpr;
	}

	// 5. WRITE (should be writable)
	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t data  [page_size * xlc_type];
		logical_page *lpw = new logical_page(page_size * xlc_type);
		for (i = 0; i < page_size * xlc_type; i++)
		{
			data[i] = data_byte;
		}
		lpw->setBuf(data, page_size * xlc_type);
		ret = b.writePage(data_byte, lpw);
		ASSERT_EQ(ret, page_size * xlc_type);
		delete lpw;
	}

	// 2. READ (should return written data)
	for (data_byte = 0; data_byte < num_pages; data_byte++)
	{
		uint8_t rdata [page_size * xlc_type];
		logical_page *lpr = new logical_page(page_size * xlc_type);
		b.readPage(data_byte, lpr);
		for(i = 0; i < page_size * xlc_type;i++)
		{
			EXPECT_EQ(data_byte, lpr->getBuf()[i]);
		}
		delete lpr;
	}

}

TEST(WOMCode, getBlockSizes)
{
	int num_pages = 15;
	int page_size = 20;
	int qlc_type  = 4;
	int tlc_type  = 3;
	int block_number = 0;
	long unsigned sz;

	class block b_no_wom (num_pages, page_size, qlc_type, block_number);
	sz = b_no_wom.getLogicalBlockSize();
	ASSERT_EQ(sz, num_pages * page_size * qlc_type);
	ASSERT_EQ(b_no_wom.getLogicalBlockSize(), b_no_wom.getPhysicalBlockSize());

	class block b_wom2_3 (num_pages, page_size, tlc_type, block_number, WOM_2_3);

	sz = b_wom2_3.getLogicalBlockSize();
	ASSERT_EQ((sz * 3)/ 2 , b_wom2_3.getPhysicalBlockSize());

	class block b_wom2_4_code1 (num_pages, page_size, qlc_type, block_number, WOM_2_4_CODE1);
	sz = b_wom2_4_code1.getLogicalBlockSize();
	ASSERT_EQ(sz * 2, b_wom2_4_code1.getPhysicalBlockSize());

	class block b_wom2_4_code2 (num_pages, page_size, qlc_type, block_number, WOM_2_4_CODE2);
	sz = b_wom2_4_code2.getLogicalBlockSize();
	ASSERT_EQ(sz * 2, b_wom2_4_code2.getPhysicalBlockSize());
}

/*
	wom_coding codeType();
	void setCodeType(wom_coding c);
	int encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int data_size);
	int decode(uint8_t *data_buf, uint8_t *code_buf, int code_buf_size);
	uint8_t getNextCode(uint8_t data_bits, uint8_t previous_code_bits);
	uint8_t getDataBits(uint8_t code_word);
		
*/

// TEST no_wom code, return data as nextCodeBit, irrespective of previous code bit.
// test for all 4 code words.

TEST(WomCode, NoWomBasic)
{
	coder C(NO_WOM);

	uint8_t i, j;
	for (i=0;i<4;i++) {
		for (j=0;j<4;j++) {
			ASSERT_EQ(j, C.getNextCode(j, i)); // data, prev_code
		}
		ASSERT_EQ(C.getDataBits(i), i);
	}
}

// TEST no_wom code, if data is > 0x03, return 0xFF.
// if prev code > 0x03, return 0xFF
// if code > 0x03, no valid databits, so return 0xFF.

TEST(WomCode, NoWomNegative)
{
	coder C(NO_WOM);
	ASSERT_EQ(0xFF, C.getNextCode(0x04,0x00));
	ASSERT_EQ(0xFF, C.getNextCode(0x00,0x04));
	ASSERT_EQ(0xFF, C.getDataBits(0x04));
}

// WOM_2_3

// TEST 2_3 wom code, for prev code word corresponding to same data as current data,
// return the same code word.

TEST(WomCode, Wom23SameData)
{
	coder C(WOM_2_3);
	uint8_t num_generations=2;
	uint8_t num_inputs=4;

	uint8_t i, j;
	for(i = 0 ; i < num_inputs ; i++) {
		for(j=0; j < num_generations; j++) {
			ASSERT_EQ(wom_2_3[i][j],C.getNextCode(i, wom_2_3[i][j]));
		}
	}

	for(i = 0 ; i < num_inputs ; i++) {
		for(j=0; j < num_generations; j++) {
			ASSERT_EQ(i,C.getDataBits(wom_2_3[i][j]));
		}
	}
}

// TEST2_3 wom code, for prev code word corresponding to different data as current data,
// return new code word. try this with 3 other current data, and 4 codes, for both generations.

TEST(WomCode, Wom23DifferentData)
{
	coder C(WOM_2_3);
	uint8_t num_generations=2;
	uint8_t num_inputs=4;

	uint8_t row, gen, ip_data;
	for(ip_data = 0 ; ip_data < num_inputs; ip_data++) {
		for(row = 0 ; row < num_inputs ; row++) {
			for(gen=1; gen < num_generations-1; gen++) {
				if(ip_data != row) {
					ASSERT_EQ(wom_2_3[ip_data][gen+1],C.getNextCode(ip_data, wom_2_3[row][gen]));
				}
			}
		}
	}

	for(row = 0 ; row < num_inputs ; row++) {
		for(gen=0; gen < num_generations; gen++) {
			ASSERT_EQ(row,C.getDataBits(wom_2_3[row][gen]));
		}
	}
}

// TEST2_3 wom code, for prev code word as terminal code, return 0xFF

TEST(WomCode, Wom23LastData)
{
	coder C(WOM_2_3);

	uint8_t num_generations=2;
	uint8_t num_inputs=4;

	uint8_t row, gen=1, ip_data; // fixed gen to last generation
	for(ip_data = 0 ; ip_data < num_inputs; ip_data++) {
		for(row = 0 ; row < num_inputs; row++) {
			if(ip_data != row) {
				ASSERT_EQ(0xFF,C.getNextCode(ip_data, wom_2_3[row][gen]));
			}
		}
	}
	ASSERT_EQ(C.getDataBits(0x08), 0xFF);
}

// 2_4_code1

// TEST 2_4 wom code, for prev code word corresponding to same data as current data,
// return the same code word.

TEST(WomCode, Wom24Code1SameData)
{
	coder C(WOM_2_4_CODE1);
	uint8_t num_generations=3;
	uint8_t num_inputs=4;

	uint8_t i, j;
	for(i = 0 ; i < num_inputs ; i++) {
		for(j=0; j < num_generations; j++) {
			ASSERT_EQ(wom_2_4_code1[i][j],C.getNextCode(i, wom_2_4_code1[i][j]));
		}
	}

	for(i = 0 ; i < num_inputs ; i++) {
		for(j=0; j < num_generations; j++) {
			ASSERT_EQ(i,C.getDataBits(wom_2_4_code1[i][j]));
		}
	}
}

// TEST2_3 wom code, for prev code word corresponding to different data as current data,
// return new code word. try this with 3 other current data, and 4 codes, for both generations.

TEST(WomCode, Wom24Code1DifferentData)
{
	coder C(WOM_2_4_CODE1);
	uint8_t num_generations=3;
	uint8_t num_inputs=4;

	uint8_t row, gen, ip_data;
	for(ip_data = 0 ; ip_data < num_inputs; ip_data++) {
		for(row = 0 ; row < num_inputs ; row++) {
			for(gen=1; gen < num_generations-1; gen++) {
				if(ip_data != row) {
					ASSERT_EQ(wom_2_4_code1[ip_data][gen+1],C.getNextCode(ip_data, wom_2_4_code1[row][gen]));
				}
			}
		}
	}

	for(row = 0 ; row < num_inputs ; row++) {
		for(gen=0; gen < num_generations; gen++) {
			ASSERT_EQ(row,C.getDataBits(wom_2_4_code1[row][gen]));
		}
	}
}

// TEST2_4 wom code, for prev code word as terminal code, return 0xFF

TEST(WomCode, Wom24LastData)
{
	coder C(WOM_2_4_CODE1);

	uint8_t num_generations=3;
	uint8_t num_inputs=4;

	uint8_t row, gen=1, ip_data; // fixed gen to last generation
	for(ip_data = 0 ; ip_data < num_inputs; ip_data++) {
		for(row = 0 ; row < num_inputs; row++) {
			if(ip_data != row) {
				ASSERT_EQ(0xFF,C.getNextCode(ip_data, wom_2_3[row][gen]));
			}
		}
	}
	ASSERT_EQ(C.getDataBits(0x10), 0xFF);
}

// TEST 4_2_code2 code, if data is > 0x0F, return 0xFF.
// TEST 4_2_code2 TODO -> there is a same state transition code here.. how do we differentiate 
// between the two states?

TEST(WomCode, Wom24Code2SameData)
{
	coder C(WOM_2_4_CODE2);
	uint8_t num_generations=4;
	uint8_t num_inputs=4;

	uint8_t i, j;
	for(i = 0 ; i < num_inputs ; i++) {
		for(j=0; j < num_generations; j++) {
			ASSERT_EQ(wom_2_4_code2[i][j],C.getNextCode(i, wom_2_4_code2[i][j]));
		}
	}

/*	
	FIXME
	When we read b0111, we do not know if we should interpret it as 00 or 11.
	Do we also need the generation number of the write to differentiate between 
	00 f''(x) and 11 f'(x), or is there another way to differentiate between the two?

	for(i = 0 ; i < num_inputs ; i++) {
		for(j=0; j < num_generations; j++) {
			ASSERT_EQ(i,C.getDataBits(wom_2_4_code2[i][j]));
		}
	}
*/
}

TEST(WomCode, getGeneration)
{
	coder C0(NO_WOM);
	coder C1(WOM_2_3);
	coder C2(WOM_2_4_CODE1);
	ASSERT_EQ(0,C1.getGeneration(0x00));
	ASSERT_EQ(1,C1.getGeneration(0x04));
	ASSERT_EQ(1,C1.getGeneration(0x01));
	ASSERT_EQ(2,C1.getGeneration(0x03));
	ASSERT_EQ(2,C1.getGeneration(0x05));

	ASSERT_EQ(0,C2.getGeneration(0x00));
	ASSERT_EQ(1,C2.getGeneration(0x04));
	ASSERT_EQ(2,C2.getGeneration(0x0C));
	ASSERT_EQ(2,C2.getGeneration(0x09));
	ASSERT_EQ(3,C2.getGeneration(0x0E));
}

// TEST2_4_code2 wom code, for prev code word corresponding to different data as current data,
// return new code word. try this with 3 other current data, and 4 codes, for both generations.
/*

With current state as 0x0111, and input as 0x01, how can one differentiate between going to 
state 01, f''(x) i.e. 0x1000 v/s going to state 01, f'''(x) ie. state 1100

TEST(WomCode, Wom24Code2DifferentData)
{
	coder C(WOM_2_4_CODE2);
	uint8_t num_generations=4;
	uint8_t num_inputs=4;

	uint8_t row, gen, ip_data;
	for(ip_data = 0 ; ip_data < num_inputs; ip_data++) {
		for(row = 0 ; row < num_inputs ; row++) {
			for(gen=0; gen < num_generations-1; gen++) {
	//			printf("ip_data %d gen %d row %d womcode %d\n", ip_data, gen, row, wom_2_4_code2[row][gen]);
				if(ip_data != row) {
					ASSERT_EQ(wom_2_4_code2[ip_data][gen+1],C.getNextCode(ip_data, wom_2_4_code2[row][gen]));
				}
			}
		}
	}
	
	for(row = 0 ; row < num_inputs ; row++) {
		for(gen=0; gen < num_generations; gen++) {
			ASSERT_EQ(row,C.getDataBits(wom_2_4_code1[row][gen]));
		}
	}
	
}
*/

// TEST2_4 wom code, for prev code word as terminal code, return 0xFF
/*
TEST(WomCode, Wom24LastData)
{
	coder C(WOM_2_4_CODE2);

	uint8_t num_generations=2;
	uint8_t num_inputs=4;

	uint8_t row, gen=1, ip_data; // fixed gen to last generation
	for(ip_data = 0 ; ip_data < num_inputs; ip_data++) {
		for(row = 0 ; row < num_inputs; row++) {
			if(ip_data != row) {
				ASSERT_EQ(0xFF,C.getNextCode(ip_data, wom_2_3[row][gen]));
			}
		}
	}
	ASSERT_EQ(C.getDataBits(0x10), 0xFF);
}
*/


//  int encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int data_size);
//  int decode(uint8_t *data_buf, uint8_t *code_buf, int code_buf_size);

// TEST if any of the parameters have null values, the result is -1. Check the same for decode.
TEST(EncodeDecode, NoWomNullValues)
{
	coder C(WOM_2_4_CODE2);

	uint8_t data_buf[100] = {0x01};
	uint8_t previous_code_buf[200] = { 0x00};
	uint8_t code_buf[200] = {0x00};
	
	int data_size=100;
	int ret;

	ASSERT_EQ(-1,C.encode(NULL, previous_code_buf, code_buf, data_size));
	ASSERT_EQ(-1,C.encode(data_buf, NULL, code_buf, data_size));
	ASSERT_EQ(-1,C.encode(data_buf, previous_code_buf, NULL, data_size));

	ASSERT_EQ(-1,C.decode(NULL, code_buf, data_size));
	ASSERT_EQ(-1,C.decode(data_buf, NULL, data_size));
}

// TEST: code data once with prev_code_buf = 0000, decode code_buf. check if its same as data.
TEST(EncodeDecode, BasicEncodeDecodeNowom)
{
	coder C0(NO_WOM);

	int i;
	uint8_t wdata_buf[100];
	for(i = 0 ; i < 100 ; i++)
		wdata_buf[i] = 0xA9;

	uint8_t rdata_buf[100];
	for(i = 0 ; i < 100 ; i++)
		rdata_buf[i] = 0x00;

	// NO WOM Case::

	uint8_t previous_code_buf_nowom[100];
	for(i = 0 ; i < 100 ; i++)
		previous_code_buf_nowom[i]=0x00;

	uint8_t code_buf_nowom[100];
	for(i = 0 ; i < 100 ; i++)
		code_buf_nowom[i]=0x00;

	int data_size=100;
	int ret;

	ASSERT_EQ(800,C0.encode(wdata_buf, previous_code_buf_nowom, code_buf_nowom, 100));

	for (i = 0 ; i < 100 ; i++)
	{
		ASSERT_EQ(wdata_buf[i], code_buf_nowom[i]);
	}

	ASSERT_EQ(800,C0.decode(rdata_buf, code_buf_nowom, 100));
	EXPECT_TRUE( 0 == std::memcmp(wdata_buf, rdata_buf, 100));
	
	for (i = 0 ; i < 100 ; i++)
	{
		ASSERT_EQ(wdata_buf[i], rdata_buf[i]);
	}	
}


TEST(WomCode, BasicEncodeZeroTo1stGeneration)
{
	coder C1(WOM_2_3);
	ASSERT_EQ(0x00, C1.getNextCode(0x00,0x00));
	ASSERT_EQ(0x04, C1.getNextCode(0x01,0x00));
	ASSERT_EQ(0x02, C1.getNextCode(0x02,0x00));
	ASSERT_EQ(0x01, C1.getNextCode(0x03,0x00));
}

TEST(WomCode, BasicEncode1stTo1stGeneration)
{
	coder C2(WOM_2_4_CODE1);
	ASSERT_EQ(0x00, C2.getNextCode(0x00,0x00));
	ASSERT_EQ(0x04, C2.getNextCode(0x01,0x04));
	ASSERT_EQ(0x02, C2.getNextCode(0x02,0x02));
	ASSERT_EQ(0x01, C2.getNextCode(0x03,0x01));
}

TEST(EncodeDecode, BasicEncodeDeencode23)
{
	// WOMCODE 2_3

	coder C1(WOM_2_3);

	int code_buf_size=150;
	int data_buf_size=100;
	int i;

	uint8_t wdata_buf[100];
	uint8_t rdata_buf[100];

	uint8_t previous_code_buf_23[code_buf_size];
	for(i = 0 ; i < code_buf_size; i++)
		previous_code_buf_23[i]=0x00;

	uint8_t code_buf_23[code_buf_size];
	for(i = 0 ; i < code_buf_size ; i++)
		code_buf_23[i]=0x00;

	for(i = 0 ; i < data_buf_size ; i++) {
		wdata_buf[i] = 0xAF;
		rdata_buf[i] = 0x00;
	}

	ASSERT_EQ(code_buf_size*8,C1.encode(wdata_buf, previous_code_buf_23, code_buf_23, data_buf_size));

	ASSERT_EQ(data_buf_size*8,C1.decode(rdata_buf, code_buf_23, code_buf_size));

	EXPECT_TRUE( 0 == std::memcmp(wdata_buf, rdata_buf, data_buf_size));

	for (i = 0 ; i < data_buf_size ; i++)
	{
		ASSERT_EQ(wdata_buf[i], rdata_buf[i]);
	}

//	uint8_t previous_code_buf_24[200]={0x00};
//	uint8_t code_buf_24[200];
}

TEST(EncodeDecode, BasicEncodeDeencode24)
{
	// WOMCODE 2_3

	coder C2(WOM_2_4_CODE1);

	int code_buf_size=200;
	int data_buf_size=100;
	int i;

	uint8_t wdata_buf[100];
	uint8_t rdata_buf[100];

	uint8_t previous_code_buf_24[code_buf_size];
	for(i = 0 ; i < code_buf_size; i++)
		previous_code_buf_24[i]=0x00;

	uint8_t code_buf_24[code_buf_size];
	for(i = 0 ; i < code_buf_size ; i++)
		code_buf_24[i]=0x00;

	for(i = 0 ; i < data_buf_size ; i++) {
		wdata_buf[i] = 0xAF;
		rdata_buf[i] = 0x00;
	}

	ASSERT_EQ(code_buf_size*8,C2.encode(wdata_buf, previous_code_buf_24, code_buf_24, data_buf_size));

	ASSERT_EQ(data_buf_size*8,C2.decode(rdata_buf, code_buf_24, code_buf_size));

	EXPECT_TRUE( 0 == std::memcmp(wdata_buf, rdata_buf, data_buf_size));

	for (i = 0 ; i < data_buf_size ; i++)
	{
		ASSERT_EQ(wdata_buf[i], rdata_buf[i]);
	}
}

// TEST: code data 2nd and 3rd time, the first time prev_code_buf ==0, second time, prev code buf
// is the same as code buf the first time. data_buf is different from code buf.

/*
	encode (WD1, C0, C1, d_size)
	decode (RD1, C1, c_size);
	RD1 == WD1

	encode (WD2, C1, C2, d_size)
	decode(RD2, C2, c_size);
	RD2 == D2
*/

TEST(EncodeDecode, MultiEncodeDeencode23)
{
	// WOMCODE 2_3

	coder C1(WOM_2_3);

	int c_size=150;
	int d_size=100;
	int i;

	uint8_t wd1[100];
	uint8_t wd2[100];

	uint8_t rd1[100];
	uint8_t rd2[100];

	uint8_t c0[c_size];
	uint8_t c1[c_size];
	uint8_t c2[c_size];

	for(i = 0 ; i < c_size; i++) {
		c0[i]=0x00;
		c1[i]=0x00;
		c2[i]=0x00;
	}


	for(i = 0 ; i < d_size ; i++) {
		wd1[i] = i % 0xFF;
		wd2[i] = i % 0x0A;
		rd1[i] = 0x00;
		rd2[i] = 0x00;
	}

	// first round encode/decode.
	ASSERT_EQ(c_size*8,C1.encode(wd1, c0, c1, d_size));
	ASSERT_EQ(d_size*8,C1.decode(rd1, c1, c_size));
	EXPECT_TRUE( 0 == std::memcmp(wd1, rd1, d_size));

	// second round encode/decode.
	ASSERT_EQ(c_size*8,C1.encode(wd2, c1, c2, d_size));
	ASSERT_EQ(d_size*8,C1.decode(rd2, c2, c_size));
	EXPECT_TRUE( 0 == std::memcmp(wd2, rd2, d_size));
}

/*
	encode (WD1, C0, C1, d_size)
	decode (RD1, C1, c_size);
	RD1 == WD1

	encode (WD2, C1, C2, d_size)
	decode(RD2, C2, c_size);
	RD2 == D2

	encode (WD3, C2, C3, d_size)
	decode(RD3, C2, c_size);
	RD3 == D3

*/

TEST(EncodeDecode, MultiEncodeDeencode24)
{
	// WOMCODE 2_3

	coder C2(WOM_2_4_CODE1);

	int c_size=200;
	int d_size=100;
	int i;

	uint8_t wd1[100];
	uint8_t wd2[100];
	uint8_t wd3[100];

	uint8_t rd1[100];
	uint8_t rd2[100];
	uint8_t rd3[100];

	uint8_t c0[c_size];
	uint8_t c1[c_size];
	uint8_t c2[c_size];
	uint8_t c3[c_size];

	for(i = 0 ; i < c_size; i++) {
		c0[i]=0x00;
		c1[i]=0x00;
		c2[i]=0x00;
		c3[i]=0x00;
	}


	for(i = 0 ; i < d_size ; i++) {
		wd1[i] = i % 0xFF;
		wd2[i] = i % 0x0A;
		wd3[i] = i % 0x0A;
		rd1[i] = 0x00;
		rd2[i] = 0x00;
		rd3[i] = 0x00;
	}

	// first round encode/decode.
	ASSERT_EQ(c_size*8,C2.encode(wd1, c0, c1, d_size));
	ASSERT_EQ(d_size*8,C2.decode(rd1, c1, c_size));
	EXPECT_TRUE( 0 == std::memcmp(wd1, rd1, d_size));

	// second round encode/decode.
	ASSERT_EQ(c_size*8,C2.encode(wd2, c1, c2, d_size));
	ASSERT_EQ(d_size*8,C2.decode(rd2, c2, c_size));
	EXPECT_TRUE( 0 == std::memcmp(wd2, rd2, d_size));

	// third round encode/decode.
	ASSERT_EQ(c_size*8,C2.encode(wd3, c2, c3, d_size));
	ASSERT_EQ(d_size*8,C2.decode(rd3, c3, c_size));
	EXPECT_TRUE( 0 == std::memcmp(wd3, rd3, d_size));
}

/*
TEST If prev_code has highest state, and new input is same as that from coded string, return code
without making any changes in previous_code, but keep valid bitmap as true.
return size = data_size - number of false bits in valid bitmap.

WOM_2_3
Prev_Code
	111 111 111 111 111 111 111 111
	0xFF 0xFF 0xFF

Input_Bitmap
	T T T T T T T T

Input_Data
	00  00  00  00  00  00  00  00	
	0x00 0x00

New_Code
	0xFF 0xFF 0xFF

Output_Bitmap
	T T T T T T T T

Return:
	24 bits
*/

TEST(EncodeDecodeInvalidBits, NoInvalidBits)
{
	coder C1(WOM_2_3);

	int c_size=3;
	int d_size=2;
	int i;

	uint8_t wdata_buf[d_size];
	uint8_t rdata_buf[d_size];

	int num_code_bits = (c_size * 8) / C1.getNumCodeBits();
	vector <bool> valid_code_bitmap(true, num_code_bits);

	// reference code bitmap that remains true.
	// used for comparision after sending valid code bitmap
	// for womCoding.
	vector <bool> alltrue_code_bitmap(true, num_code_bits);

	uint8_t previous_code_buf_23[c_size];
	for(i = 0 ; i < c_size; i++)
		previous_code_buf_23[i]=0x00;

	uint8_t code_buf_23[c_size];
	for(i = 0 ; i < c_size ; i++)
		code_buf_23[i]=0x00;

	for(i = 0 ; i < d_size ; i++) {
		wdata_buf[i] = 0xAF;
		rdata_buf[i] = 0x00;
	}

	for(i = 0 ; i < num_code_bits ; i++) {
		valid_code_bitmap[i] = true;
		alltrue_code_bitmap[i] = true;
	}

	ASSERT_EQ(c_size*8,C1.encode(wdata_buf, previous_code_buf_23, code_buf_23, d_size, valid_code_bitmap, 0));

	ASSERT_EQ(d_size*8,C1.decode(rdata_buf, code_buf_23, c_size, valid_code_bitmap));

	EXPECT_TRUE( 0 == std::memcmp(wdata_buf, rdata_buf, d_size));

	ASSERT_EQ( valid_code_bitmap , alltrue_code_bitmap);

	for (i = 0 ; i < d_size ; i++)
	{
		ASSERT_EQ(wdata_buf[i], rdata_buf[i]);
	}
}

/*

TEST If prev_code has highest state, and new input is different from coded string, return code
without making any changes in previous_code, but set valid bit from true to false. 
return size = data_size - number of false bits in valid bitmap.
 
WOM_2_3
Prev_Code
	111 111 111 111 111 111 111 111
	0xFF 0xFF 0xFF

Input_Bitmap
	T T T T T T T T

Input_Data
	00  01  10  11  00  01  11  10
	0x1B 0x1E

New_Code
	111 111 111 111 111 111 111 111
	0xFF 0xFF 0xFF

Output_Bitmap
	T F F F F F F F

Return:
	3 bits

===========	Decode =======
	00 00
	0x00
2 bits
*/

TEST(EncodeDecodeInvalidBits, SkipUnprogrammableStates)
{
	coder C1(WOM_2_3);

	int c_size=3;
	int d_size=2;
	int i;

	uint8_t wdata_buf[d_size];
	uint8_t rdata_buf[d_size];

	int num_code_bits = (c_size * 8) / C1.getNumCodeBits();
	vector <bool> valid_code_bitmap (num_code_bits, true);
	// reference code bitmap that remains true.
	// used for comparision after sending valid code bitmap
	// for womCoding.
	vector <bool> alltrue_code_bitmap(num_code_bits, true);

	uint8_t previous_code_buf_23[c_size];
	for(i = 0 ; i < c_size; i++)
		previous_code_buf_23[i]=0xFF;

	uint8_t code_buf_23[c_size];
	for(i = 0 ; i < c_size ; i++)
		code_buf_23[i]=0x00;

	for(i = 0 ; i < d_size ; i++) {
		if(i%2 == 0) {
			wdata_buf[i] = 0x1B;
			rdata_buf[i] = 0x00;
		}else {
			wdata_buf[i] = 0x1E;
			rdata_buf[i] = 0x00;
		}
	}

	for(i = 0 ; i < num_code_bits ; i++) {
		valid_code_bitmap[i] = true;
		alltrue_code_bitmap[i] = true;
	}

	ASSERT_EQ(3,C1.encode(wdata_buf, previous_code_buf_23, code_buf_23, d_size, valid_code_bitmap, 0));

	for(i = 0 ; i < num_code_bits ; i++)
	{
		if(i == 0) {
			ASSERT_EQ(valid_code_bitmap[i], true);
		}else {
			ASSERT_EQ(valid_code_bitmap[i], false);
		}
	}

	ASSERT_EQ(2,C1.decode(rdata_buf, code_buf_23, c_size, valid_code_bitmap));

	ASSERT_EQ(0x00, rdata_buf[0]);

	EXPECT_TRUE( 0 == std::memcmp(previous_code_buf_23, code_buf_23, c_size));
}

/*

TEST If prev_code has all invalid states, code should return 0.
prev_code and new_code should remain the same.
valid bitmap should be all false.

WOM_2_3
Prev_Code
	111 111 111 111 111 111 111 111
	0xFF 0xFF 0xFF

Input_Bitmap
	T T T T T T T T

Input_Data
	01  10  11  01  10  11  01  10
	0x6D 0xB6

New_Code
	111 111 111 111 111 111 111 111
	0xFF 0xFF 0xFF

Output_Bitmap
	F F F F F F F F

Return:
	0
*/

TEST(EncodeDecodeInvalidBits, NoMoreProgrammableStates)
{
	coder C1(WOM_2_3);

	int c_size=3;
	int d_size=2;
	int i;

	uint8_t wdata_buf[d_size];
	uint8_t rdata_buf[d_size];

	int num_code_bits = (c_size * 8) / C1.getNumCodeBits();
	vector<bool> valid_code_bitmap(num_code_bits, true);
	// reference code bitmap that remains true.
	// used for comparision after sending valid code bitmap
	// for womCoding.
	vector<bool> allfalse_code_bitmap(num_code_bits, true);

	uint8_t previous_code_buf_23[c_size];
	for(i = 0 ; i < c_size; i++)
		previous_code_buf_23[i]=0xFF;

	uint8_t code_buf_23[c_size];
	for(i = 0 ; i < c_size ; i++)
		code_buf_23[i]=0x00;

	for(i = 0 ; i < d_size ; i++) {
		if(i%2 == 0) {
			wdata_buf[i] = 0x6D;
			rdata_buf[i] = 0x00;
		}else {
			wdata_buf[i] = 0xB6;
			rdata_buf[i] = 0x00;
		}
	}

	for(i = 0 ; i < num_code_bits ; i++) {
		valid_code_bitmap[i] = true;
		allfalse_code_bitmap[i] = false;
	}

	ASSERT_EQ(0,C1.encode(wdata_buf, previous_code_buf_23, code_buf_23, d_size, valid_code_bitmap, 0));

	for(i = 0 ; i < num_code_bits ; i++)
	{
		ASSERT_EQ(valid_code_bitmap[i], false);
	}
	
	ASSERT_EQ(0,C1.decode(rdata_buf, code_buf_23, c_size, valid_code_bitmap));

	ASSERT_EQ(0x00, rdata_buf[0]);

	EXPECT_TRUE( 0 == std::memcmp(previous_code_buf_23, code_buf_23, c_size));
}

/*
 TEST If valid bitmap has few false bits, skip encoding those previous bits.
 return bitmap with same (or more) invalid bits.
 return data size = same as number of false bits in valid_map.

WOM_2_3
Prev_Code
	111 111 111 111 111 111 111 111
	0xFF 0xFF 0xFF

Input_Bitmap
	T F T F T F T F

Input_Data
	01  10  00  01  10  11  00  10
			--
	0x61 0xB2
	(Only the --ed bit gets encoded)

New_Code
	111 111 111 111 111 111 111 111
	0xFF 0xFF 0xFF

Output_Bitmap
	F F F F F F F F

Return:
	0 bits

Decode:
	return 2 bits
	00
	0x00
*/

TEST(EncodeDecodeInvalidBits, MixedValidBitMap)
{
	coder C1(WOM_2_3);

	int c_size=3;
	int d_size=2;
	int i;

	uint8_t wdata_buf[d_size];
	uint8_t rdata_buf[d_size];

	int num_code_bits = (c_size * 8) / C1.getNumCodeBits();
	vector <bool> valid_code_bitmap(num_code_bits, true);
	// reference code bitmap that remains true.
	// used for comparision after sending valid code bitmap
	// for womCoding.
	vector <bool> allfalse_code_bitmap(num_code_bits, true);

	uint8_t previous_code_buf_23[c_size];
	for(i = 0 ; i < c_size; i++)
		previous_code_buf_23[i]=0xFF;

	uint8_t code_buf_23[c_size];
	for(i = 0 ; i < c_size ; i++)
		code_buf_23[i]=0x00;

	for(i = 0 ; i < d_size ; i++) {
		if(i%2 == 0) {
			wdata_buf[i] = 0x61;
			rdata_buf[i] = 0x00;
		}else {
			wdata_buf[i] = 0xB2;
			rdata_buf[i] = 0x00;
		}
	}

	for(i = 0 ; i < num_code_bits ; i++) {
		if(i%2 == 0) {
			valid_code_bitmap[i] = true;
		} else {
			valid_code_bitmap[i] = false;
		}
		allfalse_code_bitmap[i] = false;
	}

	ASSERT_EQ(0,C1.encode(wdata_buf, previous_code_buf_23, code_buf_23, d_size, valid_code_bitmap, 0));

	for(i = 0 ; i < num_code_bits ; i++)
	{
		ASSERT_EQ(valid_code_bitmap[i], false);
	}
	
	ASSERT_EQ(0,C1.decode(rdata_buf, code_buf_23, c_size, valid_code_bitmap));

	ASSERT_EQ(0x00, rdata_buf[0]);

	EXPECT_TRUE( 0 == std::memcmp(previous_code_buf_23, code_buf_23, c_size));
}

/*
Mixed states of valid and invalid bits

WOM_2_3
Prev_Code
	101 111 101 111 101 111 101 111
	0xBD 0xFB 0xEF

Input_Bitmap
	T T T T T T T T

Input_Data
	00  01  01  00  01  00  00  01
	0x14 0x41

New_Code
	101 111 101 111 101 111 101 111
	0xFF 0xFF 0xFF

Output_Bitmap
	F F T T T T F F

Return:
	12 bits
*/

TEST(EncodeDecodeInvalidBits, MixedValidBitMap2)
{
	coder C1(WOM_2_3);

	int c_size=3;
	int d_size=2;
	int i;

	uint8_t wdata_buf[d_size];
	uint8_t rdata_buf[d_size];

	int num_code_bits = (c_size * 8) / C1.getNumCodeBits();
	vector <bool> valid_code_bitmap(num_code_bits, true);
	// reference code bitmap that remains true.
	// used for comparision after sending valid code bitmap
	// for womCoding.
	vector <bool> allfalse_code_bitmap(num_code_bits, true);

	uint8_t previous_code_buf_23[c_size];
	for(i = 0 ; i < c_size; i++)
		previous_code_buf_23[i]=0xFF;

	uint8_t code_buf_23[c_size];
	for(i = 0 ; i < c_size ; i++)
		code_buf_23[i]=0x00;

	for(i = 0 ; i < d_size ; i++) {
		if(i%2 == 0) {
			wdata_buf[i] = 0x61;
			rdata_buf[i] = 0x00;
		}else {
			wdata_buf[i] = 0xB2;
			rdata_buf[i] = 0x00;
		}
	}

	for(i = 0 ; i < num_code_bits ; i++) {
		if(i%2 == 0) {
			valid_code_bitmap[i] = true;
		} else {
			valid_code_bitmap[i] = false;
		}
		allfalse_code_bitmap[i] = false;
	}

	ASSERT_EQ(0,C1.encode(wdata_buf, previous_code_buf_23, code_buf_23, d_size, valid_code_bitmap, 0));

	for(i = 0 ; i < num_code_bits ; i++)
	{
	//		printf("false:i%d, bmap %d\n", i, valid_code_bitmap[i]);
			ASSERT_EQ(valid_code_bitmap[i], false);
	}

	ASSERT_EQ(0,C1.decode(rdata_buf, code_buf_23, c_size, valid_code_bitmap));

	ASSERT_EQ(0x00, rdata_buf[0]);

	EXPECT_TRUE( 0 == std::memcmp(previous_code_buf_23, code_buf_23, c_size));
}

/*
 TEST If valid bitmap has entirely false bits, return 0.
 current_code  = prev_code.
 return = 0.

CODE()

WOM_4_2
Prev_Code
	1011 1110 1111 1011 1110 1111 1000 0010
	0xBE 	  0xFB 	    0xEF      0x82

Input_Bitmap
	T T T T T T T T

Input_Data
	00  01  01  00  01  00  00  01
	0x14 0x41

New_Code
	1011 1110 1111 1011 1110 1111 1011 1000
	0xBE 	  0xFB 	    0xEF      0xB8

Output_Bitmap
	F F T T F F T T

Return:
	16 bits

Decode:
	0x14
Return:
	8 bits

*/

TEST(EncodeDecodeInvalidBits, MixedInvalidatedBits_WOM24)
{
	coder C2(WOM_2_4_CODE1);

	int c_size=4;
	int d_size=2;
	int i;

	uint8_t wdata_buf[d_size];
	uint8_t rdata_buf[d_size];

	int num_code_bits = (c_size * 8) / C2.getNumCodeBits();
	vector <bool> valid_code_bitmap(num_code_bits, true);
	// reference code bitmap that remains true.
	// used for comparision after sending valid code bitmap
	// for womCoding.

	uint8_t previous_code_buf_24[c_size];
	for(i = 0 ; i < c_size; i++)
		previous_code_buf_24[i]=0xFF;

	previous_code_buf_24[0]=0xBE;
	previous_code_buf_24[1]=0xFB;
	previous_code_buf_24[2]=0xEF;
	previous_code_buf_24[3]=0x82;

	uint8_t code_buf_24[c_size];
	for(i = 0 ; i < c_size ; i++)
		code_buf_24[i]=0x00;

	wdata_buf[0]=0x14;
	wdata_buf[1]=0x41;

	for(i=0;i<d_size;i++)
		rdata_buf[i]=0x00;

	for(i = 0 ; i < num_code_bits ; i++) {
		valid_code_bitmap[i] = true;
	}

	ASSERT_EQ(16,C2.encode(wdata_buf, previous_code_buf_24, code_buf_24, d_size, valid_code_bitmap, 0));

	for(i = 0 ; i < num_code_bits ; i++)
	{
		if(i == 2 || i == 3 || i == 6 || i == 7) {
			ASSERT_EQ(valid_code_bitmap[i], true);
		}else {
			ASSERT_EQ(valid_code_bitmap[i], false);
		}
	}

	ASSERT_EQ(8,C2.decode(rdata_buf, code_buf_24, c_size, valid_code_bitmap));

	ASSERT_EQ(0x14, rdata_buf[0]);
}

/*
 TEST If valid bitmap has entirely false bits, return 0.
 current_code  = prev_code.
 return = 0.

CODE()

WOM_2_3
Prev_Code
	101 111 101 111 101 111 101 111
	0xBE 0xFB 0xEF

Input_Bitmap
	T T T T T T T T

Input_Data
	00  01  01  00  01  00  00  01
	0x14 0x41

New_Code
	101 111 101 111 101 111 101 111
	0xBD 0xFB 0xEF

Output_Bitmap
	F T F F F F F F

Return:
	3 bits

Decode:
	0x00

*/

TEST(EncodeDecodeInvalidBits, MixedInvalidatedBits3)
{
	coder C1(WOM_2_3);

	int c_size=3;
	int d_size=2;
	int i;

	uint8_t wdata_buf[d_size];
	uint8_t rdata_buf[d_size];

	int num_code_bits = (c_size * 8) / C1.getNumCodeBits();
	vector <bool> valid_code_bitmap(num_code_bits, true);
	// reference code bitmap that remains true. 
	// used for comparision after sending valid code bitmap
	// for womCoding.
	vector <bool> allfalse_code_bitmap(num_code_bits, false);

	uint8_t previous_code_buf_23[c_size];
	for(i = 0 ; i < c_size; i++)
		previous_code_buf_23[i]=0xFF;

	uint8_t code_buf_23[c_size];
	for(i = 0 ; i < c_size ; i++)
		code_buf_23[i]=0x00;

	previous_code_buf_23[0]=0xBE;
	previous_code_buf_23[1]=0xFB;
	previous_code_buf_23[2]=0xEF;

	wdata_buf[0]=0x14;
	wdata_buf[1]=0x41;

	for(i=0;i<d_size;i++)
		rdata_buf[i]=0x00;

	for(i = 0 ; i < num_code_bits ; i++) {
		valid_code_bitmap[i] = true;
	}

	ASSERT_EQ(3,C1.encode(wdata_buf, previous_code_buf_23, code_buf_23, d_size, valid_code_bitmap, 0));

	for(i = 0 ; i < num_code_bits ; i++)
	{
		if(i == 1) {
			ASSERT_EQ(valid_code_bitmap[i], true);
		}else {
			ASSERT_EQ(valid_code_bitmap[i], false);
		}
	}
	
	ASSERT_EQ(2,C1.decode(rdata_buf, code_buf_23, c_size, valid_code_bitmap));
	
	ASSERT_EQ(0x00, rdata_buf[0]);

	EXPECT_TRUE( 0 == std::memcmp(previous_code_buf_23, code_buf_23, c_size));
}

// TEST: code data 1st time. read the value. code data second time, with the same databuf used the first
// time, read the value. both values should be the same.

TEST(EncodeDecode, ReProgrammeWithSameData24)
{
	// WOMCODE 2_4

	coder C2(WOM_2_4_CODE1);

	int code_buf_size=200;
	int data_buf_size=100;
	int i;

	uint8_t wdata_buf[100];
	uint8_t rdata_buf[100];

	uint8_t previous_code_buf_24[code_buf_size];
	for(i = 0 ; i < code_buf_size; i++)
		previous_code_buf_24[i]=0x00;

	uint8_t code_buf_24[code_buf_size];
	uint8_t new_buf_24[code_buf_size];

	for(i = 0 ; i < code_buf_size ; i++) {
		code_buf_24[i]=0x00;
		new_buf_24[i]=0x00;
	}

	for(i = 0 ; i < data_buf_size ; i++) {
		wdata_buf[i] = 0xAF;
		rdata_buf[i] = 0x00;
	}

	ASSERT_EQ(code_buf_size*8,C2.encode(wdata_buf, previous_code_buf_24, code_buf_24, data_buf_size));

	ASSERT_EQ(data_buf_size*8,C2.decode(rdata_buf, code_buf_24, code_buf_size));

	EXPECT_TRUE( 0 == std::memcmp(wdata_buf, rdata_buf, data_buf_size));

	for (i = 0 ; i < data_buf_size ; i++)
	{
		ASSERT_EQ(wdata_buf[i], rdata_buf[i]);
	}

	ASSERT_EQ(code_buf_size*8,C2.encode(wdata_buf, code_buf_24, new_buf_24, data_buf_size));

	for (i = 0 ; i < code_buf_size ; i++)
	{
		ASSERT_EQ(code_buf_24[i], new_buf_24[i]);
	}
	EXPECT_TRUE( 0 == std::memcmp(code_buf_24, new_buf_24, code_buf_size));

	ASSERT_EQ(data_buf_size*8,C2.decode(rdata_buf, code_buf_24, code_buf_size));

	EXPECT_TRUE( 0 == std::memcmp(wdata_buf, rdata_buf, data_buf_size));

	for (i = 0 ; i < data_buf_size ; i++)
	{
		ASSERT_EQ(wdata_buf[i], rdata_buf[i]);
	}
}

TEST(EncodeDecode, ReProgrammeWithSameData23)
{
	// WOMCODE 2_3

	coder C1(WOM_2_3);

	int code_buf_size=150;
	int data_buf_size=100;
	int i;

	uint8_t wdata_buf[100];
	uint8_t rdata_buf[100];

	uint8_t previous_code_buf_23[code_buf_size];
	for(i = 0 ; i < code_buf_size; i++)
		previous_code_buf_23[i]=0x00;

	uint8_t code_buf_23[code_buf_size];
	uint8_t new_buf_23[code_buf_size];

	for(i = 0 ; i < code_buf_size ; i++) {
		code_buf_23[i]=0x00;
		new_buf_23[i]=0x00;
	}

	for(i = 0 ; i < data_buf_size ; i++) {
		wdata_buf[i] = 0xAF;
		rdata_buf[i] = 0x00;
	}

	ASSERT_EQ(code_buf_size*8,C1.encode(wdata_buf, previous_code_buf_23, code_buf_23, data_buf_size));

	ASSERT_EQ(data_buf_size*8,C1.decode(rdata_buf, code_buf_23, code_buf_size));

	EXPECT_TRUE( 0 == std::memcmp(wdata_buf, rdata_buf, data_buf_size));

	for (i = 0 ; i < data_buf_size ; i++)
	{
		ASSERT_EQ(wdata_buf[i], rdata_buf[i]);
	}

	ASSERT_EQ(code_buf_size*8,C1.encode(wdata_buf, code_buf_23, new_buf_23, data_buf_size));

	for (i = 0 ; i < code_buf_size ; i++)
	{
		ASSERT_EQ(code_buf_23[i], new_buf_23[i]);
	}
	EXPECT_TRUE( 0 == std::memcmp(code_buf_23, new_buf_23, code_buf_size));

	ASSERT_EQ(data_buf_size*8,C1.decode(rdata_buf, code_buf_23, code_buf_size));

	EXPECT_TRUE( 0 == std::memcmp(wdata_buf, rdata_buf, data_buf_size));

	for (i = 0 ; i < data_buf_size ; i++)
	{
		ASSERT_EQ(wdata_buf[i], rdata_buf[i]);
	}
}

// TEST write data of same size as block, read back the data. data should match.

TEST(BlockReadWrite, BasicRWNOWOM)
{
	int np=20;
	int ps=500;
	int xlc=3;
	int bno=0;
	enum wom_coding womCode=NO_WOM;
	int i;

	block *b = new block(np, ps, xlc, bno, womCode);
	
	int lbsize = b->getLogicalBlockSize();
	int pbsize = b->getPhysicalBlockSize();

	uint8_t *blocktowrite = new uint8_t[lbsize]();
	uint8_t *blocktoread = new uint8_t[lbsize]();

	for(i = 0 ; i < lbsize ; i++)
	{
		blocktowrite[i] = rand() % 255;
	}

//	printf("physical block size %d logical block size %d\n", pbsize, lbsize);

	b->writeToBlock(blocktowrite, lbsize);
	b->readFromBlock(blocktoread, lbsize);


	EXPECT_TRUE( 0 == std::memcmp(blocktowrite, blocktoread, lbsize));
}

TEST(BlockReadWrite, BasicRW23_WOM)
{
	int np=40;
	int ps=300;
	int xlc=3;
	int bno=0;
	enum wom_coding womCode=WOM_2_3;
	int i;

	block *b = new block(np, ps, xlc, bno, womCode);
	
	int lbsize = b->getLogicalBlockSize();
	int pbsize = b->getPhysicalBlockSize();

	uint8_t *blocktowrite = new uint8_t[lbsize]();
	uint8_t *blocktoread = new uint8_t[lbsize]();

	for(i = 0 ; i < lbsize ; i++)
	{
		blocktowrite[i] = (uint8_t)(i % 255);
	}

	b->writeToBlock(blocktowrite, lbsize);
	b->readFromBlock(blocktoread, lbsize);

	EXPECT_TRUE( 0 == std::memcmp(blocktowrite, blocktoread, lbsize));
}

TEST(BlockReadWrite, BasicRW24_CODE1)
{
	int np=40;
	int ps=300;
	int xlc=4;
	int bno=0;
	enum wom_coding womCode=WOM_2_4_CODE1;
	int i;

	block *b = new block(np, ps, xlc, bno, womCode);
	
	int lbsize = b->getLogicalBlockSize();
	int pbsize = b->getPhysicalBlockSize();

	uint8_t *blocktowrite = new uint8_t[lbsize]();
	uint8_t *blocktoread = new uint8_t[lbsize]();

	for(i = 0 ; i < lbsize ; i++)
	{
		blocktowrite[i] = (uint8_t)(i % 255);
	}

	b->writeToBlock(blocktowrite, lbsize);
	b->readFromBlock(blocktoread, lbsize);

	EXPECT_TRUE( 0 == std::memcmp(blocktowrite, blocktoread, lbsize));
}

/* Encode a buffer. get encoded buffer back. 
	Write the buffer.
	Read raw buffer.
	Compare with encoded buffer.
*/

TEST(BlockReadWrite, ReadRawBlock_24)
{
	int np=450;
	int ps=30;
	int xlc=4;
	int bno=0;
	enum wom_coding womCode=WOM_2_4_CODE1;
	int i;
	int *generation_count =  new int[5]();	

	coder C(WOM_2_4_CODE1);

	block *b = new block(np, ps, xlc, bno, womCode);
	
	int lbsize = b->getLogicalBlockSize();
	int pbsize = b->getPhysicalBlockSize();

	uint8_t *blocktowrite = new uint8_t[lbsize]();
	uint8_t *blocktoread = new uint8_t[pbsize]();

	uint8_t *prev_code = new uint8_t[pbsize]();
	uint8_t *next_code = new uint8_t[pbsize]();

	for(i = 0 ; i < lbsize ; i++)
	{
		blocktowrite[i] = (uint8_t)(i % 255);
	}
	int ret;

	ret = b->writeToBlock(blocktowrite, lbsize);
	ASSERT_EQ(ret, lbsize);
	ret = C.encode(blocktowrite, prev_code, next_code, lbsize);
	ASSERT_EQ(ret/8 , pbsize);

	ret = b->readRawBlock(blocktoread);
	ASSERT_EQ( ret, pbsize);

	EXPECT_TRUE( 0 == std::memcmp(next_code, blocktoread, pbsize));

	b->getGenerations(generation_count);

	for (i = 0 ; i < 5; i ++) {
		printf("i=%d %d\n", i, generation_count[i]);
	}

	delete blocktowrite;
	blocktowrite = nullptr;
	delete blocktoread;
	blocktoread  = nullptr;
	delete b;
	b=nullptr;
	delete prev_code;
	prev_code = nullptr;
	delete next_code;
	next_code = nullptr;
}

/*
// TEST write data of larger size than block. read back data. only part of data should match.
// Return value should be same as block size.

TEST(BlockReadWrite, ExtraWriteRead)
{
	
}

// TEST write data of smaller size than block. read back data. only part of data should match.
// Return value should be same as size of buffer written.

TEST(BlockReadWrite, ShortWriteRead)
{
	
}
*/

// TEST write 2/3 different buffers to the block. read back after each write. contents should
// match contents of the original data written after each generation.

TEST(BlockReadWrite, DifferentDataMultiWrite23)
{
	int np=1;
	int ps=3;
	int xlc=3;
	int bno=0;
	enum wom_coding womCode=WOM_2_3;
	int i;

	block *b = new block(np, ps, xlc, bno, womCode);
	
	int lbsize = b->getLogicalBlockSize();
	int pbsize = b->getPhysicalBlockSize();

	uint8_t *blocktowrite1 = new uint8_t[lbsize]();
	uint8_t *blocktowrite2 = new uint8_t[lbsize]();
	uint8_t *blocktoread1 = new uint8_t[lbsize]();
	uint8_t *blocktoread2 = new uint8_t[lbsize]();
	uint8_t *blockrawread1 = new uint8_t[pbsize]();
	uint8_t *blockrawread2 = new uint8_t[pbsize]();

	for(i = 0 ; i < lbsize ; i++)
	{
		blocktowrite1[i] = 0xFF;//(uint8_t)(i % 255);
		blocktowrite2[i] = 0xAA;//(uint8_t)((lbsize-i-1) % 255);
	}

	int ret;
	ret = b->writeToBlock(blocktowrite1, lbsize);
	ASSERT_EQ(ret, lbsize);
	ret = b->readFromBlock(blocktoread1, lbsize);
	ASSERT_EQ(ret, lbsize);

	ret = b->readRawBlock(blockrawread1);
	ASSERT_EQ(ret,pbsize);

//	for(i = 0; i < lbsize; i++) {
//		printf("i=%d w%x r%x\n", i, blocktowrite1[i], blocktoread1[i]);
//	}

	EXPECT_TRUE( 0 == std::memcmp(blocktowrite1, blocktoread1, lbsize));

	ret = b->writeToBlock(blocktowrite2, lbsize);
	ASSERT_EQ(ret, lbsize);
	ret = b->readFromBlock(blocktoread2, lbsize);
	ASSERT_EQ(ret, lbsize);

	ret = b->readRawBlock(blockrawread2);
	ASSERT_EQ(ret,pbsize);

//	for(i = 0; i < lbsize; i++) {
//		printf("i=%d w%x r%x\n", i, blocktowrite2[i], blocktoread2[i]);
//	}

//	EXPECT_TRUE( 0 == std::memcmp(blocktowrite2, blocktoread2, lbsize));
}


TEST(BlockReadWrite, DifferentDataMultiWrite24_code1)
{
	int np=40;
	int ps=300;
	int xlc=4;
	int bno=0;
	enum wom_coding womCode=WOM_2_4_CODE1;
	int i;

	block *b = new block(np, ps, xlc, bno, womCode);
	
	int lbsize = b->getLogicalBlockSize();
	int pbsize = b->getPhysicalBlockSize();

	uint8_t *blocktowrite = new uint8_t[lbsize]();
	uint8_t *blocktoread = new uint8_t[lbsize]();

	for(i = 0 ; i < lbsize ; i++)
	{
		blocktowrite[i] = (uint8_t)(i % 255);
	}

	b->writeToBlock(blocktowrite, lbsize);
	b->readFromBlock(blocktoread, lbsize);

	EXPECT_TRUE( 0 == std::memcmp(blocktowrite, blocktoread, lbsize));	

	b->writeToBlock(blocktowrite, lbsize);
	b->readFromBlock(blocktoread, lbsize);

	EXPECT_TRUE( 0 == std::memcmp(blocktowrite, blocktoread, lbsize));

	b->writeToBlock(blocktowrite, lbsize);
	b->readFromBlock(blocktoread, lbsize);

	EXPECT_TRUE( 0 == std::memcmp(blocktowrite, blocktoread, lbsize));

}



// TEST write 2/3 same buffers to the block. Reading back should return the same data again and again.

/*
TEST(BlockReadWrite, SameDataMultiWrite)
{
	
}
*/


// TEST: code data 4th time - try coding it again with different data. than previous write.
// check if you get invalid string.

// For this testcase, we need to make sure that data that is subsiquently written to each of
// the different 2 data bits are different from the previous two data bits. For this we do
// use the following pattern:

//	00 01 10 11 -> 0x1B
//	01 10 11 00 -> 0x6C
//	10 11 00 01 -> 0xB1
//	11 00 01 10 -> 0x6C
//	
//	writing last data should return in 0xFF string....


/*
Do 3 writes.
get generations at the end of write. get cells in last generation.

take one more data buffer. encode page. 
       	encode should return bits_writable == bits in valid bitmap.
       	assert bits in valid bitmap < lpsz.
       	print generation. this generation should have each generation > 3_generation.
       	write this to page.

read back page. use decodeMap with valid bitmap. 
decoded data should match amount of data that was written earlier.
*/

TEST(PageReadWriteOverflow, OverLimitWrites_WOM23)
{
	int page_size=50;
	int xlc_type=3;

	coder *C = new coder(WOM_2_3);

	int page_group_size = page_size * xlc_type;
	int lpsize = (page_group_size  * C->getNumDataBits()) / C->getNumCodeBits();

	if(page_group_size % xlc_type != 0) {
		printf("page_group_size %d unaligned to xlc type %d\n",page_group_size, xlc_type);
		assert(0);
	}

	uint8_t *wbuf1 = new uint8_t[lpsize]();
	uint8_t *rbuf1 = new uint8_t[lpsize]();
	uint8_t *wbuf2 = new uint8_t[lpsize]();
	uint8_t *rbuf2 = new uint8_t[lpsize]();
	uint8_t *wbuf3 = new uint8_t[lpsize]();
	uint8_t *rbuf3 = new uint8_t[lpsize]();
	uint8_t *wbuf4 = new uint8_t[lpsize]();
	uint8_t *rbuf4 = new uint8_t[lpsize]();

	uint8_t *prev_code_page = new uint8_t[page_group_size]();
	uint8_t *read_code_page = new uint8_t[page_group_size]();
	uint8_t *next_code_page = new uint8_t[page_group_size]();

	int i;
	long unsigned rval, wval, c_bits, d_bits;

	
	vector <bool> valid_code_bitmap((page_group_size * 8) / C->getNumCodeBits() , true);
	for(i = 0 ; i < lpsize ; i++) {
		wbuf1[i]=0xFF;
		wbuf2[i]=0xFA;
		wbuf3[i]=0xA5;
		wbuf4[i]=0x00;
	}

	page_group *page_group_obj = new page_group(page_size,xlc_type);

	// 1st write
	// read buffer into prev_code_page
	rval = page_group_obj->read_page(prev_code_page, page_group_size);
	// encode buffer
	ASSERT_EQ(rval , page_group_size);
	memset(next_code_page, 0, page_group_size);
	c_bits = C->encode(wbuf1, prev_code_page, next_code_page, lpsize, valid_code_bitmap, 0);
	ASSERT_EQ(c_bits/8, page_group_size);
	wval = page_group_obj->write_page (next_code_page,page_group_size);
	ASSERT_EQ (wval, page_group_size);
	rval = page_group_obj->read_page(read_code_page,page_group_size);
	ASSERT_EQ (rval, page_group_size);
	EXPECT_TRUE(0 == std::memcmp(read_code_page, next_code_page, page_group_size));
	d_bits = C->decode(rbuf1,read_code_page, page_group_size, valid_code_bitmap);

	ASSERT_EQ (d_bits / 8, lpsize);
	EXPECT_TRUE(0 == std::memcmp(wbuf1, rbuf1, lpsize));

	/*
	printf("1st generation write\n");
	long unsigned *generations_after_1 = new long unsigned[5]();
	page_group_obj->getGenerations(generations_after_1, WOM_2_3);
	for(i = 0 ; i < 5 ; i++) {
		printf("%d %lu\n", i , generations_after_1[i]);
	}
	delete []generations_after_1;
	*/

	int num_valid_cells=0;
	for ( i = 0 ; i < valid_code_bitmap.size () ; i++)
	{
		if(valid_code_bitmap[i] == true) {
			num_valid_cells++;
		}
	}
	ASSERT_EQ(num_valid_cells, valid_code_bitmap.size());

	page_group_obj->resetPageWithoutErase();
	// 2nd write

	rval = page_group_obj->read_page(prev_code_page, page_group_size);
	// encode buffer
	memset(next_code_page, 0, page_group_size);
	ASSERT_EQ(page_group_size, rval);
	c_bits = C->encode(wbuf2, prev_code_page, next_code_page, lpsize, valid_code_bitmap, 0);
	ASSERT_EQ(c_bits/8, page_group_size);
	wval = page_group_obj->write_page (next_code_page,page_group_size);
	ASSERT_EQ (wval, page_group_size);
	rval = page_group_obj->read_page(read_code_page,page_group_size);
	ASSERT_EQ (rval, page_group_size);
	EXPECT_TRUE(0 == std::memcmp(read_code_page, next_code_page, page_group_size));
	d_bits = C->decode(rbuf2,read_code_page, page_group_size, valid_code_bitmap);

	//printf("page group size %d lpsize %d rval %lu\n", page_group_size, lpsize, rval);
	ASSERT_EQ (d_bits / 8, lpsize);
	EXPECT_TRUE(0 == std::memcmp(wbuf2, rbuf2, lpsize));
	
	page_group_obj->resetPageWithoutErase();

	/*
	printf("2nd generation write\n");
	long unsigned *generations_after_2 = new long unsigned[5]();
	page_group_obj->getGenerations(generations_after_2, WOM_2_3);
	for(i = 0 ; i < 5 ; i++) {
		printf("%d %lu\n", i , generations_after_2[i]);
	}
	delete []generations_after_2;
	*/

	num_valid_cells=0;
	for ( i = 0 ; i < valid_code_bitmap.size () ; i++)
	{
		if(valid_code_bitmap[i] == true) {
			num_valid_cells++;
		}
	}
	ASSERT_EQ(num_valid_cells, valid_code_bitmap.size());

	// 3rd write

	// read page
	rval = page_group_obj->read_page(prev_code_page, page_group_size);
	ASSERT_EQ(page_group_size, rval);
	// encode page
	memset(next_code_page, 0, page_group_size);
	c_bits = C->encode(wbuf3, prev_code_page, next_code_page, lpsize, valid_code_bitmap, 0);
	// write page
	wval = page_group_obj->write_page (next_code_page, wval);
	// make sure valid code bitmap has the same number of true bits as the total
	// number of codable cells in the flash.
	num_valid_cells=count(valid_code_bitmap.begin(), valid_code_bitmap.end(), true);
	ASSERT_EQ(c_bits / C->getNumCodeBits(), num_valid_cells);
	ASSERT_EQ(page_group_size, wval);
	rval = page_group_obj->read_page(read_code_page,page_group_size);
	ASSERT_EQ(page_group_size, rval);
	EXPECT_TRUE(0 == std::memcmp(read_code_page, next_code_page, page_group_size));
	d_bits = C->decode(rbuf3,read_code_page, page_group_size, valid_code_bitmap);
	ASSERT_EQ(d_bits , num_valid_cells * C->getNumDataBits());

	/*
	long unsigned *generations_after_3 = new long unsigned[5]();

	page_group_obj->getGenerations(generations_after_3, WOM_2_3);
	for(i = 0 ; i < 5 ; i++) {
		printf("%d %lu\n", i , generations_after_3[i]);
	}
	delete []generations_after_3;
	*/

	EXPECT_TRUE(0 == std::memcmp(rbuf3, wbuf3, d_bits / 8));

	delete prev_code_page;
	delete next_code_page;
	delete page_group_obj;
	delete C;
	delete []wbuf1;
	delete []wbuf2;
	delete []wbuf3;
	delete []wbuf4;
	delete []rbuf1;
	delete []rbuf2;
	delete []rbuf3;
	delete []rbuf4;
}

/*
	// do 3 times:
	// take data buffer. encode page. write to page.
	// read back data from page, decode. data should match.

	// get generations at the end of write. get cells in last generation.

	// take one more data buffer. encode page. 
	//	encode should return bits_writable == bits in valid bitmap = 0
	//	assert bits in valid bitmap = 0
	//	print generation. this generation should have each generation = 3_generation.
	//	write this to page.

	//write to page.
	// this should
*/

TEST(PageReadWriteOverflow, WOM24_UnwritableAfter3rdGeneration)
{
	int page_size=50;
	int xlc_type=4;

	coder *C = new coder(WOM_2_4_CODE1);

	int page_group_size = page_size * xlc_type;
	int lpsize = (page_group_size  * C->getNumDataBits()) / C->getNumCodeBits();

	if(page_group_size % xlc_type != 0) {
		assert(0);
	}

	uint8_t *wbuf1 = new uint8_t[lpsize]();
	uint8_t *rbuf1 = new uint8_t[lpsize]();
	uint8_t *wbuf2 = new uint8_t[lpsize]();
	uint8_t *rbuf2 = new uint8_t[lpsize]();
	uint8_t *wbuf3 = new uint8_t[lpsize]();
	uint8_t *rbuf3 = new uint8_t[lpsize]();
	uint8_t *wbuf4 = new uint8_t[lpsize]();
	uint8_t *rbuf4 = new uint8_t[lpsize]();

	uint8_t *prev_code_page = new uint8_t[page_group_size]();
	uint8_t *read_code_page = new uint8_t[page_group_size]();
	uint8_t *next_code_page = new uint8_t[page_group_size]();

	int i;
	int rval, wval;
	long unsigned c_bits, d_bits;

	
	vector <bool> valid_code_bitmap((page_group_size * 8) / C->getNumCodeBits() , true);
	for(i = 0 ; i < lpsize ; i++) {
		wbuf1[i]=0xFF;
		wbuf2[i]=0x00;
		wbuf3[i]=0xAA;
		wbuf4[i]=0x55;
	}

	page_group *page_group_obj = new page_group(page_size,xlc_type);

	// 1st write
	// read buffer into prev_code_page
	rval = page_group_obj->read_page(prev_code_page, page_group_size);
	// encode buffer
	ASSERT_EQ(rval , page_group_size);
	memset(next_code_page, 0, page_group_size);
	c_bits = C->encode(wbuf1, prev_code_page, next_code_page, lpsize, valid_code_bitmap, 0);
	ASSERT_EQ(c_bits/8, page_group_size);
	wval = page_group_obj->write_page (next_code_page,page_group_size);
	ASSERT_EQ (wval, page_group_size);
	rval = page_group_obj->read_page(read_code_page,page_group_size);
	ASSERT_EQ (rval, page_group_size);
	EXPECT_TRUE(0 == std::memcmp(read_code_page, next_code_page, page_group_size));
	d_bits = C->decode(rbuf1,read_code_page, page_group_size, valid_code_bitmap);

	ASSERT_EQ (d_bits / 8, lpsize);
	EXPECT_TRUE(0 == std::memcmp(wbuf1, rbuf1, lpsize));

	int num_valid_cells= count(valid_code_bitmap.begin(), valid_code_bitmap.end(), true);
	ASSERT_EQ(num_valid_cells, valid_code_bitmap.size());

	page_group_obj->resetPageWithoutErase();

	// 2nd write
	rval = page_group_obj->read_page(prev_code_page, page_group_size);
	// encode buffer
	memset(next_code_page, 0, page_group_size);
	ASSERT_EQ(page_group_size, rval);
	c_bits = C->encode(wbuf2, prev_code_page, next_code_page, lpsize, valid_code_bitmap, 0);
	ASSERT_EQ(c_bits, page_group_size * 8);
	wval = page_group_obj->write_page(next_code_page, page_group_size);
	ASSERT_EQ (wval, page_group_size);
	memset(read_code_page, 0, page_group_size);
	rval = page_group_obj->read_page (read_code_page, page_group_size);
	ASSERT_EQ (rval, page_group_size);
	EXPECT_TRUE(0 == std::memcmp(read_code_page, next_code_page, page_group_size));

	d_bits = C->decode(rbuf2,read_code_page, page_group_size, valid_code_bitmap);

	ASSERT_EQ (d_bits / 8, lpsize);
	EXPECT_TRUE(0 == std::memcmp(wbuf2, rbuf2, lpsize));
	
	page_group_obj->resetPageWithoutErase();
	/*
	printf("2nd generation write\n");
	long unsigned *generations_after_2 = new long unsigned[5]();
	page_group_obj->getGenerations(generations_after_2, WOM_2_4_CODE1);
	for(i = 0 ; i < 5 ; i++) {
		printf("%d %lu\n", i , generations_after_2[i]);
	}
	delete []generations_after_2;
	*/
	num_valid_cells= count(valid_code_bitmap.begin(), valid_code_bitmap.end(), true);
	ASSERT_EQ(num_valid_cells, valid_code_bitmap.size());

	// 3rd write

	// read page
	rval = page_group_obj->read_page(prev_code_page, page_group_size);
	ASSERT_EQ(page_group_size, rval);
	// encode page
	memset(next_code_page, 0, page_group_size);
	c_bits = C->encode(wbuf3, prev_code_page, next_code_page, lpsize, valid_code_bitmap, 0);
	// write page
	wval = page_group_obj->write_page (next_code_page, wval);
	// make sure valid code bitmap has the same number of true bits as the total
	// number of codable cells in the flash.
	num_valid_cells=count(valid_code_bitmap.begin(), valid_code_bitmap.end(), true);
	ASSERT_EQ(c_bits / C->getNumCodeBits(), num_valid_cells);
	ASSERT_EQ(page_group_size, wval);
	rval = page_group_obj->read_page(read_code_page,page_group_size);
	ASSERT_EQ(page_group_size, rval);
	EXPECT_TRUE(0 == std::memcmp(read_code_page, next_code_page, page_group_size));

	d_bits = C->decode(rbuf3,read_code_page, page_group_size, valid_code_bitmap);
	ASSERT_EQ(d_bits , num_valid_cells * C->getNumDataBits());
	/*
	long unsigned *generations_after_3 = new long unsigned[5]();

	page_group_obj->getGenerations(generations_after_3, WOM_2_4_CODE1);
	for(i = 0 ; i < 5 ; i++) {
		printf("%d %lu\n", i , generations_after_3[i]);
	}
	delete []generations_after_3;
	*/
	EXPECT_TRUE(0 == std::memcmp(rbuf3, wbuf3, d_bits / 8));
	page_group_obj->resetPageWithoutErase();

	// 4th write

	// read page
	rval = page_group_obj->read_page(prev_code_page, page_group_size);
	ASSERT_EQ(page_group_size, rval);
	// encode page
	memset(next_code_page, 0, page_group_size);
	c_bits = C->encode(wbuf4, prev_code_page, next_code_page, lpsize, valid_code_bitmap, 0);
	// write page
	wval = page_group_obj->write_page (next_code_page, wval);
	// make sure valid code bitmap has the same number of true bits as the total
	// number of codable cells in the flash.
	num_valid_cells=count (valid_code_bitmap.begin(), valid_code_bitmap.end(), true);
	ASSERT_EQ(c_bits , num_valid_cells * C->getNumCodeBits());
	ASSERT_EQ(page_group_size, wval);
	rval = page_group_obj->read_page(read_code_page,page_group_size);
	ASSERT_EQ(page_group_size, rval);
	EXPECT_TRUE(0 == std::memcmp(read_code_page, next_code_page, page_group_size));
	d_bits = C->decode(rbuf4,read_code_page, page_group_size, valid_code_bitmap);
	ASSERT_EQ(d_bits, 0);
	ASSERT_EQ(d_bits , num_valid_cells * C->getNumDataBits());
	/*
	long unsigned *generations_after_4 = new long unsigned[5]();

	page_group_obj->getGenerations(generations_after_4, WOM_2_4_CODE1);
	for(i = 0 ; i < 5 ; i++) {
		printf("%d %lu\n", i , generations_after_4[i]);
	}
	delete []generations_after_4;
	*/

	EXPECT_TRUE(0 == std::memcmp(rbuf4, wbuf4, d_bits / 8));

	delete prev_code_page;
	delete next_code_page;
	delete page_group_obj;
	delete C;
	delete []wbuf1;
	delete []wbuf2;
	delete []wbuf3;
	delete []wbuf4;
	delete []rbuf1;
	delete []rbuf2;
	delete []rbuf3;
	delete []rbuf4;
}

/*
 run encode() from start offset, with all valid bitmap
 returns code with length = lbn - start offset.
 read back page. use decodeMap with start offset.
 returns all data from start offset upto end of offset.
 decoded data should match amount of data that was written earlier.
*/

TEST(PageReadWriteOverflow, WriteFromDifferentPageStartAllvalidPageMap)
{
	int page_size=50;
	int xlc_type=4;

	coder *C = new coder(WOM_2_4_CODE1);

	int page_group_size = page_size * xlc_type;
	int lpsize = (page_group_size  * C->getNumDataBits()) / C->getNumCodeBits();

	if(page_group_size % xlc_type != 0) {
		assert(0);
	}

	uint8_t *wbuf1 = new uint8_t[lpsize]();
	uint8_t *rbuf1 = new uint8_t[lpsize]();

	uint8_t *prev_code_page = new uint8_t[page_group_size]();
	uint8_t *read_code_page = new uint8_t[page_group_size]();
	uint8_t *next_code_page = new uint8_t[page_group_size]();

	int i;
	int rval, wval;
	long unsigned c_bits, d_bits;
	long unsigned startCellOffset=2;

	vector <bool> valid_code_bitmap((page_group_size * 8) / C->getNumCodeBits() , true);
	long unsigned byte_aligned_read_offset=0;

	for(i = 0 ; i < lpsize ; i++) {
		wbuf1[i]=0xFF;
	}

	page_group *page_group_obj = new page_group(page_size,xlc_type);

	// read buffer into prev_code_page
	// keep writing to different startCellOffsets.

	for (startCellOffset = 0 ; startCellOffset < (page_size * 8); startCellOffset++) {
		memset(prev_code_page, 0, page_group_size);
		rval = page_group_obj->read_page(prev_code_page, page_group_size);
		// encode buffer
		ASSERT_EQ(rval , page_group_size);
		memset(next_code_page, 0, page_group_size);
		c_bits = C->encode(wbuf1, prev_code_page, next_code_page, lpsize, valid_code_bitmap, startCellOffset);
		ASSERT_EQ(c_bits, (page_group_size * 8)  - (startCellOffset * C->getNumCodeBits()) );
		wval = page_group_obj->write_page (next_code_page,page_group_size);
		ASSERT_EQ (wval, page_group_size);
		rval = page_group_obj->read_page(read_code_page,page_group_size);
		ASSERT_EQ (rval, page_group_size);
		byte_aligned_read_offset = ((startCellOffset * C->getNumCodeBits()) + 8) / 8;
		if(page_group_size > byte_aligned_read_offset) {
			EXPECT_TRUE(0 == std::memcmp(read_code_page + byte_aligned_read_offset,
					next_code_page + byte_aligned_read_offset ,
					page_group_size - byte_aligned_read_offset ));
		}
		d_bits = C->decode(rbuf1,read_code_page, page_group_size, valid_code_bitmap);
	
		ASSERT_EQ (d_bits, lpsize * 8);

		byte_aligned_read_offset = ((startCellOffset * C->getNumDataBits()) + 8) / 8;
		if(lpsize > byte_aligned_read_offset) {
			EXPECT_TRUE(0 == std::memcmp(wbuf1 + byte_aligned_read_offset,
					rbuf1 + byte_aligned_read_offset,
					lpsize - byte_aligned_read_offset));
		}
		int num_valid_cells= count(valid_code_bitmap.begin(), valid_code_bitmap.end(), true);
		ASSERT_EQ(num_valid_cells, valid_code_bitmap.size());

		page_group_obj->resetPageWithoutErase();
	}

	delete page_group_obj;
	delete C;

	delete []wbuf1;
	delete []rbuf1;

	delete []prev_code_page;
	delete []read_code_page;
	delete []next_code_page;
}

// run encode() from start offset, with all valid bitmap
// returns code with length = lbn - start offset - validbitmap(falses).
// read back page. use decodeMap with start offset and invalidBitmap.
// returns all data from start offset upto end of block - bits in validbitmap set to false.
// decoded data should match amount of data that was written earlier.

TEST(PageReadWriteOverflow, WriteDifferentPageStartWithInvalidBitmap)
{
	int page_size=1;
	int xlc_type=4;

	coder *C = new coder(WOM_2_4_CODE1);

	int page_group_size = page_size * xlc_type;
	int lpsize = (page_group_size  * C->getNumDataBits()) / C->getNumCodeBits();

	if(page_group_size % xlc_type != 0) {
		assert(0);
	}

	uint8_t *wbuf1 = new uint8_t[lpsize]();
	uint8_t *rbuf1 = new uint8_t[lpsize]();

	uint8_t *prev_code_page = new uint8_t[page_group_size]();
	uint8_t *read_code_page = new uint8_t[page_group_size]();
	uint8_t *next_code_page = new uint8_t[page_group_size]();

	int i;
	int rval, wval;
	long unsigned c_bits, d_bits;
	long unsigned startCellOffset=2;

	vector <bool> valid_code_bitmap((page_group_size * 8) / C->getNumCodeBits() , true);
	long unsigned byte_aligned_read_offset=0;

	for (i = 0 ; i < valid_code_bitmap.size() ; i++)
	{
		if (i % 2 == 0) {
			valid_code_bitmap[i]=false;
		}else {
			valid_code_bitmap[i]=true;
		}
	}

	for(i = 0 ; i < lpsize ; i++) {
		wbuf1[i]=0xFF;
	}

	page_group *page_group_obj = new page_group(page_size,xlc_type);

	memset(prev_code_page, 0, page_group_size);
	// read page from disk
	rval = page_group_obj->read_page(prev_code_page, page_group_size);
	// encode buffer
	ASSERT_EQ(rval , page_group_size);
	memset(next_code_page, 0, page_group_size);
	// encode page read from disk.
	c_bits = C->encode(wbuf1, prev_code_page, next_code_page, lpsize, valid_code_bitmap, startCellOffset);
	// this variable stores the number of cells from startCellOffset upto end of valid_code_bitmap that are set.
	int num_relevant_valid_cells = count (valid_code_bitmap.begin() + startCellOffset, valid_code_bitmap.end() , true);
	ASSERT_EQ(c_bits, num_relevant_valid_cells   * C->getNumCodeBits());
	wval = page_group_obj->write_page (next_code_page,page_group_size);
	ASSERT_EQ (wval, page_group_size);
	rval = page_group_obj->read_page(read_code_page,page_group_size);
	ASSERT_EQ (rval, page_group_size);
	byte_aligned_read_offset = ((startCellOffset * C->getNumCodeBits()) + 8) / 8;
	if(page_group_size > byte_aligned_read_offset) {
		EXPECT_TRUE(0 == std::memcmp(read_code_page + byte_aligned_read_offset,
				next_code_page + byte_aligned_read_offset ,
				page_group_size - byte_aligned_read_offset ));
	}
	d_bits = C->decode(rbuf1,read_code_page, page_group_size, valid_code_bitmap);
	
	ASSERT_EQ (d_bits, count(valid_code_bitmap.begin(), valid_code_bitmap.end(), true) * C->getNumDataBits() );

	byte_aligned_read_offset = ((startCellOffset * C->getNumDataBits()) + 8) / 8;

	int wbit_index = startCellOffset * C->getNumDataBits();
	int rbit_index = startCellOffset * C->getNumDataBits();

	int byte_offset_start = 0;
	if(lpsize > byte_aligned_read_offset) {
		for ( i = startCellOffset * C->getNumDataBits() ; i < num_relevant_valid_cells * C->getNumDataBits() ; i+=C->getNumDataBits())
		{
			byte_offset_start = i / 8;
			if(!valid_code_bitmap[i / C->getNumDataBits()]) {
				wbit_index += C->getNumDataBits();
			}else {
				printf("i %d byte_offset %d windex %x rindex %x\n", i, byte_offset_start, wbuf1[wbit_index/8], rbuf1[rbit_index/8]);
				wbit_index += C->getNumDataBits();
				rbit_index += C->getNumDataBits();
				//ASSERT_EQ(wbuf1[w_index], rbuf1[r_index++]);
				//EXPECT_TRUE(0 == std::memcmp(wbuf1 + byte_aligned_read_offset,
				//rbuf1 + byte_aligned_read_offset,
				//num_relevant_valid_cells * C->getNumDataBits()));
			}
		}
	}
	page_group_obj->resetPageWithoutErase();

	delete page_group_obj;
	delete C;

	delete []wbuf1;
	delete []rbuf1;

	delete []prev_code_page;
	delete []read_code_page;
	delete []next_code_page;
}

/*
TEST(PageReadWriteOverflow, OverLimitWrites_WOM23_DifferentStartOffset)
{
	
//	write 3 times, different set of data.
//	write 4 th time. 
//		1. resultant write should return less number of bits than first write.
//		2. it should return list of pbn offsets that the lbns start from.
	
}

TEST(PageReadWriteOverflow, OverLimitWrites_WOM24_CODE1_DifferentStartOffset)
{

}
*/

// These tests work, uncomment them afterwards.

TEST(BlockReadWrite, Wom23_Repeated) 
{
	int np=53;
	int ps=24;
	int xlc=3;
	int bno=0;
	enum wom_coding womCode=WOM_2_3;
	int i;
	int ret;

	block *b = new block(np, ps, xlc, bno, womCode);
	
	int lbsize = b->getLogicalBlockSize();
	int pbsize = b->getPhysicalBlockSize();
	int lpsize = lbsize / np;	

	uint8_t *blocktowrite1 = new uint8_t[lbsize]();
	uint8_t *blocktoread1 = new uint8_t[lbsize]();
	uint8_t *blocktowrite2 = new uint8_t[lbsize]();
	uint8_t *blocktoread2 = new uint8_t[lbsize]();

//	printf("%s():IN LAST TESTCASE\n", __func__);

	for(i = 0 ; i < lbsize ; i++)
	{
		blocktowrite1[i] = (uint8_t)(i % 255);
		blocktowrite2[i] = (uint8_t)((lbsize - i) % 255);
	}

	b->writeToBlock(blocktowrite1, lbsize);
	b->readFromBlock(blocktoread1, lbsize);

//	printf("Read block\n");
//	for(int i=0; i< lbsize ; i++) {
//		printf("i=%d write=%x read=%x\n", i, blocktowrite1[i], blocktoread1[i]);
//	}

	EXPECT_TRUE( 0 == std::memcmp(blocktowrite1, blocktoread1, lbsize));

	b->resetBlockWithoutErase();

	b->writeToBlock(blocktowrite2, lbsize);
	b->readFromBlock(blocktoread2, lbsize);

//	printf("Read block\n");
//	for(int i=0; i< lbsize ; i++) {
//		printf("i=%d wrote=%x read=%x\n", i, blocktowrite2[i] , blocktoread2[i]);
//	}
	
	EXPECT_TRUE( 0 == std::memcmp(blocktowrite2, blocktoread2, lbsize));
}

TEST(BlockReadWrite, Wom24_Repeated) 
{
	int np=15;
	int ps=90;
	int xlc=4;
	int bno=0;
	enum wom_coding womCode=WOM_2_4_CODE1;
	int i;
	int ret;

	block *b = new block(np, ps, xlc, bno, womCode);
	
	int lbsize = b->getLogicalBlockSize();
	int pbsize = b->getPhysicalBlockSize();
	int lpsize = lbsize / np;	

	uint8_t *blocktowrite1 = new uint8_t[lbsize]();
	uint8_t *blocktoread1 = new uint8_t[lbsize]();
	uint8_t *blocktowrite2 = new uint8_t[lbsize]();
	uint8_t *blocktoread2 = new uint8_t[lbsize]();

	for(i = 0 ; i < lbsize ; i++)
	{
		blocktowrite1[i] = (uint8_t)(i % 255);
		blocktowrite2[i] = (uint8_t)((lbsize - i) % 255);
	}
	
	b->writeToBlock(blocktowrite1, lbsize);
	b->readFromBlock(blocktoread1, lbsize);

//	printf("Read block\n");
//	for(int i=0; i< lbsize ; i++) {
//		printf("i=%d write=%x read=%x\n", i, blocktowrite1[i], blocktoread1[i]);
//	}

	EXPECT_TRUE( 0 == std::memcmp(blocktowrite1, blocktoread1, lbsize));

	b->resetBlockWithoutErase();

	b->writeToBlock(blocktowrite2, lbsize);
	b->readFromBlock(blocktoread2, lbsize);

//	printf("Read block\n");
//	for(int i=0; i< lbsize ; i++) {
//		printf("i=%d wrote=%x read=%x\n", i, blocktowrite2[i] , blocktoread2[i]);
//	}
	
	EXPECT_TRUE( 0 == std::memcmp(blocktowrite2, blocktoread2, lbsize));
}

// TODO: do this when we have invalid state. 
// 1. Markks Cell Invalid, Writes Data to it again.
// 2. Marks Multiple Cells Invalid. Writes To cell and reads back from 
// Cells until the written value is read back.

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
