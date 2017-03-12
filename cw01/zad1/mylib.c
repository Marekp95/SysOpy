#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct address{
    char * country;
    char * city;
    char * street;
    char * house_number;
} address;
typedef struct date{
    int year;
    int month;
    int day;
} date;
typedef struct node{
    struct node * prev;
    struct node * next;
    char * name;
    char * surname;
    struct address * address;
    char * email;
    date * date_of_birth;
    long int phone;
} node;
typedef struct list{
    node * record;
} list;
int compare_name(node * left, node * right){
    if(strcmp(left -> surname,right -> surname)>0){
        return 1;
    }else if(strcmp(left -> surname,right -> surname)<0){
        return -1;
    }else if(strcmp(left -> name,right -> name)>0){
        return 1;
    }else if(strcmp(left -> name,right -> name)<0){
        return -1;
    }else
        return 0;
}
int compare_name_desc(node * left, node * right){
    return compare_name(right,left);
}
int compare_date(node* left, node* right){
    if(left->date_of_birth->year>right->date_of_birth->year)
        return 1;
    else if(left->date_of_birth->year<right->date_of_birth->year)
        return -1;
    else if(left->date_of_birth->month>right->date_of_birth->month)
        return 1;
    else if(left->date_of_birth->month<right->date_of_birth->month)
        return -1;
    else if(left->date_of_birth->day>right->date_of_birth->day)
        return 1;
    else if(left->date_of_birth->day<right->date_of_birth->day)
        return -1;
    else return 0;
}
int compare_date_desc(node* left, node* right){
    return compare_date(right,left);
}
void _sort(node * e,int (*comparator)(node *, node *)){
    while(e->prev!=NULL){
        e=e->prev;
    }
    node * result = NULL;
    while(e!=NULL){
        node * start = e;
        node * min = e;
        while(e->next!=NULL){
            if(comparator(min,e->next)>0){
                min = e->next;
            }
            e=e->next;
        }
        if(min->prev==NULL){
            e=min->next;
            if(e!=NULL)
                e->prev=NULL;
        }else if(min->next==NULL){
            if(min->prev!=NULL){
                min->prev->next=NULL;
                e=start;
            }
            else
                e=NULL;
        }else{
            min->prev->next=min->next;
            min->next->prev=min->prev;
            e=start;
        }
        if(result==NULL){
            result=min;
            result->next=NULL;
            result->prev=NULL;
        }else{
            result->next=min;
            min->prev=result;
            min->next=NULL;
            result=result->next;
        }
    }
    return;
}
void sort_list(list * l, int (*comparator)(node *, node *)){
    _sort(l->record,comparator);
    return;
}
list * create_list(){
    list * l=NULL;
    l=malloc(sizeof(list));
    l->record=NULL;
    return l;
}
void add_contact_to_list(list * l,char * name, char * surname, int day, int month, int year, long int phone, char * email, char * country, char * city, char * street, char * house_number){
    node * n = NULL;
    n=malloc(sizeof(node));
    if(l->record==NULL){
        l->record=n;
        n->next=NULL;
        n->prev=NULL;
    }else{
        n->next=l->record;
        n->prev=l->record->prev;
        l->record->prev=n;
        if(n->prev!=NULL)
            n->prev->next=n;
    }
    n->name=malloc(strlen(name)+1);
    strcpy(n->name,name);
    n->surname=malloc(strlen(surname)+1);
    strcpy(n->surname,surname);
    n->phone=phone;
    n->email=malloc(strlen(email)+1);
    strcpy(n->email,email);
    n->date_of_birth=malloc(sizeof(date));
    n->date_of_birth->day=day;
    n->date_of_birth->month=month;
    n->date_of_birth->year=year;
    n->address=malloc(sizeof(address));
    n->address->city=malloc(strlen(city)+1);
    strcpy(n->address->city,city);
    n->address->country=malloc(strlen(country)+1);
    strcpy(n->address->country,country);
    n->address->street=malloc(strlen(street)+1);
    strcpy(n->address->street,street);
    n->address->house_number=malloc(strlen(house_number)+1);
    strcpy(n->address->house_number,house_number);
    return;
}
node * find_by_name(list * l,char * name, char * surname){
    if(l->record==NULL)
        return NULL;
    node * tmp = NULL;
    tmp=l->record;
    while(tmp!=NULL){
        if(strcmp(tmp->name,name)==0 && strcmp(tmp->surname,surname)==0){
            return tmp;
        }
        tmp=tmp->next;
    }
    tmp=l->record->prev;
    while(tmp!=NULL){
        if(strcmp(tmp->name,name)==0 && strcmp(tmp->surname,surname)==0){
            return tmp;
        }
        tmp=tmp->prev;
    }
    return NULL;
}
void remove_contact(list *l, node * n){
    if(n!=NULL) {
        l->record = NULL;
        if (n->next != NULL) {
            n->next->prev = n->prev;
            l->record = n->next;
        }
        if (n->prev != NULL) {
            n->prev->next = n->next;
            l->record = n->prev;
        }
        free(n->name);
        free(n->surname);
        free(n->email);
        free(n->address->country);
        free(n->address->city);
        free(n->address->street);
        free(n->address->house_number);
        free(n->address);
        free(n->date_of_birth);
        free(n);
    }
    return;
}
void delete_list(list * l){
    if(l->record!=NULL) {
        node * tmp = l->record;
        while(tmp->prev!=NULL)
            tmp = tmp -> prev;
        while(tmp!=NULL){
            node * current = tmp;
            tmp = tmp->next;
            remove_contact(l, current);
        }
    }
    free(l);
}
void print_contact(node * n){
    if(n!=NULL)
        printf("%s %s, ur: %d/%d/%d\n",n->name,n->surname,n->date_of_birth->day,n->date_of_birth->month,n->date_of_birth->year);
}
void print_contacts(list * l){
    if(l->record!=NULL){
        node * n = l->record;
        while(n->prev!=NULL){
            n=n->prev;
        }
        while(n!=NULL){
            print_contact(n);
            n=n->next;
        }
    }else
        printf("List is empty\n");
}
