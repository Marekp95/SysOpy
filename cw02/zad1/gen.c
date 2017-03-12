#include <stdio.h>
#include <stdlib.h>

int main(int argc,char * argv[]) {
    if(argc>=4) {
        char * file_name = argv[1];
        int number_of_records = atoi(argv[3]);
        int size_of_record = atoi(argv[2]);
        FILE *file;
        file = fopen(file_name, "w");
        if (file != NULL) {
            printf("file created\n");
            srand(10);
            for (int i = 0; i < number_of_records; i++) {
                for (int j = 0; j < size_of_record; j++) {
                    putc((char) (rand() % 94 + 33), file);
                }
                if(i+1<number_of_records)
                    putc('\n', file);
            }
            fclose(file);
        }
        else {
            printf("file creation error\n");
        }
    }
    return 0;
}