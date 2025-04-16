# 🎯 CLI-Based Quiz Application

A secure, terminal-based Quiz Management System built in **C** as part of a capstone project. The system includes separate **Teacher (Admin)** and **Student** modes, encrypted question storage, interactive navigation, subject-wise quizzes, and score tracking.

---

## 🚀 Features

### 🔐 Admin
- View all the results irrespective of the subject
### 👨‍🏫 Teacher Mode
- Subject-wise login using secure password
- Add, edit, and store multiple-choice questions
- XOR-based encryption for question security
- Save encrypted questions in subject-specific files
- Subject based access into student results.

### 🎓 Student Mode
- Roll number-based authentication
- Select quiz subject
- Decryption of question sets during load
- Navigate quiz with **Next, Previous, Skip, Jump to Question**
- Real-time answer status tracking
- Final score displayed and logged with date and time

---

## 🛠️ Technologies Used

- **Language:** C (GCC)
- **File Handling:** For permenently storing questions, options and answers and student results
- **Encryption:** XOR cipher
- **Platform Compatibility:** Windows, Linux (cross-platform with `getch()` handling)

---

## 🧠 How It Works

- Teachers input questions, options and answers via CLI and store it into file after encryption `<subject>_questions.txt`
- Students log in, choose subjects, and take quizzes
- The system tracks scores, saves responses, and logs attempts in `results.txt`

---

## 📂 File Structure

.
├── quiz.c                  // Main source code
├── results.txt             // Student results log
├── <subject>_questions.txt // Encrypted question bank per subject
├── README.md               // This file
└── quiz_application.exe    // Quiz apllication executable file

---

## ⚙️ Setup and Run

1. **Compile the program:**

   gcc main.c -o quiz

2. **Run the executable:**

   ./quiz

3. **Follow CLI prompts to enter Admin/Teacher or Student Mode**

---

## 📄 Result Format

Example entry in `results.txt`:

Date: 2025-04-16 Time: 19:32:10
Subject: Computer Networks
Roll No: 220124
Score: 8 / 10

---

## ⛓️ Encryption Method

A simple XOR cipher is used to protect quiz content. Though basic, it prevents easy tampering with question files.

---

## 🧭 Quiz Navigation Controls

| Key | Function          |
|-----|-------------------|
| A-D | Select Answer     |
| N   | Next Question     |
| P   | Previous Question |
| J   | Jump to Question  |
| S   | Skip Question     |
| Q   | Quit              |


---

## 👤 Author

Developed by **Soorya Narayanan, Jyothir Adithya, Samridhi Rauthan** 
