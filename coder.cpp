#include <cassert>
#include "coder.hpp"

coder :: coder (wom_coding code_word_type)
{
	code_type = code_word_type;
	switch (code_type) {
		case NO_WOM:
			data_bits_per_codeword = 2;
			code_bits_per_codeword = 2;
			wom_table = no_wom;
			num_generations = 1;
			einval_cell_type = NO_SPACE;
			invalid_value = 0xFF;
			break;
		case WOM_2_3:
			data_bits_per_codeword = 2;
			code_bits_per_codeword = 3;
			wom_table = wom_2_3;
			num_generations = 3;
			einval_cell_type = ADDITIONAL_SPACE;
			invalid_value = 0xFF;
			break;
		case WOM_2_4_CODE1:
			data_bits_per_codeword = 2;
			code_bits_per_codeword = 4;
			wom_table = wom_2_4_code1;
			num_generations = 3;
			einval_cell_type = ADDITIONAL_SPACE;
			invalid_value = 0xFF;
			break;
		case WOM_2_4_CODE2:
			data_bits_per_codeword = 2;
			code_bits_per_codeword = 4;
			wom_table = wom_2_4_code2;
			num_generations = 4;
			einval_cell_type = INVALD_BIT;
			invalid_value = 0x0F;
			break;
		default:
			data_bits_per_codeword = 0;
			code_bits_per_codeword = 0;
			wom_table = NULL;
			num_generations = 0;
			einval_cell_type = NO_SPACE;
			invalid_value = 0xFF;
	}
}

uint8_t coder:: getNextCode(uint8_t data_bits, uint8_t previous_code_bits)
{
	int columns = 4;
	int rows    = 4;
	int i, j;

	if(data_bits > 0x03) {
		printf("data bits = %d greater than 0x11\n", data_bits);
		return 0xFF;
	}
	
	if(previous_code_bits > 0x0F) {
		printf("previous_code_bits = %d greater than 0x0F\n", previous_code_bits);
		return 0xFF;
	}

	// return first column value.
	if(previous_code_bits == 0x00) {
		return wom_table[data_bits][0];
	}

	// scan current row and check if any of the codes are same as previously existing code.
	// if yes, return previous_code_bits.
	for(uint8_t col = 0 ; col < columns ; col++)
	if (wom_table[data_bits][col] == previous_code_bits) {
		return previous_code_bits;
	}

	for (i = 0 ; i < rows ; i++ ) {
		for (j = 0 ; j < columns - 1; j++) {
			if(wom_table[i][j] == previous_code_bits) {
				if(data_bits == i) {
					return wom_table[data_bits][j];
				}else{
					return wom_table[data_bits][j+1]; 
				}
			}								
		}
	}
	// if input data is same as that for previous generation code, return previously written code.
	if(wom_table[data_bits][columns -1] == previous_code_bits)
		return previous_code_bits;

	return 0xFF;
}

uint8_t coder :: getDataBits(uint8_t code_word)
{
	int columns = num_generations;
	int rows    = 4;
	int i, j;

	if(code_word > 0x0F) {
		return 0xFF;
	}

	for (i = 0 ; i < rows ; i++)
	{
		for(j = 0; j < columns ; j++)
		{
			if(wom_table[i][j] == code_word)
			{
//				printf("wom table i %d j %d =%d\n", i, j, wom_table[i][j]);
				return i;
			}	
		}
	}
	return 0xFF;
}

bool coder:: getValidity(bool valid_bit, uint8_t previous_cell_type)
{
	if (einval_cell_type == ADDITIONAL_SPACE) {
		return valid_bit;
	}else if(einval_cell_type == INVALD_BIT) {
		return previous_cell_type == invalid_value;
	}
}

/* returns the code buf size that was generated */
// data_size is the size of the data_buffer i.e. the number of bytes of non-encoded data that needs to be written.
// code_size is computed from data_size.
// valid_code_bitmap is a bitmap of size code_buf_size * 8 / C->getNumCodeBits().

int coder:: encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int data_size, 
		vector <bool> &valid_code_bitmap, long unsigned firstCell)
{
	int i, num_bits = data_size * 8;
	uint8_t data_byte;
	uint32_t data_byte_index;
	uint8_t data_bit_index;
	uint8_t data_bits;
	uint8_t code_bits, previous_code_bits;
	uint32_t code_size;

	unsigned code_bit_index=0, previous_code_bit_index=0, invalid_code_bits=0;
	unsigned code_byte_index=0, previous_code_byte_index=0;
	uint8_t previous_code_bit, code_bit;
	uint8_t code_byte=0, previous_code_byte = 0;
	uint8_t j=0;

	// return null if any of the buffers are not initialized.
	if(data_buf == NULL || previous_code_buf == NULL || code_buf == NULL)  {
		return -1;
	}

	code_size = data_size * code_bits_per_codeword / data_bits_per_codeword;

	// make sure caller has sent empty code buffer.
	for(i = 0 ; i < code_size ; i++)
	{
		if(code_buf[i] != 0x00) {
			printf("caller needs to initialize codebuf to 0x00 at offset %d for codesize %d\n", i, code_size);
			assert(0);
		}
	}

	// if firstCell to be programmed has been specified
	// copy all bits from previous_code_buf
	// to code_buf from 0 to firstCell.
	if(firstCell != 0) {
		// byte level copy
		for (i = 0 ; i < (firstCell * getNumCodeBits()) / 8 ; i++ )
		{
			code_buf[i] = previous_code_buf[i];
		}
		code_bit_index = firstCell * getNumCodeBits();
		previous_code_bit_index = firstCell * getNumCodeBits();
	}

	// this loop iterates through different data bits. it will recreate data bits
	// if current bits were not programmed.
	for(i = (firstCell * data_bits_per_codeword) ; i < num_bits ; i+= data_bits_per_codeword) {
		// if we have reached the last code bit, break
		if(code_bit_index == code_size * 8) {
			break;
		}
		data_byte_index = i / 8;
		data_bit_index  = i % 8;

		data_byte = data_buf[data_byte_index];
		data_bits = ((data_byte >> (6 - data_bit_index)) &  0x03);
		assert(data_bits <= 0x03);

		previous_code_bits = 0;
		for (j = 0 ; j < code_bits_per_codeword ; j++)
		{
			previous_code_byte_index = previous_code_bit_index / 8;
			previous_code_byte = previous_code_buf[previous_code_byte_index];
			if(previous_code_byte_index >= code_size) {
				printf("previous_code_byte_index = %d previous_code_bit_index %d\
					code_size = %d code_bit_index = %d\n",
					previous_code_byte_index, previous_code_bit_index, code_size, code_bit_index);
				assert(previous_code_byte_index < code_size);
			}
			previous_code_bit = ((previous_code_byte >> (7 - previous_code_bit_index % 8)) & 0x01);
			previous_code_bits = previous_code_bits | (previous_code_bit << (code_bits_per_codeword - j - 1));
			previous_code_bit_index++;
		}
		assert(previous_code_bits <= 0x0F);
		code_bits = getNextCode(data_bits, previous_code_bits);
		if (valid_code_bitmap[code_bit_index / code_bits_per_codeword])  {
			if(code_bits == 0xFF) {
				valid_code_bitmap[code_bit_index/ code_bits_per_codeword]=false;
				code_bits = previous_code_bits;
				invalid_code_bits+= code_bits_per_codeword;
				i-=data_bits_per_codeword;
			}
			for (j = 0 ; j < code_bits_per_codeword ; j++)
			{
				code_byte_index = code_bit_index  / 8;
				code_byte = code_buf[code_byte_index];
				code_bit = (code_bits >> code_bits_per_codeword - j - 1) & 0x01;
				code_byte = code_byte | (code_bit << (7 - (code_bit_index % 8)));
				code_bit_index++;
				code_buf[code_byte_index]=code_byte;
			}
		} // valid cell bitmap is False. skip the current code. retain old data index.
		else {
			code_bits = previous_code_bits;
			for (j = 0 ; j < code_bits_per_codeword ; j++)
			{
				code_byte_index = code_bit_index  / 8;
				code_byte = code_buf[code_byte_index];
				code_bit = (code_bits >> code_bits_per_codeword - j - 1) & 0x01;
				code_byte = code_byte | (code_bit << (7 - (code_bit_index % 8)));
				code_bit_index++;
				code_buf[code_byte_index]=code_byte;
			}
			i-=data_bits_per_codeword;
			invalid_code_bits += code_bits_per_codeword;
		}
	}
	return (code_bit_index - invalid_code_bits - (firstCell * getNumCodeBits()));
}

int coder:: encode(uint8_t *data_buf, uint8_t *previous_code_buf, uint8_t *code_buf, int data_size)
{
	int code_size = data_size * code_bits_per_codeword / data_bits_per_codeword;
	int num_code_bits = code_size * 8 / code_bits_per_codeword;
	vector<bool> valid_code_bitmap;
	int i;

	for(i = 0 ; i < num_code_bits ; i++) {
		valid_code_bitmap.push_back(true);
	}
	return encode(data_buf, previous_code_buf, code_buf, data_size, valid_code_bitmap);
}

/* returns size of the data (number of bits) retrieved from the codeword
valid_code_bitmap is a bitmap of cells. Its size = code_buf_size * 8 / C->getNumCodeBits() .

Note that in case of part overflow coded page and part original coded page write, we will decode the entire page.
The caller needs to read/compare bits that were decoded.

 */
int coder:: decode(uint8_t *data_buf, uint8_t *code_buf, int code_buf_size, vector<bool> &valid_code_bitmap)
{
	long unsigned code_buf_bit_index;
	long unsigned code_buf_byte_index;
	uint8_t code_buf_byte;
	uint8_t code_word=0;
	uint8_t code_word_bit=0;
	uint8_t left_shift_offset=0;
	uint8_t data_bits;
	long unsigned data_buf_byte_index, data_buf_bit_index=0;
	uint8_t data_byte;
	int dbuf_size;
	uint8_t msb_bit, lsb_bit;
	uint8_t msb_left_shifted;


	if(data_buf == NULL || code_buf == NULL)
		return -1;

	for (code_buf_bit_index = 0 ; code_buf_bit_index < (code_buf_size * 8) ; code_buf_bit_index++)
	{
		code_buf_byte_index = code_buf_bit_index / 8;
		code_buf_byte = code_buf[code_buf_byte_index];

		code_word_bit = (code_buf_byte >> (7 - (code_buf_bit_index % 8))) & 0x01;
		left_shift_offset = (code_bits_per_codeword - (code_buf_bit_index % code_bits_per_codeword) - 1);
		assert(left_shift_offset < code_bits_per_codeword);

		code_word = code_word | (code_word_bit << left_shift_offset);
		assert(code_word < (1 << (code_bits_per_codeword)));
		if(code_buf_bit_index % code_bits_per_codeword == code_bits_per_codeword -1) {
			// if valid bitmap is false, do not process this code further.
			// keep the data bits as is.
			if(valid_code_bitmap[code_buf_bit_index / code_bits_per_codeword] == false) {
				code_word=0x00;
				continue;
			}
			// decode codebyte.
			data_bits = getDataBits(code_word);
			assert(data_bits <= 0x03);
			// extract MSB from the data Bits
			data_buf_byte_index = data_buf_bit_index / 8;
			data_byte = data_buf[data_buf_byte_index];
			msb_bit =  ((data_bits & 0x02) >> 1);
			assert(msb_bit <= 0x01);
			msb_left_shifted= msb_bit << (7 - (data_buf_bit_index % 8));
			data_byte = data_byte | msb_left_shifted;
			data_buf[data_buf_byte_index] = data_byte;
			data_buf_bit_index++;

			// extract LSB from the data Bits
			data_buf_byte_index = data_buf_bit_index / 8;
			data_byte = data_buf[data_buf_byte_index];
			lsb_bit = data_bits & 0x01;
			data_byte = data_byte | (lsb_bit << (7 - (data_buf_bit_index % 8)) );
			// store final data_byte back in data_buf
			data_buf[data_buf_byte_index]=data_byte;
			data_buf_bit_index++;
			code_word=0x00;
		}
	}
	return (data_buf_bit_index);
}

int coder:: decode(uint8_t *data_buf, uint8_t *code_buf, int code_buf_size)
{
	int i;
	int num_code_bits = code_buf_size * 8 / code_bits_per_codeword;
	vector <bool> valid_code_bitmap(num_code_bits, true);

	for(i = 0 ; i < num_code_bits; i++)
	{
		valid_code_bitmap[i]=true;
	}
	return decode(data_buf, code_buf, code_buf_size, valid_code_bitmap);
}

int coder:: getNumCodeBits()
{
	return code_bits_per_codeword;
}

int coder:: getNumDataBits()
{
	return data_bits_per_codeword;
}

uint8_t coder:: getGeneration(uint8_t code_word)
{
	int i, j;
	if(code_word == 0x00)
		return 0;
	for (i = 0 ; i < 4 ; i++) {
		for ( j = 0 ; j < 4 ; j++) {
			if (code_word == wom_table[i][j]) {
				return j + 1;
			}
		}
	}
}
