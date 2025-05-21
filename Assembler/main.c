#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_TOKENS 10 

/* Definition der Register */
typedef struct { const char* name; const char* bits; } register_map_t;

/* Gibt an welche Binärdarstellung ein Register hat */
static const register_map_t register_token_map[] = {
    {"r0", "00"}, {"r1", "01"}, {"r2", "10"}, {"r3", "11"}, 
    {NULL, NULL}
};

/* Entfernt Leerzeichen links vom string */
static char* ltrim(char* str) {
    while (isspace((unsigned char)* str)) str++;
    return str;
}

/* Entfernt Leerzeichen rechts vom string */
static void rtrim(char* str) {
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;
    *(end + 1) = '\0';
}

/* Register in Binärstring umwandeln */
static const char* register_to_bin(const char* token) {
    for (int i = 0; register_token_map[i].name != NULL; i++) {
        if (strcasecmp(token, register_token_map[i].name) == 0) {
            return register_token_map[i].bits;
        }
    }

    return NULL;
}

/* Wandelt einen integer in die entsprechende string Zweierkomplement Binärdarstellung um */
static int int_to_bin(char* out, int value, int bits) {
    int min = -(1 << (bits - 1));
    int max = (1 << (bits - 1)) - 1;

    if (value < min || value > max) {
        fprintf(stderr, "Error: Value %d out of range for %d bits\n", value, bits);
        return -1;
    }

    /* Schneide die unteren #bits bit aus value heraus */
    unsigned int uvalue = (unsigned int)(value & ((1u << bits) - 1));

    for (int i = bits - 1; i >= 0; i--) {
        out[bits - 1 - i] = (uvalue & (1u << i)) ? '1' : '0';
    }

    out[bits] = '\0';
    return 0;
}

/* Übersetze die Instruktionen */
static int assemble_line(const char* line, char* out) {
    char buffer[MAX_LINE];
    strncpy(buffer, line, MAX_LINE);
    buffer[MAX_LINE - 1] = '\0'; 

    /* Kommentare entfernen */
    char* comment_start = strchr(buffer, '#');
    if (comment_start) {
        *comment_start = '\0';
    }

    /* Zerlege eine Instruktion in einzelne Tokens */
    char* token[MAX_TOKENS] = { NULL };
    int token_count = 0;
    char* ptr = strtok(buffer, ", \t\n");
    while (ptr != NULL && token_count < MAX_TOKENS) {
        token[token_count++] = ptr;
        ptr = strtok(NULL, ", \t\n");
    }

    /* Leere Zeile */
    if (token_count == 0) {
        return 1;
    }

    /* Konvertiere in Kleinbuchstaben */
    for (int i = 0; i < token_count; i++) {
        for (char* p = token[i]; *p; p++) {
            *p = tolower((unsigned char)*p);
        }
    }

    /* Opcode bestimmen */
    const char* opcode = token[0];

    if (strcmp(opcode, "add") == 0 && token_count == 4) {
        const char* rs = register_to_bin(token[1]);
        const char* rt = register_to_bin(token[2]);
        const char* rd = register_to_bin(token[3]);

        if (!rs || !rt || !rd) {
          return -2;
        }
        
        sprintf(out, "0000%s%s%s000000", rs, rt, rd);
        return 0;
    }
    
    if (strcmp(opcode, "addi") == 0 && token_count == 4) {
        const char* rs = register_to_bin(token[1]);
        const char* rt = register_to_bin(token[2]);
        
        if (!rs || !rt) {
          return -2; 
        }

        char immediate[9];
        if (int_to_bin(immediate, atoi(token[3]), 8) != 0) {
            return -3;
        }

        sprintf(out, "0001%s%s%s", rs, rt, immediate);
        return 0;
    }

    if (strcmp(opcode, "load") == 0 && token_count == 4) {
        const char* rs = register_to_bin(token[1]);
        const char* rt = register_to_bin(token[2]);

        if (!rs || !rt) {
            return -2;
        }

        char immediate[9];

        if (int_to_bin(immediate, atoi(token[3]), 8) != 0) {
            return -3;
        }

        sprintf(out, "0010%s%s%s", rs, rt, immediate);
        return 0;
    }

    if (strcmp(opcode, "store") == 0 && token_count == 4) {
        const char* rs = register_to_bin(token[1]);
        const char* rt = register_to_bin(token[2]);

        if (!rs || !rt) {
            return -2;
        }

        char immediate[9];

        if (int_to_bin(immediate, atoi(token[3]), 8) != 0) {
            return -3;
        }

        sprintf(out, "0011%s%s%s", rs, rt, immediate);
        return 0;
    }

    if (strcmp(opcode, "jump") == 0 && token_count == 2) {
        char immediate[13];
        if (int_to_bin(immediate, atoi(token[1]), 12) != 0) {
            return -3;
        }

        sprintf(out, "0100%s", immediate);
        return 0;
    }

    if ((strcmp(opcode, "bne") == 0 ||
         strcmp(opcode, "beq") == 0 ||
         strcmp(opcode, "blt") == 0 ||
         strcmp(opcode, "bgt") == 0) && token_count == 4) {
        const char* rs = register_to_bin(token[1]);
        const char* rt = register_to_bin(token[2]);

        if (!rs || !rt) {
            return -2;
        }

        char immediate[9];
        if (int_to_bin(immediate, atoi(token[3]), 8) != 0) {
            return -3;
        }

        if (strcmp(opcode, "bne") == 0) {
            sprintf(out, "0101%s%s%s", rs, rt, immediate);
        } else if (strcmp(opcode, "beq") == 0) {
            sprintf(out, "0110%s%s%s", rs, rt, immediate);
        } else if (strcmp(opcode, "blt") == 0) {
            sprintf(out, "0111%s%s%s", rs, rt, immediate);
        } else if (strcmp(opcode, "bgt") == 0) {
            sprintf(out, "1000%s%s%s", rs, rt, immediate);
        }

        return 0;
    }

    if (strcmp(opcode, "li") == 0 && token_count == 3) {
        const char* rt = register_to_bin(token[1]);
        
        if (!rt) {
            return -2;
        }

        char immediate[9];

        if (int_to_bin(immediate, atoi(token[2]), 8) != 0) {
            return -3;
        }

        sprintf(out, "1001%s00%s", rt, immediate);

        return 0;
    }

    if (strcmp(opcode, "and") == 0 && token_count == 4) {
        const char* rs = register_to_bin(token[1]);
        const char* rt = register_to_bin(token[2]);
        const char* rd = register_to_bin(token[3]);

        if (!rs || !rt || !rd) {
            return -2;
        }

        sprintf(out, "1010%s%s%s000000", rs, rt, rd);
        return 0;
    }

    if (strcmp(opcode, "or") == 0 && token_count == 4) {
        const char* rs = register_to_bin(token[1]);
        const char* rt = register_to_bin(token[2]);
        const char* rd = register_to_bin(token[3]);

        if (!rs || !rt || !rd) {
            return -2;
        }

        sprintf(out, "1011%s%s%s000000", rs, rt, rd);
        return 0;
    }

    if (strcmp(opcode, "not") == 0 && token_count == 3) {
        const char* rs = register_to_bin(token[1]);
        const char* rd = register_to_bin(token[2]);

        if (!rs || !rd) {
            return -2;
        }

        sprintf(out, "1100%s00%s000000", rs, rd);
        return 0;
    }

    /*Unbekannter Befehl*/
    return -4;
}

int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        fprintf(stderr, "Aufruf: %s <input_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Fehler beim Öffnen der Eingabedatei");
        return EXIT_FAILURE;
    }

    FILE* output_file = fopen(argv[2], "w");
    if (!output_file) {
        perror("Fehler beim Öffnen der Ausgabedatei");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    char line[MAX_LINE];
    char output[MAX_LINE];
    unsigned int line_number = 0;
    int error = 0;
    while (fgets(line, sizeof(line), input_file)) {
        line_number += 1;
        char* trimmed_line = ltrim(line);
        rtrim(trimmed_line);
        int return_value = assemble_line(trimmed_line, output);

        /* Befehl wurde erfolgreich übersetzt */
        if (return_value == 0) {
            fprintf(output_file, "%s\n", output);
        }

        /* Irgendetwas lief bei der Übersetzung des Befehls falsch */
        if (return_value == 1) {
            continue; // Leere Zeile
        } else if (return_value == -1) {
            fprintf(stderr, "Fehler: Zeile %u: Ungültige Zeile\n", line_number);
            error = 1;
            break;
        } else if (return_value == -2) {
            fprintf(stderr, "Fehler: Zeile %u: Ungültiges Register\n", line_number);
            error = 1;
            break;
        } else if (return_value == -3) {
            fprintf(stderr, "Fehler: Zeile %u: Ungültiger Wert\n", line_number);
            error = 1;
            break;
        } else if (return_value == -4) {
            fprintf(stderr, "Fehler: Zeile %u: Unbekannter Befehl\n", line_number);
            error = 1;
            break;
        }
    }

    fclose(input_file);
    fclose(output_file);

    if (error) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
