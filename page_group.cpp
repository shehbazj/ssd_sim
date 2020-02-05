#include "page_group.hpp"

using namespace std;

bool page_group :: isCurrentCellValid()
{
	if(pm.isPageValid() && (readFromCell() != pm.getInvalidState())){
		return true;
	}
	return false;
}

// returns number of bytes written, or -1 if write did not occur successfully.
// eg buf size is 4096 for a TLC flash. 4096 % 3 = 1!
// TLC Flash requires buf size to be (4096 * 3).

int page_group :: write_page(uint8_t *buf, int buf_size)
{
	int bit_index_of_buffer = 0;
	int byte_index_of_buffer = 0;
	uint8_t byte_to_write = 0;
	uint8_t bit_to_write = 0;
	uint8_t bit_offset=0;
	int data_byte_offset=0;
	int data_written=0;

	if(!isPageEmpty()) {
		printf("%s():page not empty\n", __func__);
		return -1;
	}

	if(!isPageValid()) {
		printf("%s():page not valid\n", __func__);
		return -1;
	}

	if(buf == NULL) {
		cout << __func__ << "():buf not initialized by caller" << endl;
		return -1;
	}

	if(buf_size % pm.getCellType() != 0) {
		cout << __func__ << "():non cell aligned write bufsize = " << buf_size << "cell type " << pm.getCellType() << " exiting" << endl;
		return -1;
	}

	if(buf_size != pm.getPageSize() * pm.getCellType()) {
		cout << __func__ << "():buf size " << buf_size << " pg size * cell type " <<  pm.getPageSize() * pm.getCellType() << " should match " << endl;
		return -1;
	}

	// create data to be written of cell size from bits.
	int data_bit_offset=0;
	for(data_bit_offset = 0 ; data_bit_offset < pm.getPageSize() * 8 * pm.getCellType(); )
	{
		uint8_t data = 0;
		for (int i = 0 ; i < pm.getCellType() ; i++) {
			data_byte_offset = data_bit_offset / 8;
			bit_offset = data_bit_offset % 8;
			data = data | ( ((buf[data_byte_offset] >> (7-bit_offset)) & 0x01) << (pm.getCellType()-i-1));
			data_bit_offset++;
		}
		if(isCurrentCellValid()) {
			// write cell sized data to the cell.
			data_written = writeToCell(data);
			assert(data_written == data);
			if(data_written == -1) {
				cout << "Tried writing to cell " << pm.getCurrentCell()
					<< " data " << data << " data written "
					<< data_written << endl;
				return data_written;
			}
		}else {
			//printf("%s():current cell invalid\n", __func__);
			if(moveToNextValidCell() == -1) {
				cout << "Could not find next valid Cell " << endl;
				return bit_offset;
			}
		}
	}
	assert(data_bit_offset == pm.getPageSize() * 8 * pm.getCellType());
	return (data_bit_offset + 1)/8;
}

/* buf needs to be initialized by the caller. 
	return -1 if read is unsuccessful 
	a) buf is not allocated.
	b) buf_size is not cell aligned.
	return number of bytes read if read is successful.
*/

int page_group:: read_page(uint8_t *buf, int buf_size)
{
	uint8_t cell_data;
	int bit_index_of_buffer = 0;
	int byte_index_of_buffer = 0;
	uint8_t byte_to_write = 0;
	uint8_t bit_to_write = 0;
	int num_cells;
	int i;
	uint8_t cellbits[pm.getCellType()];

	num_cells = (buf_size * 8) / pm.getCellType();

	if (buf == NULL) {
		cout << "buffer is null " << endl;
		return -1;
	}
	/*
	if(isPageEmpty()) {
		cout << "page is empty " << endl;
		return 0;
	}
	*/
	if(isPageValid() == false)
	{
		return -1;
	}
	if(buf_size % pm.getCellType() != 0) {
		cout << __func__ << "():non cell aligned read, exiting" << endl;
		return -1;
	}
	if(buf_size != pm.getPageSize() * pm.getCellType()) {
		return -1;
	}
	// reads happening at cell grannularity.
	for(i = 0 ; i < num_cells ; i++)
	{
		cell_data = readFromCell(i);
		assert(cell_data != -1);
		if(cell_data > (1<< pm.getCellType()) ) {
			printf("%s(): cell data %d max data %d\n", __func__, cell_data, (1 << (pm.getCellType() -1)));
			assert(cell_data < (1<< pm.getCellType()));
		}
		for (int j = 0; j < pm.getCellType(); j++) {
			cellbits[j] = cell_data >> (pm.getCellType() - j - 1) & 0x01;
		}
		// write to buffer at bit grannularity.
		bit_to_write=0;
		for (int j = 0; j < pm.getCellType(); j++) {
			byte_to_write = buf[byte_index_of_buffer];
			bit_to_write  = (cellbits[j] << (7 - (bit_index_of_buffer % 8)) );
			byte_to_write |=  bit_to_write;
			buf[byte_index_of_buffer]=byte_to_write;
			bit_index_of_buffer++;
			byte_index_of_buffer = bit_index_of_buffer / 8;
		}
	}
	return (i * pm.getCellType()/ 8);
}

/* constructor for page_group. page group initializes a group of pages that can be programmed together. 
	ps  = pageSize in bytes.
	xlc = number of cells in each page. 
*/

page_group:: page_group(int ps, int xlc, enum wom_coding w = NO_WOM) : pm (ps, xlc, w)
{
	data = new uint8_t[pm.getPageSize() * xlc]();
}

void page_group :: erase_page()
{
	for (uint32_t i = 0 ; i < pm.getPageSize() * pm.getCellType() ; i++)	{
		data[i] = 0x00;
	}
	pm.setPageValidity(true);
	pm.setValidCells(0);
	pm.setCurrentCell(0);
}

page_group:: ~page_group()
{
	delete [] data;
}

// writes to current_cell. increments current_cell by 1
// data is xlc bit data that needs to be written to the page group.
// caller must invoke isPageFull() before calling writeToCell().
// ERROR handling needs to be done beforehand.

// if a data larger than max value in cell is attempted to be written, we written -1.
// TODO: reserve some bits to represent invalid states, and make cell invalid once
// a data more than the maximum value is being written.

int page_group :: writeToCell(uint8_t bits_to_write)
{
	// note bits to write is a 4 bit data for QLC and 3 bit data for TLC.
	int byte_offset_to_write;
	int bit_offset_to_write;
	uint8_t dataReadFromCell;
	uint8_t max_data_write = 1 << pm.getCellType();
	
	if(pm.isPageFull())
	{
		cout << "page is full " << endl;
		return -1;
	}
	
	if (bits_to_write >= max_data_write){
		cout << "bits to write " << bits_to_write << " are greater than max write " << max_data_write << endl;
		return -1;
	}

	dataReadFromCell = readFromCell(pm.getCurrentCell());
	if(!pm.isValidStateToWrite(dataReadFromCell)) {
		cout << "Cell " << pm.getCurrentCell() << " does not have valid data to re-write to " << dataReadFromCell << endl;
		return -1;
	}

	byte_offset_to_write = pm.getCurrentCell() / 8;
	bit_offset_to_write  = pm.getCurrentCell() % 8;

	for (int i = 1 ; i <= pm.getCellType() ; i++) {
		uint8_t byte_to_update = data[byte_offset_to_write + (pm.getPageSize() * (i - 1))];
		uint8_t bit_value = (bits_to_write >> (pm.getCellType() -i)) & 0x01;
		if(bit_value == 1) {
			byte_to_update |=  (1 << (7-bit_offset_to_write));
		}else {
			byte_to_update &= ~(1 << (7-bit_offset_to_write));
		}
		data[byte_offset_to_write + (pm.getPageSize()) * (i -1)] = byte_to_update;
	}

	dataReadFromCell = readFromCell(pm.getCurrentCell());
	pm.setCurrentCell(pm.getCurrentCell() + 1);
	assert(dataReadFromCell != -1);
	assert(dataReadFromCell == bits_to_write);
	return dataReadFromCell;
}

uint32_t page_group :: moveToNextValidCell()
{
	if(pm.isPageFull()) {
		return -1;
	}
	while(readFromCell() == pm.getInvalidState()){
		pm.decValidCells();
		pm.setCurrentCell(pm.getCurrentCell() + 1);
		if(pm.isPageFull()) {
			return -1;
		}
	}
	return pm.getCurrentCell();
}

int page_group :: readFromCell()
{
	return readFromCell_internal(pm.getCurrentCell());
}

int page_group :: readFromCell(uint32_t cell)
{
	return readFromCell_internal(cell);
}

int page_group :: readFromCell_internal(uint32_t cell_no)
{
	int byte_offset_to_read = cell_no / 8;
	int bit_offset_to_read  = cell_no % 8;
	uint8_t result=0;
	uint8_t bit_value, byte_to_read;

	assert((cell_no / 8) < pm.getPageSize());

	for ( uint8_t i = 1 ; i <= pm.getCellType() ; i++) {
		byte_to_read = data[byte_offset_to_read + (pm.getPageSize() * (i-1))];
		bit_value = ((byte_to_read >> (7 -bit_offset_to_read)) & 0x01);
		result |= (bit_value << (pm.getCellType() - i));
	}
	assert(result < (1 << pm.getCellType()) );
	return result;
}

void page_group :: printCell(uint32_t cell_position)
{
	printf("%x ",readFromCell_internal(cell_position));
}

bool page_group :: isPageValid()
{
	return pm.isPageValid();
}

bool page_group :: isPageEmpty()
{
	return (pm.getCurrentCell() == 0);
}

void page_group :: resetPageWithoutErase()
{
	pm.setCurrentCell(0);
}

void page_group:: setPageValidStatus(bool b)
{
	pm.setPageValidity(b);
}

uint32_t page_group:: getPageSize()
{
	return pm.getPageSize();
}

uint8_t page_group:: getCellType()
{
	return pm.getCellType();
}

enum wom_coding page_group:: getWomCode()
{
	return pm.getWomCode();
}

void page_group:: getGenerations(long unsigned *generation_count, enum wom_coding w)
{
	int pgsize = pm.getPageSize() * pm.getCellType();
	int byte_no, bit_no, code_bits, bit_off;
	int bit_offset;
	int bit;
	int i;
	int gen;
	uint8_t *readBuf = data;

	coder C(w);
	for (i = 0 ; i < 5 ; i++ ) {
		assert(generation_count[i] == 0);
	}

	for(bit_no = 0 ; bit_no < pm.getPageSize() * 8 ; bit_no++)
	{
		code_bits = 0;
		for(i = 0 ; i < C.getNumCodeBits() ; i++) {
			byte_no = (bit_no / 8) + (i * pm.getPageSize());
			bit_off = bit_no % 8;
			bit = readBuf[byte_no] >> (7-bit_off) & 0x01;
			code_bits = code_bits | (bit << (C.getNumCodeBits() - 1 - i));
		}
		gen = C.getGeneration(code_bits);
		assert(gen <= 4);
		generation_count[gen]++;
	}
}
