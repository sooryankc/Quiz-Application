
# 🎯 CLI-Based Quiz Application

A secure, terminal-based **Quiz Management System** built in C as part of a capstone project. This system supports **Teacher (Admin)** and **Student** modes with encrypted question storage, interactive navigation, subject-wise quizzes, and score tracking.

---

## 🚀 Features

### 🔐 Admin
- View all results across subjects
- Secure login with credentials

### 👨‍🏫 Teacher Mode
- Subject-wise login with credentials
- Add,manage and view multiple-choice questions
- XOR-based encryption for question security
- Save questions to subject-specific encrypted files
- View student results filtered by subject
- Control resubmission from students
- Set timer for each quiz in minutes(5-30)

### 🎓 Student Mode
- Roll number-based login
- Subject selection for quizzes
- Decryption of questions at load time
- Interactive quiz navigation:
   - Skip, Jump, Next, Previous
- Answer tracking during quiz
- Final score display and logging with timestamp

---

## 🛠️ Technologies Used

- **Language**: C
- **Encryption**: XOR cipher
- **File Handling**: For persistent storage of quizzes and results
- **Cross-platform Input Handling**: Supports both Windows and Linux (`getch()` and `kbhit()` compatibility)

---

## 🧠 How It Works

1. **Teachers** input time, permission status for resubmission, MCQs and correct answers through CLI.
2. The quiz data is encrypted and saved as `<subject>_questions.txt`.
3. **Students** log in with roll numbers and select the subject.
4. Check if student has already taken the quiz.
5. If already quiz ws taken by the roll no. based on the resubmission permission, quiz will be proceeded.
6. Questions are decrypted at runtime.
7. The quiz 
8. The system calculates scores and logs results in `results.txt`.

---

## 📂 File Structure

```
.
├── quiz.c                  # Main source code
├── results.txt             # Student results log
├── <subject>_questions.txt # Encrypted question files
├── quiz_application.exe    # Executable (Windows)
└── README.txt              # Project documentation
```

## 📄 Function Descriptions

✅ **`printQuizAsciiArt()`**  
Displays stylized ASCII banner for branding and aesthetic.

✅ **`encryptDecrypt(char *text)`**  
Encrypts or decrypts data using XOR with a static key. Used to secure quiz content.

✅ **`enterQuestions(char* teacherUsername)`**  
Allows teachers to enter new questions, options, and answers.  
Data is encrypted and saved in `<subject>_questions.txt`.

✅ **`loadQuestionFile(int studentIndex, int subjectIndex)`**  
Loads encrypted quiz content from file and decrypts it for student use.

✅ **`takeQuiz(int studentIndex, int subjectIndex)`**  
Manages the entire quiz experience:
- Displays questions with options
- Handles timer and keyboard input
- Allows navigation (Next, Previous, Skip, Jump, Quit)
- Tracks answer status until timeout or submission

✅ **`saveResultToFile()`**  
Saves quiz result (roll number, subject, score, date/time) to `results.txt`.

✅ **`viewAllResults(const char* role, const char* subject)`**  
Displays all results for a subject (for teachers) or all subjects (for admins).

✅ **`viewStudentResult(int rollNo)`**  
Displays past results for a specific student using roll number.

✅ **`studentLogin(int rollNo)`**  
Handles student entry, subject selection, and initiates quiz.

✅ **`teacherMenu()`**  
Provides teacher interface to:
- Enter questions
- View results
- Delete their own quiz files
- View saved questions

✅ **`adminMenu()`**  
Provides admin interface to:
- View all student results
- Clear the result log

✅ **`getPassword(char *password)`**  
Secure password input that hides user input.  
Works on both Windows and Linux/macOS.

✅ **`kbhit_cross()` & `getch_cross()`**  
Cross-platform, non-blocking keyboard input functions.  
Used in the quiz loop for real-time input handling while displaying a timer.

✅ **`printBorderLine()`**  
Prints a styled border line in the terminal UI for clean layout separation.

✅ **`printCenteredWithBorder(const char *text)`**  
Prints text centered inside a border (e.g., `| text |`) for neat quiz display.

✅ **`printCenteredWithBorderTruncate(const char *text)`**  
Like `printCenteredWithBorder`, but trims long strings to prevent overflow.

✅ **`clearAllResults()`**  
Clears all entries from `results.txt`. Only available to Admins.

✅ **`deleteQuiz()`**  
Allows a teacher to delete their own subject quiz file securely.

✅ **`viewSavedQuestions()`**  
Decrypts and displays saved questions, options, and answers for a teacher.

```
```
### 3️⃣ Use
Follow the prompts to enter as Admin, Teacher, or Student.

---

## 📄 Result Format
---

Example entry in `results.txt`:
```
Date: 2025-04-16 Time: 19:32:10 Subject: Computer Networks Roll No: 220124 Score: 8 / 10
```

---

## ⛓️ Encryption

- Questions and answers are encrypted using a simple **XOR cipher**.
- Helps protect quiz integrity from tampering.

---

## 🧭 Quiz Navigation Controls

| Key | Action              |
|-----|---------------------|
| A-D | Select Answer       |
| N   | Next Question       |
| P   | Previous Question   |
| J   | Jump to Question    |
| S   | Skip Question       |
| Q   | Quit the Quiz       |

---

## 👤 Authors

Developed with ❤️ by:
- **Soorya Narayanan**
- **Jyothir Adithya**
- **Samridhi Rauthan**
