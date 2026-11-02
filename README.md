# Student Management System (C)

A menu-driven, console-based Student Management System written in C (C99). It stores student records in a local binary file so records remain available after the program closes.

## Features
- Add a student with a unique ID, name, age, course and marks.
- View all saved student records.
- Search for a student by ID.
- Update a student's details.
- Delete a student after confirmation.
- Validate numeric input and marks (0–100).

## Requirements
- A C compiler that supports C99 (GCC, Clang, or MinGW).
- No third-party libraries are required.

## Compile and run

### Linux / macOS
```bash
gcc -std=c99 -Wall -Wextra -pedantic student_management.c -o student_management
./student_management
```

### Windows (MinGW)
```bash
gcc -std=c99 -Wall -Wextra -pedantic student_management.c -o student_management.exe
student_management.exe
```

The program creates `students.dat` in the current working directory the first time a record is saved. Keep this file if you want to preserve your records. Do not edit it manually.

## Project files
- `student_management.c` — program source code.
- `requirements.txt` — compiler and dependency information.
- `Project_Report.pdf` — project documentation.
- `README.md` — this guide.

## Notes
- Student IDs must be unique.
- Marks must be between 0 and 100.
- This is an educational, local-file project; it does not include user login, encryption, networking or a database server.
- The binary data file is specific to the program's structure/compiler environment. Back it up before moving records between different systems.

## Author
Add your name, class, college and submission date to the project report before submitting.
