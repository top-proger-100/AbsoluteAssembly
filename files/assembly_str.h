#ifndef ASSEMBLY_STR_H
#define ASSEMBLY_STR_H

struct params {
	int size;
	int capacity;
} typedef params;

void readFile(char**** strings, char* filename, params* prs);

#endif