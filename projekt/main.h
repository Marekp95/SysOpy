#ifndef MAIN_H
#define MAIN_H

#define SIZE 7
#define STRUCT_SIZE (SIZE*2+2) * sizeof(int)
#define EKSPRES 3
#define POSPIESZNY 2
#define TOWAROWY 1
#define FALSE 0
#define TRUE 1

struct memory {
    int bonus;
    int sum_of_priority;
    int add_id;
    int get_id;
    int t[SIZE];
    int typ[SIZE];
    int is_signalized[SIZE];
};

#endif