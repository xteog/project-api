#include <stdio.h>
#include <stdlib.h>
#include <string.h> // TODO togli
#define PLUS '+'
#define Y '|'
#define N '/'
#define DIM 64
#define MIN_LEN 17
#define INT 4
#define MEM_DIM 1048576
#define STR_DIM 10


typedef struct node{
    char c;
    struct node *nextChar;
    struct node *nextSon;
} node_t;

typedef struct filter{
    char c;
    int n;
    int pos;
    struct filter *next;
} filter_t;

typedef struct mem{
    void* pointer;
    int full;
    struct mem *next;
} mem_t;

mem_t *mem=NULL;


void input_int(int*);
int char_index(char);
void input_str(char*);
void str_init(char**, int);
int string_compare(const char*, const char*);
filter_t* add_filter(filter_t*, char, int, int);
filter_t* add_contFilter(filter_t*, char, int, int);
void free_filter(filter_t*, int);
node_t* input_tree(node_t*, char*);
node_t* insert_word(node_t*, const char*);
int check_exists(node_t*, const char*, int);
void letters_occurrences(const char*, int*, int);
filter_t* check_guess(const char*, const char*, char*, filter_t*, filter_t**, int*, int, int*);
int stampa_filtrate(node_t*, filter_t*, filter_t*, int, int);
void free_mem(int);
void mem_init();
void* my_malloc(int);


int main() {
    char *word, *guess, *checked, *strFiltered[STR_DIM];
    int wordLen, attempts, i, win = 0, run = 1, cont[DIM], flag = 1;
    node_t* tree;
    filter_t *filter, *contFilter;
    tree = NULL;
    filter = NULL;
    contFilter = NULL;

    input_int(&wordLen);
    word = my_malloc(wordLen+1);
    checked = my_malloc(wordLen+1);
    if(wordLen < 17)
        guess = my_malloc(MIN_LEN+1);
    else
        guess = my_malloc(wordLen+1);

    str_init(strFiltered, wordLen+1);

    tree = input_tree(tree, guess);
    if(string_compare(guess, "+inserisci_inizio")){
        tree = input_tree(tree, guess);
        input_str(guess);
    }

    input_str(word);
    input_int(&attempts);
    i = 0;
    while(run){
        input_str(guess);
        if(i < attempts && win == 0 && guess[0] != PLUS){
            if(check_exists(tree, guess, wordLen)){
                letters_occurrences(word, cont, wordLen); // TODO non scansionare se rimangono pochi elementi (fai un array)
                if(flag)
                    filter = check_guess(word, guess, checked, filter, &contFilter, cont, wordLen, &win);
                else
                    filter = check_guess_lighter(word, guess, checked, filter, &contFilter, strFilter, cont, wordLen, &win);
                if(cont < 10)
                    flag = 0;
                if(win)
                    printf("ok\n");
                else
                    printf("%s\n%d\n", checked, stampa_filtrate(tree, filter, contFilter, wordLen, 0));
                i++;
            } else
                printf("not_exists\n");
        } else {
            if(i == attempts && win == 0){
                printf("ko\n");
                win = 1;
            }
            if(string_compare(guess, "+nuova_partita")){
                win = 0;
                i = 0;
                flag = 1;
                free_filter(filter, 0);
                free_filter(contFilter, 0);
                contFilter = NULL;
                filter = NULL;
                input_str(word);
                input_int(&attempts);
            } else if(guess[0] != PLUS)
                run = 0;
        }

        if(string_compare(guess, "+inserisci_inizio"))
            tree = input_tree(tree, guess);
        else if (string_compare(guess, "+stampa_filtrate"))
            stampa_filtrate(tree, filter, contFilter, wordLen, 1);
    }

    free_mem(0);

    return 0;
}


void input_int(int *n){
    int c;

    *n = 0;
    c = getchar_unlocked();
    while(c >= '0' && c <= '9'){
        *n = *n * 10 + c - '0';
        c = getchar_unlocked();
    }
}


int char_index(const char c){
    int i = -1;

    if(c == '-')
        i = 0;
    else if(c >= '0' && c <= '9')
        i = c - '0' + 1;
    else if(c >= 'A' && c <= 'Z')
        i = c - 'A' + 1 + 10;
    else if(c == '_')
        i = 1 + 10 + 26;
    else if(c >= 'a' && c <= 'z')
        i = c - 'a' + 1 + 10 + 26 + 1;

    return i;
}


void input_str(char *str){
    int c;
    int i = 0;

    c = getchar_unlocked();
    while(c != '\n' && c != -1){
        str[i] = (char) c;
        c = getchar_unlocked();
        i++;
    }
    str[i] = '\0';
}


void str_init(char* str[STR_DIM], const int len){
    int i;

    for(i = 0; i < STR_DIM; i++){
        str[i] = my_malloc(len);
        str[i][len-1] = '\0';
    }
}



int string_compare(const char* first, const char* second){
    int i;

    for(i = 0; first[i] != '\0' && first[i] == second[i]; i++);

    return first[i] == '\0' && first[0] != '\0';
}


filter_t* add_filter(filter_t *h, const char c, const int p, const int max){
    filter_t *tmp, *curr, *prec;
    int flag;

    tmp = my_malloc(sizeof(filter_t));

    if(tmp){
        tmp->n = max;
        tmp->c = c;
        if(max == 0)
            tmp->pos = 0;
        else
            tmp->pos = p;
        tmp->next = NULL;
        if(h && c >= h->c && h->n != -1){
            for(prec = h, curr = h; curr && curr->c < c && curr->n != -1; prec = curr, curr = curr->next);
            if(curr != NULL && curr->c == c && curr->n != -1){
                if(max == 0){
                    if(curr->n == 0)
                        free_mem(24);
                    else{
                        tmp->pos = p;
                        tmp->next = curr->next;
                        curr->next = tmp;
                    }
                }
                else{
                    for(prec = curr, flag = 1; curr && curr->c == c && flag && curr->n != -1; curr = curr->next)
                        if(curr->pos == p)
                            flag = 0;
                    if(flag){
                        tmp->n = 1;
                        tmp->next = prec->next;
                        prec->next = tmp;
                    }
                    else
                        free_mem(24);
                    if(max > prec->n)
                        prec->n = max;
                }
            }
            else{
                tmp->next = curr; //TODO ottimizza
                prec->next = tmp;
            }
        }
        else{
            tmp->n = max;
            tmp->next = h;
            h = tmp;
        }
    } else
        printf("Error add_filter\n");

    return h;
}


filter_t* add_contFilter(filter_t *h, const char c, const int max, const int flag){
    filter_t *prec, *curr, *tmp;

    if(h && c >= h->c && h->n != -1){
        for(prec = h, curr = h; curr && c > curr->c && curr->n != -1; prec = curr, curr = curr->next);
        if(curr != NULL && c == curr->c && curr->n != -1) {
            if(curr->n < max)
                curr->n = max;
            curr->pos = flag;
        }
        else{
            tmp = my_malloc(sizeof(filter_t));
            tmp->c = c;
            tmp->n = max;
            tmp->pos = flag;
            tmp->next = curr;
            prec->next = tmp;
        }
    }
    else{
        tmp = my_malloc(sizeof(filter_t));
        tmp->c = c;
        tmp->n = max;
        tmp->pos = flag;
        tmp->next = h;
        h = tmp;
    }

    return h;
}


void free_filter(filter_t *h, const int flag){
    filter_t *tmp;

    while(h){
        tmp = h;
        h = h->next;
        if(flag)
            free(tmp);
        else
            tmp->n = -1;
    }
}


node_t* input_tree(node_t *tree, char *str){
    input_str(str);
    while(str[0] != PLUS){
        tree = insert_word(tree, str);
        input_str(str);
    }

    return tree;
}


node_t* insert_word(node_t* tree, const char* word){
    int i, flag;
    node_t *curr, *tmp, *prec;
    flag = 1;
    i = 0;

    if(tree == NULL){
        tree = my_malloc(24);
        tree->c = word[0];
        tree->nextSon = NULL;
        tree->nextChar = NULL;
        flag = 0;
        i = 1;
    }
    curr = tree;
    prec = tree;

    //TODO make lighter
    for(; word[i] != '\0'; i++){
        tmp = my_malloc(24);
        tmp->nextSon = NULL;
        tmp->nextChar = NULL;
        if(flag){
            if(curr->c < word[i]) {
                for(prec = curr; curr && curr->c < word[i]; prec = curr, curr = curr->nextChar);
                if(curr != NULL && curr->c == word[i]){
                    free_mem(24);
                    prec = curr;
                    curr = curr->nextSon;
                }
                else{
                    tmp->c = word[i];
                    tmp->nextChar = curr;
                    prec->nextChar = tmp;
                    curr = prec->nextChar;
                    flag = 0;
                }
            }
            else{
                if(curr->c != word[i]) {
                    tmp->c = word[i];
                    tmp->nextChar = curr;
                    if(curr == tree){
                        tree = tmp;
                        curr = tree;
                    }
                    else{
                        prec->nextSon = tmp;
                        curr = prec->nextSon;
                    }
                    flag = 0;
                }
                else{
                    free_mem(24);
                    prec = curr;
                    curr = curr->nextSon;
                }
            }
        }
        else{
            tmp->c = word[i];
            curr->nextSon = tmp;
            curr = curr->nextSon;
        }
    }

    return tree;
}


int check_exists(node_t* tree, const char *word, const int len){
    int flag, i;

    //TODO use binary search (until reaches index higher than word[i])
    for(i = 0, flag = 1; i < len && flag; i++, tree = tree->nextSon) {
        for (; tree->nextChar != NULL && tree->c != word[i]; tree = tree->nextChar);
        if(tree->nextChar == NULL && tree->c != word[i])
            flag = 0;
    }

    return flag;
}


void letters_occurrences(const char *word, int *cont, const int len){
    int i;

    memset(cont, 0, DIM * INT);

    for(i = 0; i < len; i++)
        cont[char_index(word[i])]++;
}


filter_t *check_guess(const char* first, const char* second, char *checked, filter_t *filter, filter_t **contFilter, int *cont, const int len, int *win){
    int i, j, max, m, flag;
    filter_t *curr, *h;

    // usa la lista filter al posto di cont
    memset(checked, '0', len+1);
    checked[len] = '\0';
    for(i = 0, j = 0; i < len; i++)
        if(first[i] == second[i]) {
            checked[i] = PLUS;
            cont[char_index(first[i])]--;
            j++;
            for(m = 0, max = 0; m < len; m++)
                if(checked[m] == PLUS && second[m] == second[i])
                    max++;
            filter = add_filter(filter, second[i], i + 1, max);
        }

    if(j != len){
        for(i = 0; i < len; i++){
            j = char_index(second[i]);
            if(cont[j] > 0 && checked[i] == '0'){
                checked[i] = Y;
                for(m = 0, max = 0; m < len; m++)
                    if((checked[m] == PLUS || checked[m] == Y) && second[m] == second[i])
                        max++;
                filter = add_filter(filter, second[i], -i - 1, max);
                cont[j]--;
            }
            else if(cont[j] == 0 && checked[i] == '0'){
                checked[i] = N;
                filter = add_filter(filter, second[i], -i - 1, 0);
            }
        }
    } else
        *win = 1;

    curr=filter;
    while(curr){
        if(curr->n != 0){
            for(h = curr, flag = 1; h && h->c == curr->c; h = h->next)
                if(h->n == 0)
                    flag = 0;
            *contFilter = add_contFilter(*contFilter, curr->c, curr->n, !flag);
            curr = h;
        }
        else
            curr = curr->next;
    }

    return filter;
}


int stampa_filtrate(node_t* tree, filter_t* filter, filter_t* contFilter, const int wordLen, const int cond){
    filter_t *curr_1;
    int flag, run = 1, i = 0, j, cont = 0, cont2;
    node_t *path[wordLen], *h;
    h = tree;

    //TODO crea un array ausiliario sìdove salvare cont e modifica quello
    //TODO salvare filter all'interno di un array e poi fare ricerca binaria
    while(run){
        if(h == NULL && i == 0){
            run = 0;
        }
        else if(h == NULL){
            i--;
            h = path[i]->nextChar; // --i

            if(i == wordLen - 1){
                for(curr_1 = contFilter, flag = 1; curr_1 && flag; curr_1 = curr_1->next) {
                    for (j = 0, cont2 = 0; j < wordLen; j++)
                        if(path[j]->c == curr_1->c)
                            cont2++;
                    if((cont2 < curr_1->n && !(curr_1->pos)) || (cont2 != curr_1->n && curr_1->pos))
                        flag = 0;
                }
                if(flag){
                    if(cond){
                        for(j = 0; j < wordLen; j++)
                            printf("%c", path[j]->c); // TODO salvale dentro una stringa con il path per semplificare stampa filtrate
                        printf("\n");
                    }
                    cont++;
                }
            }
            path[i] = NULL;
        }
        else{
            for(curr_1=filter, flag = 1; curr_1 && flag == 1 && curr_1->n != -1; curr_1 = curr_1->next) {
                if(curr_1->c == h->c){
                    if (curr_1->pos == 0)
                        flag = 0;
                    else if (curr_1->pos < 0){
                        if (-curr_1->pos == i + 1)
                            flag = 0;
                    }
                    else if (curr_1->pos == i + 1 && curr_1->n != 0)
                        flag = 2;
                }
                else if (curr_1->pos == i + 1 && curr_1->n != 0)
                    flag = 0; // in questa condiizione vai a cercare h corrispondente
            }
            if(flag){
                path[i++] = h;
                h = h->nextSon;
            }
            else{
                h = h->nextChar;
            }
        }
    }

    return cont;
}


int stampa_filtrate_lighter(char** strFilter, filter_t* filter, filter_t* contFilter, const int wordLen, const int cond){
    filter_t *curr_1;
    int flag, run = 1, i = 0, j, cont = 0, cont2;

    for(i = 0; i < STR_DIM; i++)



    return cont;
}


void free_mem(const int size){
    mem_t *tmp;

    if(size == 0)
        while(mem){
            tmp = mem;
            mem = mem->next;
            free(tmp->pointer);
            free(tmp);
        }
    else
        mem->full -= size;
}


void mem_init(){
    mem_t *tmp;

    tmp = malloc(sizeof(mem_t));
    tmp->pointer = malloc(MEM_DIM);
    tmp->full = 0;
    tmp->next = mem;
    mem = tmp;
}


void* my_malloc(const int size){
    void *address;

    if(mem == NULL)
        mem_init();
    else if(mem->full + size >= MEM_DIM)
        mem_init();

    address = mem->pointer + mem->full;
    mem->full += size;

    return address;
}
