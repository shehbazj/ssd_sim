#ifndef PAGE_MD_H
#define PAGE_MD_H

#include <stdint.h>
#include <stdio.h>
#include "coder.hpp"

class page_metadata {
	private:
		bool is_page_valid;
		int page_size;
		int num_valid_cells;
		uint8_t num_writes_to_page;
		int cell_type; 	// 2/3/4
		uint32_t current_cell; 	// value will range from 0 - (8 * page_size)
		int invalid_state;	// currently hardcoded to 0xFF TODO Change for WOM EC.
		enum wom_coding womCode;
	public:
		page_metadata(int page_size, int level, enum wom_coding);
		bool isPageFull();
		void setPageValidity (bool status);
		int getValidCells();
		int decValidCells();
		int getPageSize();
		int getCellType();
		bool isPageValid();
		void setValidCells(uint32_t);
		uint32_t getCurrentCell();
		uint8_t getInvalidState();
		bool isValidStateToWrite(uint8_t cellstate);
		void setCurrentCell(uint32_t cell_position);
		enum wom_coding getWomCode();
};

#endif
