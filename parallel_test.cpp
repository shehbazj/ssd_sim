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

int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
