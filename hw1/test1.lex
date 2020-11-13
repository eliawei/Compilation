%{
    #include <stdio.h>
    int num_chars = 0;
    int num_lines = 0;
    int num_strings = 0;
%}
string ([^\n\t ])+                /* optional macro definition */
%%
{string}            { ++num_strings;
                      num_chars += yyleng;}
\n                  { ++num_lines; }
\t                  ;            /* does nothing */
.                   ;            /* does nothing */

%%
int main() {
    yylex();        /* calls the function called yylex() */
    printf("The number of lines is %d\n", num_lines );
    printf("The number of characters is %d\n", num_chars );
    printf("The number of strings is %d\n", num_strings );
    exit(0);
}

int yywrap() {}    /* this is needed by some systems. */