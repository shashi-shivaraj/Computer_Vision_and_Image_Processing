/*Header file inclusions*/
#include "huffman_header.h" 

/*Function to implement Huffman encoding*/
unsigned char* huffman_encode(unsigned char *original_data,unsigned int file_size,unsigned int &encoded_data_size)
{
	unsigned char *encoded_data = NULL;
	unsigned int  *freq_count = NULL;
	node_data *head = NULL ,*tail = NULL,*curr = NULL;
	node_data *symbol_list_head = NULL,*symbol_list_tail = NULL;
	node_data **node_pointers = NULL;
	unsigned char cur_byte = 0;
	unsigned char symbol_count = 0;
	int ret = 0,i = 0,j = 0;
	unsigned int header_size = 0;
	unsigned int total_bit_count = 0;
	
#ifdef DEBUG_MODE
	FILE *encode_fpt = NULL;
#endif /*DEBUG_MODE*/

	freq_count = (unsigned int *)calloc(256,sizeof(unsigned int));/*Allocating a counter for each possible byte (0-255)*/
	if(!freq_count)
	{
		printf("memeory allocation failed\n");/*calloc operation failed*/
		return NULL; /*return*/
	}

	node_pointers = (node_data **)calloc(256,sizeof(node_data*));
	if(!node_pointers)
	{
		printf("memeory allocation failed\n");/*calloc operation failed*/
		return NULL; /*return*/
	}

	/*encode the data using Huffman compression algo*/

	/*Parse the data to calculate the freq/occurance of each byte*/
	/*Constuct a queue simultanoeusly*/
	for(i=0;i<file_size;i++)
	{
		cur_byte = original_data[i];
		if(!freq_count[cur_byte])
		{
			/*create a node for the symbol*/
			curr = create_node(&head,curr,cur_byte,1,NULL,NULL,1);
			if(!curr)
			{
				printf("create_node failed\n");
				return NULL;
			}
			node_pointers[cur_byte] = curr;
			freq_count[cur_byte]++;
			symbol_count ++;
		}
		else
		{
			node_pointers[cur_byte]->freq =  node_pointers[cur_byte]->freq + 1; /*increment the counter*/
			freq_count[cur_byte]++;
		}
	}

	/*256 possible symbols;
	unsigned char range[0-255];
	0=symbol_count 1 ....255=symbol_count 256*/
	symbol_count = symbol_count-1;

	/*store the tail pointer*/
	tail = curr;

	/*sort the nodes with frequencies in ascending order-
	-ties keep in original symbol order*/
	sort_list(&head,&tail);

	symbol_list_head = head; /*pointer to head of the sorted list*/
	symbol_list_tail = tail; /*pointer to the tail of the sorted link list*/

	/*Build a Binary Tree based on the sorted link list*/
	CreateBinaryTree(&head,&tail); /*head & tail will now points to the root node*/

	/*Traverse the tree and update the codes for each symbol/leaf*/
	TraverseBinaryTree(symbol_list_head,head->freq);

	/*possible encoded file size considering header + negatoive compression*/
	/*one byte for symbolcount + symbolcount*5 for symbol_freq pairs +
	4 bytes for totalbits used + filesize*2 for possible negative compression+*/
	encoded_data_size = 1+(symbol_count+1)*5+4+file_size*2; 

	/*Allocate memory to store encoded data*/
	encoded_data = (unsigned char*)calloc(encoded_data_size,sizeof(unsigned char)); /*Allocation size twice to handle negative compression if any*/
	if(!encoded_data)/*Error handling*/
	{
		printf("memory allocation failed\n");/*calloc operation failed*/
		return NULL;		/*return*/
	}
	encoded_data_size = 0; /*will contain actual enoced file size */

	/*Create the header info of the encoded file*/
	CreateHeaderInfo(encoded_data,symbol_list_head,&symbol_count,&header_size);

	/*Encode the symbols in the file with its corresponding bit sequences*/
	EncodeData(encoded_data+header_size+sizeof(total_bit_count),original_data,file_size,
		symbol_list_head,node_pointers,
		&encoded_data_size,&total_bit_count);

	/*copy the header*/
	memcpy(encoded_data+header_size,(void*)&total_bit_count,sizeof(total_bit_count));
	/*update the size*/
	encoded_data_size = encoded_data_size+header_size+4;
	
#ifdef DEBUG_MODE
	printf("[huff] encoded_data_size = %d\n",encoded_data_size);
#endif /*DEBUG_MODE*/

	/*free the heap allocations*/
	if(freq_count)
	{
		free(freq_count);
		freq_count = NULL;
	}

	if(node_pointers)
	{
		free(node_pointers);
		node_pointers = NULL;
	}

	/*delete the entire tree*/
	Destroy_Tree(head);
	
#ifdef DEBUG_MODE
	printf("\nHuffman encoded file size = %d bytes\n\n",encoded_data_size+header_size+4);
	
	/*write the encoded data to a file*/
	encode_fpt = fopen("secret_compressed.dump","wb");
	if(!encode_fpt)
	{
		printf("fopen failed for %s",encoded_file);
		return NULL;
	}
	
	/*Write encoded data  to file*/
	ret = fwrite(encoded_data,1,encoded_data+header_size+sizeof(total_bit_count),encode_fpt);
	if(ret != encoded_data_size)
	{
		printf("fwrite failed with %d\n",ret);
	}
	
	if(encode_fpt)
	{
		fclose(encode_fpt);
		encode_fpt = NULL;
	}	
#endif /*DEBUG_MODE*/

	return encoded_data;
}

/*Function to implement Huffman decoding*/
unsigned char * huffman_decode(unsigned char *original_data,
								unsigned int file_size,unsigned int &decoded_data_size)
{
	unsigned char *decoded_data = NULL;	
	node_data *head = NULL,*tail = NULL,*curr = NULL;
	node_data *symbol_list_head = NULL,*symbol_list_tail = NULL;
	unsigned int bits2decode = 0,byteindex = 0,uncompress_size = 0;
	int ret = 0,i = 0;
	unsigned char cur_byte = 0;

	/*parse the header*/
	Parse_Header(original_data,&head,&tail,&bits2decode,&byteindex,&uncompress_size);

	/*Memory Allocations*/
	decoded_data = (unsigned char*)calloc(uncompress_size,sizeof(unsigned char));
	if(!decoded_data)/*Error handling*/
	{
		printf("memory allocation failed\n");/*calloc operation failed*/
		return NULL;		/*return*/
	}

	/*Note : List already sorted (stored in the header)*/
	/*sort the list*/
	/*sort_list(&head,&tail);*/

	symbol_list_head = head; /*pointer to head of the sorted list*/
	symbol_list_tail = tail; /*pointer to the tail of the sorted link list*/

	/*Build a Binary Tree based on the sorted link list*/
	CreateBinaryTree(&head,&tail); /*head & tail will now points to the root node*/ 

	/*Decode the encoded data by finding the pattern symbol in the binary tree*/
	DecodeData(head,original_data,decoded_data,
			   bits2decode,byteindex,
			   file_size,&decoded_data_size);

#ifdef DEBUG_MODE

	printf("\nHuffman Decoded data size %d bytes\n",decoded_data_size);
	char decoded_file[MAX_FILENAME] = {0};	/*character array to store the filename*/
	strcpy(decoded_file,"Huffman_DECODED.dump");
	/*write the encoded data to a file*/
	decode_fpt = fopen(decoded_file,"wb");
	if(!decode_fpt)
	{
		printf("fopen failed for %s",decoded_file);
		return;
	}
	
	/*Write header info to file*/
	ret = fwrite(decoded_data,1,decoded_data_size,decode_fpt);
	if(ret != decoded_data_size)
	{
		printf("fwrite failed with %d\n",ret);
	}

	/*Free the memory allocated*/
	if(decode_fpt)
	{
		fclose(decode_fpt);
		decode_fpt = NULL;
	}
#endif /*DEBUG_MODE*/

	/*delete the entire tree*/
	Destroy_Tree(head);

	return decoded_data;
}

/*Function to create a node*/
node_data*  create_node(node_data **list_head,node_data *prev,
						unsigned char byte,unsigned int freq,
						node_data *left,node_data *right,unsigned char isSymbol)
{
	node_data *head = NULL ,*curr = NULL;

	/*allocate memory to add a node*/
	curr = (node_data*)malloc(sizeof(node_data));
	if(!curr)
	{
		printf("malloc failed\n");
		return NULL;
	}

	curr->byte = byte;
	curr->freq = freq;
	curr->isSymbol = isSymbol;

	/*update the table with the new entry*/
	if(!*list_head)
		*list_head = curr;

	if(prev)
		prev->next = curr;

	curr->next = NULL;
	curr->pattern = NULL;

	curr->left = left;
	curr->right = right;
	curr->root = NULL;

	if(left)
		left->root = curr;
	if(right)
		right->root = curr;

	return curr;
}

/*Function to sort the linked list*/
void sort_list(node_data **head,node_data **tail)
{
	node_data *curr = NULL,*next = NULL,*temp = NULL,*prev = NULL ;
	int count = 0;
	unsigned char temp_byte = 0;
	int	temp_freq = 0;

	curr = *head;
	/*traverse through the linked list*/
	while(curr)
	{
		if(!curr->next) /*one iternation complete */
		{	
			*tail = curr;	
			if(count)
			{
				curr = *head; /*reset for another iteration*/
				prev = NULL;
				count = 0;
			}
			else
				break; /*list is sorted*/
		}
		next = curr->next;

		if(curr->freq > next->freq)/*exchange the data to swap*/
		{
			if(curr == *head)
			{
				*head = next;
			}

			/*update the pointers*/
			temp = next->next;
			next->next = curr;
			curr->next = temp;

			if(prev)
				prev->next = next;

			temp = curr;
			curr = next;
			next = temp;
			count ++;
		}
		prev = curr;
		curr = curr->next;
	}
}

/*Function to build a Binary Tree based on the sorted link list*/
void CreateBinaryTree(node_data **head,node_data **tail)
{
	node_data *list_head = NULL,*last_symbol = NULL;
	node_data *curr = NULL,*next = NULL,*new_node=NULL;

	list_head = *head;
	curr = list_head;

	/*traverse through the linked list*/
	while(curr)
	{
		next = curr->next; /*right subtree*/

		/*create new node and enqueue*/
		new_node = create_node(head,*tail,(unsigned char)NULL,next->freq+curr->freq,curr,next,0);

		/*update the head and tail of the queue*/
		*tail = new_node;
		*head = next->next;

		/*link all the leafs which are symbols*/
		if(curr->isSymbol)
		{
			if(last_symbol)
				last_symbol->next = curr;
			last_symbol = curr;
			last_symbol->next = NULL;
		}
		else
		{
			curr->next = NULL;
		}

		if(next->isSymbol)
		{
			if(last_symbol)
				last_symbol->next = next;
			last_symbol = next;
			last_symbol->next = NULL;
		}
		else
		{
			next->next = NULL;
		}

		/*sort the queue*/
		sort_list(head,tail);

		curr = *head;
		if(!curr->next) /*one iternation complete */
		{	
			break; /*binary tree constructed*/
		}
	}

}

/*Function to traverse the binary tree and update the bit codes*/
void TraverseBinaryTree(node_data *symbol_list_head,unsigned int rootfreq)
{
	node_data *head = NULL,*curr = NULL,*leaf_root = NULL,*temp_root = NULL;
	unsigned char *pattern = NULL;
	unsigned int count = 0,i=0;
	head = symbol_list_head;

	/*Allocate memory for the pattern*/
	pattern = (unsigned char*)calloc(rootfreq,sizeof(unsigned char));
	if(!pattern)
	{
		printf("memory allocation failed\n");
		return;
	}

	curr = head;
	while(curr)/*traverse along all the leaf nodes*/
	{
		count = 0;
		memset(pattern,0,sizeof(pattern));
		leaf_root = curr->root;
		temp_root = curr;

		/*Traverse to the root node of the tree from each leaf
		Left = 0 Right = 1
		bit pattern will be in reverse order*/
		while(leaf_root)
		{
			if(temp_root == leaf_root->left)
			{
				pattern[count] = 0;
				count ++;
			}
			else if(temp_root == leaf_root->right)
			{	
				pattern[count] = 1;
				count ++;
			}
			else
			{
				printf("[TraverseBinaryTree]This is a bug in the code");
			}
			temp_root = leaf_root;
			leaf_root = leaf_root->root;
		}

		curr->pattern_len =  count; 
		/*Allocate memory to store the bit pattern*/
		curr->pattern = (unsigned char*)calloc(count,sizeof(unsigned char));
		if(!curr->pattern)
		{
			printf("memory allocation failed\n");
			return;
		}

		/*function to copy bit pattern in right order node -> leaf*/
		copydata(curr->pattern,pattern,count);

#ifdef DEBUG
		printf("\nSymbol = %c Freq = %d\n Code = ",curr->byte,curr->freq);
		for(i = 0;i<curr->pattern_len;i++)
		{
			printf("%d",curr->pattern[i]);
		}
#endif /*DEBUG*/

		/*traverse to the next symbol/leaf in tree*/
		curr = curr->next;
	}
}

/*Function to copy data in bytes*/
void copydata(unsigned char *dest,unsigned char *src,unsigned int len)
{
	int i = 0,j=0;
	for(i=len-1;i>=0;i--)
	{
		dest[j] = src[i];
		j++;
	}
}

/*Create the header info of the encoded file*/
void CreateHeaderInfo(unsigned char *dest,node_data *head,
					  unsigned char *symbol_count,unsigned int *len)
{

	node_data *list_head = head,*curr = head;
	unsigned char *header = dest;
	unsigned int encoded_size = 0;

	/*1st byte = total no. of symbols*/
	header[0] = *symbol_count;
	encoded_size ++;

	/*store symbol-frequency pairs for each symbol*/
	while(curr)
	{
		header[encoded_size] = curr->byte;
		encoded_size ++;

		memcpy(header+encoded_size,&curr->freq,sizeof(curr->freq));
		encoded_size = encoded_size + sizeof(curr->freq);

		curr = curr->next;
	}

	/*store the header size*/
	*len = encoded_size;
}

/*Encode the symbols in the file with its corresponding bit sequences*/
void EncodeData(unsigned char * encoded_data,
				unsigned char * original_data,unsigned int file_size,
				node_data *head,node_data **node_pointers,
				unsigned int *encoded_data_size,unsigned int *total_bit_count)
{
	node_data *node = NULL;
	unsigned char curr_byte = 0,bits_left = 7;
	unsigned int byte_count = 0,bits_count = 0;
	int i = 0,j=0;

	/*parse each byte of the original file and encode each*/
	for(i=0;i<file_size;i++)
	{
		node = node_pointers[original_data[i]];
		if(node->byte != original_data[i])
		{
			printf("this is a bug in code\n");
			break;
		}

		for(j=0;j<node->pattern_len;j++)
		{
			encoded_data[byte_count] = encoded_data[byte_count]|node->pattern[j]<<bits_left;
			bits_count++;
			bits_left --;
			if(0 == bits_count % BYTE2BITS)
			{
				byte_count++;
				bits_left = 7;
			}
		}
	}

	*total_bit_count	= bits_count;
	*encoded_data_size  = byte_count+1; /*To include memory index zero */
}

/*Destroy the binary tree*/
void Destroy_Tree(node_data *root_node)
{
	if(!root_node)
		return;

	/*delete the sub tress first*/
	Destroy_Tree(root_node->left);
	Destroy_Tree(root_node->right);

	/*delete the node*/
	if(root_node->pattern)
		free(root_node->pattern);
	root_node->pattern = NULL;

	if(root_node)
		free(root_node);
	root_node = NULL;
}

/*Function to parse the header of the encoded data*/
void Parse_Header(unsigned char *original_data,node_data **head,node_data **tail,
				  unsigned int *bits2decode,unsigned int *byteindex,
				  unsigned int *uncompress_size)
{
	node_data *curr = NULL;
	unsigned int freq = 0,symbol_count = 0,total_freq = 0;
	unsigned char symbol = 0;

	int i = 0,j =1;

	symbol_count  = (unsigned int)original_data[0];

	for(i=0;i<symbol_count+1;i++)
	{
		symbol = original_data[j];
		freq = freq|original_data[j+1]|original_data[j+2]<<8|original_data[j+3]<<16|original_data[j+4]<<24;
		total_freq = total_freq+freq;

		/*create a node for the symbol*/
		curr = create_node(head,curr,symbol,freq,NULL,NULL,1);
		if(!curr)
		{
			printf("create_node\n");
			return;
		}

		j = j+5;
		freq = 0;
	}

	*bits2decode = original_data[j]|original_data[j+1]<<8|original_data[j+2]<<16|original_data[j+3]<<24;
	*byteindex = j+4;

	*tail = curr;
	*uncompress_size  = total_freq;
}

/*Decode the encoded data by finding the pattern symbol in the binary tree*/
void DecodeData(node_data *head,
				unsigned char *original_data,unsigned char *decoded_data,
				unsigned int bits2decode,unsigned int byteindex,
				unsigned int file_size,unsigned int *decoded_size)
{
	unsigned char *bit_pattern = NULL;
	node_data *curr = NULL,*prev = NULL;
	int i = 0,j=0,bits_left = 7,link_count = 0,size = 0;
	unsigned char byte = 0;

	/*allocate memory to store the bit pattern*/
	bit_pattern = (unsigned char*)calloc((file_size - byteindex)*BYTE2BITS,
		sizeof(unsigned char));
	if(!bit_pattern)
	{
		printf("memory allocation failed\n");
		return;
	}

	/*scan all the encoded data bytes in the file*/
	for(i=byteindex;i<file_size;i++)
	{	
		/*read one byte and store their binary pattern*/
		byte = original_data[i];
		while(bits_left > -1)
		{
			bit_pattern[j] = (byte >> bits_left) & 0x01;
			bits_left --;
			j++;
		}
		bits_left = 7;
	}

	i = 0;
	curr = head;
	/*Traverse the tree from root as per bit_pattern generated*/
	while(bits2decode)
	{
		byte = bit_pattern[i];
		if(0 == byte)
		{
			prev = curr;
			curr = curr->left;
		}
		else if(1 == byte)
		{
			prev = curr;
			curr = curr->right;
		}

		if(curr->isSymbol)/*leaf node readed*/
		{
			decoded_data[size] = curr->byte; 
			size ++;
			curr = head; 
		}

		bits2decode --;
		i++;
	}

	*decoded_size = size;

	/*deallocate the memory*/
	if(bit_pattern)
		free(bit_pattern);
	bit_pattern = NULL;
}
