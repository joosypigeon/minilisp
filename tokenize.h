#ifndef TOKENIZE_H
#define TOKENIZE_H

extern int current_token;
extern int num_tokens;
extern char *tokens[];

void tokenize(const char* input);
char *read_file(const char *filename);

#endif // TOKENIZE_H