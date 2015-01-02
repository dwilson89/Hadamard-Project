#include "hadamard.h"
#include <stdlib.h>
#include <stdio.h> //Include the standard IO library

// create the hadamard matrix
void create_hadamard_matrix(char** h, int N){

    //Set to defualt H1 size - keeps track of the current H matrix size
    int currentN = 1;

    //create a starting point - first digit is black so is equal to 1, white is equal to 0.
    h[0][0] = 1;

    //will keep track of previous square dimension
    int previousN;

    int block;
    int i;
    int j;

    //while the matrix hasnt been filled in to its maximum size
    while((currentN != N)){

        previousN = currentN;
        currentN = currentN * 2;

        //for loop that will put the blocks together
        //the "first" block is always established by this point so will need another 3
        for(block = 1; block < 4; block++){

            //want to keep this going till it fills up
            for(i = 0; i < previousN; i++){

                for(j = 0; j < previousN; j++){

                    //top left block
                    if(block == 1){

                        h[0 + i][previousN + j] = h[i][j];

                    //bottom right block
                    }else if(block == 2){

                        h[previousN + i][0 + j] = h[i][j];

                    //if on the "(lower right block)"" bits are inverted
                    } else if(block == 3){

                        //if intial h value  == 0 change to an 1, else vice versa
                        if (h[i][j] == 0){

                            h[previousN + i][previousN + j] = 1;

                        } else {

                            h[previousN + i][previousN + j] = 0;

                        }
                    }//end if
                }//end for
            }//end for
        }//end for
    }//end while
}//end create_hadamard_matrix(char** h, int N)

// convert to lookup table
void convert_to_lookup_table(unsigned long long* lookup, char **h, int N){

    unsigned long long lookUpBits; //intialize at the start of every loop

    int lookupTableLength = N*2;
    int index = 0;
    const int MAX = 64;

    unsigned long long inverted;

    //store the value for a particular location in the h matrix
    char tempHValue;

    //reset the "new" bit to be added to the lookup table
    if(N < MAX){

        inverted = ~(~0llu << N);

        lookUpBits = ~(~0llu << N);

    } else {

        inverted = ~0llu;
        lookUpBits = 0xffffffffffffffffllu;
    }

    int j;

    while(index < lookupTableLength){

        //for the first 64 rows
        if(index < N){

            //loop through j index of the h matrix - to create "new" bit
            for(j = 0; j < N; j++){

                //Get the value stored at that place in h
                tempHValue = h[index][j];

                //if zero, check if bit is turned on if so clear it.
                if(tempHValue == 0){

                    //if bit j is turned on
                    if(lookUpBits & ((unsigned long long)1 << j)){

                        //clear the bit at that location
                        lookUpBits &= ~((unsigned long long)1 << j);
                    }//end if

                //This isn't neccessarily needed, though maybe if done differently in the future
                } else if (tempHValue == 1){

                    //if bit j isn't turned on
                    if(!(lookUpBits & ((unsigned long long)1 << j))){

                        //set the bit at that location
                        lookUpBits |= ((unsigned long long)1 << j);

                    }//end if
                }//end if
            }//end for

            //add new long to table
            lookup[index] = lookUpBits;

        } else {//its the next 64 rows
            //invert its (index - 64)th bit
            lookup[index] = (lookup[(index - N)]) ^ inverted;

        }//end else i

        index++;
    }//end while
}

// read in a pgm image
void pgm_read(char* fin, char** img, int u, int v){


    //open file
    FILE *imgFile;
    imgFile = fopen (fin, "rb");

    if(imgFile == NULL){

        printf("Error: Could not open file %s in mode rb", fin);
        exit(1);
    }

    char header;
    int type;
    int headerU;
    int headerV;
    int depth;

    int i, j;
    
    //remove the header line
    //while(getc(imgFile) != '\n');

    fscanf(imgFile, "%c%s %d %d %d\n",&header, &type, &headerU, &headerV, &depth);

    //populate the img matrix
    for(i = 0; i < v; i++){

        for(j = 0; j < u; j++){

            img[i][j] = getc(imgFile);

        }//end for
    }//end for

    //this will close the file
    fclose(imgFile);
}

// write out a pgm image
void pgm_write(char* fout, char** img,  int format, int u, int v, int depth){

    //open file
    FILE *outputImg;
    outputImg = fopen(fout, "wb");

    if(outputImg == NULL){

        printf("Error: Could not open file %s in mode wb", fout);
        exit(1);
    
    }//end if

    //prints the output file the header - unsure on the end line at this stage
    fprintf(outputImg, "P%d %d %d %d\n", format, u, v, depth);

    int i, j;

    //output to a new file
    for(i = 0; i < v; i++){

        for(j = 0; j < u; j++){

            fputc(img[i][j], outputImg);

        }//end for
    }//end for

    //close file
    fclose(outputImg);

}//end function

// write the encoded image out
void enc_write(char* fout, char** img, int u, int v, unsigned long long* lookup){

    //open file
    FILE *outputImg;
    outputImg = fopen(fout, "wb");

    if(outputImg == NULL){

        printf("Error: Could not open file %s in mode wb", fout);
        exit(1);

    }//end if

    unsigned long long temp;

    int i, j;
    //output to a new file
    for(i = 0; i < v; i++){

        for(j = 0; j < u; j++){

            if(img[i][j] > 127){

                printf("Error: Cannot handle characters with value greater than 127");
                exit(1);

            }

            //need to convert byte to a 64 bit hadamard code
            temp = lookup[img[i][j]];

            fwrite(&temp, 8, 1, outputImg);

        }//end for

    }//end for

    //close file
    fclose(outputImg);

}//end function

// read the encoding in
void enc_read(unsigned long long* encin, char* fin, int u, int v){

    //open file
    FILE *imgFile;
    imgFile = fopen(fin, "rb");

    if(imgFile == NULL){

        printf("Error: Could not open file %s in mode rb", fin);
        exit(1);

    }//end if

    int index = 0;

    int i, j;

    //read in
    for(i = 0; i < v; i++){

        for(j = 0; j < u; j++){

            fread(&encin[index], 8, 1, imgFile);

            index++;
        }//end for

    }//end for

    //close file
    fclose(imgFile);

}

// decode the image
void dec_img(char** img, unsigned long long* encin, unsigned long long* lookup, int u, int v, int N){

    //used to keep track of matching bit
    unsigned long long temp;
    unsigned long long compareResult;

    int lookupCount = N*2;

    //index value
    int bestMatch;
    int matchCount;
    int previousCount = N;
    int tempCount = 0;
    int index = 0;

    int i, j, k, l;

    //for each each element in the image, grab the next encine and add it to the matrix
    //for each element in encine
    for(i = 0; i < v; i++){

        for(j = 0; j < u; j++){

            //grab the encin value
            temp = encin[index];

            index++;

            //reset the count
            previousCount = N;

            //search for match
            //if one is found break the loop, else keep track of the closest match
            for(k = 0; k < lookupCount; k++){

                //XOR it and count the left over ones
                compareResult = temp ^ lookup[k];
                matchCount = 0;

                //count unmatching ones
                for(l = 0; l < N; l++, compareResult >>=1){

                    matchCount += compareResult&1;

                }

                //if there is a perfect match in count
                if(matchCount == tempCount){

                    bestMatch = k;
                    //break out of the loop perfect match
                    break;

                } else if (matchCount < previousCount) {

                    bestMatch = k;
                    previousCount = matchCount;

                }
            }
            //the best matched value is stored
            img[i][j] = bestMatch;

        }
    }
}

// pack encoded image and write to file
void enc_pack(char* fin, char* fout){

    //open file
    FILE *outputImg;
    outputImg = fopen(fout, "wb");

    if(outputImg == NULL){

        printf("Error: Could not open file %s in mode wb", fout);
        exit(1);

    }//end if

    //open file
    FILE *imgFile;
    imgFile = fopen (fin, "rb");

    if(imgFile == NULL){

        printf("Error: Could not open file %s in mode rb", fin);
        exit(1);
    }

    //if there is a header....
    //read first bit if its a P.... insert who line into packed file
    char check = getc(imgFile);
    char check2 = getc(imgFile);
    int checkInt = check2 - '0';

    int u;
    int v;
    int depth;
    char c;

    unsigned long long mask =~(~0llu << 8);
    unsigned long long charHolder =~(~0llu << 8);
    unsigned long long packedBits = 0llu;
    unsigned long long test = 0;
    
    //keep tracks of bits
    int count = 0;
    int maskMove;

    //not needed
    if((check == 'P') && ((checkInt > 0)&&(checkInt < 7))){

        fscanf(imgFile, " %d %d %d\n", &u, &v, &depth);
        fprintf(outputImg, "%c%d %d %d %d\n", check, checkInt, u, v, depth);

    } else {//should never do this

        packedBits = check;
        packedBits = (packedBits << 7);
        count++;
        packedBits = packedBits | check2;
        packedBits = (packedBits << 7);
        count++;

    }

    //extract the current 64bit, and use bitwise operation to shift it left every 8 places
    while((c = getc(imgFile)) != EOF){

        count++;

        packedBits = (packedBits << 7) | c;

        //if 8 bits have been packed, send them to the output file
        if(count == 8){

            maskMove = count - 2;

            while(maskMove >= 0){

                charHolder = (packedBits >> 8*maskMove) & mask;
                //printf("%llu\n", charHolder);
                maskMove--;
                fputc(charHolder, outputImg);

            }

            //maskMove = count - 2;
            count = 0;
            packedBits = 0;
        }
    }

    //incase while ended without enough bytes, might need trailing zeros
    if(count != 0){

            maskMove = count - 1;

            //add on needed trailing zeroes
            packedBits = packedBits << count;

            while(maskMove >= 0){

                charHolder = (packedBits >> 8*maskMove) & mask;
                maskMove--;
                fputc(charHolder, outputImg);
            }
    }

    //this will close the file
    fclose(imgFile);
    //close file
    fclose(outputImg);
}

/*
Unpack the PGM pixel values in fin from 7-bits to 8-bits and store in fout.
*/
// unpack encoded image and write to file
void enc_unpack(char* fin, char* fout){

    //open file
    FILE *outputImg;
    outputImg = fopen(fout, "wb");

    if(outputImg == NULL){

        printf("Error: Could not open file %s in mode wb", fout);
        exit(1);

    }//end if

    //open file
    FILE *imgFile;
    imgFile = fopen (fin, "rb");

    if(imgFile == NULL){

        printf("Error: Could not open file %s in mode rb", fin);
        exit(1);
    }

    //if there is a header....
    //read first bit if its a P.... insert who line into packed file
    char check = getc(imgFile);
    int typeNum, u, v, depth;

    char c;
    unsigned char c2;

    char check2 = getc(imgFile);
    int checkInt = check2 - '0';

    //might get rid of it and just leave it at 0
    unsigned long long packedBits = 0;
    unsigned long long clearBits = ~0llu;
    //mask to help unpack it back to a 8 bit
    unsigned long long mask = ~(~0llu << 8);
    unsigned long long charHolder;
    unsigned long long testmask = ~(~0llu << 7);

    //current byte unpacking
    int n;
    int count = 0;
    int byteCount = 0;

    if((check == 'P') && ((checkInt > 0)&&(checkInt < 7))){

        fscanf(imgFile, " %d %d %d\n", &u, &v, &depth);
        fprintf(outputImg, "%c%d %d %d %d\n", check, checkInt, u, v, depth);//still not sure on the end line

    } else {

        packedBits = (packedBits << 8) | check;
        count++;
        packedBits = (packedBits << 8) | check2;
        count++;
    }

    //extract the current 64bit, and use bitwise operation to shift it left every 8 places
    while((!feof(imgFile)) && (byteCount < u*v)){

        byteCount++;

        c = getc(imgFile);

        if(feof(imgFile)){

            break;
        }

        //some reason need to convert to an unsigned char
        c2 = c;

        packedBits = (packedBits << 8) | c2;
        count++;

        if(count == 7){

            //special case EOF
            if(byteCount >= u*v){
                break;
            }

            count++;
            //get the remaining 0s
            packedBits = (packedBits << count);

            //inside a while loop
            while(count > 0){

                count--;

                //shift it 8 bits everytime inorder to clear
                clearBits = clearBits >> 8;

                //shift right by 1
                packedBits = packedBits >> 1;
                //send it off to the file using a mask.
                n = count * 8;

                charHolder = ((packedBits >> n) & mask);

                fputc(charHolder, outputImg);

                //clear sent bits to zero
                packedBits = packedBits & clearBits;

            }

            byteCount++;
            clearBits = ~0llu;
            packedBits = 0;
            c2 = c;

        }
    }

    //if it ends prematurely, unpack the rest
    if(count != 0){

        //count--;
        //get the remaining 0s
        //packedBits = (packedBits << count);
        clearBits = ~(~0llu << 8*count);

        //inside a while loop
        while(count > 0){

            count--;

            //shift it 8 bits everytime inorder to clear
            clearBits = clearBits >> 8;

            //shift right by 1
            packedBits = packedBits >> 1;
            //send it off to the file using a mask.
            n = count * 8;

            charHolder = ((packedBits >> n) & mask);

            fputc(charHolder, outputImg);

            //clear sent bits to zero
            packedBits = packedBits & clearBits;
        }
    }

    //this will close the file
    fclose(imgFile);
    //close file
    fclose(outputImg);

}

// extract message in hadamard encoding
void dec_msg(char* fout, unsigned long long* encin, unsigned long long* lookup, int u, int v, int N){

    //open file
    FILE *outputImg;
    outputImg = fopen(fout, "wb");

    if(outputImg == NULL){

        printf("Error: Could not open file %s in mode wb", fout);
        exit(1);

    }//end if

    //alien alphabet in order
    char alphabet[] = {'@','#','M','B','H','A','&','G','h','9','3','X','2','5','S',
    'i','s','r',';',':',',','.',' ','\'','\"','%','=','+','!','_','\r','\n'};

    //used to keep track of matching bit
    unsigned long long temp;
    unsigned long long compareResult;
    unsigned long long test;

    int errors = N;

    //index value
    int bestMatch;
    int matchCount;
    int previousCount = N;
    int tempCount = 0;
    int index = 0;

    int errorOne;
    int errorTwo;
    int correctErrorOne;
    int correctErrorTwo;

    int i, j, k, l;

    int length = u*v;

    while(index < length){

    //for each each element in the image, grab the next encine and add it to the matrix
    //for each element in encine
    //for(i = 0; i < v; i++){

        //for(j = 0; j < u; j++){

            //grab the encin value
            temp = encin[index];
            errors = N;
            index++;
            //search for match
            //if one is found break the loop, else keep track of the closest match
            for(k = 0; k < (N*2); k++){

                //XOR it and count the left over ones
                compareResult = temp ^ lookup[k];
                matchCount = 0;
                errorOne = 0;
                errorTwo = 0;

                //count unmatching ones
                for(l = 0; l < N; l++, compareResult >>=1){

                    matchCount += compareResult&1;

                    if((matchCount == 1)&&(compareResult&1)){

                        errorOne = l;

                    } else if ((matchCount == 2)&&(compareResult&1)){

                        errorTwo = l;

                    }

                }
                //printf("matchCount %i %i %i\n", matchCount, errorOne, errorTwo);

                //if there is a perfect match in count
                if(matchCount == 0){

                    bestMatch = k;
                    errors = 0;
                    //break out of the loop perfect match
                    break;


                } else if (matchCount < errors) {

                    correctErrorOne = errorOne;
                    correctErrorTwo = errorTwo;
                    bestMatch = k;
                    errors = matchCount;
                }
            }

            if(errors != 0){

                if(errors == 2){

                    correctErrorOne = (N/2 - 1) - correctErrorOne;
                    correctErrorTwo = (N-1) - correctErrorTwo;
                    fputc(alphabet[correctErrorTwo], outputImg);
                    fputc(alphabet[correctErrorOne], outputImg);

                } else {

                    if(correctErrorOne > 31){

                        correctErrorOne = (N-1) - correctErrorOne;
                        fputc(alphabet[correctErrorOne], outputImg);

                    } else {
                        correctErrorOne = (N/2 - 1) - correctErrorOne;
                        fputc(alphabet[correctErrorOne], outputImg);
                    }
                }
            }
        //}
    }
	fclose(outputImg);
}
