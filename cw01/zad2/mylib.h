//#ifndef mylib.h
//#define mylib.h
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
    int phone;
} node;
typedef struct list{
    node * record;
} list;
int compare_name(node *, node *);
int compare_name_desc(node * , node *);
int compare_date(node*, node*);
int compare_date_desc(node*, node*);
void _sort(node *,int (*comparator)(node *, node *));
void sort_list(list * l, int (*comparator)(node *, node *));
list * create_list(void);
void add_contact_to_list(list *,char *, char *, int, int, int, int, char *, char *, char *, char *, char *);
node * find_by_name(list *,char *, char *);
void remove_contact(list *, node *);
void delete_list(list *);
void print_contact(node *);
void print_contacts(list *);
//#endif
