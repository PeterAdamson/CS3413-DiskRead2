//Author Peter Adamson
//This program expects the following syntax when being run: ./a.out a n disk_layout.txt < sample.txt
//where a is letter indicating the algorithm to use (F for FCFS, T for SSTF, S for SCAN, C for C-SCAN, L for LOOK, and O for C-LOOK), n is the initial position of the disk head, disk_layout.txt is the file containing the disk layout, and sample.txt is the request file to be read.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//define the node structure for the list
typedef struct qNode Node;
struct qNode
{
	int firstBlock;
	int nextBlock;
	Node *next;	
};

//global variable declarations
int requests[10];
int headMov;
int currHead;
int headPtr;
int request;

//function declarations
void FCFS();
void SSTF();
void SCAN();
void CSCAN();
void LOOK();
void CLOOK();
void initialize();//reads in a job from standard input
void enqueue(Node *newJob);
Node *readDisk();
void findChain(int request);

//global pointers to the start(head) and end(tail) of the queue
Node *head;
Node *tail;

int main(int argc, char **argv)
{
	//set up the request to be added
	int count = 0;
	headMov = 0;
	currHead = 5000;
	headPtr = 0;
	currHead = atoi(argv[2]);

	//open the file
	FILE *file = fopen(argv[3], "r+");

	//load the disk layout into the list
	Node *block;
	block = readDisk(file);
	while(block != NULL)
	{
		enqueue(block);
		block = readDisk(file);
	}

	/*check that we are not at the end of the file and that the format is correct.
	if so, set up request (loads first 10 requests into the array)*/
	while(count < 10)
	{
		if(!feof(stdin) && (1 == scanf("%d", &request)))	//valid request
		{
			requests[count] = request;
		}
		else if(feof(stdin))	//less than 10 requests
		{
			break;
		}
		count = count + 1;
	}
	
	//check which algorithm to use
	if(strcmp(argv[1],"F") == 0)
	{
		FCFS();
	}
	else if(strcmp(argv[1],"T") == 0)
	{
		SSTF();
	}
	else if(strcmp(argv[1],"S") == 0)
	{
		SCAN();
	}
	else if(strcmp(argv[1],"C") == 0)
	{
		CSCAN();
	}
	else if(strcmp(argv[1],"L") == 0)
	{
		LOOK();
	}
	else if(strcmp(argv[1],"O") == 0)
	{
		CLOOK();
	}
}

//sets the head and tail pointers to null and indicates to the user that the pointers are ready to use
void initialize()
{
	head = tail = NULL;
}

//loads a job into the end of the list
void enqueue(Node *newBlock)
{
	//set up the job to be added
	Node *temp = NULL;
	temp = (Node*)malloc(sizeof(Node));
	temp->firstBlock = newBlock->firstBlock;
	temp->nextBlock = newBlock->nextBlock;
	temp->next = NULL;
	if(tail == NULL)	//the list must be empty, so set both head and tail to temp
	{
		head = temp;
		tail = temp;
	}
	else			//the list is not empty, so add the job to the end of the list
	{
		tail->next = temp;
		tail = temp;	
	}
}

//reads in a job from standard input
Node *readDisk(FILE *file)
{
	//set up the block to be added
	int firstBlock;
	int nextBlock;
	Node *newBlock = NULL;

	/*check that we are not at the end of the file and that the format is correct.
	if so, set up and return the job*/
	if(!feof(file) && (2 == fscanf(file,"%d %d", &firstBlock, &nextBlock)))
	{
		newBlock = (Node*)malloc(sizeof(Node));
		newBlock->firstBlock = firstBlock;
		newBlock->nextBlock = nextBlock;
		newBlock->next = NULL;
	}

	return newBlock;
}

void FCFS()
{
	int requestToService;
	int c = 0;

	//loop as long as there are requests to be read
	while(c < 10)
	{
		requestToService = requests[headPtr];
		findChain(requestToService);

		if(requestToService > currHead)	//the request is larger than our current head location
		{
			headMov = headMov + (requestToService - currHead);
		}
		else	//the request is less than or equal to our current head location
		{
			headMov = headMov + (currHead - requestToService);
		}

		currHead = requestToService;

		if(!feof(stdin) && 1 == scanf("%d", &request))	//valid request
		{
			requests[headPtr] = request;
		}
		else	//no more requests
		{
			c++;
		}

		headPtr = headPtr + 1;

		if(headPtr > 9)	//head pointer needs to be wrapped around
		{
			headPtr = 0;
		}
	}
	printf("FCFS total head movement: %d\n",headMov);
}

void SSTF()
{
	int requestToService;
	int smallestDiff;
	int location;
	int c = 0;
	int i;

	//loop as long as there are requests to be read
	while(c < 10)
	{
		if(requests[0] > currHead)	//the request is larger than our current head location
		{
			smallestDiff = requests[0] - currHead;
			location = 0;
		}
		else				//the request is smaller than or equal to our current head location
		{
			smallestDiff = currHead - requests[0];
			location = 0;
		}

		//loop through the currently loaded requests
		for(i = 0; i < 10; i++)
		{
			if(requests[i] >= 0 && requests[i] > currHead)	//check that value is valid and check if request is larger than our current head location
			{
				if(requests[i] - currHead < smallestDiff)	//we have a new shortest seek time
				{
					smallestDiff = requests[i] - currHead;
					location = i;
				}
			}
			else if(requests[i] >= 0)	//check that value is valid, our request is less than or equal to our current head location
			{
				if(currHead - requests[i] < smallestDiff)	//we have a new shortest seek time
				{
					smallestDiff = currHead - requests[i];
					location = i;
				}
			}
		}

		requestToService = requests[location];
		findChain(requestToService);

		if(requestToService > currHead)	//the request is larger than our current head location
		{
			headMov = headMov + (requestToService - currHead);
		}
		else	//the request is less than or equal to our current head location
		{
			headMov = headMov + (currHead - requestToService);
		}

		currHead = requestToService;

		for(i = location; i < 9; i ++)
		{
			requests[i] = requests[i + 1];
		}

		if(!feof(stdin) && 1 == scanf("%d", &request))	//valid request
		{
			requests[9] = request;
		}
		else	//no more requests
		{
			requests[9 - c] = -1;
			c++;
		}
	}
	printf("SSTF total head movement: %d\n",headMov);
}

void SCAN()
{
	printf("head starts moving right\n");
	int c = 0;
	int i;
	int direction = 1;	//0 for left, 1 for right
	while(c < 10)
	{
		if(direction == 1)	//we are moving right
		{
			for(i = 0; i < 10; i++)
			{
				if(requests[i] == currHead)	//we have found a request
				{
					findChain(requests[i]);
					if(!feof(stdin) && 1 == scanf("%d", &request))	//valid request
					{
						requests[i] = request;
					}
					else	//no more requests
					{
						requests[i] = -1;
						c++;
					}
				}
			}
			currHead = currHead + 1;
			if(c < 10)	//don't increase head movement if we are finished
			{
				headMov = headMov + 1;
			}
			if(currHead == 9999)	//we have reached the end, begin moving left
			{
				direction = 0;
			}
		}
		if(direction == 0)	//we are moving left
		{
			for(i = 0; i < 10; i++)
			{
				if(requests[i] == currHead)	//we have found a request
				{
					findChain(requests[i]);
					if(!feof(stdin) && 1 == scanf("%d", &request))	//valid request
					{
						requests[i] = request;
					}
					else	//no more requests
					{
						requests[i] = -1;
						c++;
					}
				}
			}
			currHead = currHead - 1;
			if(c < 10)	//don't increase head movement if we are finished
			{
				headMov = headMov + 1;
			}
			if(currHead == 0)	//we have reached the end, begin moving right
			{
				direction = 1;
			}
		}
	}
	printf("SCAN total head movement: %d\n",headMov);
}

void CSCAN()
{
	printf("head moves right\n");
	int c = 0;
	int i;
	while(c < 10)
	{
		for(i = 0; i < 10; i++)
		{
			if(requests[i] == currHead)	//we have found a request
			{
				findChain(requests[i]);
				if(!feof(stdin) && 1 == scanf("%d", &request))	//valid request
				{
					requests[i] = request;
				}
				else	//no more requests
				{
					c++;
				}
			}
		}
		currHead = currHead + 1;
		if(currHead == 9999)	//we have reached the end, begin moving left
		{
			currHead = 0;
		}
		if(c < 10)	//don't increase head movement if we are finished
		{
			headMov = headMov + 1;
		}
	}
	printf("CSCAN total head movement: %d\n",headMov);
}

void LOOK()
{
	printf("head starts moving right\n");
	int c = 0;
	int i;
	int direction = 1;	//0 for left, 1 for right
	int extreme;
	while(c < 10)
	{
		if(direction == 1)	//we are moving right
		{
			extreme = requests[0];
			for(i = 0; i < 10; i++)
			{
				if(requests[i] == currHead && requests[i] != -1)	//we have found a request
				{
					findChain(requests[i]);
					if(!feof(stdin) && 1 == scanf("%d", &request))	//valid request
					{
						requests[i] = request;
					}
					else	//no more requests
					{
						requests[i] = -1;
						c++;
					}
				}
				if(requests[i] > extreme && requests[i] != -1)	//we have a new ending point
				{
					extreme = requests[i];
				}
			}
			currHead = currHead + 1;
			if(c < 10)	//don't increase head movement if we are finished
			{
				headMov = headMov + 1;
			}
			if(currHead == extreme)	//we have reached the end, begin moving left
			{
				direction = 0;
			}
		}
		if(direction == 0)	//we are moving left
		{
			extreme = requests[0];
			for(i = 0; i < 10; i++)
			{
				if(requests[i] == currHead && requests[i] != -1)	//we have found a request
				{
					findChain(requests[i]);
					if(!feof(stdin) && 1 == scanf("%d", &request))	//valid request
					{
						requests[i] = request;
					}
					else	//no more requests
					{
						requests[i] = -1;
						c++;
					}
				}
				if(requests[i] < extreme && requests[i] != -1)	//we have a new ending point
				{
					extreme = requests[i];
				}
			}
			currHead = currHead - 1;
			if(c < 10)	//don't increase head movement if we are finished
			{
				headMov = headMov + 1;
			}
			if(currHead == extreme)	//we have reached the end, begin moving right
			{
				direction = 1;
			}
		}
	}
	printf("LOOK total head movement: %d\n",headMov);
}

void CLOOK()
{
	printf("head moves right\n");
	int c = 0;
	int i;
	int lowExtreme;
	int highExtreme;
	while(c < 10)
	{
		lowExtreme = requests[0];
		highExtreme = requests[0];
		for(i = 0; i < 10; i++)
		{
			if(requests[i] == currHead)	//we have found a request
			{
				findChain(requests[i]);
				if(!feof(stdin) && 1 == scanf("%d", &request))	//valid request
				{
					requests[i] = request;
				}
				else	//no more requests
				{
					c++;
				}
			}
			if(requests[i] > highExtreme)
			{
				highExtreme = requests[i];
			}
			if(requests[i] < lowExtreme)
			{
				lowExtreme = requests[i];
			}
		}
		currHead = currHead + 1;
		if(currHead == highExtreme)	//we have reached the end, begin moving left
		{
			currHead = lowExtreme;
		}
		if(c < 10)	//don't increase head movement if we are finished
		{
			headMov = headMov + 1;
		}
	}
	printf("CLOOK total head movement: %d\n",headMov);
}

void findChain(int request)
{
	int req = request;
	Node *toFind = head;
	while(1)
	{
		if(toFind->firstBlock == req)	//we have found the correct block
		{
			printf("%d",toFind->firstBlock);
			int nextBl = toFind->nextBlock;
			if(nextBl == -1)	//the chain has ended
			{
				printf("\n");
				break;	
			}
			else	//the chain has not ended
			{
				printf("->");
				req = nextBl;
			}
		}
		else	//we have not found the correct block
		{
			toFind = toFind->next;
		}
	
		if(toFind == NULL)	//we have hit the end of the list without finding the correct block.  Assuming that only valid requests are allowed, this means that toFind must have been located ahead of the request, so loop toFind back to the beginning
		{
			toFind = head;
		}
	}
}
