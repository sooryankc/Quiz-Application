
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
7. The system calculates scores and logs results in `results.txt`.

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
