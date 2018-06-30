

#include<stdio.h>
#include<stdlib.h>
#define NUMBER_OF_BLOCKS 1
#define BLOCK_SIZE 10485760



char* memory_array[NUMBER_OF_BLOCKS];

struct node{
	short start;
	short stop;
	node* next;
};

node* free_list[NUMBER_OF_BLOCKS];

void* allocate_first_node(short i, short actual_size);
node* create_node(short start, short stop);
void* ss_malloc(int size);
short getline(char* pointer);
void add_node1(short a, short b, short c, short d);
void check_for_merge();
void ss_free(void*);



void* allocate_from_this_list(int i, int actual_size){
	node* temp = free_list[i];
	while (temp->next != NULL){
		if (temp->next->stop - temp->next->start + 1 >= actual_size){
			//NUMBER_OF_BLOCKS is exactly equal
			if (temp->next->stop - temp->next->start + 1 == actual_size){
				memory_array[i][temp->next->start] = actual_size - 2;
				temp->next = temp->next->next;
				return (void*)memory_array[i][temp->next->start + 2];
			}
			//space is greater than the actual size

			if (temp->next->stop - temp->next->start + 1 > actual_size){
				//write code to break down the node into two piecces
				int high = temp->next->stop;
				int low = temp->next->start;

				node* temp1 = create_node(low + actual_size, high);

				temp1->next = temp->next->next;
				temp->next = temp1;

				*((short*)(&memory_array[i][low])) = (actual_size - 2);
				return (void*)memory_array[i][low + 2];

			}
		}
		temp = temp->next;
	}


}

void* allocate_first_node(short i, short actual_size){
	if (memory_array[i] == NULL)
	{
		memory_array[i] = (char*)malloc(BLOCK_SIZE);

	}
	if (free_list[i] == NULL){
		free_list[i] = create_node(0, actual_size - 1);

		*((short*)(&memory_array[i][0])) = actual_size - 2;
		return (void*)&memory_array[i][2];
	}

	short high = free_list[i]->stop;
	short low = free_list[i]->start;

	if (high - low + 1 == actual_size){
		//free(free_list[i]);
		free_list[i] = free_list[i]->next;
		//free_list[i] = NULL;

		//put the NUMBER_OF_BLOCKS onto the node

		memory_array[i][0] = actual_size - 2;
		return (void*)&memory_array[i][2];

	}
	if (high - low + 1 > actual_size){
		//free_list[i] = NULL;
		node* temp1 = create_node(low + actual_size, high);
		temp1->next = free_list[i]->next;
		free_list[i] = temp1;
		*((short*)(&memory_array[i][low])) = actual_size - 2;
		return (void*)&memory_array[i][low + 2];
	}
}

void* ss_malloc(int size){
	short actual_size = (short)size + 2;
	//traverse the 12 linkedlist
	//when you get a block which is free and is bigger than size +2, 
	//write the size in the first two block and return the poiter back
	//update the linked list.
	for (short i = 0; i < NUMBER_OF_BLOCKS; i++){

		//write code to traverse the linked list and find the free space


		if (free_list[i] == NULL){
			memory_array[i] = (char*)malloc(BLOCK_SIZE);
			free_list[i] = create_node(0, BLOCK_SIZE -1);
			return allocate_first_node(i, actual_size);
		}

		node* temp = free_list[i];

		//if only one node in the list
		if (temp->stop - temp->start + 1 >= actual_size){
			//change node
			return allocate_first_node(i, actual_size);
		}
		while (temp != NULL && temp->next != NULL){
			//if the space is enough
			if (temp->next->stop - temp->next->start + 1 >= actual_size){
				//alocate this space or else a part of this space
				//not first
				return allocate_from_this_list(i, actual_size);
			}
			temp = temp->next;
		}
	}
	return NULL;
}

node* create_node(short start, short stop){
	node* temp = (node*)malloc(sizeof(node));
	temp->start = start;
	temp->stop = stop;
	temp->next = NULL;
	return temp;
}






void ss_free(void* pointer){
	short size = *((short*)(pointer)-1);
	short actual_size = size + 2;
	short line = getline((char*)pointer);
	short start = (short)(((char*)pointer - memory_array[line]) / sizeof(char));
	short stop = start + size - 1;
	start = start - 2;
	//three different cases for free
	// the cur_start isequal to pre_end -> join pre and current node
	//cur_stop is equal to next_stop -> join cur and next
	//cur start = pre_start && cur_stop = next_start -> merge all the three nodes

	for (int i = start; i <= stop; i++){
		memory_array[line][i] = '0';
	}
	add_node1(size, line, start, stop);

	//check_for_merge();
	//check_for_merge();

}



short getline(char* pointer){
	for (short i = 0; i < 12; i++){
		if ((pointer >= memory_array[i] && pointer < memory_array[i] + NUMBER_OF_BLOCKS) && i != 12){
			return i;
		}
	}
}


void add_node1(short size, short line, short start, short stop){
	node* temp = free_list[line];

	if (temp != NULL && temp->start == stop){
		temp->start = start;
		return;
	}

	if (temp != NULL && temp->start > stop){
		node* temp1 = create_node(start, stop);
		temp1->next = temp;
		free_list[line] = temp1;
		return;
	}

	while (temp->next != NULL){
		//pre_stop is cur_start but not equal to next_start
		if (temp->stop == start && temp->next->start > stop){
			temp->stop = stop;
			return;
		}
		// cur_stop = next_start -> but not equal to the pre
		if (temp->stop < start && temp->next->start == stop){
			temp->next->start = start;
			return;
		}
		//pre and next values are equal-> merge three nodes
		if (temp->stop == start && temp->next->start == stop){
			temp->stop = temp->next->stop;
			temp->next = temp->next->next;
			return;
		}
		if (temp->stop < start && temp->next->start > stop){
			// add node in between
			node* temp1 = create_node(start, stop);
			temp1->next = temp->next;
			temp->next = temp1;
			return;
		}

		temp = temp->next;
	}
}

void ss_debug(short line){
	printf("Debug for line %d\n", line);
	node* temp = free_list[line];
	while (temp != NULL){
		printf("start %d stop %d\n", temp->start, temp->stop);
		temp = temp->next;
	}
}		