#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

// Cross-platform getch() function for Linux/macOS
char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif
#define MAX_QUESTIONS 100
#define MAX_OPTIONS 4
#define MAX_LENGTH 256
#define KEY 0xAA // Simple XOR key for encryption
#define MAX_SUBJECT 5
const char* subjects[MAX_SUBJECT] = { "Physics", "Chemistry", "Math", "Biology", "English" };

typedef struct {
    char sub[MAX_SUBJECT][50];
    char password[MAX_SUBJECT][50];
} Teacher;

typedef struct {
    char question[MAX_LENGTH];
    char options[MAX_OPTIONS][MAX_LENGTH];
    int correctOption;
} Question;

Question quiz[MAX_QUESTIONS];
int totalQuestions = 0;
int numOptions = 4;
int quizDuration;
#define MAX_STUDENTS 1000


// Quiz Array
char questions[MAX_QUESTIONS][100];
char options[MAX_QUESTIONS][4][100];
char correctAnswers[MAX_QUESTIONS];

// Student Array
int rollNumbers[MAX_STUDENTS];
char studentAnswers[MAX_STUDENTS][MAX_QUESTIONS];
int studentScores[MAX_STUDENTS];
int totalStudents = 0;
int questionStatus[MAX_STUDENTS][MAX_QUESTIONS];


// Function to print quiz ASCII art
void printQuizAsciiArt() {
    printf(
        "                                                  \n"
        "                                                  \n"
        " .d88b.       db    db      d888888b      d88888D \n"
        ".8P  Y8.      88    88        `88'        YP  d8' \n"
        "88    88      88    88         88            d8'  \n"
        "88    88      88    88         88           d8'   \n"
        "`8P  d8'      88b  d88        .88.         d8' db \n"
        " `Y88'Y8       Y8888P       Y888888P      d88888P \n"
        "                                                  \n"
        "                                                  \n"
    );
}


// XOR Encryption Function
void encryptDecrypt(char *text) {
    for (int i = 0; text[i] != '\0'; i++) {
        text[i] ^= KEY; // XOR each character
    }
}
// Function to enter quiz questions (Teacher) and save to file
// Function to enter and encrypt quiz questions

// Function to enter and encrypt quiz questions
void enterQuestions(char* teacherUsername) {
    printQuizAsciiArt();
    char filename[100];
    sprintf(filename, "%s_questions.txt", teacherUsername);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    int totalQuestions;
    printf("Enter number of questions (Max %d): ", MAX_QUESTIONS);
    scanf("%d", &totalQuestions);
    if (totalQuestions > MAX_QUESTIONS) totalQuestions = MAX_QUESTIONS;
    while (getchar() != '\n'); // Clear buffer after scanf

    for (int i = 0; i < totalQuestions; i++) {
        system("cls");
        printQuizAsciiArt();
        printf("\nQuestion %d: ", i + 1);
        printf("\nEnter Question %d: ", i + 1);
        fgets(questions[i], sizeof(questions[i]), stdin);
        questions[i][strcspn(questions[i], "\n")] = 0;
        encryptDecrypt(questions[i]);
        fprintf(file, "%s\n", questions[i]);

        for (int j = 0; j < 4; j++) {
            printf("Option %c: ", 'A' + j);
            fgets(options[i][j], sizeof(options[i][j]), stdin);
            options[i][j][strcspn(options[i][j], "\n")] = 0;
            encryptDecrypt(options[i][j]);
            fprintf(file, "%s\n", options[i][j]);
        }

        printf("Enter Correct Answer (A/B/C/D): ");
        char ans;
        scanf(" %c", &ans);
        while (getchar() != '\n');
        correctAnswers[i] = toupper(ans);
        encryptDecrypt(&correctAnswers[i]);
        fprintf(file, "%c\n", correctAnswers[i]);
    }

    fclose(file);
    printf("Questions successfully recorded and encrypted in '%s'!\n", filename);
}


// Function to display quiz navigation status
void displayQuestionStatus(int studentIndex) {
    printf("\nAttended --> ");
    int attendedExists = 0, notAttendedExists = 0;
    for (int i = 0; i < totalQuestions; i++) {
        if (questionStatus[studentIndex][i] == 1) {
            printf("[%d] ", i + 1); // Answered
            attendedExists = 1;
        }
    }
    if (!attendedExists) {
        printf("None");
    }

    printf("\nNot attended --> ");
    for (int i = 0; i < totalQuestions; i++) {
        if (questionStatus[studentIndex][i] == 0) {
            printf("[%d] ", i + 1); // Unanswered or Skipped
            notAttendedExists = 1;
        }
    }
    if (!notAttendedExists) {
        printf("None");
    }
    printf("\n");
}

// Function to save student results to a file
void saveResultToFile(int rollNo, int score, int totalQuestions, const char* subjectName) {
    FILE *file = fopen("results.txt", "a"); // Open the file in append mode
    if (file == NULL) {
        printf("Error: Unable to open results file!\n");
        return;
    }

    // Get the current date and time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Format: YYYY-MM-DD HH:MM:SS
    char dateTime[20];
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", t);

    // Write the result to the file
    fprintf(file, "Date/Time: %s, Subject: %s, Roll No: %d, Score: %d/%d\n", 
            dateTime, subjectName, rollNo, score, totalQuestions);

    fclose(file);
    printf("Your result has been saved successfully!\n");
}

// Function to take the quiz (Student)
void takeQuiz(int studentIndex, int subjectIndex) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    char answer;
    int currentQuestion = 0;

    time_t startTime = time(NULL);

    while (1) {
        system("cls");
        printQuizAsciiArt();

        // Show attended/not attended status before each question
        displayQuestionStatus(studentIndex);

        printf("Q%d: %s\n", currentQuestion + 1, questions[currentQuestion]);
        for (int i = 0; i < 4; i++) {
            printf("%c. %s\n", 'A' + i, options[currentQuestion][i]);
        }

        printf("\nYour Answer (A-D), S=Skip, N=Next, P=Previous, J=Jump, Q=Quit: ");
        answer = toupper(getchar());
        while (getchar() != '\n');

        if (answer >= 'A' && answer <= 'D') {
            studentAnswers[studentIndex][currentQuestion] = answer;
            questionStatus[studentIndex][currentQuestion] = 1;

            int allAnswered = 1;
            for (int i = 0; i < totalQuestions; i++) {
                if (questionStatus[studentIndex][i] == 0) {
                    allAnswered = 0;
                    break;
                }
            }
            if (allAnswered) break;

            int foundNext = 0;
            for (int i = 1; i <= totalQuestions; i++) {
                int next = (currentQuestion + i) % totalQuestions;
                if (questionStatus[studentIndex][next] == 0) {
                    currentQuestion = next;
                    foundNext = 1;
                    break;
                }
            }
            if (!foundNext) break;
        } else if (answer == 'S' || answer == 'N') {
            if (currentQuestion < totalQuestions - 1)
                currentQuestion++;
        } else if (answer == 'P') {
            if (currentQuestion > 0)
                currentQuestion--;
        } else if (answer == 'J') {
            int qNum;
            printf("\nEnter question number to jump to (1-%d): ", totalQuestions);
            scanf("%d", &qNum);
            while (getchar() != '\n');
            if (qNum >= 1 && qNum <= totalQuestions)
                currentQuestion = qNum - 1;
        } else if (answer == 'Q') {
            break;
        }
    }

    int score = 0;
    for (int i = 0; i < totalQuestions; i++) {
        if (questionStatus[studentIndex][i] == 1 &&
            toupper(studentAnswers[studentIndex][i]) == toupper(correctAnswers[i])) {
            score++;
        }
    }

    studentScores[studentIndex] = score;
    printf("\nQuiz completed! Your score is: %d/%d\n", score, totalQuestions);
    saveResultToFile(rollNumbers[studentIndex], score, totalQuestions, subjects[subjectIndex]);
    system("pause");
}


// Function to load questions from file
void loadQuestionFile(int studentIndex, int subjectIndex) {
    char filename[100];
    sprintf(filename, "%s_questions.txt", subjects[subjectIndex]);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Quiz not yet availible for '%s'. Please contact the teacher.\n", subjects[subjectIndex]);
        return;
    }

    totalQuestions = 0;
    while (fgets(questions[totalQuestions], sizeof(questions[totalQuestions]), fp)) {
        questions[totalQuestions][strcspn(questions[totalQuestions], "\n")] = 0;
        encryptDecrypt(questions[totalQuestions]); // Decrypt question

        for (int i = 0; i < 4; i++) {
            fgets(options[totalQuestions][i], sizeof(options[totalQuestions][i]), fp);
            options[totalQuestions][i][strcspn(options[totalQuestions][i], "\n")] = 0;
            encryptDecrypt(options[totalQuestions][i]); // Decrypt option
        }

        char correct;
        fscanf(fp, " %c\n", &correct);
        correctAnswers[totalQuestions] = correct;
        encryptDecrypt(&correctAnswers[totalQuestions]); // Decrypt correct answer

        totalQuestions++;
        if (totalQuestions >= MAX_QUESTIONS) break;
    }

    fclose(fp);

    memset(studentAnswers[studentIndex], 0, sizeof(studentAnswers[studentIndex]));
    memset(questionStatus[studentIndex], 0, sizeof(questionStatus[studentIndex]));

    printf("\nQuiz for '%s' loaded successfully!\n", subjects[subjectIndex]);
    takeQuiz(studentIndex, subjectIndex);
}

// Function for student login and quiz selection
// Function to handle student login and quiz selection
void studentLogin(int rollNo) {
    int studentIndex = -1;

    // Check if student already exists
    for (int i = 0; i < totalStudents; i++) {
        if (rollNumbers[i] == rollNo) {
            studentIndex = i;
            break;
        }
    }

    // If new student, register them
    if (studentIndex == -1) {
        rollNumbers[totalStudents] = rollNo;
        studentIndex = totalStudents;
        totalStudents++;
    }

    int subSelect;
    printf("\nSelect subject for quiz:\n");
    for (int i = 0; i < MAX_SUBJECT; i++) {
        printf("%d. %s\n", i + 1, subjects[i]);
    }
    printf("Enter your choice (1-%d): ", MAX_SUBJECT);
    scanf("%d", &subSelect);

    if (subSelect >= 1 && subSelect <= MAX_SUBJECT) {
        loadQuestionFile(studentIndex, subSelect - 1); // Load questions for the selected subject
    } else {
        printf("Invalid subject selection!\n");
    }
}


// Function to view all results (Teacher/Admin)
void viewAllResults(const char* role, const char* teacherSubject) {
    printQuizAsciiArt();
    FILE *file = fopen("results.txt", "r");
    if (file == NULL) {
        printf("Error: Unable to find results file!\n");
        return;
    }

    char line[256];
    printf("\n----- Results -----\n");

    while (fgets(line, sizeof(line), file)) {
        // If the user is a teacher, only show results for their subject
        if (strcmp(role, "Teacher") == 0) {
            if (strstr(line, teacherSubject) != NULL) {
                printf("%s", line); // Print the result if it matches the teacher's subject
            }
        }
        // If the user is an admin, show all results
        else if (strcmp(role, "Admin") == 0) {
            printf("%s", line); // Print all results
        }
    }

    fclose(file);
}

// Function to view individual result (Student)
// ...existing code...
void viewStudentResult(int rollNo) {
    printQuizAsciiArt();
    FILE *file = fopen("results.txt", "r");
    if (file == NULL) {
        printf("Error: Unable to find results file!\n");
        return;
    }

    char line[256];
    int found = 0;

    printf("\n----- Your Result -----\n");
    char rollNoStr[20];
    // Add comma to ensure unique match
    sprintf(rollNoStr, "Roll No: %d,", rollNo);

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, rollNoStr) != NULL) {
            printf("%s", line);
            found = 1;
        }
    }

    if (!found) {
        printf("No results found for Roll No: %d\n", rollNo);
    }

    fclose(file);
}


// To hide the password
#ifdef _WIN32
    #include <conio.h>
    void getPassword(char *password) {
        int i = 0;
        char ch;
        while ((ch = _getch()) != '\r') { // Stop on Enter key
            if (ch == '\b' && i > 0) { // Handle backspace
                printf("\b \b");
                i--;
            } else if (ch != '\b') {
                password[i++] = ch;
                printf("*");
            }
        }
        password[i] = '\0'; // Null-terminate string
    }
#else
    #include <termios.h>
    #include <unistd.h>
    void getPassword(char *password) {
        struct termios oldt, newt;
        int i = 0;
        char ch;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        while ((ch = getchar()) != '\n' && i < 49) {
            if (ch == '\b' && i > 0) {
                printf("\b \b");
                i--;
            } else {
                password[i++] = ch;
                printf("*");
            }
        }
        password[i] = '\0';

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
#endif

int verify_teacher(char* teacherUsername) {
    Teacher t1 = {
        .sub = {"Physics", "Chemistry", "Math", "Biology", "English"},
        .password = {"1234", "5678", "91011", "1213", "1415"}
    };

    char tempusr[50], temppswd[50];
    printf("Username and subject name are the same\n");
    printf("Enter username: ");
    fgets(tempusr, sizeof(tempusr), stdin);
    tempusr[strcspn(tempusr, "\n")] = 0; // Remove trailing newline

    printf("Enter password: ");
    getPassword(temppswd);
    printf("\n");

    for (int i = 0; i < MAX_SUBJECT; i++) {
        if (strcmp(t1.sub[i], tempusr) == 0) {
            if (strcmp(t1.password[i], temppswd) == 0) {
                printf("Access Granted!\nWelcome, %s\n", t1.sub[i]);
                strcpy(teacherUsername, t1.sub[i]);  // Store the username
                return 1;  // Return 1 to indicate success
            } else {
                printf("Wrong Password!\n");
                return 0;  // Return 0 for failure
            }
        }
    }
    printf("Username not found!\n");
    return 0;  // Return 0 if the username is not found
}

void teacherMenu() {
    char teacherUsername[50];  // Variable to store the username

    while (getchar() != '\n'); // Clear input buffer before username input

    if (verify_teacher(teacherUsername)==1) {  // Only proceed if password is correct
        int choice;
        while (1) {
            system("cls");
            printQuizAsciiArt(); // Print quiz ASCII art
            printf("\n--- Quiz System - Teacher Menu ---\n");
            printf("1. Enter Questions\n");
            printf("2. View Results\n");
            printf("3. Go Back to Main Menu\n");
            printf("Enter choice: ");
            
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input! Please enter a number between 1 and 3.\n");
                while (getchar() != '\n'); // Clear the invalid input
                continue;
            }

            switch (choice) {
                case 1: 
                    enterQuestions(teacherUsername);  // Pass the username to enterQuestions
                    break;
                case 2: 
                    viewAllResults("Teacher", teacherUsername); // Pass role and subject
                    break;
                case 3: 
                    return; // Exit the menu
                default: 
                    printf("Invalid choice! Try again.\n");
            }
        }
    } else {
        printf("Access Denied!\n");
    }
}

// Student menu
void student_menu() {
    int choice, rollNo;

    printf("Enter Roll Number: ");
    scanf("%d", &rollNo);

    while (1) {
        system("cls");
        printQuizAsciiArt(); // Print quiz ASCII art
        printf("\n--- Quiz System - Student Menu ---\n");
        printf("1. Take Quiz\n");
        printf("2. View Result\n");
        printf("3. Go Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                studentLogin(rollNo); // Pass roll number to studentLogin
                break;
            case 2: 
                viewStudentResult(rollNo); // Pass roll number to viewStudentResult
                break;
            case 3: 
                return;
            default: 
                printf("Invalid choice! Try again.\n");
        }
    }
}

// Function to verify admin credentials
int verify_admin() {
    const char adminUsername[] = "Admin";
    const char adminPassword[] = "admin123";

    char tempUsername[50], tempPassword[50];

    printf("Enter Admin Username: ");
    getchar(); // Clear input buffer
    fgets(tempUsername, sizeof(tempUsername), stdin);
    tempUsername[strcspn(tempUsername, "\n")] = 0; // Remove trailing newline

    printf("Enter Admin Password: ");
    getPassword(tempPassword);
    printf("\n");

    if (strcmp(tempUsername, adminUsername) == 0 && strcmp(tempPassword, adminPassword) == 0) {
        printf("Access Granted! Welcome, Admin.\n");
        return 1; // Success
    } else {
        printf("Invalid Admin Credentials!\n");
        return 0; // Failure
    }
}

// Admin menu
void adminMenu() {
    if (!verify_admin()) {
        printf("Access Denied!\n");
        return; // Exit if login fails
    }

    int choice;
    while (1) {
        system("cls");
        printQuizAsciiArt();
        printf("\n--- Quiz System - Admin Menu ---\n");
        printf("1. View All Results\n");
        printf("2. Go Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                viewAllResults("Admin", NULL); // Admin can view all results
                break;
            case 2: 
                return;
            default: 
                printf("Invalid choice! Try again.\n");
        }
    }
}


// Main Menu
int main() {
    int choice;
    while (1) {
        system("cls"); // Clear the console
        printQuizAsciiArt(); // Print quiz ASCII art
        printf("\n--- Quiz System - Main Menu ---\n");
        printf("1. Teacher\n");
        printf("2. Student\n");
        printf("3. Admin\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                teacherMenu(); 
                break;
            case 2: 
                student_menu(); 
                break;
            case 3: 
                adminMenu(); 
                break;
            case 4: 
                printf("Thanks for using the Quiz System!\n");
                return 0;
            default: 
                printf("Invalid choice! Try again.\n");
        }
    }
}
