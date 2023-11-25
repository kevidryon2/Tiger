#include <stdbool.h>
#include <stdint.h>

int needle(char *n, char **h, int lh);
char *compile(char *script);
void exec(char *binscript);
int search_begin(char **restrict array, int num_elements, char *restrict string);
int startswith(char *s, char *c);
int endswith(char *restrict s, char *restrict end);
char *combine(char *restrict a, char *restrict b);
char *ntoken(char *const s, char *d, int t);
int count(char *const s, char c, int l);
uint32_t parse_ip(char *const s);
