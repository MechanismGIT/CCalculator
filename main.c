#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LEN 32
#define PI 3.141592653589793238

// Function to tokenize the input expression
char** tokenize(const char* expr, int* token_count) {
char** tokens = malloc(MAX_TOKENS * sizeof(char*));
*token_count = 0;
int i = 0;
int len = strlen(expr);

while (i < len) {
if (isspace(expr[i])) {
i++;
continue;
}

if (strchr("+-*/()", expr[i])) {
tokens[*token_count] = malloc(2);
tokens[*token_count][0] = expr[i];
tokens[*token_count][1] = '\0';
(*token_count)++;
i++;
} else if (isdigit(expr[i]) || expr[i] == '.') {
int start = i;
while (i < len && (isdigit(expr[i]) || expr[i] == '.')) {
i++;
}
int tok_len = i - start;
tokens[*token_count] = malloc(tok_len + 1);
strncpy(tokens[*token_count], expr + start, tok_len);
tokens[*token_count][tok_len] = '\0';
(*token_count)++;
} else {
fprintf(stderr, "Invalid character: %c\n", expr[i]);
for (int j = 0; j < *token_count; j++) {
free(tokens[j]);
}
free(tokens);
return NULL;
}
}

return tokens;
}

// Function to check if a token is a number
int is_number(const char *token) {
for (int i = 0; token[i]; i++) {
if (!isdigit(token[i]) && token[i] != '.') return 0;
}
return 1;
}

// Function to insert implicit multiplication
char** insert_implicit_multiplication(char **tokens, int len, int *new_len) {
char **result = malloc(sizeof(char*) * (len * 2));
int j = 0;

for (int i = 0; i < len; i++) {
result[j++] = strdup(tokens[i]);

if (i < len - 1) {
int curr_is_num_or_close = is_number(tokens[i]) || strcmp(tokens[i], ")") == 0;
int next_is_num_or_open = is_number(tokens[i+1]) || strcmp(tokens[i+1], "(") == 0;

if (curr_is_num_or_close && next_is_num_or_open) {
result[j++] = strdup("*");
}
}
}

*new_len = j;
return result;
}

// Function to get the precedence of an operator
int precedence(char op) {
if (op == '+' || op == '-') return 1;
if (op == '*' || op == '/') return 2;
return 0;
}

// Function to apply an operator to two operands
double apply_operator(double a, double b, char op) {
switch (op) {
case '+': return a + b;
case '-': return a - b;
case '*': return a * b;
case '/': return a / b;
default: return 0;
}
}

// Function to evaluate an expression in Reverse Polish Notation (RPN)
double evaluate_rpn(char** rpn_tokens, int rpn_len) {
double stack[MAX_TOKENS];
int stack_size = 0;

for (int i = 0; i < rpn_len; i++) {
if (is_number(rpn_tokens[i])) {
stack[stack_size++] = atof(rpn_tokens[i]);
} else {
double b = stack[--stack_size];
double a = stack[--stack_size];
stack[stack_size++] = apply_operator(a, b, rpn_tokens[i][0]);
}
}

return stack[0];
}

double evaluate_expression(char** fixed, int new_count) {
char* operator_stack[MAX_TOKENS];
int operator_stack_size = 0;
char* rpn_tokens[MAX_TOKENS];
int rpn_len = 0;

for (int i = 0; i < new_count; i++) {
char* token = fixed[i];

if (is_number(token)) {
rpn_tokens[rpn_len++] = token;
} else if (strcmp(token, "(") == 0) {
operator_stack[operator_stack_size++] = token;
} else if (strcmp(token, ")") == 0) {
while (operator_stack_size > 0 && strcmp(operator_stack[operator_stack_size - 1], "(") != 0) {
rpn_tokens[rpn_len++] = operator_stack[--operator_stack_size];
}
operator_stack_size--;
} else {
while (operator_stack_size > 0 &&
precedence(operator_stack[operator_stack_size - 1][0]) >= precedence(token[0])) {
rpn_tokens[rpn_len++] = operator_stack[--operator_stack_size];
}
operator_stack[operator_stack_size++] = token;
}
}

while (operator_stack_size > 0) {
rpn_tokens[rpn_len++] = operator_stack[--operator_stack_size];
}

return evaluate_rpn(rpn_tokens, rpn_len);
} 


// Main function to process the input expression and evaluate the result
int main() {
const char* expr = "3 + 4 * (2 - 1)(5 + 2)";
int count;

char** tokens = tokenize(expr, &count); // FIXED DECLARATION

printf("[---------------- TOKENIZED EQUATION ----------------]\n");

if (tokens) {
for (int i = 0; i < count; i++) {
printf("Token[%d]: %s\n", i, tokens[i]);
}

printf("\n[---------------- FIX IMPLICIT MULTIPLICATION ----------------]\n");

int new_count;
char** fixed = insert_implicit_multiplication(tokens, count, &new_count); // FIXED DECLARATION

for (int i = 0; i < new_count; i++) {
printf("Token[%d]: %s\n", i, fixed[i]);
}

printf("\n[---------------- EVALUATE ----------------]\n");
double result = evaluate_expression(fixed, new_count);
printf("Result: %f\n", result);
printf("\n\n\n");

for (int i = 0; i < count; i++) free(tokens[i]);
free(tokens);

for (int i = 0; i < new_count; i++) free(fixed[i]);
free(fixed);
}

return 0;
}