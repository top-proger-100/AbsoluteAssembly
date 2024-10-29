#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "files/assembly_str.h"
#include "files/hash_table.h"

#define mnemonicSize 32 
#define mnemonicLength 29
#define mnemonicDirectiveInd 6

char keys[29][6] = {
        "START","END","BYTE","WORD",
        "RESB","RESW","LDX","LDA",
		"STA","STX","ADD","SUB",
		"MUL","DIV","AND","OR",
		"XOR","NOT","CMP","JMP",
		"JE","JNE","JG","JGE",
		"JL","JLE","JZ","MOV",
		"HLT",
};

int getFlag(const char* string) {
	char* str = malloc(256*sizeof(char));
	memcpy(str, string, 256);
	char* ptr = strtok(str, ",");
	for (int i = 0; i < strlen(ptr); i++) {
		if (!isalpha(ptr[i])) {
			return false;
		}
	}
	return true;
}

void startNameTable(Item** hashTableName, int tableNameSize, char*** strings, int size) {
	for (int i = 0; i < size; i++) {
		if (strings[i][2][0] != '\0') {
			bool flag = getFlag(strings[i][2]);
			char* str = malloc(256*sizeof(char));
			memcpy(str, strings[i][2], 256);
			char* ptr = strtok(str, ",");
			if (flag) { 
				Item item;
				item.key = ptr;
				item.flag = false;
				item.addres = 0;
				Item* itm = search(hashTableName, item.key, tableNameSize);
				if (itm == NULL) insert(hashTableName, item, tableNameSize);
			}
		}
	}
}

int firstStep(int* startValue, char*** strings, int size, Item** hashTableName, int tableNameSize) {
	int count = 0;
	*startValue = 0;
	count = atoi(strings[0][2]);
	int num = count;
	int i = 0;
	while(num > 0) {
		*startValue += (num%10)*pow(16, i);
		i++;
		num /= 10;
	} 
	count = *startValue;
	for (int i = 1; i < size; i++) {
		if (strings[i][0][0] != '\0') {
			Item* itm = search(hashTableName, strings[i][0], tableNameSize);
			if (itm != NULL) {
				itm->flag = true;
				itm->addres = count;
			}
		}
		count += 3;
	}
	return count;
}

void setMnemonicTable(Item** mnemonicTable) {
	for (int i = 0; i < mnemonicDirectiveInd; i++) {
		Item item;
		item.key = keys[i];
		item.flag = false;
		item.addres = 0;
		insert(mnemonicTable, item, mnemonicSize);
	}
    for (int i = mnemonicDirectiveInd; i < mnemonicLength; i++) {
        Item item;
        item.key = keys[i];
		item.flag = true;
		item.addres = i;
        insert(mnemonicTable, item, mnemonicSize);
    }
}

void secondStep(int val, int count, char*** strings, int size, Item** mnemonicTable, Item** hashTableName, int tableNameSize) {
	char** buf = malloc(size*sizeof(char*));
	for (int i = 0; i < size; i++) buf[i] = malloc(256*sizeof(char));
	char* filename = "result/listing.txt";
	FILE* fp = fopen(filename, "w");
	if (fp) {
		snprintf(buf[0], 256*sizeof(char), ":02%04X02%04XXX", val, count-val-3);
		count = 0;
		fprintf(fp, " Счёт.  КОП   X    Адр.     Маш.      Программа\n");
		fprintf(fp, " разм.                      ком.\n");
		fprintf(fp, "%04Xh |     |   |       |         | %s %s %s %s\n", count,
			strings[0][0], strings[0][1], strings[0][2], strings[0][3]);
		count = val;
		for (int i = 1; i < size-1; i++) {
			int cop = search(mnemonicTable, strings[i][1], mnemonicSize)->addres;
			int isCommandFlag = search(mnemonicTable, strings[i][1], mnemonicSize)->flag;
			if (isCommandFlag) {
				if (strings[i][2][0] != '\0') {
					char* str = malloc(256*sizeof(char));
					memcpy(str, strings[i][2], 256);
					str = strtok(str, ",");
					Item* item = search(hashTableName,  str, tableNameSize);
					char* ptr = NULL;
					while (str != NULL) {
						ptr = str;
						str = strtok(NULL, ",");
					}
					bool flag = false;
					if (strcmp(ptr, "X") == 0) {
						flag = true;
					}
					int addres = item->addres;
					if (flag) addres += 0b1000000000000000;
					snprintf(buf[i], 256*sizeof(char), ":03%04X00%02X%04XXX", count, cop, addres);
					fprintf(fp, "%04Xh | %02Xh | %d | %04Xh | %02X%04Xh | %s %s %s %s\n", count,
					cop, flag, item->addres, cop, addres, strings[i][0], strings[i][1], strings[i][2], strings[i][3]);
				} else {
					snprintf(buf[i], 256*sizeof(char), ":03%04X00%02X0000XX", count, cop);
					fprintf(fp, "%04Xh | %02Xh | 0 | 0000h | %02X0000h | %s %s %s %s\n", count,
					cop, cop, strings[i][0], strings[i][1], strings[i][2], strings[i][3]);
				}
			} else {
				int machine_command = atoi(strings[i][2]);
				if (strcmp(strings[i][1], "RESW") == 0 || strcmp(strings[i][1], "RESB") == 0) machine_command = 0;
				snprintf(buf[i], 256*sizeof(char), ":03%04X00%06XXX", count, machine_command);
				fprintf(fp, "%04Xh |     |   |       | %06Xh | %s %s %s %s\n", count, machine_command,
					strings[i][0], strings[i][1], strings[i][2], strings[i][3]);
			}
			count += 3;
		}
		snprintf(buf[size-1], 256*sizeof(char), ":00%04X01XX", val);
		fprintf(fp, "%04Xh |     |   |       |         | %s %s %s %s\n", count,
			strings[size-1][0], strings[size-1][1], strings[size-1][2], strings[size-1][3]);
		fclose(fp);
	}
	filename = "result/object_code.txt";
	fp = fopen(filename, "w");
	if (fp) {
		for (int i = 0; i < size; i++) {
			fprintf(fp, "%s\n", buf[i]);
		}
		fclose(fp);
	}
	for (int i = 0; i < size; i++) free(buf[i]);
	free(buf);
}

int main(int argc, char** argv) {
	char* filename = argv[1];
	params prs;
	prs.size = 0;
	prs.capacity = 10;
	char*** strings = (char***)malloc(prs.capacity*sizeof(char**));
	for (int j = 0; j < prs.capacity; j++) {
		strings[j] = (char**)malloc(4*sizeof(char*));
		for (int i = 0; i < 4; i++) {
			strings[j][i] = (char*)malloc(256*sizeof(char));
		}
	}

	readFile(&strings, filename, &prs);

	// длина хеш-таблицы имён
	int tableNameLength = 0;
	for (int i = 0; i < prs.size; i++) {
		if (strings[i][2][0] != '\0') {
			bool flag = getFlag(strings[i][2]);
			if (flag) tableNameLength++;
		}
	}

	// определение хеш-таблицы имён
	int tableNameSize = (int)pow(2, ceil(log2(tableNameLength)));
	Item** hashTableName = malloc(tableNameSize*sizeof(Item*)); // динамическая память
	for (int i = 0; i < tableNameSize; i++) hashTableName[i] = NULL;
	startNameTable(hashTableName, tableNameSize, strings, prs.size);

    // первый проход(заполнение хеш-таблицы имён)
	int val;
	int count = firstStep(&val, strings, prs.size, hashTableName, tableNameSize);

	// заполнение хеш-таблицы мнемоник
    Item** hashTableMnemonic = malloc(mnemonicSize*sizeof(Item*));
    for (int i = 0; i < mnemonicSize; i++) hashTableMnemonic[i] = NULL;
	setMnemonicTable(hashTableMnemonic);

	// проверка заполняемости хеш-таблицы имён
	bool flag = true;
	for (int i = 0; i < tableNameSize; i++) {
		if (hashTableName[i] != NULL) {
			if (hashTableName[i]->flag == 0) {
				flag = false;
				break;
			}
		}
	}
	if (flag) {
		// второй проход (листинг и объектный код)
		secondStep(val, count, strings, prs.size, hashTableMnemonic, hashTableName, tableNameSize);
	}


	for (int i = 0; i < tableNameSize; i++) {
        if (hashTableName[i] != NULL) free(hashTableName[i]);
    }
	free(hashTableName);
    for (int i = 0; i < mnemonicSize; i++) {
        if (hashTableMnemonic[i] != NULL) free(hashTableMnemonic[i]);
    }
	free(hashTableMnemonic);
	for (int i = 0; i < prs.capacity; i++) {
		for (int j = 0; j < 4; j++) {
			free(strings[i][j]);
		}
		free(strings[i]);
	}
	free(strings);
	return 0;
}