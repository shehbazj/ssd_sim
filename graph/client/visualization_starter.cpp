#include <iostream>
#include <map>
#include <cstdint>
#include "sockettest.h"

using namespace std;

int main()
{
	int total_size = 10;
	uint8_t *page = new uint8_t[total_size];
	int choice;
	int cell_no;
	int value;

	class MyClient C;

	// TODO create a normal C++ socket here...

	// TODO connect it to localhost:1234. make sure ./socket in graph/socket is running.

	while (1) {
		printf("1. Update Cell\n");
		printf("2. Exit\n");

		scanf("%d", &choice);

		switch (choice) {
			case 1:

				printf("enter cell number (between 0 and %d)\n", total_size- 1);
				scanf("%d", &cell_no);
				if (cell_no >= total_size) {
					printf("invalid value\n");
					break;
				}

				printf("enter a value between 0 and 9\n");
				scanf("%d", &value);
				if(value < 0 || value > 9) {
					printf("invalid value\n");
					break;
				}
				page[cell_no] = value;

				// TODO write the page array to the socket created above.
				break;
			default:	
				exit(0);
		}
	}
	
	delete [] page;
	return 0;
}
