#include <stdio.h>
#include <ncvm.h>

typedef unsigned char u8 ;

typedef struct BinSettings {
    unsigned int vm_version;
} BinSettings;

int main(int argc, char* argv[]) {
    unsigned char w_buffer[6] = {
        1, 2, 3, 4, 5, 6
    };

    FILE* wr = fopen("test.bin", "wb");
    fwrite(w_buffer, 1, 6, wr);
    fclose(wr);

    //unsigned char buffer[10];
    //FILE *ptr;
    //ptr = fopen("test.bin","rb");  // r for read, b for binary

    //fread(buffer,sizeof(buffer),1,ptr); // read 10 bytes to our buffer

    



    FILE *bin_file = fopen("test.bin", "rb");

    BinSettings settings;
    fread(&settings, sizeof(BinSettings), 1, bin_file);


    //fseek(fp, 0L, SEEK_END);
    //unsigned long sz = ftell(fp);

    //fseek(fp, 0L, SEEK_SET);
    unsigned char buffer[1];
    while (!feof(bin_file) && fread(buffer, 1, 1, bin_file) == 1) {
        
        printf("%d\n", buffer[0]);
    }
}
