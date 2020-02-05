#include "page_md.hpp"

uint8_t page_metadata:: getInvalidState()
{
	return invalid_state;
}

// TODO change this for WOM EC writes.
bool page_metadata:: isValidStateToWrite(uint8_t cellstate)
{
	if (cellstate == invalid_state) {
		return false;
	}
	return true;
}

uint32_t page_metadata :: getCurrentCell()
{
	return current_cell;
}

void page_metadata :: setCurrentCell(uint32_t cell_position)
{
	current_cell = cell_position;
}

int page_metadata :: getPageSize()
{
	return page_size;
}

int page_metadata :: getCellType()
{
	return cell_type;
}

page_metadata :: page_metadata(int ps, int xlc, enum wom_coding wc=NO_WOM)
{
	is_page_valid = true;
	page_size = ps;
	num_valid_cells = page_size;
	num_writes_to_page = 0;
	cell_type = xlc;
	current_cell = 0;
	invalid_state = 0xFF;
	womCode = wc;
}

enum wom_coding page_metadata :: getWomCode()
{
	return womCode;
}

void page_metadata:: setPageValidity (bool status)
{
	is_page_valid = status;
}

int page_metadata:: getValidCells()
{
	return num_valid_cells;	
}

void page_metadata:: setValidCells(uint32_t num_cells)
{
	num_valid_cells= num_cells;	
}

int page_metadata:: decValidCells()
{
	num_valid_cells--;
	if (num_valid_cells == 0) {	// TODO change this to max_invalid_cells check.
		is_page_valid = false;
	}
}

// current_cell reaches N.
bool page_metadata :: isPageFull()
{
	if (current_cell == (page_size * 8))
		return true;
	return false;
}

bool page_metadata :: isPageValid()
{
	return is_page_valid;
}

