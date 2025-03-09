#include <stdio.h>
#include <string.h>
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
#define MAX_STUDENTS 1000


// Quiz Array
char questions[MAX_QUESTIONS][200];
char options[MAX_QUESTIONS][4][100];
char correctAnswers[MAX_QUESTIONS];
int totalQuestions = 0;

// Student Array
int rollNumbers[MAX_STUDENTS];
char studentAnswers[MAX_STUDENTS][MAX_QUESTIONS];
int studentScores[MAX_STUDENTS];
int totalStudents = 0;
int questionStatus[MAX_STUDENTS][MAX_QUESTIONS];

// Function to validate only A, B, C, D input
char getValidAnswer() {
    char answer;
    while (1) {
        printf("Select option (A/B/C/D): ");
        scanf(" %c", &answer);
        if (answer == 'A' || answer == 'B' || answer == 'C' || answer == 'D'||answer =='a'||answer =='b'||answer =='c'||answer =='d') {
            return answer;
        }
        printf("Invalid choice! Enter A, B, C, or D.\n");
    }
}

// Function to enter quiz questions (Teacher)
void enterQuestions() {
    printf("Enter number of questions (Max %d): ", MAX_QUESTIONS);
    scanf("%d", &totalQuestions);
    if (totalQuestions > MAX_QUESTIONS) totalQuestions = MAX_QUESTIONS;

    for (int i = 0; i < totalQuestions; i++) {
        printf("\nEnter Question %d: ", i + 1);
        getchar();
        fgets(questions[i], sizeof(questions[i]), stdin);

        for (int j = 0; j < 4; j++) {
            printf("Option %c: ", 'A' + j);
            fgets(options[i][j], sizeof(options[i][j]), stdin);
        }

        printf("Enter Correct Answer (A/B/C/D): ");
        correctAnswers[i] = getValidAnswer();
    }
    printf("Questions successfully recorded!\n");
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

// Function to take the quiz (Student)
void takeQuiz(int studentIndex) {
    int score = 0;
    int qIndex = 0;
    char choice;
    memset(questionStatus[studentIndex], 0, sizeof(questionStatus[studentIndex]));

    while (1) {
        displayQuestionStatus(studentIndex);
        printf("\nQ%d: %s", qIndex + 1, questions[qIndex]);
        for (int j = 0; j < 4; j++) {
            printf("%c. %s", 'A' + j, options[qIndex][j]);
        }

        printf("\nPress S to skip, J to jump, N for next, P for previous, or F to finish: ");
        scanf(" %c", &choice);

        if (choice == 'S' || choice == 's') { 
            // Skip question (do not mark as attended)
            if (qIndex < totalQuestions - 1) qIndex++;
        } 
        else if (choice == 'J' || choice == 'j') {
            // Jump to a specific question
            int jumpTo;
            printf("Enter question number to jump to (1-%d): ", totalQuestions);
            scanf("%d", &jumpTo);
            if (jumpTo >= 1 && jumpTo <= totalQuestions) {
                qIndex = jumpTo - 1;
            } else {
                printf("Invalid question number!\n");
            }
        }
        else {
            // Answer question
            studentAnswers[studentIndex][qIndex] = getValidAnswer();
            questionStatus[studentIndex][qIndex] = 1; // Mark as attended

            if ((choice == 'N' || choice == 'n') && qIndex < totalQuestions - 1) qIndex++;
            else if ((choice == 'P' || choice == 'p') && qIndex > 0) qIndex--;
            else if (choice == 'F' || choice == 'f') break;
        }
    }

    for (int i = 0; i < totalQuestions; i++) {
        if (studentAnswers[studentIndex][i] == correctAnswers[i]) {
            score++;
        }
    }
    studentScores[studentIndex] = score;
    printf("\nQuiz Completed! Your Score: %d/%d\n", score, totalQuestions);
}



// Student Login and Record Handling
void studentLogin() {
    int rollNo, studentIndex = -1;
    printf("Enter Roll Number: ");
    scanf("%d", &rollNo);

    for (int i = 0; i < totalStudents; i++) {
        if (rollNumbers[i] == rollNo) {
            studentIndex = i;
            break;
        }
    }

    if (studentIndex == -1) {
        rollNumbers[totalStudents] = rollNo;
        studentIndex = totalStudents;
        totalStudents++;
    }

    takeQuiz(studentIndex);
}


// View all student scores (Teacher)
void viewAllResults() {
    printf("\n----- Student Results -----\n");
    for (int i = 0; i < totalStudents; i++) {
        printf("Roll No: %d, Score: %d/%d\n", rollNumbers[i], studentScores[i], totalQuestions);
    }
}


// View individual result (Student)
void viewStudentResult() {
    int rollNo;
    printf("Enter Roll Number: ");
    scanf("%d", &rollNo);

    for (int i = 0; i < totalStudents; i++) {
        if (rollNumbers[i] == rollNo) {
            printf("Your Score: %d/%d\n", studentScores[i], totalQuestions);
            return;
        }
    }
    printf("Roll number not found!\n");
}


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
int verify_teacher() {
    char psswd[] = "adminpasswd";
    char temp[50];

    printf("Enter password: ");
    getPassword(temp);
    printf("\n");

    if (strcmp(psswd, temp) == 0) {
        printf("Access Granted!\n");
        return 1;
    } else {
        printf("Wrong Password!\n");
        return 0;
    }
}

void teacherMenu() {
    if (verify_teacher()) {  // Only proceed if password is correct
        int choice;
        while (1) {
            printf("\n--- Teacher Menu ---\n");
            printf("1. Enter Questions\n");
            printf("2. View Results\n");
            printf("3. Go Back to Main Menu\n");
            printf("Enter choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1: enterQuestions(); break;
                case 2: viewAllResults(); break;
                case 3: return;
                default: printf("Invalid choice! Try again.\n");
            }
        }
    } else {
        printf("Access Denied!\n");
    }
}


// Student Menu
void student_menu(){
    int choice;
    while (1){
        printf("\n--- Quiz System - Student Menu ---\n");
        printf("1. Take Quiz\n");
        printf("2. View Result\n");
        printf("3. Go Back to Main Menu \n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:studentLogin(); break;
            case 2:viewStudentResult(); break;
            case 3: return;
            default: printf("Invalid choice! Try again.\n");
        }
    }
}


// Main Menu
int main() {
    int choice,n=0;
    while (1) {
        printf("\n--- Quiz System - Main Menu  ---\n");
        printf("1. Teacher\n");
        printf("2. Student \n");
        printf("3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
            case 1: teacherMenu(); break;
            case 2: student_menu(); break;
            case 3: printf("Thanks");return(0);break;
            default: printf("Invalid choice! Try again.\n");
        }
    }
}
