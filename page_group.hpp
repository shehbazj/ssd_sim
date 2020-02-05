#ifndef PAGE_GROUP_H
#define PAGE_GROUP_H

#include <stdint.h>
#include <iostream>
#include <assert.h>
#include "page_md.hpp"
#include "coder.hpp"

class page_group {
	private:
		uint8_t *data;
		page_metadata pm;
		int readFromCell_internal(uint32_t cellNo);
	public:
		page_group(int page_size, int xlc, enum wom_coding w);
		~page_group();
		bool isCurrentCellValid();
		bool isPageValid();
		void setPageValidStatus(bool status);
		bool isPageEmpty();
		int  writeToCell(uint8_t data_to_write);
		int readFromCell();
		int readFromCell(uint32_t cell);
		uint32_t moveToNextValidCell();
		void printCell(uint32_t cell_position);
		int write_page(uint8_t *buf, int buf_size);
		int read_page(uint8_t *buf, int buf_size);
		void resetPageWithoutErase();
		void erase_page();
		uint32_t getPageSize();
		uint8_t getCellType();
		enum wom_coding getWomCode();
		void getGenerations(long unsigned *generation_count, enum wom_coding);
};

#endif
