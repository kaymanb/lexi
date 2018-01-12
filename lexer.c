#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int MAXWS = 100;
int TYPE_LENGTH = 4;

enum token_type {
    T_SOMETHING = 0,
    T_OPERATION = 1,
    T_NUMBER = 2,
    T_VARIABLE = 3
};

typedef struct token {
    enum token_type type;
    char *value;
} token;

enum node_type {
    N_VARIABLE = 0,
    N_OPERATION =  1,
    N_NUMBER = 2
};

typedef struct ast_node {
    enum node_type type;
    char *symbol;

    union {
        struct ast_node **children; 
        int value;
    };
} ast_node;

static int lex_file(FILE *file, struct token *tokens);

static struct ast_node *parse_number(struct token *tokens, int *j);
static struct ast_node *parse_variable(struct token *tokens, int *j);
static struct ast_node *parse_operation(struct token *tokens, int *j);

int main() {
    
    FILE *file;
    file = fopen("dummy.lxi", "r");
    struct token tokens[100];
    int count = 0;

    if (file) {
        printf("Starting lexing...\n");
        count = lex_file(file, tokens);
    }

    // *** PARSER *** //
    printf("Starting parsing...\n");
    
    int j = 0;
    struct ast_node *root = NULL;

    while (j < count) {
        enum token_type type = tokens[j].type;
        if (type == T_NUMBER) {
            root = parse_number(tokens, &j);

        } else if (type == T_VARIABLE) {
            root = parse_variable(tokens, &j); 
        
        } else if (type == T_OPERATION) {
            root = parse_operation(tokens, &j); 
        }
        j += 1;
    }
    printf("%s\n", root->symbol);
    return 0;
}

struct ast_node *parse_operation(struct token *tokens, int *j) {
    int index = *j;
    struct ast_node *operation_node = malloc(sizeof(struct ast_node));
    operation_node->type = N_OPERATION;
    operation_node->symbol = strdup(tokens[index].value);
    operation_node->children = malloc(sizeof(struct ast_node*) * 2);
    operation_node->children[0] = NULL; 
    operation_node->children[0] = NULL; 
    *j = index + 1;
    return operation_node;
}

struct ast_node *parse_variable(struct token *tokens, int *j) {
    int index = *j;
    struct ast_node *var_node = malloc(sizeof(struct ast_node));
    var_node->type = N_VARIABLE;
    var_node->symbol = strdup(tokens[index].value);
    return var_node;
}

struct ast_node *parse_number(struct token *tokens, int *j) {
    int index = *j;
    char *end;
    struct ast_node *number_node = malloc(sizeof(struct ast_node));
    number_node->type = N_NUMBER;
    number_node->value = strtol(tokens[index].value, &end, 10);
    *j = index + 1;
    return number_node;
}

int lex_file(FILE *file, struct token *tokens) {
    
    char word[MAXWS];

    char *regex_arr[TYPE_LENGTH];
    enum token_type types_arr[TYPE_LENGTH];

    regex_arr[0] = "^:[[:alnum:]]";
    types_arr[0] = T_VARIABLE;

    regex_arr[1] = "^[[:digit:]]";
    types_arr[1] = T_NUMBER;

    regex_arr[2] = "^[[:alpha:]][[:alpha:]]*";
    types_arr[2] = T_SOMETHING;

    regex_arr[3] = "print";
    types_arr[3] = T_OPERATION;
    
    int count = 0;
    while((fscanf(file, "%99s", word)) > 0) {

        int i;
        for (i = 0; i < TYPE_LENGTH; i += 1) {

            regex_t regex;
            int reti;
             
            reti = regcomp(&regex, regex_arr[i], 0);

            if (reti) {
                printf("Error compiling regex :(\n");
                exit(1);
            }

            reti = regexec(&regex, word, 0, NULL, 0);
            if (!reti) {
                tokens[count].type = types_arr[i];
                tokens[count].value = strdup(word);
            }
            
            regfree(&regex);
        }
        count += 1;
    }
    
    return count;
}





