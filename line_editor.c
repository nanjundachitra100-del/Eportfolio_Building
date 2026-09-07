#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 100
#define MAX_LEN 256

/* ==========================================
 * MEMBER 1: Data Structure & Core Operations
 * ========================================== */
typedef struct {
    char lines[MAX_LINES][MAX_LEN];
    int count;
} Document;

/* Inserts a line at a given 1-based index, shifting existing lines down */
int insert_line(Document *doc, int line_num, const char *text) {
    if (doc->count >= MAX_LINES) {
        printf("Error: Document is full (max %d lines).\n", MAX_LINES);
        return 0;
    }
    if (line_num < 1 || line_num > doc->count + 1) {
        printf("Error: Line number must be between 1 and %d.\n", doc->count + 1);
        return 0;
    }

    int idx = line_num - 1;
    for (int i = doc->count; i > idx; i--) {
        strncpy(doc->lines[i], doc->lines[i - 1], MAX_LEN);
    }

    strncpy(doc->lines[idx], text, MAX_LEN - 1);
    doc->lines[idx][MAX_LEN - 1] = '\0';
    doc->count++;
    return 1;
}

/* Deletes a line at a given 1-based index, shifting remaining lines up */
int delete_line(Document *doc, int line_num) {
    if (doc->count == 0) {
        printf("Error: Document is empty.\n");
        return 0;
    }
    if (line_num < 1 || line_num > doc->count) {
        printf("Error: Line number must be between 1 and %d.\n", doc->count);
        return 0;
    }

    int idx = line_num - 1;
    for (int i = idx; i < doc->count - 1; i++) {
        strncpy(doc->lines[i], doc->lines[i + 1], MAX_LEN);
    }

    doc->count--;
    return 1;
}

/* ==========================================
 * MEMBER 2: Document Display
 * ========================================== */
/* Prints all lines with line numbers */
void display(const Document *doc) {
    if (doc->count == 0) {
        printf("(empty document)\n");
        return;
    }
    for (int i = 0; i < doc->count; i++) {
        printf("%d: %s\n", i + 1, doc->lines[i]);
    }
}

/* ==========================================
 * MEMBER 3: File I/O (Save & Load)
 * ========================================== */
/* Saves document contents to a plain text file */
int save_to_file(const Document *doc, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Unable to open file '%s' for writing.\n", filename);
        return 0;
    }
    for (int i = 0; i < doc->count; i++) {
        fprintf(fp, "%s\n", doc->lines[i]);
    }
    fclose(fp);
    return 1;
}

/* Loads document contents from a plain text file */
int load_from_file(Document *doc, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Unable to open file '%s' for reading.\n", filename);
        return 0;
    }

    doc->count = 0;
    char buffer[MAX_LEN];
    while (fgets(buffer, sizeof(buffer), fp) && doc->count < MAX_LINES) {
        buffer[strcspn(buffer, "\r\n")] = '\0'; // Strip newline
        strncpy(doc->lines[doc->count], buffer, MAX_LEN - 1);
        doc->lines[doc->count][MAX_LEN - 1] = '\0';
        doc->count++;
    }

    fclose(fp);
    return 1;
}

/* ==========================================
 * MEMBER 2 & 3: REPL Interface & Input Parsing
 * ========================================== */
int main(void) {
    Document doc = { .count = 0 };
    char input[MAX_LEN + 64];

    printf("=== Simple C Line Editor ===\n");
    printf("Type 'p' to print, 'i' to insert, 'd' to delete, 's' to save, 'l' to load, 'q' to quit.\n\n");

    while (1) {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Remove trailing newline
        input[strcspn(input, "\r\n")] = '\0';

        // Skip empty prompt enters
        if (input[0] == '\0') {
            continue;
        }

        // Quit
        if (strcmp(input, "q") == 0) {
            printf("Exiting editor.\n");
            break;
        }

        // Print document
        if (strcmp(input, "p") == 0) {
            display(&doc);
        }
        // Insert line: i <line_num> <text>
        else if (input[0] == 'i' && (input[1] == ' ' || input[1] == '\0')) {
            int line_num;
            char text[MAX_LEN] = "";
            if (sscanf(input, "i %d %[^\n]", &line_num, text) >= 1) {
                if (insert_line(&doc, line_num, text)) {
                    printf("Line inserted at %d.\n", line_num);
                }
            } else {
                printf("Usage: i <line_number> <text>\n");
            }
        }
        // Delete line: d <line_num>
        else if (input[0] == 'd' && (input[1] == ' ' || input[1] == '\0')) {
            int line_num;
            if (sscanf(input, "d %d", &line_num) == 1) {
                if (delete_line(&doc, line_num)) {
                    printf("Line %d deleted.\n", line_num);
                }
            } else {
                printf("Usage: d <line_number>\n");
            }
        }
        // Save file: s <filename>
        else if (input[0] == 's' && (input[1] == ' ' || input[1] == '\0')) {
            char filename[128];
            if (sscanf(input, "s %127s", filename) == 1) {
                if (save_to_file(&doc, filename)) {
                    printf("Document saved to %s.\n", filename);
                }
            } else {
                printf("Usage: s <filename>\n");
            }
        }
        // Load file: l <filename>
        else if (input[0] == 'l' && (input[1] == ' ' || input[1] == '\0')) {
            char filename[128];
            if (sscanf(input, "l %127s", filename) == 1) {
                if (load_from_file(&doc, filename)) {
                    printf("Document loaded from %s (%d lines).\n", filename, doc.count);
                }
            } else {
                printf("Usage: l <filename>\n");
            }
        }
        // Unknown command
        else {
            printf("Unknown command. Available commands:\n");
            printf("  p                  - Print all lines\n");
            printf("  i <line> <text>    - Insert text at specified line\n");
            printf("  d <line>           - Delete line at index\n");
            printf("  s <filename>       - Save to file\n");
            printf("  l <filename>       - Load from file\n");
            printf("  q                  - Quit editor\n");
        }
    }

    return 0;
}