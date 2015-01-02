// create the hadamard matrix
void create_hadamard_matrix(char** h, int N);
// convert to lookup table
void convert_to_lookup_table(unsigned long long* lookup, char **h, int N);
// read in a pgm image
void pgm_read(char* fin, char** img, int u, int v);
// write out a pgm image
void pgm_write(char* fout, char** img,  int format, int u, int v, int depth);
// write the encoded image out 
void enc_write(char* fout, char** img, int u, int v, unsigned long long* lookup);
// read the encoding in
void enc_read(unsigned long long* encin, char* fin, int u, int v);
// decode the image
void dec_img(char** img, unsigned long long* encin, unsigned long long* lookup, int u, int v, int N);
// pack encoded image and write to file
void enc_pack(char* fin, char* fout);
// unpack encoded image and write to file
void enc_unpack(char* fin, char* fout);
// extract message in hadamard encoding
void dec_msg(char* fout, unsigned long long* encin, unsigned long long* lookup, int u, int v, int N);