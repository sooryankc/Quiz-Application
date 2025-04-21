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
#define BORDER_WIDTH 80
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
int quizDuration=5;
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

// Function prototypes
void printBorderLine(void);
void printCenteredWithBorder(const char* text);
void printEmptyBorderLine(void);
void printCenteredWithBorderTruncate(const char* text);
void printQuizAsciiArt(void);
void encryptDecrypt(char *text);
void enterQuestions(char* teacherUsername);
void loadQuestionFile(int studentIndex, int subjectIndex);
void saveResultToFile(int rollNo, int score, int totalQuestions, const char* subjectName);
void viewStudentResult(int rollNo);
void displayQuestionStatus(int studentIndex);
int kbhit_cross(void);
char getch_cross(void);
void takeQuiz(int studentIndex, int subjectIndex);
void studentLogin(int rollNo);
void viewAllResults(const char* role, const char* subjectFilter);
void clearAllResults(void);
void getPassword(char *password);
int verify_teacher(char* teacherUsername);
void teacherMenu(void);
void student_menu(void);
int verify_admin(void);
void adminMenu(void);
void viewSavedQuestions(const char* teacherUsername);
void displayQuizResults(int rollNo, int score, int totalQuestions, const char* subjectName);
void deleteQuiz(const char* teacherUsername);

// Function to print a border line
void printBorderLine() {
    for (int i = 0; i < BORDER_WIDTH; i++) printf("=");
    printf("\n");
}
// Function to print centered text with border
void printCenteredWithBorder(const char* text) {
    int len = (int)strlen(text);
    int pad = (BORDER_WIDTH - 2 - len) / 2; // Calculate padding for centering
    if (pad < 0) pad = 0; // Ensure no negative padding
    printf("|");
    for (int i = 0; i < pad; i++) printf(" "); // Left padding
    printf("%s", text); // Left-aligned text
    for (int i = 0; i < BORDER_WIDTH - 2 - pad - len; i++) printf(" "); // Right padding
    printf("|\n");
}
// Function to print empty border line
void printEmptyBorderLine() {
    printf("|");
    for (int i = 0; i < BORDER_WIDTH - 2; i++) printf(" ");
    printf("|\n");
}
// Function to print centered text with border and truncate if too long
void printCenteredWithBorderTruncate(const char* text) {
    int maxLen = BORDER_WIDTH - 4; // 2 for borders, 2 for padding
    char buf[512];
    strncpy(buf, text, maxLen); // Truncate the text if it exceeds max length
    buf[maxLen] = '\0';
    printCenteredWithBorder(buf); // Use the updated function for left-aligned centering
}

// Function to print quiz ASCII art
void printQuizAsciiArt() {
    const char* art[] = {
        "                                                  ",
        "                                                  ",
        " .d88b.       db    db      d888888b      d88888D ",
        ".8P  Y8.      88    88        `88'        YP  d8' ",
        "88    88      88    88         88            d8'  ",
        "88    88      88    88         88           d8'   ",
        "`8P  d8'      88b  d88        .88.         d8' db ",
        " `Y88'Y8       Y8888P       Y888888P      d88888P ",
        "                                                  ",
        "                                                  "
    };
    printBorderLine();
    for (int i = 0; i < 10; i++) {
        printCenteredWithBorder(art[i]);
    }
    printBorderLine();
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
    char filename[100];
    sprintf(filename, "%s_questions.txt", teacherUsername);

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Set quiz duration in minutes (5-30). Enter 0 for no time limit: ");
    scanf("%d", &quizDuration);
    if (quizDuration < 0) quizDuration = 0; // Ensure no negative values
    if (quizDuration > 30) quizDuration = 30;
    fprintf(file, "DURATION:%d\n", quizDuration);
    while (getchar() != '\n');

    // Add resubmission policy
    char allowResubmission;
    printf("Allow resubmission of this quiz? (Y/N): ");
    scanf(" %c", &allowResubmission);
    while (getchar() != '\n'); // Clear input buffer
    allowResubmission = toupper(allowResubmission); // Convert to uppercase for consistency
    if (allowResubmission != 'Y' && allowResubmission != 'N') {
        printf("Invalid input! Defaulting to 'N' (No resubmission allowed).\n");
        allowResubmission = 'N';
    }
    fprintf(file, "RESUBMISSION:%c\n", allowResubmission);

    int totalQ;
    printf("Enter number of questions (Max %d): ", MAX_QUESTIONS);
    scanf("%d", &totalQ);
    if (totalQ > MAX_QUESTIONS) totalQ = MAX_QUESTIONS;
    while (getchar() != '\n');

    for (int i = 0; i < totalQ; i++) {
        system("cls");
        printQuizAsciiArt();
        printf("Enter Question %d of %d (or type Q/q to return to menu)\n", i + 1, totalQ);

        printf("Type your question :");
        fgets(questions[i], sizeof(questions[i]), stdin);
        questions[i][strcspn(questions[i], "\n")] = 0;

        // Check for quit command
        if (strcasecmp(questions[i], "Q") == 0 || strcasecmp(questions[i], "q") == 0) {
            printf("Exiting question entry and returning to Teacher Menu...\n");
            system("pause");
            break;
        }
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
    printf("Questions successfully recorded in '%s'!\n", filename);
    system("pause");
    fflush(stdin); // Clear input buffer
}
// Function to load questions from file
void loadQuestionFile(int studentIndex, int subjectIndex) {
    char filename[100];
    sprintf(filename, "%s_questions.txt", subjects[subjectIndex]);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error: Quiz not yet available for '%s'. Please contact the teacher.\n", subjects[subjectIndex]);
        system("pause");
        fflush(stdin); // Clear input buffer
        return;
    }

    // Read duration from the first line
    char line[256];
    if (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "DURATION:", 9) == 0) {
            quizDuration = atoi(line + 9);
        } else {
            quizDuration = 5; // Default if not found
            fseek(fp, 0, SEEK_SET); // Rewind if not duration line
        }
    }

    // Read resubmission policy
    char resubmissionPolicy = 'N'; // Default to no resubmission
    if (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "RESUBMISSION:", 13) == 0) {
            resubmissionPolicy = toupper(line[13]);
        }
    }

    // Check if the student has already taken the quiz
    FILE *resultsFile = fopen("results.txt", "r");
    if (resultsFile) {
        char rollNoStr[40];
        sprintf(rollNoStr, "Roll No: %d,", rollNumbers[studentIndex]);
        int alreadyTaken = 0;

        while (fgets(line, sizeof(line), resultsFile)) {
            if (strstr(line, rollNoStr) != NULL && strstr(line, subjects[subjectIndex]) != NULL) {
                alreadyTaken = 1;
                break;
            }
        }
        fclose(resultsFile);

        if (alreadyTaken) {
            if (resubmissionPolicy == 'Y') {
                printf("\nYou have already taken the quiz for '%s'.\n", subjects[subjectIndex]);
                printf("Do you want to retake the quiz? (Y/N): ");
                char choice;
                scanf(" %c", &choice);
                while (getchar() != '\n'); // Clear input buffer
                if (toupper(choice) != 'Y') {
                    printf("Returning to the student menu...\n");
                    system("pause");
                    fclose(fp);
                    return;
                }
            } else {
                printf("\nYou have already taken the quiz for '%s'. Resubmission is not allowed.\n", subjects[subjectIndex]);
                system("pause");
                fclose(fp);
                return;
            }
        }
    }

    totalQuestions = 0;
    while (fgets(questions[totalQuestions], sizeof(questions[totalQuestions]), fp)) {
        questions[totalQuestions][strcspn(questions[totalQuestions], "\n")] = 0; // Remove newline
        encryptDecrypt(questions[totalQuestions]); // Decrypt question

        // Read options
        for (int i = 0; i < 4; i++) {
            if (!fgets(options[totalQuestions][i], sizeof(options[totalQuestions][i]), fp)) {
                printf("Error: Malformed quiz file (missing options for question %d). Please contact teacher.\n", totalQuestions + 1);
                fclose(fp);
                system("pause");
                fflush(stdin); // Clear input buffer
                return;
            }
            options[totalQuestions][i][strcspn(options[totalQuestions][i], "\n")] = 0; // Remove newline
            encryptDecrypt(options[totalQuestions][i]); // Decrypt option
        }

        // Read correct answer
        char correct;
        if (fscanf(fp, " %c\n", &correct) != 1) {
            printf("Error: Malformed quiz file (missing correct answer for question %d). Please contact the teacher.\n", totalQuestions + 1);
            fclose(fp);
            system("pause");
            fflush(stdin); // Clear input buffer
            return;
        }
        correctAnswers[totalQuestions] = correct;
        encryptDecrypt(&correctAnswers[totalQuestions]); // Decrypt correct answer

        totalQuestions++;
        if (totalQuestions >= MAX_QUESTIONS) break;
    }

    fclose(fp);

    if (totalQuestions == 0) {
        printf("No questions found for this quiz. Please contact the teacher.\n");
        system("pause");
        fflush(stdin); // Clear input buffer
        return;
    }

    memset(studentAnswers[studentIndex], 0, sizeof(studentAnswers[studentIndex]));
    memset(questionStatus[studentIndex], 0, sizeof(questionStatus[studentIndex]));

    printf("\nQuiz for '%s' loaded successfully!\n", subjects[subjectIndex]);

    // Clear the input buffer after system("pause")
    system("pause");
    fflush(stdin); // Clear input buffer

    takeQuiz(studentIndex, subjectIndex);
}

// Function to save student results to a file
void saveResultToFile(int rollNo, int score, int totalQuestions, const char* subjectName) {
    FILE *file = fopen("results.txt", "a"); // Open the file in append mode
    if (file == NULL) {
        printf("Unable to open results file! Please contact the teacher/admin.\n");
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

// Function to view individual result (Student)
void viewStudentResult(int rollNo) {
    system("cls");
    printQuizAsciiArt();
    printBorderLine();
    printEmptyBorderLine();
    printCenteredWithBorder("----- Your Result -----");
    printEmptyBorderLine();

    FILE *file = fopen("results.txt", "r");
    if (file == NULL) {
        printCenteredWithBorder("Unable to find results file! Please contact the teacher/admin.");
        printBorderLine();
        system("pause"); // <-- Add this line
        return;
    }

    char line[256];
    int found = 0;
    char rollNoStr[40];
    sprintf(rollNoStr, "Roll No: %d,", rollNo);

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, rollNoStr) != NULL) {
            printCenteredWithBorder(line);
            found = 1;
        }
    }
    if (!found) {
        char msg[80];
        snprintf(msg, sizeof(msg), "No results found for Roll No: %d", rollNo);
        printCenteredWithBorder(msg);
    }
    fclose(file);
    printBorderLine();
    system("pause"); // <-- Add this line
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



#ifdef _WIN32
#include <conio.h>
#include <windows.h>
int kbhit_cross() { return _kbhit(); }
char getch_cross() { return _getch(); }
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
int kbhit_cross() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}
char getch_cross() {
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

// Function to display quiz results page
void displayQuizResults(int rollNo, int score, int totalQuestions, const char* subjectName) {
    system("cls"); // Clear the screen
    printQuizAsciiArt();
    printBorderLine();
    printEmptyBorderLine();
    printCenteredWithBorder("----- Quiz Results -----");
    printEmptyBorderLine();

    char resultLine[100];
    snprintf(resultLine, sizeof(resultLine), "Roll Number: %d", rollNo);
    printCenteredWithBorder(resultLine);

    snprintf(resultLine, sizeof(resultLine), "Subject: %s", subjectName);
    printCenteredWithBorder(resultLine);

    snprintf(resultLine, sizeof(resultLine), "Score: %d/%d", score, totalQuestions);
    printCenteredWithBorder(resultLine);

    printEmptyBorderLine();
    printBorderLine();
    printf("Press any key to return to the main menu...\n");
    getch(); // Wait for user input
}


// Function to take the quiz (Student)
void takeQuiz(int studentIndex, int subjectIndex) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Clear input buffer

    int currentQuestion = 0;
    time_t startTime = time(NULL);
    int timeLimit = quizDuration * 60;
    int timeout = 0;

    while (1) {
        char answer = 0;
        int inputReceived = 0;
        int lastRemaining = -1;

        while (!inputReceived) {
            time_t now = time(NULL);
            int elapsed = (int)(now - startTime);
            int remaining = (quizDuration == 0) ? -1 : timeLimit - elapsed; // No time limit if quizDuration is 0

            if (quizDuration != 0 && remaining <= 0) { // Only check timeout if there is a time limit
                timeout = 1;
                break;
            }

            // Only redraw if timer changed or first draw
            if (remaining != lastRemaining) {
                system("cls");
                printQuizAsciiArt();
                printBorderLine();
                if (quizDuration == 0) { // Display "No Time Limit" if there is no time limit
                    printCenteredWithBorder("No Time Limit");
                } else {
                    char timerLine[40];
                    snprintf(timerLine, sizeof(timerLine), "Time Remaining: %02d:%02d", remaining / 60, remaining % 60);
                    printCenteredWithBorder(timerLine); // Keep the timer center-aligned
                }
                printBorderLine();

                // Show attended/not attended status
                displayQuestionStatus(studentIndex);

                printBorderLine();

                // Print question (truncate if too long)
                if (currentQuestion >= 0 && currentQuestion < totalQuestions) {
                    char qLine[300];
                    snprintf(qLine, sizeof(qLine), "Q%d: %s", currentQuestion + 1, questions[currentQuestion]);
                    printCenteredWithBorderTruncate(qLine);

                    // Print options (truncate if too long)
                    for (int i = 0; i < 4; i++) {
                        char oLine[300];
                        snprintf(oLine, sizeof(oLine), "%c. %s", 'A' + i, options[currentQuestion][i]);
                        printCenteredWithBorderTruncate(oLine);
                    }
                } else {
                    printCenteredWithBorder("Error: Invalid question index.");
                }

                printBorderLine();
                printCenteredWithBorder("Your Answer (A/B/C/D), S=Skip, N=Next, P=Previous, J=Jump, Q=Quit: ");
                printBorderLine();

                lastRemaining = remaining;
            }

            // Non-blocking input check
            if (kbhit_cross()) {
                answer = toupper(getch_cross());
                inputReceived = 1;
                break;
            }
#ifdef _WIN32
            Sleep(200);
#else
            usleep(200000);
#endif
        }
        if (timeout) break;

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
    printBorderLine();
    if (timeout) {
        printCenteredWithBorder("Time's up! Your quiz has been auto-submitted.");
    }
    char scoreMsg[80];
    snprintf(scoreMsg, sizeof(scoreMsg), "Your score is: %d/%d", score, totalQuestions);
    printCenteredWithBorder(scoreMsg);
    printBorderLine();
    saveResultToFile(rollNumbers[studentIndex], score, totalQuestions, subjects[subjectIndex]);

    // Call the new function to display the results page
    displayQuizResults(rollNumbers[studentIndex], score, totalQuestions, subjects[subjectIndex]);
}

// Function for student login and quiz selection
// Function to handle student login and quiz selection
void viewAllResults(const char* role, const char* subjectFilter) {
    system("cls");
    printQuizAsciiArt();
    printBorderLine();
    printEmptyBorderLine();
    printCenteredWithBorder("----- Results -----");
    printEmptyBorderLine();

    FILE *file = fopen("results.txt", "r");
    if (file == NULL) {
        printCenteredWithBorder("Error: Unable to find results file!");
        printBorderLine();
        system("pause");
        return;
    }

    char line[256];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        if (subjectFilter == NULL || strstr(line, subjectFilter) != NULL) {
            printCenteredWithBorder(line);
            found = 1;
        }
    }
    if (!found) {
        printCenteredWithBorder("No results found.");
    }
    fclose(file);
    printBorderLine();
    system("pause");
}

// Function for Admin to clear all results
void clearAllResults() {
    FILE *file = fopen("results.txt", "w"); // Open in write mode to clear the file
    if (file == NULL) {
        printf("No results saved yet!\n");
        system("pause");
        return;
    }
    fclose(file);
    printf("All results have been cleared successfully!\n");
    system("pause");
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

// Function to view saved questions (Teacher)
void viewSavedQuestions(const char* teacherUsername) {
    char filename[100];
    sprintf(filename, "%s_questions.txt", teacherUsername);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: No saved questions found for '%s'.\n", teacherUsername);
        system("pause");
        return;
    }

    system("cls");
    printQuizAsciiArt();
    printBorderLine();
    printEmptyBorderLine();
    printCenteredWithBorder("--- Saved Questions ---");
    printEmptyBorderLine();

    char line[256];
    int questionNumber = 0;

    // Read and display quiz duration
    if (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "DURATION:", 9) == 0) {
            char durationMsg[50];
            snprintf(durationMsg, sizeof(durationMsg), "Quiz Duration: %d minutes", atoi(line + 9));
            printCenteredWithBorder(durationMsg);
        }
    }

    // Read and display resubmission policy
    if (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "RESUBMISSION:", 13) == 0) {
            char resubmissionMsg[50];
            snprintf(resubmissionMsg, sizeof(resubmissionMsg), "Resubmission Allowed: %c", line[13]);
            printCenteredWithBorder(resubmissionMsg);
        }
    }

    printEmptyBorderLine();

    // Read and display questions, options, and answers
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Remove trailing newline
        encryptDecrypt(line); // Decrypt the question or option

        // Display the question header for every question
        char questionHeader[50];
        snprintf(questionHeader, sizeof(questionHeader), "Question %d:", questionNumber + 1);
        printCenteredWithBorder(questionHeader);

        // Display the question
        printCenteredWithBorder(line);

        // Read and display options
        for (int i = 0; i < 4; i++) {
            if (fgets(line, sizeof(line), file)) {
                line[strcspn(line, "\n")] = 0; // Remove trailing newline
                encryptDecrypt(line); // Decrypt the option
                char optionLine[300];
                snprintf(optionLine, sizeof(optionLine), "%c. %s", 'A' + i, line);
                printCenteredWithBorder(optionLine);
            }
        }

        // Read and display the correct answer
        if (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0; // Remove trailing newline
            encryptDecrypt(line); // Decrypt the correct answer
            char answerLine[50];
            snprintf(answerLine, sizeof(answerLine), "Correct Answer: %s", line);
            printCenteredWithBorder(answerLine);
        }

        questionNumber++; // Increment the question number
        printEmptyBorderLine();
    }

    fclose(file);
    printBorderLine();
    system("pause");
}

// Function to verify teacher credentials
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

void deleteQuiz(const char* teacherUsername) {
    char filename[100];
    sprintf(filename, "%s_questions.txt", teacherUsername);

    // Check if the file exists
    if (remove(filename) == 0) {
        printf("Quiz for '%s' has been successfully deleted.\n", teacherUsername);
    } else {
        printf("Error: Unable to delete quiz for '%s'. File may not exist.\n", teacherUsername);
    }
    system("pause");
}

// Function for Teacher menu
void teacherMenu() {
    char teacherUsername[50];

    while (getchar() != '\n'); // Clear input buffer before username input

    if (verify_teacher(teacherUsername) == 1) {
        int choice;
        while (1) {
            system("cls");
            printQuizAsciiArt();
            printBorderLine();
            printEmptyBorderLine();
            printCenteredWithBorder("--- Quiz System - Teacher Menu ---");
            printEmptyBorderLine();
            printCenteredWithBorder("1. Enter Questions");
            printCenteredWithBorder("2. View Results");
            printCenteredWithBorder("3. View Saved Questions and quiz details");
            printCenteredWithBorder("4. Delete Quiz");
            printCenteredWithBorder("5. Go Back to Main Menu");
            printEmptyBorderLine();
            printBorderLine();
            printf("Enter choice: ");
            
            if (scanf("%d", &choice) != 1) {
                printf("Invalid input! Please enter a number between 1 and 4.\n");
                while (getchar() != '\n');
                continue;
            }

            switch (choice) {
                case 1: 
                    enterQuestions(teacherUsername);
                    break;
                case 2: 
                    viewAllResults("Teacher", teacherUsername);
                    break;
                case 3: 
                    viewSavedQuestions(teacherUsername);
                    break;
                case 4:
                    deleteQuiz(teacherUsername);
                    break;
                case 5: 
                    return;
                default: 
                    printf("Invalid choice! Try again.\n");
                    system("pause");
            }
        }
    } else {
        printf("Access Denied!\n");
        system("pause");
    }
}

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
    while (getchar() != '\n'); // Clear input buffer

    if (subSelect >= 1 && subSelect <= MAX_SUBJECT) {
        // Check if the student has already taken the quiz for the selected subject
        char subjectName[50];
        strcpy(subjectName, subjects[subSelect - 1]);

        FILE *file = fopen("results.txt", "r");
        if (file == NULL) {
            printf("Proceeding to quiz...\n");
        } else {
            char line[256];
            char rollNoStr[40];
            sprintf(rollNoStr, "Roll No: %d,", rollNo);
            int alreadyTaken = 0;
            char resubmissionPolicy = 'N'; // Default to no resubmission

            // Check resubmission policy from the quiz file
            char quizFilename[100];
            sprintf(quizFilename, "%s_questions.txt", subjects[subSelect - 1]);
            FILE *quizFile = fopen(quizFilename, "r");
            if (quizFile) {
                char quizLine[256];
                while (fgets(quizLine, sizeof(quizLine), quizFile)) {
                    if (strncmp(quizLine, "RESUBMISSION:", 13) == 0) {
                        resubmissionPolicy = toupper(quizLine[13]);
                        break;
                    }
                }
                fclose(quizFile);
            } else {
                printf("Error: Unable to open quiz file for '%s'.\n", subjectName);
            }

            // Debug print for resubmission policy
            if (resubmissionPolicy == 'Y') {
                printf("Resubmission is allowed for '%s'.\n", subjectName);
            } else {
                printf("Resubmission is not allowed for '%s'.\n", subjectName);
            }

            while (fgets(line, sizeof(line), file)) {
                if (strstr(line, rollNoStr) != NULL && strstr(line, subjectName) != NULL) {
                    alreadyTaken = 1;
                    break;
                }
            }
            fclose(file);

            if (alreadyTaken) {
                if (resubmissionPolicy == 'Y') {
                    printf("\nYou have already taken the quiz for '%s'.\n", subjectName);
                    printf("Do you want to retake the quiz? (Y/N): ");
                    char choice;
                    scanf(" %c", &choice);
                    while (getchar() != '\n'); // Clear input buffer
                    if (toupper(choice) != 'Y') {
                        printf("Returning to the student menu...\n");
                        system("pause");
                        return;
                    }
                } else {
                    printf("\nYou have already taken the quiz for '%s'. Resubmission is not allowed.\n", subjectName);
                    system("pause");
                    return;
                }
            }
        }

        // Load questions and take the quiz
        loadQuestionFile(studentIndex, subSelect - 1);
    } else {
        printf("Invalid subject selection!\n");
        system("pause");
    }
}

// Student menu
void student_menu() {
    int choice, rollNo;

    printf("Enter Roll Number: ");
    scanf("%d", &rollNo);

    while (1) {
        system("cls");
        printQuizAsciiArt();
        printBorderLine();
        printEmptyBorderLine();
        printCenteredWithBorder("--- Quiz System - Student Menu ---");
        printEmptyBorderLine();
        printCenteredWithBorder("1. Take Quiz");
        printCenteredWithBorder("2. View Result");
        printCenteredWithBorder("3. Go Back to Main Menu");
        printEmptyBorderLine();
        printBorderLine();
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                studentLogin(rollNo);
                break;
            case 2: 
                viewStudentResult(rollNo);
                break;
            case 3: 
                return;
            default: 
                printf("Invalid choice! Try again.\n");
                system("pause");
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
        system("pause");
        return 1; // Success
    } else {
        printf("Invalid Admin Credentials!\n");
        system("pause");
        return 0; // Failure
    }
}

// Admin menu
void adminMenu() {
    if (!verify_admin()) {
        printf("Access Denied!\n");
        return;
    }

    int choice;
    while (1) {
        system("cls");
        printQuizAsciiArt();
        printBorderLine();
        printEmptyBorderLine();
        printCenteredWithBorder("--- Quiz System - Admin Menu ---");
        printEmptyBorderLine();
        printCenteredWithBorder("1. View All Results");
        printCenteredWithBorder("2. View Results by Subject");
        printCenteredWithBorder("3. Clear All Results");
        printCenteredWithBorder("4. Go Back to Main Menu");
        printEmptyBorderLine();
        printBorderLine();
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: 
                viewAllResults("Admin", NULL); // View all results
                break;
            case 2: {
                int subSelect;
                printf("\nSelect subject to view results:\n");
                for (int i = 0; i < MAX_SUBJECT; i++) {
                    printf("%d. %s\n", i + 1, subjects[i]);
                }
                printf("Enter your choice (1-%d): ", MAX_SUBJECT);
                scanf("%d", &subSelect);
                while (getchar() != '\n'); // Clear input buffer
                if (subSelect >= 1 && subSelect <= MAX_SUBJECT) {
                    viewAllResults("Admin", subjects[subSelect - 1]); // View results for a specific subject
                } else {
                    printf("Invalid subject selection!\n");
                    system("pause");
                }
                break;
            }
            case 3: 
                clearAllResults(); // Clear all results
                break;
            case 4: 
                return; // Go back to the main menu
            default: 
                printf("Invalid choice! Try again.\n");
                system("pause");
        }
    }
}


// Main Menu
int main() {
    int choice;
    while (1) {
        system("cls");
        printQuizAsciiArt();
        printBorderLine();
        printEmptyBorderLine();
        printCenteredWithBorder("--- Quiz System - Main Menu ---");
        printEmptyBorderLine();
        printCenteredWithBorder("1. Teacher");
        printCenteredWithBorder("2. Student");
        printCenteredWithBorder("3. Admin");
        printCenteredWithBorder("4. Exit");
        printEmptyBorderLine();
        printBorderLine();
        printf("Enter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number between 1 and 4.\n");
            while (getchar() != '\n'); // Clear the input buffer
            system("pause");
            continue;
        }

        if (choice == 1) {
            teacherMenu();
        } else if (choice == 2) {
            student_menu();
        } else if (choice == 3) {
            adminMenu();
        } else if (choice == 4) {
            system("cls");
            printQuizAsciiArt();
            printBorderLine();
            printf("Thanks for using the Quiz System!\n");
            system("pause");
            system("cls");
            return 0;
        } else {
            printf("Invalid choice! Try again.\n");
            system("pause");
        }
    }
}
