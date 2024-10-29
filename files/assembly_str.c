#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "assembly_str.h"


void readFile(char**** strings, char* filename, params* prs) {
	char buffer[256];
	int commInd = 24;
	FILE *fp = fopen(filename, "r");
	int pos = 0;
	if (fp) {
		while((fgets(buffer, 256, fp)) != NULL) {
			char* ptr = &buffer[0];
			int ind = 0;
			for (ind = 0; ind < 3 && *ptr != '\n'; ind++) {
				if (pos < commInd) {
					int i = 0;
					for(i = 0; !isspace(*ptr); i++) {
						(*strings)[prs->size][ind][i] = *(ptr++);
						pos++;
						if (pos >= commInd) {
							free((*strings)[prs->size][ind]);
							(*strings)[prs->size][ind] = (char*)malloc(256*sizeof(char));
							(*strings)[prs->size][ind][0] = '\0';
							break;
						}
					}
					(*strings)[prs->size][ind][i] = '\0';
					while(isspace(*ptr) && pos < commInd) {
						ptr++;
						pos++;
					}
				}
			}
			if (isspace(buffer[commInd-1]) && pos >= commInd) {
				int i = 0;
				for (i = 0; *ptr != '\n'; i++) {
					(*strings)[prs->size][ind][i] = *(ptr++);
				}
				(*strings)[prs->size][ind][i] = '\0';
			}
			prs->size++;
			if (prs->size == prs->capacity) {
				prs->capacity *= 2;
				*strings = (char***)realloc(*strings, (prs->capacity) * sizeof(char**));
				for (int i = prs->size; i < prs->capacity; i++) {
					(*strings)[i] = (char**)malloc(4*sizeof(char*));
					for (int j = 0; j < 4; j++) {
						(*strings)[i][j] = (char*)malloc(256*sizeof(char));
					}
				}
			}
			pos = 0;
		}
		fclose(fp);
	}	
}