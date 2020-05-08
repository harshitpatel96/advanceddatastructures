/* CSC 541 Advanced Data Structures
 * 			Assign 3
 *    Disk Based Merge Sort
 * 		   Harshit Patel
 * 			(@hpatel24)
 */

/* Credits
 * Bhavesh Agrawal (@basagrwa)
 * 			helped me figure out how a single struct that keeps track of file and the contents,
 * 			that we read from it, can help me remove unwanted gibberish that was coming in my
 * 			output file. (NOTE: No direct code sharing was done, we only discussed why it is important
 * 			and then I implemented it on my own
 * sprintf function usage
 * 			https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm
 * One lecture for better understanding of replacement sort
 * 			https://www.youtube.com/watch?v=YjFI9CJy6x0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/time.h>

void basic(char *input, char *output);
void multistep(char *input, char *output);
void replacement(char *input, char *output);
int findTotRuns(char *input);
void create_runs(int buffer_size, char* inpName, int i, FILE *inpFile);
int comparator(const void *a, const void *b);
void merge(int startind, int totRuns, FILE *output, char* inName, int appendingnum);
void heapify(int *heap, int n);
void sift(int *heap, int i, int n);
struct timeval totalTime(struct timeval start, struct timeval end);

typedef struct
{
	FILE* f;
	int* ptr;
	int ind;
	int left;
}file_struct;

typedef struct
{
	int index;
	int left_to_read;
}ptrs;


#define BUFFER_SIZE 1000
#define BSIZE 250
#define HSIZE 750


bool debug = false;


int main(int argc, char *argv[]){

	if (argc != 4){
			printf("Invalid number of arguments.\nPlease give only 3 arguments.");
			return 0;
	}
	char *input = argv[2];
	struct timeval begin, end;

	if (strcmp(argv[1], "--basic") == 0){

		gettimeofday(&begin, NULL);
		basic(input, argv[3]);
		gettimeofday(&end, NULL);

	}
	else if (strcmp(argv[1], "--multistep") == 0){

		gettimeofday(&begin, NULL);
		multistep(input, argv[3]);
		gettimeofday(&end, NULL);

	}
	else if (strcmp(argv[1], "--replacement") == 0){

		gettimeofday(&begin, NULL);
		replacement(input, argv[3]);
		gettimeofday(&end, NULL);

	}
	else {
		printf("Invalid method, please choose only from \"--basic\", \"--multistep\", and \"--replacement\"  ");
		return 0;
	}
	struct timeval elapsed = totalTime(begin, end);
	printf("Time: %ld.%06ld\n", elapsed.tv_sec, elapsed.tv_usec);
}

struct timeval totalTime(struct timeval begin, struct timeval end) {
	struct timeval elapsed;
	elapsed.tv_sec =	end.tv_sec - begin.tv_sec;
	elapsed.tv_usec = end.tv_usec - begin.tv_usec;

	elapsed.tv_usec = elapsed.tv_usec < 0 ? elapsed.tv_usec + 1000000 : elapsed.tv_usec-1;

	return elapsed;

}


void basic(char *inName, char *outName) {

	FILE* output = fopen(outName, "wb+");
	int totalruns = findTotRuns(inName);
	char *inputname = (char *)malloc((strlen(inName) + 4) * sizeof(char));
	strcpy(inputname, inName);
	strcat(inputname, ".%03d");
	merge(0, totalruns, output, inputname, 4);
	fclose(output);
}

int findTotRuns(char *inName) {

	FILE* input = fopen(inName, "rb");
	fseek(input, 0, SEEK_END);
	int len = (int)((size_t)ftell(input)/sizeof(int));
	fseek(input, 0, SEEK_SET);
	int totRuns = len/BUFFER_SIZE;
	if (len%BUFFER_SIZE) {
		totRuns++;
	}

	int i = 0;
	int buffer_size = 0;
	while (i < totRuns) {

		if (debug){
			printf("%d\n", ftell(input)/sizeof(int));
		}
		buffer_size = BUFFER_SIZE;
		if (i == totRuns - 1) {

			if (len%BUFFER_SIZE != 0) {
				buffer_size = len%BUFFER_SIZE;
			}
		}

		if (debug) {
			printf("%d\n", buffer_size);
		}

		create_runs(buffer_size, inName, i, input);
		i++;

	}

	fclose(input);
	return totRuns;
}


void create_runs(int buffer_size, char* inpName, int i, FILE *inpFile) {

	char *target = (char *) malloc((strlen(inpName)+4)*sizeof(char));
	int *chunk = (int *) malloc(buffer_size*sizeof(int));
	fread(chunk, sizeof(int), buffer_size, inpFile);
	qsort(chunk, buffer_size, sizeof(int), comparator);
	sprintf(target, "%s.%03d", inpName, i);
	FILE *f = fopen(target, "wb+");
	fseek(f, 0, SEEK_SET);
	fwrite(chunk, sizeof(int), buffer_size, f);
	fclose(f);
	free(target);
	free(chunk);
}

int comparator(const void *a, const void *b) {
	return (*(int *)a - *(int *)b);
}

void multistep(char *input, char *output) {

	int totRuns = findTotRuns(input);
	int file_count = 15;
	int superRuns = totRuns / file_count;
	int last_super_run = totRuns % 15;
	if (last_super_run) {
		superRuns += 1;
	}

	char* superFile = (char *) malloc((strlen(input) + 10) * sizeof(char));

	int i = 0;
	FILE *curr_super_file;
	char *super_file_name = (char *)malloc((strlen(input) + 10) * sizeof(char));
	char *inName = (char *)malloc((strlen(input) + 4) * sizeof(char));
	strcpy(super_file_name, input);
	strcat(super_file_name, ".super.%03d");
	strcpy(inName, input);
	strcat(inName, ".%03d");
	while (i < superRuns) {
		if (i == superRuns - 1) {
			if (last_super_run != 0)
			{
				file_count = last_super_run;
			}
		}
		sprintf(superFile, super_file_name, i);
		if (i == 0) {
		}
		curr_super_file = fopen(superFile, "wb");

		if (debug) {
			printf("%s\n", superFile);
		}

		merge(0 + i*15, file_count, curr_super_file, inName, 4);
		fclose(curr_super_file);
		i++;
	}

	FILE *outFile = fopen(output, "wb");

	if (debug) {
		printf("%s\n", output);
	}

	merge(0, superRuns, outFile, super_file_name, 10);
	free(super_file_name);
	free(superFile);
	free(inName);
	fclose(outFile);

}

void merge(int startind, int totRuns, FILE* output, char *inName, int appendingnum) {

	char* fName = malloc((appendingnum + strlen(inName))*sizeof(char));

	if (debug) {
		printf("totRuns: %d\n", totRuns);
	}

	file_struct files[totRuns];
	ptrs pointers[totRuns];
	int i = 0;
	int partperfile = BUFFER_SIZE/totRuns;

	int *inputbuf = malloc(BUFFER_SIZE*sizeof(int));

	while (i < totRuns) {
		sprintf(fName, inName, i + startind);
		files[i].f = fopen(fName, "rb");
		files[i].ptr = inputbuf + i * partperfile;
		i++;

	}

	i = 0;
	while (i < totRuns) {
		files[i].ind = 0;
		files[i].left = fread(files[i].ptr, sizeof(int), partperfile , files[i].f);
		i++;
	}


	int the_end = 0;
	int minVal, minInd = -1;
	int *out = malloc(BUFFER_SIZE * sizeof(int));
	int outInd = 0;

	while (the_end < totRuns) {

		minVal = INT_MAX, minInd = -1;
		i = 0;
		while (i < totRuns) {

			if (files[i].left != 0) {

				if (files[i].ind != files[i].left) {

					if (*(files[i].ptr + files[i].ind) < minVal) {

						minInd = i;
						minVal = *(files[i].ptr + files[i].ind);

					}
				}
				else {
					files[i].left = fread(files[i].ptr, sizeof(int), partperfile , files[i].f);
					files[i].ind = 0;

					if (files[i].left == 0) {
						the_end++;
						fclose(files[i].f);
					}
					else
					{
						if (*(files[i].ptr + files[i].ind) < minVal) {
							minVal = *(files[i].ptr + files[i].ind);
							minInd = i;
						}
					}

				}

			}
			i++;
		}
		if (minInd != -1) {
			out[outInd] = minVal;
			files[minInd].ind++;
			outInd++;

			if (outInd == BUFFER_SIZE) {
				outInd = 0;
				fwrite(out, sizeof(int), BUFFER_SIZE, output);
			}

		}


	}

	if (outInd != 0)
	{
		if (debug) {
			printf("%d\n", outInd);
		}
		fwrite(out, sizeof(int), outInd, output);
		fclose(output);
	}

	free(fName);
	free(inputbuf);
	free(out);
}

void replacement(char *input, char *output) {

	FILE *currFile, *outFile;
	FILE *inpFile = fopen(input, "rb");
	fseek(inpFile, 0, SEEK_END);
	int inputLen = (int)((size_t)ftell(inpFile) / sizeof(int));
	fseek(inpFile, 0, SEEK_SET);

	if (inputLen > BUFFER_SIZE)
	{
		char *pattern, *fName, *index;

		int *heap = malloc(HSIZE * sizeof(int));
		fread(heap, sizeof(int), HSIZE, inpFile);

		int *buffer = malloc(BSIZE * sizeof(int));
		fread(buffer, sizeof(int), BSIZE, inpFile);

		int *outbuf = malloc(BUFFER_SIZE * sizeof(int));

		int outInd = 0, bufInd = 0;
		int heapSize = HSIZE, file_count = 0;
		int end = 0, buff_end = 0, left = inputLen;

		fName = malloc((strlen(input) + 4) * sizeof(char));
		index = malloc(4 * sizeof(char));

		sprintf(fName, "%s.%03d", input, file_count);

		if (debug) {
			printf("%s", fName);
		}

		currFile = fopen(fName, "wb");


		left = inputLen - BUFFER_SIZE;
		heapify(heap, HSIZE);
	
		while (true)
		{
			if (end == 0 || (end != 0 && bufInd != end))
			{
				if (heapSize <= 0)
				{
					if (outInd != 0)
					{
						fwrite(outbuf, sizeof(int), outInd, currFile);
						outInd = 0;
					}
					fclose(currFile);

					file_count++;

					sprintf(index, ".%03d", file_count);
					sprintf(fName, input);
					strcat(fName, index);

					if (debug) {
						printf("fName");
					}

					// open new file
					currFile = fopen(fName, "wb");
					heapSize = HSIZE; // reset heapsize
					heapify(heap, heapSize); // heapify what we read into heapbuffer

				}
				else
				{

					outbuf[outInd] = heap[0]; // copy first element of heap to output buffer
					outInd = outInd + 1; // increase output buffer index

					if (debug) {
						printf("%d\n", heapSize);
					}

					if (heap[0] <= buffer[bufInd])
					{

						heap[0] = buffer[bufInd];
						bufInd += 1;

					}
					else
					{

						heapSize -= 1;
						heap[0] = heap[heapSize];
						heap[heapSize] = buffer[bufInd];
						bufInd += 1;

					}

					if (debug) {
						printf("%d\n", bufInd);
					}

					if (bufInd == BSIZE)
					{
						int reads = left;
						if (left >= BSIZE) {
							reads = BSIZE;
						}
						left -= reads;

						end = (left == 0 && end == 0) ? reads : end;

						if (debug) {
							printf("reads: %d, left: %d, end: %d", reads, left, end);
						}

						if (reads != 0)
						{
							fread(buffer, sizeof(int), reads, inpFile);
							bufInd = 0;
						}

					}
					heapify(heap, heapSize);
					// when output buffer is full write it to curr file
					if (outInd == BUFFER_SIZE)
					{
						outInd = 0;
						fwrite(outbuf, sizeof(int), BUFFER_SIZE, currFile);

					}

				}
			}
			else
			{
				if (heapSize > 0)
				{
					buff_end += 1;

					outbuf[outInd] = heap[0];
					outInd += 1;

					// if output buffer is full then write it out
					if (outInd == BUFFER_SIZE)
					{

						outInd = 0;
						fwrite(outbuf, sizeof(int), BUFFER_SIZE, currFile);

					}

					heapSize -= 1;
					heap[0] = heap[heapSize];

					heapify(heap, heapSize);
				}
				else
				{
					if (outInd > 0)
					{
						if (debug) {
							printf("%d\n", outInd);
						}
						fwrite(outbuf, sizeof(int), outInd, currFile);
						outInd = 0;
					}

					// close curr file
					fclose(currFile);
					file_count++;
					sprintf(index, ".%03d", file_count);
					sprintf(fName, input);
					strcat(fName, index);
					currFile = fopen(fName, "wb");
					qsort(&heap[buff_end], (size_t)(HSIZE - buff_end), sizeof(int), comparator);
					fwrite(&heap[buff_end], sizeof(int), (HSIZE - buff_end), currFile);
					fclose(currFile);
					break;
				}
			}
		}

		// if output buffer is not empty then write out its content
		if (outInd != 0)
		{
			if (debug) {
				printf("%d\n", outInd);
			}

			fwrite(outbuf, sizeof(int), outInd, currFile);

		}

		outFile = fopen(output, "wb");
		pattern = (char *)malloc(4 * sizeof(char));
		sprintf(pattern, input);
		strcat(pattern, ".%03d");
		merge(0, file_count + 1, outFile, pattern, 4);

		// close files and free up memory
		fclose(currFile);
		free(fName);
		free(pattern);
		free(index);
		free(heap);
		free(buffer);
		return;
	}
	basic(input, output);
	return;
}

void heapify(int *heap, int n)
{
    int i = n / 2;
    for (; i >= 0; --i)
    {
        sift(heap, i, n);
    }
}

void sift(int *heap, int i, int n)
{
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int smallest = i;
    if (left < n && heap[left] < heap[i])
    {
        smallest = left;
    }
    if (right < n && heap[right] < heap[smallest])
    {
        smallest = right;
    }
    if (smallest != i)
    {
        int temp = heap[i];
        heap[i] = heap[smallest];
        heap[smallest] = temp;
        sift(heap, smallest, n);
    }
}
