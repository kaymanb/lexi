#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int MAXWS = 100;
int TYPE_LENGTH = 4;

typedef struct token {
    char *type;
    char *value;
} token;

static int lex_file(FILE *file, struct token *tokens);

int main() {
    
    FILE *file;
    file = fopen("dummy.lxi", "r");
    struct token tokens[100];
    int count = 0;

    if (file) {
        printf("Starting lexing...\n");
        count = lex_file(file, tokens);
    }

    int j;
    for(j = 0; j < count; j += 1) {
        printf("%s : %s\n", tokens[j].value, tokens[j].type);
    }

    // *** PARSER *** //
    printf("\nStarting parsing...\n");

    
}

int lex_file(FILE *file, struct token *tokens) {
    
    char word[MAXWS];

    char *regex_arr[TYPE_LENGTH];
    char *types_arr[TYPE_LENGTH];

    regex_arr[0] = "^:[[:alnum:]]";
    types_arr[0]= "VARIABLE";

    regex_arr[1] = "^[[:digit:]]";
    types_arr[1] = "NUMBER";

    regex_arr[2] = "^[[:alpha:]][[:alpha:]]*";
    types_arr[2] = "SOMETHING";

    regex_arr[3] = "print";
    types_arr[3] = "KEYWORD";
    
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
                tokens[count].type = strdup(types_arr[i]);
                tokens[count].value = strdup(word);
            }
            
            regfree(&regex);
        }
        count += 1;
    }
    
    return count;
}





