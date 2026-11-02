/*
 * Student Management System
 * Language: C (C99)
 * Features: Add, view, search, update and delete student records.
 * Records are stored in a local binary file named students.dat.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DATA_FILE "students.dat"
#define TEMP_FILE "students.tmp"
#define NAME_LEN 50
#define COURSE_LEN 50

typedef struct {
    int id;
    char name[NAME_LEN];
    int age;
    char course[COURSE_LEN];
    float marks;
} Student;

static void read_line(const char *prompt, char *buffer, size_t size) {
    size_t len;
    int ch;

    for (;;) {
        printf("%s", prompt);
        if (fgets(buffer, (int)size, stdin) == NULL) {
            printf("\nInput ended. Exiting.\n");
            exit(0);
        }

        len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            return;
        }

        /* Discard the remainder of an overlong input line. */
        while ((ch = getchar()) != '\n' && ch != EOF) { }
        printf("Input is too long. Please try again.\n");
    }
}

static int read_int(const char *prompt, int min_value, int max_value) {
    char line[100];
    char *end;
    long value;

    for (;;) {
        read_line(prompt, line, sizeof(line));
        errno = 0;
        value = strtol(line, &end, 10);
        while (*end == ' ' || *end == '\t') end++;

        if (line[0] != '\0' && *end == '\0' && errno == 0 &&
            value >= min_value && value <= max_value) {
            return (int)value;
        }
        printf("Enter a whole number between %d and %d.\n", min_value, max_value);
    }
}

static float read_marks(const char *prompt) {
    char line[100];
    char *end;
    float value;

    for (;;) {
        read_line(prompt, line, sizeof(line));
        errno = 0;
        value = strtof(line, &end);
        while (*end == ' ' || *end == '\t') end++;

        if (line[0] != '\0' && *end == '\0' && errno == 0 &&
            value >= 0.0f && value <= 100.0f) {
            return value;
        }
        printf("Enter valid marks between 0 and 100.\n");
    }
}

static int find_student(int id, Student *result) {
    FILE *fp = fopen(DATA_FILE, "rb");
    Student current;

    if (fp == NULL) return 0;

    while (fread(&current, sizeof(Student), 1, fp) == 1) {
        if (current.id == id) {
            if (result != NULL) *result = current;
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

static void print_heading(void) {
    printf("\n%-8s %-22s %-6s %-20s %-8s\n",
           "ID", "NAME", "AGE", "COURSE", "MARKS");
    printf("---------------------------------------------------------------------\n");
}

static void print_student(const Student *s) {
    printf("%-8d %-22.22s %-6d %-20.20s %-8.2f\n",
           s->id, s->name, s->age, s->course, s->marks);
}

static void add_student(void) {
    Student s;
    FILE *fp;

    printf("\n--- Add Student ---\n");
    s.id = read_int("Enter student ID: ", 1, 1000000000);

    if (find_student(s.id, NULL)) {
        printf("A student with this ID already exists.\n");
        return;
    }

    read_line("Enter student name: ", s.name, sizeof(s.name));
    if (s.name[0] == '\0') {
        printf("Name cannot be empty.\n");
        return;
    }

    s.age = read_int("Enter age (1-120): ", 1, 120);
    read_line("Enter course: ", s.course, sizeof(s.course));
    if (s.course[0] == '\0') {
        printf("Course cannot be empty.\n");
        return;
    }
    s.marks = read_marks("Enter marks (0-100): ");

    fp = fopen(DATA_FILE, "ab");
    if (fp == NULL) {
        perror("Could not open data file");
        return;
    }

    if (fwrite(&s, sizeof(Student), 1, fp) != 1) {
        printf("Could not save the student record.\n");
    } else {
        printf("Student record added successfully.\n");
    }
    fclose(fp);
}

static void view_students(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    Student s;
    int count = 0;

    printf("\n--- All Student Records ---\n");
    if (fp == NULL) {
        printf("No records found yet. Add a student first.\n");
        return;
    }

    print_heading();
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        print_student(&s);
        count++;
    }
    fclose(fp);

    if (count == 0) printf("No records found.\n");
    else printf("\nTotal students: %d\n", count);
}

static void search_student(void) {
    int id;
    Student s;

    printf("\n--- Search Student ---\n");
    id = read_int("Enter student ID to search: ", 1, 1000000000);

    if (find_student(id, &s)) {
        print_heading();
        print_student(&s);
    } else {
        printf("Student with ID %d was not found.\n", id);
    }
}

static void update_student(void) {
    int id, found = 0;
    Student s;
    FILE *source, *temp;

    printf("\n--- Update Student ---\n");
    id = read_int("Enter student ID to update: ", 1, 1000000000);

    source = fopen(DATA_FILE, "rb");
    if (source == NULL) {
        printf("No records are available to update.\n");
        return;
    }

    temp = fopen(TEMP_FILE, "wb");
    if (temp == NULL) {
        perror("Could not create temporary file");
        fclose(source);
        return;
    }

    while (fread(&s, sizeof(Student), 1, source) == 1) {
        if (s.id == id) {
            found = 1;
            printf("Enter the new details:\n");
            {
                char new_name[NAME_LEN];
                char new_course[COURSE_LEN];
                read_line("New name (press Enter to keep current): ", new_name, sizeof(new_name));
                if (new_name[0] != '\0') {
                    strcpy(s.name, new_name);
                }
                s.age = read_int("New age (1-120): ", 1, 120);
                read_line("New course (press Enter to keep current): ", new_course, sizeof(new_course));
                if (new_course[0] != '\0') {
                    strcpy(s.course, new_course);
                }
                s.marks = read_marks("New marks (0-100): ");
            }
        }

        if (fwrite(&s, sizeof(Student), 1, temp) != 1) {
            printf("Error writing updated records.\n");
            fclose(source);
            fclose(temp);
            remove(TEMP_FILE);
            return;
        }
    }

    fclose(source);
    fclose(temp);

    if (!found) {
        remove(TEMP_FILE);
        printf("Student with ID %d was not found.\n", id);
        return;
    }

    if (remove(DATA_FILE) != 0 || rename(TEMP_FILE, DATA_FILE) != 0) {
        perror("Could not replace the data file");
        printf("Your temporary data is in %s if recovery is needed.\n", TEMP_FILE);
        return;
    }
    printf("Student record updated successfully.\n");
}

static void delete_student(void) {
    int id, found = 0;
    Student s;
    FILE *source, *temp;
    char confirm[10];

    printf("\n--- Delete Student ---\n");
    id = read_int("Enter student ID to delete: ", 1, 1000000000);

    source = fopen(DATA_FILE, "rb");
    if (source == NULL) {
        printf("No records are available to delete.\n");
        return;
    }

    temp = fopen(TEMP_FILE, "wb");
    if (temp == NULL) {
        perror("Could not create temporary file");
        fclose(source);
        return;
    }

    while (fread(&s, sizeof(Student), 1, source) == 1) {
        if (s.id == id) {
            found = 1;
            continue;
        }
        if (fwrite(&s, sizeof(Student), 1, temp) != 1) {
            printf("Error writing records.\n");
            fclose(source);
            fclose(temp);
            remove(TEMP_FILE);
            return;
        }
    }

    fclose(source);
    fclose(temp);

    if (!found) {
        remove(TEMP_FILE);
        printf("Student with ID %d was not found.\n", id);
        return;
    }

    printf("Are you sure you want to delete this record? (yes/no): ");
    read_line("", confirm, sizeof(confirm));
    if (strcmp(confirm, "yes") != 0 && strcmp(confirm, "YES") != 0) {
        remove(TEMP_FILE);
        printf("Deletion cancelled.\n");
        return;
    }

    if (remove(DATA_FILE) != 0 || rename(TEMP_FILE, DATA_FILE) != 0) {
        perror("Could not replace the data file");
        printf("Your temporary data is in %s if recovery is needed.\n", TEMP_FILE);
        return;
    }
    printf("Student record deleted successfully.\n");
}

int main(void) {
    int choice;

    printf("=====================================\n");
    printf("       STUDENT MANAGEMENT SYSTEM\n");
    printf("=====================================\n");

    for (;;) {
        printf("\nMAIN MENU\n");
        printf("1. Add student\n");
        printf("2. View all students\n");
        printf("3. Search student by ID\n");
        printf("4. Update student\n");
        printf("5. Delete student\n");
        printf("6. Exit\n");

        choice = read_int("Choose an option (1-6): ", 1, 6);

        switch (choice) {
            case 1: add_student(); break;
            case 2: view_students(); break;
            case 3: search_student(); break;
            case 4: update_student(); break;
            case 5: delete_student(); break;
            case 6:
                printf("Thank you for using Student Management System!\n");
                return 0;
            default:
                printf("Invalid option.\n");
        }
    }
}
