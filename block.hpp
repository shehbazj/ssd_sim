#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <memory>
#include "coder.hpp"
#include "page_group.hpp"
#include <chrono>

using namespace std;

class coder;

enum blockStatus {
	EMPTY=1,
	FULL,
	PARTIAL_FULL,
	MARKED_FOR_GC
};

enum blockOverflowConfig {
	LINEAR=1,
	SKIPPED,
	DEDICATED
};

class logical_page {
	private:
		uint8_t *buf;
		uint32_t byte_size;
	public:
		logical_page(uint32_t byte_size);
		uint8_t getLogicalPageSize();
		uint8_t* getBuf();
		void setBuf(uint8_t *buf, int buf_size);
		~logical_page();
};

class block {
	private:
		vector <page_group *> page_array;
		enum blockStatus bstatus;
		int num_valid_pages;
		int num_pages;
		int block_number;
		enum blockOverflowConfig boc;
		coder *C;
	public:
		block(int np, int ps, int xlc, long block_number, enum wom_coding womCode, enum blockOverflowConfig=LINEAR);

		enum blockStatus getBlockStatus();
		void setBlockStatus(enum blockStatus);

		int getNumValidPages();
		int setNumValidPages(int num_pages);

		int getNumPages();
		int setNumPages(int num_pages);

		// returns block size in bytes.
		long unsigned getPhysicalBlockSize();
		long unsigned getLogicalBlockSize();

		int getCodingScheme();
		int getWomCode();

		// while erasing a block, get All Valid page numbers
		vector <long unsigned> getAllValidWrittenPageNumbers();
		// read contents of a logical page at a time.

		// l needs to be initialized by the caller.
		int readPage(int pbn, logical_page *l);
		int writePage(int pbn, logical_page *l);
		int invalidatePage(int pbn);

		int writeToBlock(vector < logical_page * > logical_pages_to_write, vector <unsigned long>ppn);
		int readFromBlock(vector <unsigned long> ppns, vector <logical_page *> lplist);
		void setBlockNumber(long i);

		int writeToBlock(uint8_t *buf, int data_size);
		int readFromBlock(uint8_t *buf, int data_size);
		int readRawBlock(uint8_t *buf);
		void resetBlockWithoutErase();

		void getGenerations(int *);
		void eraseBlock();
		~block();
};

#endif
