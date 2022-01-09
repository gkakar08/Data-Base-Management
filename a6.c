#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "hash.h"

#define BUFFER	256
// function prototypes
void and(char*, char*, char*);
int set2idx(char*, int*);
void getString(char*, int, char*);
int entries(char*);
long getIdx(char*, char*);
void query(char*, int, char*, int, char*);

// main function
int main(int argc, char* argv[]) {
    // command line argument check
    if (argc < 3) {
        printf("Error: please provide building and room number!\n");
        exit(-1);
    }

    // helping variables
    char subject[BUFFER], courseNo[BUFFER];
    long room, building;
    int i, sizeOfCodes, * subjectCodes, * courseCode;
    char from[BUFFER], to[BUFFER], days[BUFFER];

    // get index of room and building
    room = getIdx("room", argv[2]);
    building = getIdx("building", argv[1]);

    // get the index set of room and building
    query("code", -1, "room", room, "roomIndices.set");
    query("code", -1, "building", building, "buildingIndices.set");

    // intersection of both room and building
    and ("roomIndices.set", "buildingIndices.set", "buildingRoomIndices.set");

    // get the indices
    subjectCodes = malloc(BUFFER);
    sizeOfCodes = set2idx("buildingRoomIndices.set", subjectCodes);

    courseCode = malloc(1);
    i = 0;

    char pDays[BUFFER];
    char pFrom[BUFFER];
    char pTo[BUFFER];
    char pSubject[BUFFER];
    char pNo[BUFFER];
    // iterate over the indices
    while (i < sizeOfCodes)
    {
        if(i > 0){
            strcpy(pSubject,subject);
            strcpy(pNo,courseNo);
            strcpy(pDays,days);
            strcpy(pFrom,from);
            strcpy(pTo,to);
        }
    	// get the days
        query("code", subjectCodes[i], "days", -1, "days.set");
        set2idx("days.set", courseCode);
        getString("days", courseCode[0], days);

        // get the from
        query("code", subjectCodes[i], "from", -1, "from.set");
        set2idx("from.set", courseCode);
        getString("from", courseCode[0], from);

        // get the to
        query("code", subjectCodes[i], "to", -1, "to.set");
        set2idx("to.set", courseCode);
        getString("to", courseCode[0], to);

        // get subject name
        query("code", subjectCodes[i], "subject", -1, "subject.set");
        set2idx("subject.set", courseCode);
        getString("subject", courseCode[0], subject);

        // get the course number
        query("code", subjectCodes[i], "courseno", -1, "course.set");
        set2idx("course.set", courseCode);
        getString("courseno", courseCode[0], courseNo);


        if(!((strcmp(subject,pSubject) == 0)&&(strcmp(courseNo,pNo) == 0)&&(strcmp(days,pDays) == 0)&&(strcmp(from,pFrom) == 0)&&(strcmp(to,pTo) == 0))){
            printf("%s*%s %s %s - %s\n", subject, courseNo, days, from, to);
        }
        // print all values
        //printf("%s * %s %s %s - %s\n", subject, courseNo, days, from, to);
        i++;
    }
    // free allocated memory
    free(subjectCodes);
    free(courseCode);
    return 0;
}

int entries(char* filebase)
{
    char filename[BUFFER];
    int ent;

    strcpy(filename, filebase);
    strcat(filename, ".idx");
    FILE* fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_END);
    ent = ftell(fp) / sizeof(long);
    fclose(fp);

    return ent;
}

long getIdx(char* baseFileName, char* val)
{
    char idxname[BUFFER];
    char txtname[BUFFER];

    char* basename;
    char* value;
    long hash_table[HASHSIZE];

    // identify text file name
    strcpy(idxname, baseFileName);
    strcat(idxname, ".idx");
    strcpy(txtname, baseFileName);
    strcat(txtname, ".txt");

    // basefile for hashing
    basename = baseFileName;

    // target value
    value = val;

    // load hashtable from file into memory
    get_hashtable(basename, hash_table);

    // open text file
    FILE* idxfile = fopen(idxname, "r");
    FILE* txtfile = fopen(txtname, "r");

    // print result of hash_lookup
    long index = hash_lookup(value, hash_table, idxfile, txtfile);
    fclose(idxfile);
    fclose(txtfile);

    return index;
}

void query(char* code, int subjectNo, char* baseName, int buildingRoomNo, char* output)
{
    char* v1 = code;
    int i1 = subjectNo;
    char* v2 = baseName;
    int i2 = buildingRoomNo;

    int n1 = entries(v1);
    int n2 = entries(v2);


    char filename[BUFFER];

    strcpy(filename, v1);
    strcat(filename, "_");
    strcat(filename, v2);
    strcat(filename, ".rel");

    FILE* fp = fopen(filename, "rb");
    char* array = malloc(n1 * n2);
    fread(array, 1, n1 * n2, fp);
    fclose(fp);

    fp = fopen(output, "wb");
    if ((i1 == -1) && (i2 >= 0))
    {
        for (int i = 0; i < n1; i++)
        {
            int index = i * n2 + i2;
            fwrite(array + index, 1, 1, fp);
        }
    }

    if ((i1 > 0) && (i2 == -1))
    {
        for (int j = 0; j < n2; j++)
        {
            int index = i1 * n2 + j;
            fwrite(array + index, 1, 1, fp);
        }
    }
    free(array);
    fclose(fp);
}

void and(char* f1, char* f2, char* result)
{
    FILE* fp1 = fopen(f1, "rb");
    FILE* fp2 = fopen(f2, "rb");
    FILE* fp3 = fopen(result, "wb");
    char b1, b2, b3;

    while (fread(&b1, 1, 1, fp1) == 1 && fread(&b2, 1, 1, fp2))
    {
        b3 = b1 && b2;
        fwrite(&b3, 1, 1, fp3);
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
}

int set2idx(char* filename, int* output)
{
    char boolean;

    FILE* fp = fopen(filename, "rb");
    int count = 0;
    for (int i = 0; fread(&boolean, 1, 1, fp) == 1; i++)
    {
        if (boolean)
        {
            output[count] = i;
            count++;
        }
    }
    return count;
}

void getString(char* baseName, int baseNumber, char* output)
{
    char* basename;
    int idx, idx2;

    basename = baseName;
    idx = baseNumber;

    char txtfile[BUFFER];
    char idxfile[BUFFER];
    char buffer[BUFFER];

    FILE* fptxt, * fpidx;

    strcpy(txtfile, basename);
    strcat(txtfile, ".txt");

    strcpy(idxfile, basename);
    strcat(idxfile, ".idx");

    fptxt = fopen(txtfile, "r");
    fpidx = fopen(idxfile, "r");

    fseek(fpidx, sizeof(long) * idx, SEEK_SET);
    if (fread(&idx2, sizeof(long), 1, fpidx) != 1)
    {
        fprintf(stderr, "Error: invalid index\n");
        exit(-1);
    }

    fseek(fptxt, idx2, SEEK_SET);
    fgets(buffer, BUFFER, fptxt);

    buffer[strlen(buffer) - 1] = '\0';
    strcpy(output, buffer);
    fclose(fptxt);
}

