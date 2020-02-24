/* CSC 541 Advanced Data Structures
 * 			Assign 2
 * In memory indexing with availability list
 * 		   Harshit Patel
 * 			(@hpatel24)
 */

/* Credits
 * fgets function
 * 			https://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
 * strtok function
 * 			https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
 * 			https://www.rosipov.com/blog/c-strtok-usage-example/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
        int key;/* Record's key */
        long off; /* Record's offset */
}index_S;

typedef struct {
        int siz;/* Hole's size */
        long off;/* Hole's offset in file */
}avail_S;

typedef enum {first, worst, best}order;
order STRATEGY;

FILE *indexF, *availF, *f;
avail_S availArr[10000];
index_S indexArr[10000];
int indexInd = 0, availInd = 0;
long recOff;
int recSiz;
char *buff;

void read_index_file();
void read_avail_file();
char **converter(char *inpbuffer);
void add(FILE *f, int key, char *data);
void find(FILE *f, int key);
void del(FILE *f, int key);
int get_available_hole(int length);
int hightolow(const void *l,const void *r);
int lowtohigh(const void *l,const void *r);
int bin_search(int target,int low,int high);
void delete(int ind);
void delHole(int available_Index);
void addHole(int updatedholesize, long updateoffset);
void add_new_hole(avail_S ava);

int main(int argc, char* argv[]){
	if (argc != 3) {
		printf("Invalid arguments: Please try again");
		return 0;
	}

	if (strcmp(argv[1], "--best-fit") == 0){
		STRATEGY = best;
	}
	else if (strcmp(argv[1], "--worst-fit") == 0){
		STRATEGY = worst;
	}
	else if (strcmp(argv[1], "--first-fit") == 0){
		STRATEGY = first;
	}
	else{
		printf("Invalid strategy: Please try again");
		return 0;
	}
	if ((f = fopen(argv[2], "r+b")) == NULL){
		f = fopen(argv[2], "w+b");
		indexF = fopen("index.bin", "w+b");
		availF = fopen("avail.bin", "w+b");
	}
	else{
		read_index_file();
		read_avail_file();
	}

	while(1){
		char line[200] = {0};
		fgets(line, 200, stdin);

		char **out = converter(line);

		if (strcmp(out[0], "end")==0){

			fclose(f);

			fseek(indexF, 0, SEEK_SET);
			fwrite(indexArr, sizeof(index_S), indexInd, indexF);
			fclose(indexF);

			fseek(availF, 0,SEEK_SET);
			fwrite(availArr, sizeof(avail_S), availInd, availF);
			fclose(availF);

			break;
		}

		else if (strcmp(out[0], "add") == 0){
			char **residue = converter(out[1]);
			int key = atoi(residue[0]);
			add(f, key, residue[1]);
		}
		else if (strcmp(out[0], "find") == 0){
			find(f, atoi(out[1]));
		}
		else if (strcmp(out[0], "del") == 0){
			del(f, atoi(out[1]));
		}
		else {
			printf("Incorrect command, please try again\n");
			break;
		}
	}
	printf("Index:\n");
	int i=0;
	for(;i<indexInd;++i){
		printf( "key=%d: offset=%ld\n", indexArr[i].key, indexArr[i].off );
	}
	int hole_siz=0;
	printf("Availability:\n");
	int j=0;
	for(;j<availInd;++j){
	hole_siz+=availArr[j].siz;
	printf( "size=%d: offset=%ld\n", availArr[j].siz, availArr[j].off );
	}
	printf( "Number of holes: %d\n", availInd);
	printf( "Hole space: %d\n", hole_siz );


}

void read_index_file(){
	indexF = fopen("index.bin", "r+b");
	index_S currentInd;
	while(!feof(indexF)){
		fread(&currentInd, sizeof(index_S), 1, indexF);
		indexArr[indexInd++] = currentInd;
	}
	indexInd--;
}

void read_avail_file(){
	availF = fopen("avail.bin", "r+b");
	avail_S currentAvail;
	while(!feof(availF)){
		fread(&currentAvail, sizeof(avail_S), 1, availF);
		availArr[availInd++] = currentAvail;
	}
	availInd--;
}
char **converter(char *inpbuffer){
	char **out = (char **)malloc(sizeof(char *) * 2);
	char *ptr = strtok(inpbuffer, " \n");
	out[0] = ptr;
	ptr = strtok(NULL, "\n");
	out[1] = ptr;
	return out;
}

void add(FILE *f, int key, char *data){
	if (bin_search(key, 0, indexInd-1) != -1){ // 3
		printf("Record with SID=%d exists\n",key);
		return;
	}
	long offset;
	int length = strlen(data);
	int available_Index = get_available_hole(length);
	if (available_Index == -1) {
		fseek(f,0,SEEK_END);
		offset=(long)ftell(f);
		fwrite(&(length),sizeof(int),1,f);
		fwrite(data,sizeof(char),length,f);
	}
	else{
		offset = availArr[available_Index].off;
		fseek(f, offset, SEEK_SET);
		fwrite(&length, sizeof(int), 1, f);
		fwrite(data, sizeof(char), length, f);

		int updatedholesize = availArr[available_Index].siz - (sizeof(int)) - length;
		int updatedoffset = offset + sizeof(int) + length;

		// Routine to delete hole
		delHole(available_Index);

		// Routine to update
		if (updatedholesize != 0){
			avail_S newavail={updatedholesize,updatedoffset};
			add_new_hole(newavail);
		}

	}
	index_S newindex = {key, offset};
	if(indexInd==0) {
			indexArr[indexInd++ ]= newindex;
	}
	else{

			int low = 0;
			int high=indexInd-1;
			int mid;
			while(low <= high) {
					mid = low + (high-low)/2;
					if(indexArr[mid].key<key) {
							low = mid+1;
					}
					else{
							high = mid-1;
					}
			}
			int i=indexInd++;
			for(; i>low; --i) {
					indexArr[i] = indexArr[i-1];
			}
			indexArr[low]=newindex;
	}
}

void find(FILE *f, int key){
	int ind = bin_search(key, 0, indexInd - 1);
	if (ind == -1){
		printf("No record with SID = %d exists\n", key);
	}
	else {
		fseek(f, indexArr[ind].off, SEEK_SET);
		fread(&recSiz, sizeof(int), 1, f);
		buff = malloc(recSiz);
		fread(buff, 1, recSiz, f);

		printf("%s\n", buff);

	}
}
void del(FILE *f, int key){
	int ind = bin_search(key, 0, indexInd-1);
	if(ind == -1) {
		printf("No record with SID=%d exists\n",key);
		return;
	}
	avail_S ava;
	ava.off = indexArr[ind].off;
	fseek(f, indexArr[ind].off, SEEK_SET);
	fread(&recSiz, sizeof(int), 1, f);
	ava.siz = sizeof(int) + recSiz;
	add_new_hole(ava);

	delete(ind);
}

void delete(int ind){
	if(ind >= indexInd) {
		return;
	}
	int i = ind;
	for(;i+1 < indexInd; ++i) {
			indexArr[i] = indexArr[i+1];
	}
	indexInd--;
}

void add_new_hole(avail_S ava){
	if(STRATEGY == first) {
		availArr[availInd++] = ava;
	}
	else if(STRATEGY == best) {
		availArr[availInd++] = ava;
		qsort(availArr, availInd, sizeof(avail_S), lowtohigh);
	}
	else if(STRATEGY == worst) {
		availArr[availInd++] = ava;
		qsort(availArr, availInd, sizeof(avail_S), hightolow);
	}
}

int lowtohigh(const void *l, const void *r){
	avail_S *left=(avail_S*)l;
	avail_S *right=(avail_S*)r;
	if(left->siz==right->siz) { return (left->off-right->off); }
	else{ return (left->siz-right->siz); }
}

int hightolow(const void *l,const void *r){
        avail_S *left=(avail_S*)l;
        avail_S *right=(avail_S*)r;
        if(left->siz==right->siz) { return (left->off-right->off); }
        else{ return (right->siz - left->siz ); }
}

int bin_search(int target,int low,int high){
	int mid = 0;
	if (high < low)
	{
		return -1;
	}

	mid = (low + high) / 2;
	if (target < indexArr[mid].key)
	{
		bin_search(target, low, mid-1);
	}
	else if (target > indexArr[mid].key)
	{
		bin_search(target, mid + 1, high);
	}
	else
	{
		return mid;
	}
}


int get_available_hole(int length){
        int i=0;
        for(; i < availInd; ++i) {
                if(availArr[i].siz>=(sizeof(int)+length)) {
                        return i;
                }
        }
        return -1;
}

void delHole(int available_Index){
	if(available_Index>=availInd) {
		return;
	}
	int i=available_Index;
	for(; i+1<availInd; ++i) {
		availArr[i]=availArr[i+1];
	}
	availInd--;
}


void addInd(int key, long offset){
	index_S newindex = {key, offset};
	if(indexInd==0) {
		indexArr[indexInd++ ]= newindex;
	}
	else{

		int low = 0;
		int high=indexInd-1;
		int mid;
		while(low <= high) {
				mid = low + (high-low)/2;
				if(indexArr[mid].key<key) {
						low = mid+1;
				}
				else{
						high = mid-1;
				}
		}
		int i=indexInd++;
		for(; i>low; --i) {
				indexArr[i] = indexArr[i-1];
		}
		indexArr[low]=newindex;
	}
}
