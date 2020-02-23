/* 		Harshit Patel 
		  hpatel24
		Spring 2019
		  CSC 541
  Advanced Data Structures */
  
/* 			Citations
 A. File reading techniques
		1.  https://www.csc2.ncsu.edu/faculty/healey/csc541/assn_1/index.html
		2.  https://computer.howstuffworks.com/c39.htm
		3.  http://www.cplusplus.com/reference/cstdio/ftell/
		
 B. How to use clock?
		1. 	https://www.csc2.ncsu.edu/faculty/healey/csc541/assn_1/index.html
*/

/* calling directories */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/* function decleration */
int linMemSearch(int *keyArray, int *seekArray, int keyLen, int seekLen, double readTime);
int binMemSearch(int *keyArray, int *seekArray, int keyLen, int seekLen, double readTime);
int binarySearch(int *keyArray, int seekElement, int left, int right);
int linDiskSearch(FILE* keyFile, int *seekArray, int keyLen, int seekLen);
int binDiskSearch(FILE* keyFile, int *seekArray, int keyLen, int seekLen);
int binarySearchonDisk(FILE* keyFile, int seekElement, int left, int right);

struct timeval start, end;

/* main function */
int main(int argc, int *argv[])
{
	/* if less than 4 arguments than throw error */
	if (argc != 4)
	{
		printf("\n Unexpected number of arguments.\nPlease provide 3 arguments.");
		return 0;
	}

	int seekIndex = 0; // index for seek file
	FILE *seekf; // file declaration

	seekf = fopen(argv[3], "rb");
	if (!seekf)
	{
		return 1;
	}

	// find file size in bytes
	fseek(seekf, 0, SEEK_END);
	int lengthofseekf = ftell(seekf);
	fseek(seekf, 0, SEEK_SET);

	// allocate appropriate memory for seek array and read the file into an array
	int *seekArray = (int*)malloc(lengthofseekf);
	while(!feof(seekf))
	{
		fread((seekArray + seekIndex), sizeof(int), 1, seekf);
		seekIndex++;
	}
	seekIndex--; // seek index is equal to no. of elements in file (and not size in bytes)

	// select algorithm according to input arguments

	// in memory linear search
	if (strcmp(argv[1], "--mem-lin") == 0)
	{
		int keyIndex = 0;
		FILE *keys;

		// open key file
		keys = fopen(argv[2], "rb");
		if (!keys)
		{
			return 1;
		}

		// find file size in bytes
		fseek(keys, 0, SEEK_END);
		int lengthofkeys = ftell(keys);
		fseek(keys, 0, SEEK_SET);

		// allocate appropriate memory for key array and read the file into array
		int *keyArray = (int*)malloc(lengthofkeys);

		gettimeofday(&start, NULL);
		while(!feof(keys))
		{
			fread((keyArray + keyIndex), sizeof(int), 1, keys);
			keyIndex++;
		}
		gettimeofday(&end, NULL);

		double readTime = ( ((double)end.tv_sec + 1.0e-6*end.tv_usec) - ((double)start.tv_sec + 1.0e-6*start.tv_usec) ); // time required to read the key file into array
		keyIndex--;
		linMemSearch(keyArray, seekArray, keyIndex, seekIndex, readTime); // search
		fclose(keys); // close key file
	}
	else if(strcmp(argv[1], "--mem-bin") == 0)
	{
		int keyIndex = 0;
		FILE *keys;

		keys = fopen(argv[2], "rb");
		if (!keys)
		{
			return 1;
		}

		fseek(keys, 0, SEEK_END);
		int lengthofkeys = ftell(keys);
		fseek(keys, 0, SEEK_SET);

		int *keyArray = (int*)malloc(lengthofkeys);
		gettimeofday(&start, NULL);
		while(!feof(keys))
		{
			fread((keyArray + keyIndex), sizeof(int), 1, keys);
			keyIndex++;
		}
		gettimeofday(&end, NULL);
		double readTime = ( ((double)end.tv_sec + 1.0e-6*end.tv_usec) - ((double)start.tv_sec + 1.0e-6*start.tv_usec) );
		keyIndex--;
		binMemSearch(keyArray, seekArray, keyIndex, seekIndex, readTime);
		fclose(keys);
	}
	else if(strcmp(argv[1], "--disk-lin") == 0)
	{
		int keyIndex = 0;
		FILE *keys;

		keys = fopen(argv[2], "rb");
		if (!keys)
		{
			return 1;
		}

		fseek(keys, 0, SEEK_END);
		int lengthofkeys = ftell(keys);
		fseek(keys, 0, SEEK_SET);
		keyIndex = lengthofkeys/sizeof(int);


		linDiskSearch(keys, seekArray, keyIndex, seekIndex);
		fclose(keys);
	}
	else if(strcmp(argv[1], "--disk-bin") == 0)
	{
		int keyIndex = 0;
		FILE *keys;

		keys = fopen(argv[2], "rb");
		if (!keys)
		{
			return 1;
		}

		fseek(keys, 0, SEEK_END);
		int lengthofkeys = ftell(keys);
		fseek(keys, 0, SEEK_SET);
		keyIndex = lengthofkeys/sizeof(int);

		binDiskSearch(keys, seekArray, keyIndex, seekIndex);
		fclose(keys);
	}
	else
	{
		printf("Error: incorrect input. Please only give one of the following input \n --mem-lin \n --mem-bin \n --disk-lin \n --disk-bin");
	}
	fclose(seekf);

	return 0;
}

int linMemSearch(int *keyArray, int *seekArray, int keyLen, int seekLen, double readTime)
{
	int hit[seekLen], sCount, kCount;
	double searchTime;

	gettimeofday(&start, NULL);
	// search through two arrays and if the element is found then save 1 to hit array else save 0
	for(sCount = 0; sCount < seekLen; sCount++)
	{
		for (kCount = 0; kCount < keyLen; kCount++)
		{
			if(seekArray[sCount] == keyArray[kCount])
			{
				hit[sCount] = 1;
				break;
			}
			else
			{
				hit[sCount] = 0;
			}
		}
	}
	gettimeofday(&end, NULL);

	searchTime = ( ((double)end.tv_sec + 1.0e-6*end.tv_usec) - ((double)start.tv_sec + 1.0e-6*start.tv_usec) );; // time require
	double totTime = readTime + searchTime; // total time

	// print result
	for(sCount = 0; sCount < seekLen; sCount++)
	{
		if(hit[sCount] == 1)
		{
			printf("%12d: Yes\n", seekArray[sCount]);
		}
		else
		{
			printf("%12d: No\n", seekArray[sCount]);
		}
	}
	printf("Time: %.6f\n", totTime);
	return 0;
}

// in memory binary search
int binMemSearch(int *keyArray, int *seekArray, int keyLen, int seekLen, double readTime)
{
	int hit[seekLen], val, sCount;
	double searchTime;

	gettimeofday(&start, NULL);
	for (sCount = 0; sCount < seekLen; sCount++)
	{
		val = binarySearch(keyArray, seekArray[sCount], 0, keyLen-1);
		hit[sCount] = (val == 1) ? 1 : 0;

	}
	gettimeofday(&end, NULL);
	double totTime = ( ((double)end.tv_sec + 1.0e-6*end.tv_usec) - ((double)start.tv_sec + 1.0e-6*start.tv_usec) );
	for(sCount = 0; sCount < seekLen; sCount++)
	{
		if(hit[sCount] == 1)
		{
			printf("%12d: Yes\n", seekArray[sCount]);
		}
		else
		{

			printf("%12d: No\n", seekArray[sCount]);
		}
	}
	printf("Time: %.6f\n", totTime);
	return 0;
}

// binary search function -> takes in two arrays and search each element of second element in first one
int binarySearch(int *keyArray, int seekElement, int left,  int right)
{
	int mid = 0;
	if (right < left)
	{
		return 0;
	}

	mid = (left + right) / 2;
	if (seekElement < keyArray[mid])
	{
		binarySearch(keyArray, seekElement, left, mid-1);
	}
	else if (seekElement > keyArray[mid])
	{
		binarySearch(keyArray, seekElement, mid + 1, right);
	}
	else
	{
		return 1;
	}

}

// on disk linear search
int linDiskSearch(FILE *keyFile, int *seekArray,  int keyLen, int seekLen)
{
	int hit[seekLen], currentKval = 0, sCount;


	gettimeofday(&start, NULL);
	// run through seek array
	for (sCount = 0; sCount < seekLen; sCount++)
	{
		// seek to start of key file
		fseek(keyFile, 0, SEEK_SET);
		// read file until end
		while(!feof(keyFile))
		{
			// read one element of size equal to integer size
			fread(&currentKval, sizeof(int), 1, keyFile);
			// if current element is equal to current element from seek array than hit == 1 else hit == 0
			if (seekArray[sCount] == currentKval)
			{
				hit[sCount] = 1;
				break;
			}
			hit[sCount] = 0;
		}
	}
	gettimeofday(&end, NULL);

	double totTime = ( ((double)end.tv_sec + 1.0e-6*end.tv_usec) - ((double)start.tv_sec + 1.0e-6*start.tv_usec) ); // total time is time required to search all elements of seek in file

	// print result
	for(sCount = 0; sCount < seekLen; sCount++)
	{
		if (hit[sCount] == 1)
		{
			printf("%12d: Yes\n", seekArray[sCount]);
		}
		else
		{
			printf("%12d: No\n", seekArray[sCount]);
		}
	}
	printf("Time: %.6f\n", totTime);

}

// on disk binary search
int binDiskSearch(FILE* keyFile, int *seekArray, int keyLen, int seekLen)
{
	int hit[seekLen], val, sCount;
	
	gettimeofday(&start, NULL);
	for (sCount = 0; sCount < seekLen; sCount++)
	{
		val = binarySearchonDisk(keyFile, seekArray[sCount], 0, keyLen-1);
		hit[sCount] = (val == 1) ? 1 : 0;
	}
	gettimeofday(&end, NULL);

	double totTime = ( ((double)end.tv_sec + 1.0e-6*end.tv_usec) - ((double)start.tv_sec + 1.0e-6*start.tv_usec) );

	for (sCount = 0; sCount < seekLen; sCount++)
	{
		if (hit[sCount] == 1)
		{
			printf("%12d: Yes\n", seekArray[sCount]);
		}
		else
		{
			printf("%12d: No\n", seekArray[sCount]);
		}
	}
	printf("Time: %.6f\n", totTime);

}

// binary search function -> takes one file and one integer and search that integer in through the file and returns 1 if found else 0
int binarySearchonDisk(FILE* keyFile, int seekElement, int left, int right)
{
	int mid = 0, currentkVal = 0;
	if (right < left)
	{
		return 0;
	}

	// seek to start of key file
	fseek(keyFile, 0, SEEK_SET);
	mid = (left + right)/2;
	fseek(keyFile, (mid*sizeof(int)), SEEK_SET); // seek to middle of key file
	fread(&currentkVal, sizeof(int), 1, keyFile); // read one element of from the file

	// if seek element is less than current element then search on left array
	if (seekElement < currentkVal)
	{
		return binarySearchonDisk(keyFile, seekElement, left, mid-1);
	}
	// if seek element is greater than current element then search on right array
	else if(seekElement > currentkVal)
	{
		return binarySearchonDisk(keyFile, seekElement, mid + 1, right);
	}
	// if seek element is equal to current element then return 1
	else
	{
		return 1;
	}
}
