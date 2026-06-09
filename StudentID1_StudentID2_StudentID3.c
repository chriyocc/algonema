// ============================================================
// ACE6123 Algorithms and Data Structures
// Trimester March/April 2026
// Take-Home Group Assignment
// ============================================================
// Student 1: [Name], ID: [ID], Major: [Major]
// Student 2: [Name], ID: [ID], Major: [Major]
// Student 3: [Name], ID: [ID], Major: [Major]
// ============================================================
// Program : Cinema Movie Management System
// Description: A CLI-based program to manage a cinema movie
//              dataset with file I/O, searching (Linear Search)
//              and sorting (Bubble Sort).
// Searching Algorithm: Linear Search - iterates through all
//   records sequentially to find matching entries. O(n).
// Sorting Algorithm  : Bubble Sort - repeatedly swaps adjacent
//   elements until the array is in the desired order. O(n^2).
// ============================================================

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MOVIES 100
#define MAX_STR 50
#define FILENAME "movies.txt"
#define INPUT_SIZE 128

typedef struct {
    int id;
    char title[MAX_STR];
    char genre[MAX_STR];
    int duration;
    float rating;
    int year;
} Movie;

Movie movies[MAX_MOVIES];
int movieCount = 0;

void createDefaultDataset(void);
int loadMovies(void);
void saveMovies(void);
void displayMainMenu(void);
void viewAllMovies(void);
void addMovie(void);
void editMovie(void);
void deleteMovie(void);
void searchMenu(void);
void searchByTitle(void);
void searchByGenre(void);
void searchByRating(void);
void searchByYear(void);
void sortMenu(void);
void sortByTitle(int ascending);
void sortByRating(int ascending);
void sortByYear(int ascending);
void sortByDuration(int ascending);
void printDivider(void);
int findMovieById(int id);
int getNextId(void);

static int readLine(const char *prompt, char *buffer, size_t size);
static void readText(const char *prompt, char *buffer, size_t size);
static int readIntRange(const char *prompt, int minValue, int maxValue);
static int readPositiveInt(const char *prompt);
static float readFloatRange(const char *prompt, float minValue, float maxValue);
static int containsPipe(const char *text);
static int containsSubstringIgnoreCase(const char *haystack, const char *needle);
static void printMovieRow(Movie movie);
static void bubbleSort(int criterion, int ascending);
static int shouldSwap(Movie left, Movie right, int criterion, int ascending);
static void maybeSaveSortedOrder(void);

/*
 * Function : main
 * Purpose  : Starts the program, loads the dataset, and runs the menu loop.
 * Returns  : Program exit status.
 */
int main(void) {
    int startChoice;
    int choice;

    createDefaultDataset();

    printf("=================================\n");
    printf("  CINEMA MOVIE MANAGEMENT SYSTEM \n");
    printf("=================================\n");
    printf("1. Open dataset\n");
    printf("0. Exit\n");
    startChoice = readIntRange("Enter choice: ", 0, 1);

    if (startChoice == 0) {
        return 0;
    }

    if (loadMovies() == 0) {
        printf("Warning: Dataset empty or failed to load.\n");
    }

    do {
        displayMainMenu();
        choice = readIntRange("Enter choice: ", 0, 6);
        switch (choice) {
            case 1: viewAllMovies(); break;
            case 2: addMovie(); break;
            case 3: editMovie(); break;
            case 4: deleteMovie(); break;
            case 5: searchMenu(); break;
            case 6: sortMenu(); break;
            case 0: printf("Goodbye!\n"); break;
            default: break;
        }
    } while (choice != 0);

    return 0;
}

/*
 * Function : createDefaultDataset
 * Purpose  : Creates movies.txt with ten records when it does not exist.
 */
void createDefaultDataset(void) {
    FILE *fp = fopen(FILENAME, "r");

    if (fp != NULL) {
        fclose(fp);
        return;
    }

    fp = fopen(FILENAME, "w");
    if (fp == NULL) {
        printf("Error: Cannot create %s\n", FILENAME);
        return;
    }

    fprintf(fp, "1|Interstellar|Sci-Fi|169|8.6|2014\n");
    fprintf(fp, "2|The Dark Knight|Action|152|9.0|2008\n");
    fprintf(fp, "3|Titanic|Drama|195|7.8|1997\n");
    fprintf(fp, "4|The Conjuring|Horror|112|7.5|2013\n");
    fprintf(fp, "5|Avengers Endgame|Action|181|8.4|2019\n");
    fprintf(fp, "6|Parasite|Drama|132|8.6|2019\n");
    fprintf(fp, "7|Inception|Sci-Fi|148|8.8|2010\n");
    fprintf(fp, "8|The Mask|Comedy|101|6.9|1994\n");
    fprintf(fp, "9|A Quiet Place|Horror|90|7.5|2018\n");
    fprintf(fp, "10|The Hangover|Comedy|100|7.7|2009\n");
    fclose(fp);
}

/*
 * Function : loadMovies
 * Purpose  : Opens movies.txt and loads all records into movies[].
 * Returns  : Number of movies successfully loaded.
 */
int loadMovies(void) {
    FILE *fp = fopen(FILENAME, "r");

    if (fp == NULL) {
        printf("Error: Cannot open %s\n", FILENAME);
        return 0;
    }

    movieCount = 0;
    while (movieCount < MAX_MOVIES &&
           fscanf(fp, "%d|%49[^|]|%49[^|]|%d|%f|%d\n",
                  &movies[movieCount].id,
                  movies[movieCount].title,
                  movies[movieCount].genre,
                  &movies[movieCount].duration,
                  &movies[movieCount].rating,
                  &movies[movieCount].year) == 6) {
        movieCount++;
    }

    fclose(fp);
    printf("%d movie(s) loaded.\n", movieCount);
    return movieCount;
}

/*
 * Function : saveMovies
 * Purpose  : Writes the current in-memory records to movies.txt.
 */
void saveMovies(void) {
    FILE *fp = fopen(FILENAME, "w");
    int i;

    if (fp == NULL) {
        printf("Error: Cannot save to %s\n", FILENAME);
        return;
    }

    for (i = 0; i < movieCount; i++) {
        fprintf(fp, "%d|%s|%s|%d|%.1f|%d\n",
                movies[i].id, movies[i].title, movies[i].genre,
                movies[i].duration, movies[i].rating, movies[i].year);
    }

    fclose(fp);
}

/*
 * Function : displayMainMenu
 * Purpose  : Prints the main menu options.
 */
void displayMainMenu(void) {
    printf("\n==============================\n");
    printf("   CINEMA MOVIE MANAGEMENT\n");
    printf("==============================\n");
    printf("1. View All Movies\n");
    printf("2. Add Movie\n");
    printf("3. Edit Movie\n");
    printf("4. Delete Movie\n");
    printf("5. Search Movies\n");
    printf("6. Sort Movies\n");
    printf("0. Exit\n");
    printf("==============================\n");
}

/*
 * Function : viewAllMovies
 * Purpose  : Prints all movie records in a formatted table.
 */
void viewAllMovies(void) {
    int i;

    if (movieCount == 0) {
        printf("No movies in dataset.\n");
        return;
    }

    printDivider();
    printf("| %-4s | %-22s | %-10s | %-10s | %-6s | %-4s |\n",
           "ID", "Title", "Genre", "Duration", "Rating", "Year");
    printDivider();
    for (i = 0; i < movieCount; i++) {
        printMovieRow(movies[i]);
    }
    printDivider();
}

/*
 * Function : addMovie
 * Purpose  : Prompts for a new movie, appends it, and saves the file.
 */
void addMovie(void) {
    Movie movie;

    if (movieCount >= MAX_MOVIES) {
        printf("Dataset is full. Cannot add more movies.\n");
        return;
    }

    movie.id = getNextId();
    readText("Enter title: ", movie.title, sizeof(movie.title));
    readText("Enter genre: ", movie.genre, sizeof(movie.genre));
    movie.duration = readPositiveInt("Enter duration (minutes): ");
    movie.rating = readFloatRange("Enter rating (0.0-10.0): ", 0.0f, 10.0f);
    movie.year = readIntRange("Enter release year (1888-2100): ", 1888, 2100);

    movies[movieCount] = movie;
    movieCount++;
    saveMovies();
    printf("Movie added successfully.\n");
}

/*
 * Function : editMovie
 * Purpose  : Finds a movie by ID and updates one selected field.
 */
void editMovie(void) {
    int id = readPositiveInt("Enter movie ID to edit: ");
    int index = findMovieById(id);
    int choice;

    if (index == -1) {
        printf("Error: Movie ID not found.\n");
        return;
    }

    printf("Current movie:\n");
    printDivider();
    printMovieRow(movies[index]);
    printDivider();
    printf("Which field to edit?\n");
    printf("1. Title  2. Genre  3. Duration  4. Rating  5. Year  0. Cancel\n");
    choice = readIntRange("Enter choice: ", 0, 5);

    if (choice == 1) readText("Enter new title: ", movies[index].title, MAX_STR);
    if (choice == 2) readText("Enter new genre: ", movies[index].genre, MAX_STR);
    if (choice == 3) movies[index].duration = readPositiveInt("Enter new duration: ");
    if (choice == 4) movies[index].rating = readFloatRange("Enter new rating: ", 0.0f, 10.0f);
    if (choice == 5) movies[index].year = readIntRange("Enter new year: ", 1888, 2100);

    if (choice == 0) {
        printf("Edit cancelled.\n");
        return;
    }

    saveMovies();
    printf("Movie updated successfully.\n");
}

/*
 * Function : deleteMovie
 * Purpose  : Removes a movie by ID after confirmation.
 */
void deleteMovie(void) {
    char answer[INPUT_SIZE];
    int id = readPositiveInt("Enter movie ID to delete: ");
    int index = findMovieById(id);
    int i;

    if (index == -1) {
        printf("Error: Movie ID not found.\n");
        return;
    }

    readLine("Are you sure? (y/n): ", answer, sizeof(answer));
    if (tolower((unsigned char)answer[0]) != 'y') {
        printf("Delete cancelled.\n");
        return;
    }

    for (i = index; i < movieCount - 1; i++) {
        movies[i] = movies[i + 1];
    }
    movieCount--;
    saveMovies();
    printf("Movie deleted successfully.\n");
}

/*
 * Function : searchMenu
 * Purpose  : Displays search choices and calls the selected search.
 */
void searchMenu(void) {
    int choice;

    do {
        printf("\n=== SEARCH MOVIES ===\n");
        printf("1. Search by Title\n");
        printf("2. Search by Genre\n");
        printf("3. Search by Rating (minimum)\n");
        printf("4. Search by Year\n");
        printf("0. Back\n");
        choice = readIntRange("Enter choice: ", 0, 4);
        if (choice == 1) searchByTitle();
        if (choice == 2) searchByGenre();
        if (choice == 3) searchByRating();
        if (choice == 4) searchByYear();
    } while (choice != 0);
}

/*
 * Function : searchByTitle
 * Purpose  : Linear Search by title using case-insensitive partial matching.
 */
void searchByTitle(void) {
    char keyword[MAX_STR];
    int found = 0;
    int i;

    readText("Enter title keyword: ", keyword, sizeof(keyword));
    for (i = 0; i < movieCount; i++) {
        if (containsSubstringIgnoreCase(movies[i].title, keyword)) {
            printMovieRow(movies[i]);
            found++;
        }
    }
    printf(found == 0 ? "No movies found.\n" : "%d result(s) found.\n", found);
}

/*
 * Function : searchByGenre
 * Purpose  : Linear Search by genre using case-insensitive partial matching.
 */
void searchByGenre(void) {
    char keyword[MAX_STR];
    int found = 0;
    int i;

    readText("Enter genre keyword: ", keyword, sizeof(keyword));
    for (i = 0; i < movieCount; i++) {
        if (containsSubstringIgnoreCase(movies[i].genre, keyword)) {
            printMovieRow(movies[i]);
            found++;
        }
    }
    printf(found == 0 ? "No movies found.\n" : "%d result(s) found.\n", found);
}

/*
 * Function : searchByRating
 * Purpose  : Linear Search for movies with rating >= minimum.
 */
void searchByRating(void) {
    float minimum = readFloatRange("Enter minimum rating: ", 0.0f, 10.0f);
    int found = 0;
    int i;

    for (i = 0; i < movieCount; i++) {
        if (movies[i].rating >= minimum) {
            printMovieRow(movies[i]);
            found++;
        }
    }
    printf(found == 0 ? "No movies found.\n" : "%d result(s) found.\n", found);
}

/*
 * Function : searchByYear
 * Purpose  : Linear Search for movies released in a selected year.
 */
void searchByYear(void) {
    int year = readIntRange("Enter release year: ", 1888, 2100);
    int found = 0;
    int i;

    for (i = 0; i < movieCount; i++) {
        if (movies[i].year == year) {
            printMovieRow(movies[i]);
            found++;
        }
    }
    printf(found == 0 ? "No movies found.\n" : "%d result(s) found.\n", found);
}

/*
 * Function : sortMenu
 * Purpose  : Displays sort choices and calls Bubble Sort variants.
 */
void sortMenu(void) {
    int choice;

    printf("\n=== SORT MOVIES ===\n");
    printf("Sort by:\n");
    printf("1. Title (A-Z)\n");
    printf("2. Rating (Highest first)\n");
    printf("3. Rating (Lowest first)\n");
    printf("4. Year (Newest first)\n");
    printf("5. Year (Oldest first)\n");
    printf("6. Duration (Longest first)\n");
    printf("7. Duration (Shortest first)\n");
    printf("0. Back\n");
    choice = readIntRange("Enter choice: ", 0, 7);

    if (choice == 1) sortByTitle(1);
    if (choice == 2) sortByRating(0);
    if (choice == 3) sortByRating(1);
    if (choice == 4) sortByYear(0);
    if (choice == 5) sortByYear(1);
    if (choice == 6) sortByDuration(0);
    if (choice == 7) sortByDuration(1);
}

/*
 * Function : sortByTitle
 * Purpose  : Bubble Sort by title in alphabetical order.
 */
void sortByTitle(int ascending) {
    bubbleSort(1, ascending);
    printf("Sorted by title (A-Z).\n");
    viewAllMovies();
    maybeSaveSortedOrder();
}

/*
 * Function : sortByRating
 * Purpose  : Bubble Sort by rating in ascending or descending order.
 */
void sortByRating(int ascending) {
    bubbleSort(2, ascending);
    printf(ascending ? "Sorted by rating (lowest first).\n" :
                       "Sorted by rating (highest first).\n");
    viewAllMovies();
    maybeSaveSortedOrder();
}

/*
 * Function : sortByYear
 * Purpose  : Bubble Sort by release year in ascending or descending order.
 */
void sortByYear(int ascending) {
    bubbleSort(3, ascending);
    printf(ascending ? "Sorted by year (oldest first).\n" :
                       "Sorted by year (newest first).\n");
    viewAllMovies();
    maybeSaveSortedOrder();
}

/*
 * Function : sortByDuration
 * Purpose  : Bubble Sort by duration in ascending or descending order.
 */
void sortByDuration(int ascending) {
    bubbleSort(4, ascending);
    printf(ascending ? "Sorted by duration (shortest first).\n" :
                       "Sorted by duration (longest first).\n");
    viewAllMovies();
    maybeSaveSortedOrder();
}

/*
 * Function : printDivider
 * Purpose  : Prints a horizontal divider for movie tables.
 */
void printDivider(void) {
    printf("+------+------------------------+------------+------------+--------+------+\n");
}

/*
 * Function : findMovieById
 * Purpose  : Finds the array index for a movie ID.
 * Returns  : Matching index, or -1 when no movie has that ID.
 */
int findMovieById(int id) {
    int i;

    for (i = 0; i < movieCount; i++) {
        if (movies[i].id == id) {
            return i;
        }
    }
    return -1;
}

/*
 * Function : getNextId
 * Purpose  : Generates the next ID as max existing ID + 1.
 * Returns  : New unique movie ID.
 */
int getNextId(void) {
    int maxId = 0;
    int i;

    for (i = 0; i < movieCount; i++) {
        if (movies[i].id > maxId) {
            maxId = movies[i].id;
        }
    }
    return maxId + 1;
}

static int readLine(const char *prompt, char *buffer, size_t size) {
    size_t length;

    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return 0;
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }
    return 1;
}

static void readText(const char *prompt, char *buffer, size_t size) {
    char input[INPUT_SIZE];

    while (1) {
        readLine(prompt, input, sizeof(input));
        if (strlen(input) == 0) {
            printf("Input cannot be empty.\n");
            continue;
        }
        if (strlen(input) >= size) {
            printf("Input must be at most %lu characters.\n", (unsigned long)size - 1);
            continue;
        }
        if (containsPipe(input)) {
            printf("Input cannot contain the pipe character.\n");
            continue;
        }
        strcpy(buffer, input);
        return;
    }
}

static int readIntRange(const char *prompt, int minValue, int maxValue) {
    char input[INPUT_SIZE];
    char *endPtr;
    long value;

    while (1) {
        readLine(prompt, input, sizeof(input));
        value = strtol(input, &endPtr, 10);
        if (input[0] == '\0' || *endPtr != '\0') {
            printf("Please enter a valid integer.\n");
            continue;
        }
        if (value < minValue || value > maxValue) {
            printf("Value must be between %d and %d.\n", minValue, maxValue);
            continue;
        }
        return (int)value;
    }
}

static int readPositiveInt(const char *prompt) {
    char input[INPUT_SIZE];
    char *endPtr;
    long value;

    while (1) {
        readLine(prompt, input, sizeof(input));
        value = strtol(input, &endPtr, 10);
        if (input[0] == '\0' || *endPtr != '\0') {
            printf("Please enter a valid integer.\n");
            continue;
        }
        if (value <= 0) {
            printf("Value must be greater than 0.\n");
            continue;
        }
        return (int)value;
    }
}

static float readFloatRange(const char *prompt, float minValue, float maxValue) {
    char input[INPUT_SIZE];
    char *endPtr;
    float value;

    while (1) {
        readLine(prompt, input, sizeof(input));
        value = strtof(input, &endPtr);
        if (input[0] == '\0' || *endPtr != '\0') {
            printf("Please enter a valid number.\n");
            continue;
        }
        if (value < minValue || value > maxValue) {
            printf("Value must be between %.1f and %.1f.\n", minValue, maxValue);
            continue;
        }
        return value;
    }
}

static int containsPipe(const char *text) {
    return strchr(text, '|') != NULL;
}

static int containsSubstringIgnoreCase(const char *haystack, const char *needle) {
    size_t hLen = strlen(haystack);
    size_t nLen = strlen(needle);
    size_t i;
    size_t j;

    if (nLen == 0) {
        return 1;
    }
    if (nLen > hLen) {
        return 0;
    }

    for (i = 0; i <= hLen - nLen; i++) {
        for (j = 0; j < nLen; j++) {
            unsigned char h = (unsigned char)haystack[i + j];
            unsigned char n = (unsigned char)needle[j];
            if (tolower(h) != tolower(n)) {
                break;
            }
        }
        if (j == nLen) {
            return 1;
        }
    }
    return 0;
}

static void printMovieRow(Movie movie) {
    printf("| %-4d | %-22.22s | %-10.10s | %4d min   | %6.1f | %-4d |\n",
           movie.id, movie.title, movie.genre, movie.duration,
           movie.rating, movie.year);
}

static void bubbleSort(int criterion, int ascending) {
    int i;
    int j;

    for (i = 0; i < movieCount - 1; i++) {
        for (j = 0; j < movieCount - 1 - i; j++) {
            if (shouldSwap(movies[j], movies[j + 1], criterion, ascending)) {
                Movie temp = movies[j];
                movies[j] = movies[j + 1];
                movies[j + 1] = temp;
            }
        }
    }
}

static int shouldSwap(Movie left, Movie right, int criterion, int ascending) {
    int comparison = 0;

    if (criterion == 1) comparison = strcmp(left.title, right.title);
    if (criterion == 2) comparison = (left.rating > right.rating) - (left.rating < right.rating);
    if (criterion == 3) comparison = left.year - right.year;
    if (criterion == 4) comparison = left.duration - right.duration;

    return ascending ? comparison > 0 : comparison < 0;
}

static void maybeSaveSortedOrder(void) {
    char answer[INPUT_SIZE];

    readLine("Save this sorted order to file? (y/n): ", answer, sizeof(answer));
    if (tolower((unsigned char)answer[0]) == 'y') {
        saveMovies();
        printf("Sorted order saved.\n");
    } else {
        printf("Sorted order not saved.\n");
    }
}
