# Iopy Data Generator


## Build instructions


1. Build sqlite separately (Recommended)

- Commands
    1. Compile sqlite: `cc -c extern/sqlite/sqlite3.c -o build/sqlite3.o`
    2. Edit settings in `src/settings.h`
    3. Compile the main program: `cc build/sqlite3.o src/main.c -o build/main`

- When you have to change your settings restart from step 2.


2. Build everything in one go

- Command: `cc extern/sqlite/sqlite3.c src/main.c -o build/main`
