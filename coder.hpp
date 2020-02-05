// member codeTable. gets initialized based on coding scheme.
// has members - num data bits, num code bits.

// has accessors - get num_data_bits, get num_code_bits.
// encode(data_buf, code_buf, data_size).
// decode(data_buf, code_buf, data_size).

// once coded WOM is created, create vector <logical_page *>
// and call writeToBlock(vector <logical_page *>

#ifndef CODER_H
#define CODER_H

#include <cstdint>
#include <cstdio>
#include <vector>

using namespace std;

enum wom_coding {
	NO_WOM = 1,
	WOM_2_3,
	WOM_2_4_CODE1,
	WOM_2_4_CODE2,
};

/*
	This variable defines how we want to track invalid cells.
	for WOM_2_3 and WOM_2_4_CODE1 , we track invalid cells using
	an additional bitmap for each cell.
	For WOM_2_4_CODE2, we track invalid cells using a einval state.
*/

enum wom_track_invalid_cells
{
	NO_SPACE = 0,
	ADDITIONAL_SPACE = 1,
	INVALD_BIT = 2
};


/* Note: All Wom code tables are kept of size 4x4 so that
a common wom_table pointer can be assigned to any of the tables
available;
*/

class coder
{

uint8_t no_wom [4][4] = {
				{0x00 ,0x00,0xFF,0xFF },
				{0x01 ,0x01,0xFF,0xFF },
				{0x02 ,0x02,0xFF,0xFF },
				{0x03 ,0x03,0xFF,0xFF }
			};

uint8_t wom_2_3 [4][4] = { 
				{0x00, 0x07,0xFF,0xFF},
				{0x04, 0x03,0xFF,0xFF},
				{0x02, 0x05,0xFF,0xFF},
				{0x01, 0x06,0xFF,0xFF}
			};

uint8_t wom_2_4_code1 [4][4] = { 
				{0x00, 0x08, 0x0F,0xFF},
				{0x04, 0x0C, 0x0B,0xFF},
				{0x02, 0x0A, 0x0D,0xFF},
				{0x01, 0x09, 0x0E,0xFF}
			};

uint8_t wom_2_4_code2 [4][4] = { 
				{0x00, 0x04, 0x07, 0x0B},
				{0x01, 0x05, 0x08, 0x0C},
				{0x02, 0x06, 0x09, 0x0D},
				{0x03, 0x07, 0x0A, 0x0E}
			};

	int data_bits_per_codeword;
	int code_bits_per_codeword;
	int num_generations;
	wom_track_invalid_cells einval_cell_type;

	wom_coding code_type;
	uint8_t (*wom_table)[4];
	uint8_t invalid_value;
	
	public:
	coder (wom_coding code_word_type);
	wom_coding codeType();
	void setCodeType(wom_coding c);
	int encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int data_size);
	int encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int data_size, vector <bool> &valid_bitmap, long unsigned firstCell=0);
	int decode(uint8_t *data_buf, uint8_t *code_buf, int code_buf_size);
	int decode(uint8_t *data_buf, uint8_t *code_buf, int code_buf_size, vector<bool> &valid_bitmap);
	uint8_t getNextCode(uint8_t data_bits, uint8_t previous_code_bits);
	uint8_t getDataBits(uint8_t code_word);
	uint8_t getGeneration(uint8_t code_word);
	int getNumCodeBits();
	int getNumDataBits();
	bool getValidity(bool valid_bit, uint8_t previous_cell_type);
};

#endif
