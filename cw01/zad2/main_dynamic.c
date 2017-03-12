#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include "mylib.h"
#include <dlfcn.h>

int main(void) {
    clock_t real1 = clock();
    struct tms * time1 = NULL;
    time1 = malloc(sizeof(struct tms));
    times(time1);
    printf("Real   time: %ld\n",((clock()-real1)*100)/CLOCKS_PER_SEC); 
    printf("User   time: %ld\n",time1->tms_utime); 
    printf("System time: %ld\n",time1->tms_stime);  
    printf("\n\n");
    void * dll = dlopen("./mylib.so",RTLD_LAZY);
     int (*compare_name)(node *, node *)=dlsym(dll,"compare_name");
     int (*compare_name_desc)(node *, node *)=dlsym(dll,"compare_name_desc");
     int (*compare_date)(node *, node *)=dlsym(dll,"compare_date");
     int (*compare_date_desc)(node *, node *)=dlsym(dll,"compare_date_desc");
     void (*sort_list)(list *, int (*comparator)(node*,node*))=dlsym(dll,"sort_list");
     list * (*create_list)(void)=dlsym(dll,"create_list");
     void (*add_contact_to_list)(list *,char *, char *, int, int, int, int, char *, char *, char *, char *, char *)=dlsym(dll,"add_contact_to_list");
     node * (*find_by_name)(list *,char *, char *)=dlsym(dll,"find_by_name");
     void (*remove_contact)(list *, node *)=dlsym(dll,"remove_contact");
     void (*delete_list)(list *)=dlsym(dll,"delete_list");
     void (*print_contact)(node *)=dlsym(dll,"print_contact");
     void (*print_contacts)(list *)=dlsym(dll,"print_contacts");
    list * l = create_list();
    int i;
    char * names[22]={"Andrzej","Mariusz","Zbigniew","Aleksander","Alojzy","Marek","Francisek","Leopold","Milosz","Adolf","Jaroslaw","Ryszard","Adrian","Piotr","Janusz","Jakub","Antoni","Angela","Wladimir","Monika","Donald","Barack"};
    char * surnames[26]={"Lis","Kot","Stonoga","Pawlak","Kargul","Brzeczyszczykiewicz","Kowalski","Nowak","Majecki","Zielinski","Kaczynski","Obama","Tusk","Putin","Merkel","Korwin-Mikke","Zandberg","Platek","Wojewodzki","Komorowski","Ziobro","Macierewicz","Duda","Ikonowicz","Petru","Palikot"};
    srand(10);

    for (i=0;i<10000;i++){
        add_contact_to_list(l,names[rand()%22],surnames[rand()%26],rand()%28+1,rand()%12+1,1920+rand()%90,987654123,"ala@123","Kraj","Miasto","Ulica","numer");
    }
    struct tms * time2 = NULL;
    time2 = malloc(sizeof(struct tms));
    times(time2);
    printf("Real   time: %ld\n",((clock()-real1)*100)/CLOCKS_PER_SEC); 
    printf("User   time: %ld\n",time2->tms_utime-time1->tms_utime); 
    printf("System time: %ld\n",time2->tms_stime-time1->tms_stime);  
    printf("\n\n");
    delete_list(l);
    l=create_list();
    for (i=0;i<10000;i++) {
        add_contact_to_list(l, names[rand() % 22], surnames[rand() % 26], rand() % 28 + 1, rand() % 12 + 1, 1920 + rand() % 90, 987654123, "ala@123", "Kraj", "Miasto", "Ulica", "numer");
    }

    sort_list(l,compare_name);
    remove_contact(l,find_by_name(l,"Donald","Tusk"));
    struct tms * time3 = NULL;
    time3 = malloc(sizeof(struct tms));
    times(time3);
    printf("Real   time: %ld\n",((clock()-real1)*100)/CLOCKS_PER_SEC); 
    printf("User   time: %ld\n",time3->tms_utime-time1->tms_utime); 
    printf("System time: %ld\n",time3->tms_stime-time1->tms_stime); 
    printf("\n\n");
    sort_list(l,compare_date_desc);

    dlclose(dll);

    struct tms * time4 = NULL;
    time4 = malloc(sizeof(struct tms));
    times(time4);
    printf("Real   time: %ld\n",((clock()-real1)*100)/CLOCKS_PER_SEC); 
    printf("User   time: %ld\n",time4->tms_utime-time1->tms_utime); 
    printf("System time: %ld\n",time4->tms_stime-time1->tms_stime);
    return 0;
}
