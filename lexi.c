#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int MAXWS = 100;
int TYPE_LENGTH = 5;

enum token_type {
    T_SOMETHING = 0,
    T_OPERATION = 1,
    T_NUMBER = 2,
    T_VARIABLE = 3,
    T_STRING = 4
};

typedef struct token {
    enum token_type type;
    char *value;
} token;

enum node_type {
    N_VARIABLE = 0,
    N_OPERATION =  1,
    N_NUMBER = 2,
    N_STRING = 3
};

typedef struct ast_node {
    enum node_type type;
    char *symbol;

    union {
        struct ast_node **children; 
        int value;
    };
} ast_node;

int lex_file(FILE *file, struct token *tokens);

struct ast_node *parse_token(struct token *tokens, int *j);
struct ast_node *parse_number(struct token *tokens, int *j);
struct ast_node *parse_variable(struct token *tokens, int *j);
struct ast_node *parse_operation(struct token *tokens, int *j);
struct ast_node *parse_string(struct token *tokens, int *j);

void print_node(struct ast_node *node, int tabs);

void *execute_print(char *string);
void *execute_node(struct ast_node *node);
void *execute_operation(struct ast_node *node);
char *execute_string(struct ast_node *node);

int main(int argc, char **argv) {
    
    if (argc != 2) {
        printf("Usage: ./lexi [filename]\n");
        return 0;
    }

    FILE *file;
    file = fopen(argv[1], "r");
    struct token tokens[100];
    int count = 0;

    if (file) {
        count = lex_file(file, tokens);
    }
    
    int num_ops = 0;
    struct ast_node **node_list = malloc(sizeof(struct ast_node *) * 100);
    if (count > 0) {
        int j = 0;
        while (j < count) {
            node_list[num_ops] = parse_token(tokens, &j);
            num_ops += 1;
        }
    }
    
    // Print AST
    //if (num_ops) {
    //    printf("\nAST:\n");
    //    int i;
    //    for (i = 0; i < num_ops; i += 1) {
    //        print_node(node_list[i], 0);
    //    }
    //    printf("\n\n");
    //}

    if (num_ops) {
        int i;
        for (i = 0; i < num_ops; i += 1) {
            execute_node(node_list[i]);
        }
    }
    return 0;
}

void *execute_node(struct ast_node *node) {
    if (node->type == N_OPERATION) {
        return execute_operation(node);
    } else if (node->type == N_STRING) {
        return execute_string(node);
    } else {
        return NULL;
    }
}

void *execute_operation(struct ast_node *node) {
    if (strcmp(node->symbol, "print") == 0) {
        return execute_print((char *) execute_node(node->children[0]));
    }
    return NULL;
}

char *execute_string(struct ast_node *node) {
    return node->symbol;
}

void *execute_print(char *string) {
    printf("%s\n", string);
    return NULL;
}

void print_node(struct ast_node *node, int tabs) {
    int i;
    for (i = 0; i < tabs; i += 1) {
        printf("  ");
    }
    printf("%s\n", node->symbol);
    if (node->children) {
        print_node(node->children[0], tabs + 1);
    }
}

struct ast_node *parse_token(struct token *tokens, int *j) {
    struct ast_node *root = NULL;

    while (!root) {
        enum token_type type = tokens[*j].type;
        if (type == T_NUMBER) {
            root = parse_number(tokens, j);

        } else if (type == T_VARIABLE) {
            root = parse_variable(tokens, j); 
        
        } else if (type == T_OPERATION) {
            root = parse_operation(tokens, j); 
        
        } else if (type == T_STRING) {
            root = parse_string(tokens, j); 
        }
        
        // Here's where we skip over the mumbo jumbo
        if (!root) {
            *j += 1;
        }
    }
    return root;
}

struct ast_node *parse_string(struct token *tokens, int *j) {
    struct ast_node *string_node = malloc(sizeof(struct ast_node));
    string_node->type = N_STRING;
    
    char no_quotes[100];
    int i = 0;
    while (tokens[*j].value[i + 2] != '\0') {
        no_quotes[i] = tokens[*j].value[i + 1];
        i += 1;
    }

    string_node->symbol = strdup(no_quotes);
    string_node->children = NULL;
    *j = *j + 1;
    return string_node;
}

struct ast_node *parse_operation(struct token *tokens, int *j) {
    struct ast_node *operation_node = malloc(sizeof(struct ast_node));
    operation_node->type = N_OPERATION;
    operation_node->symbol = strdup(tokens[*j].value);
    operation_node->children = malloc(sizeof(struct ast_node*) * 2);
    *j = *j + 1;
    operation_node->children[0] = parse_token(tokens, j); 
    return operation_node;
}

struct ast_node *parse_variable(struct token *tokens, int *j) {
    struct ast_node *var_node = malloc(sizeof(struct ast_node));
    var_node->type = N_VARIABLE;
    var_node->symbol = strdup(tokens[*j].value);
    var_node->children = NULL;
    *j += 1;
    return var_node;
}

struct ast_node *parse_number(struct token *tokens, int *j) {
    char *end;
    struct ast_node *number_node = malloc(sizeof(struct ast_node));
    number_node->type = N_NUMBER;
    number_node->symbol = "int";
    number_node->value = strtol(tokens[*j].value, &end, 10);
    number_node->children = NULL;
    *j = *j + 1;
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

    regex_arr[4] = "\"[[:alnum:]]*\"";
    types_arr[4] = T_STRING;
    
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

