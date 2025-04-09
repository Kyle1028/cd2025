#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_LEN 100

int in_char;
char current_word[MAX_WORD_LEN];

typedef enum {
    TYPE_TOKEN = 1,
    IF_TOKEN,
    MAIN_TOKEN,
    ELSE_TOKEN,
    WHILE_TOKEN,
    EQUAL_TOKEN = 11,
    GREATER_TOKEN,
    LESS_TOKEN,
    ASSIGN_TOKEN,
    SEMICOLON_TOKEN,
    LEFTPAREN_TOKEN,
    RIGHTPAREN_TOKEN,
    LEFTBRACE_TOKEN,
    RIGHTBRACE_TOKEN,
    LITERAL_TOKEN,
    PLUS_TOKEN,
    MINUS_TOKEN,
    GREATER_EQUAL_TOKEN,
    LESS_EQUAL_TOKEN,
    ID_TOKEN = 18
} TokenType;

int isalpha_custom(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isdigit_custom(int c) {
    return c >= '0' && c <= '9';
}

int isalnum_custom(int c) {
    return isalpha_custom(c) || isdigit_custom(c) || c == '_';
}

int is_fullwidth_operator(unsigned char c1, unsigned char c2, unsigned char c3) {
    if (c1 == 0xEF) {
        if (c2 == 0xBC) {
            switch (c3) {
                case 0x88: return '(';
                case 0x89: return ')';
                case 0xBB: return ';';
                case 0x9A: return ':'; // 全形冒號
                case 0x9C: return '<';
                case 0x9E: return '>';
            }
        } else if (c2 == 0xBD) {
            switch (c3) {
                case 0x9B: return '{';
                case 0x9D: return '}';
            }
        }
    }
    return 0;
}

int is_keyword(const char *word) {
    if (strcmp(word, "int") == 0 || strcmp(word, "return") == 0)
        return TYPE_TOKEN;
    if (strcmp(word, "if") == 0)
        return IF_TOKEN;
    if (strcmp(word, "main") == 0)
        return MAIN_TOKEN;
    if (strcmp(word, "else") == 0)
        return ELSE_TOKEN;
    if (strcmp(word, "while") == 0)
        return WHILE_TOKEN;
    return 0;
}

int get_id(FILE *fp) {
    int i = 0;
    while (isalnum_custom(in_char)) {
        current_word[i++] = in_char;
        in_char = fgetc(fp);
    }
    current_word[i] = '\0';

    if (in_char != EOF)
        ungetc(in_char, fp);

    int token = is_keyword(current_word);
    return token ? token : ID_TOKEN;
}

int get_number(FILE *fp) {
    int i = 0;
    while (isdigit_custom(in_char)) {
        current_word[i++] = in_char;
        in_char = fgetc(fp);
    }
    current_word[i] = '\0';

    if (in_char != EOF)
        ungetc(in_char, fp);

    return LITERAL_TOKEN;
}

int get_operator(FILE *fp) {
    char op = in_char;

    // UTF-8 多字節處理
    if ((unsigned char)op >= 0xE0) {
        unsigned char c1 = op;
        unsigned char c2 = fgetc(fp);
        unsigned char c3 = fgetc(fp);
        int full_op = is_fullwidth_operator(c1, c2, c3);
        if (full_op) {
            op = full_op;
        } else {
            ungetc(c3, fp);
            ungetc(c2, fp);
        }
    }

    in_char = fgetc(fp);

    switch (op) {
        case '=':
            if (in_char == '=') {
                in_char = fgetc(fp);
                return EQUAL_TOKEN;
            }
            return ASSIGN_TOKEN;
        case '>':
            if (in_char == '=') {
                in_char = fgetc(fp);
                return GREATER_EQUAL_TOKEN;
            }
            return GREATER_TOKEN;
        case '<':
            if (in_char == '=') {
                in_char = fgetc(fp);
                return LESS_EQUAL_TOKEN;
            }
            return LESS_TOKEN;
        case '+': return PLUS_TOKEN;
        case '-': return MINUS_TOKEN;
        case '(': return LEFTPAREN_TOKEN;
        case ')': return RIGHTPAREN_TOKEN;
        case '{': return LEFTBRACE_TOKEN;
        case '}': return RIGHTBRACE_TOKEN;
        case ';':
        case ':': return SEMICOLON_TOKEN;
    }

    if (in_char != EOF)
        ungetc(in_char, fp);

    return -1;
}

void print_token(int token, const char *lexeme) {
    const char *names[] = {
        "", "TYPE_TOKEN", "IF_TOKEN", "MAIN_TOKEN", "ELSE_TOKEN", "WHILE_TOKEN",
        "LEFTPAREN_TOKEN", "RIGHTPAREN_TOKEN", "LEFTBRACE_TOKEN", "RIGHTBRACE_TOKEN",
        "SEMICOLON_TOKEN", "EQUAL_TOKEN", "GREATER_TOKEN", "LESS_TOKEN", "ASSIGN_TOKEN",
        "LITERAL_TOKEN", "PLUS_TOKEN", "MINUS_TOKEN", "ID_TOKEN",
        "GREATER_EQUAL_TOKEN", "LESS_EQUAL_TOKEN"
    };

    if (token >= 1 && token <= 20)
        printf("%s: %s\n", lexeme, names[token]);
}

void tokenize_code(FILE *fp) {
    in_char = fgetc(fp);

    while (in_char != EOF) {
        if (in_char == ' ' || in_char == '\n' || in_char == '\t' || in_char == '\r') {
            in_char = fgetc(fp);
            continue;
        }

        if (isalpha_custom(in_char)) {
            int token = get_id(fp);
            print_token(token, current_word);
        } else if (isdigit_custom(in_char)) {
            int token = get_number(fp);
            print_token(token, current_word);
        } else {
            int token = get_operator(fp);
            if (token != -1) {
                char op_str[4] = {in_char, 0}; // 預設字元表示
                print_token(token, op_str);
            } else {
                in_char = fgetc(fp); // 忽略無法辨識的字符
            }
        }
    }
}

FILE *create_memory_file(const char *code) {
    FILE *fp = tmpfile();
    if (!fp) return NULL;
    fputs(code, fp);
    rewind(fp);
    return fp;
}

int main() {
    const char *code =
        "int main{\n"
        "int cd2025=5;\n"
        "int cd2025_ = 5；\n"
        "if （cd2025 == 5）｛\n"
        "cd2025_ = 0：\n"
        "｝\n"
        "else {\n"
        "cd2025_ = 1+2+(3+4)+5;\n"
        "}\n"
        "while (cd2025_+cd2025) {\n"
        "cd2025 = cd2025-1;\n"
        "｝\n"
        "return 0;\n"
        "}";

    FILE *fp = create_memory_file(code);
    if (!fp) {
        fprintf(stderr, "Error: Failed to create memory file.\n");
        return 1;
    }

    tokenize_code(fp);
    fclose(fp);
    return 0;
}
