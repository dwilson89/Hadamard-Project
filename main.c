#include "hadamard.h"
#include <stdio.h> 
#include <stdlib.h>

void printMessage();

int main(int argc, char *argv[])
{
    //tables and matrices
    char **h;
    char **img;
    unsigned long long* lookup;
    unsigned long long* encin;

    const int N = 64;

    //input vars
    char option;
    char* fin;
    char* fout;
    int format;
    int u;
    int v;
    int depth;

    int i;
    
	//note: if assigning the *chars try, fin = argv[2]

    if (argc < 4) { //Check we have the right number of input arguments

        printMessage();

    } else {

        sscanf(argv[1], "-%c", &option); //read in the option

        //if statement for the two that dont require memory allocation
        //create everyting and assign memory
        //alternate u can just intialise these and free up the space in these
        //function which dont need these
        if((option != 'p')&&(option != 'u')){


            //allocate momemory to the h matrix and look up table
            h = malloc(N*sizeof(char*));
            for (i = 0; i < N; i++) {
                h[i] = malloc(N*sizeof(char));
            }

            create_hadamard_matrix(h, N);

            lookup = malloc((N*2)*sizeof(long long));

            convert_to_lookup_table(lookup, h, N);

            //free up h
            for (i = 0; i < N; i++) {
                free(h[i]);
            }
            free(h);
        }

        switch(option){

            //takes 4 arguments check all four are there
            //-e encrypt file 1 and place in file 2 using u v
            case 'e':

                //including all
                if (argc < 6) { //Check we have the right number of input arguments

                    printMessage();
                    break;

                } else {

                    fin = argv[2];
                    fout = argv[3];
                    sscanf(argv[4],"%i", &u);
                    sscanf(argv[5],"%i", &v);

                }

                //allocate img memory.
                img = (char**) malloc(v * sizeof(char*));

                for (i = 0; i < u; i++ )
                {
                    img[i] = (char*) malloc(u * sizeof(char));
                }

                pgm_read(fin, img, u, v);
                enc_write(fout, img, u, v, lookup);
                break;

            //takes 6 arguments
            //-d decrypt file 1 and place in file 2 using format u v depth
            case 'd':

                //including all
                if (argc < 8) { //Check we have the right number of input arguments

                    printMessage();
                    break;

                }  else {

                    fin = argv[2];
                    fout = argv[3];
                    sscanf(argv[4], "%i", &format);
                    sscanf(argv[5], "%i", &u);
                    sscanf(argv[6], "%i", &v);
                    sscanf(argv[7], "%i", &depth);

                }

                //need to intialise encin and img
                //allocate img memory.
                img = (char**) malloc(v * sizeof(char*));

                for (i = 0; i < u; i++ )
                {
                    img[i] = (char*) malloc(u * sizeof(char));
                }

                encin = malloc((v*u)*sizeof(long long));

                enc_read(encin, fin, u, v);
                dec_img(img, encin, lookup, u, v, N);
                pgm_write(fout, img, format, u, v, depth);

                break;

            //takes 2 arguments
            //-p pack encoded data from file 1 into file 2
            case 'p':

                //including all
                if (argc < 4) { //Check we have the right number of input arguments

                    printMessage();
                    break;

                } else {

                    fin = argv[2];
                    fout = argv[3];

                }

                enc_pack(fin, fout);

                break;

            //takes 2 arguments
            //-u unpack encoded data from file 1 into file 2
            case 'u':

                //including all
                if (argc < 4) { //Check we have the right number of input arguments

                    printMessage();
                    break;

                } else {

                    fin = argv[2];
                    fout = argv[3];

                }

                enc_unpack(fin, fout);

                break;

            //takes 4 arguments
            //-m extract alien message from file 1 and place in file 2 using u v
            case 'm':

                //including all
                if (argc < 6) { //Check we have the right number of input arguments

                    printMessage();
                    break;

                } else {

                    fin = argv[2];
                    fout = argv[3];
                    sscanf(argv[4], "%d", &u);
                    sscanf(argv[5], "%d", &v);

                }

                encin = malloc((u*v)*sizeof(long long));

                enc_read(encin, fin, u, v);
                dec_msg(fout, encin, lookup, u, v, N);

                break;

            default:

                printMessage();
                break;
        }
    }
	
	return 0;
}

void printMessage(){

    printf("Usage: hadamard [option] <file 1> <file 2> {format u v depth}\n\n");
    printf("Option:\n\n");
    printf(" -e encrypt file 1 and place in file 2 using u v\n");
    printf(" -d decrypt file 1 and place in file 2 using format u v depth\n");
    printf(" -p pack encoded data from file 1 into file 2\n");
    printf(" -u unpack encoded data from file 1 into file 2\n");
    printf(" -m extract alien message from file 1 and place in file 2 using u v\n");

}
