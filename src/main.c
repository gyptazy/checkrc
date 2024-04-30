#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Necessário para a função isspace
#include "config.h"

char* trim_space(char* str) {
    char* end;

    while (isspace((unsigned char)*str) || *str == '"') str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;

    while (end > str && (isspace((unsigned char)*end) || *end == '"')) end--;

    end[1] = '\0';

    return str;
}


bool validate_option(const char* key, const char* value) {
    for (size_t i = 0; i < sizeof(config_options) / sizeof(ConfigOption); i++) {
        if (strcmp(config_options[i].name, key) == 0) {
            switch (config_options[i].type) {
                case TYPE_STRING:
                    return true;
                case TYPE_INT:
                    for (size_t j = 0; value[j] != '\0'; j++) {
                        if (!isdigit(value[j]) && !(value[j] == '-' && j == 0)) {
                            return false;
                        }
                    }
                    return true;
                case TYPE_BOOL:
                    for (int j = 0; config_options[i].valid_values[j] != NULL; j++) {
                        if (strcmp(config_options[i].valid_values[j], value) == 0) {
                            return true;
                        }
                    }
                    return false;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s /path/to/rc.conf\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    char line[1024];
    int line_number = 0;
    bool valid = true;

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        char* key = strtok(line, "=");
        if (key != NULL) {
            key = trim_space(key);
        }
        char* value = strtok(NULL, "\n");
        if (value != NULL) {
            value = trim_space(value);
        }

        if (value && !validate_option(key, value)) {
            valid = false;
            printf("Error on line %d: '%s' is not a valid value for '%s'\n", line_number, value, key);
        }
    }

    fclose(file);

    if (valid) {
        printf("Config check passed successfully.\n");
    } else {
        printf("Config check failed with errors listed above.\n");
    }

    return 0;
}
