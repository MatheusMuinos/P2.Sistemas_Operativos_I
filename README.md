# Operating Systems I - Practice 2

This repository contains three C programs (`e1.c`, `e2.c`, `e3.c`) designed to illustrate fundamental concepts of process management, memory, and concurrency in Unix-like operating systems. Below is a description of each file, its purpose, how it works, and the expected results.

---

## e1.c
**Purpose:**
Demonstrate process creation with `fork()`, variable scope (global, local, dynamic), and competition for the standard input device.

**How it works:**
- The program creates a child process using `fork()`.
- Both parent and child print their PID and the memory addresses/values of three variables: global, local, and dynamically allocated.
- Both processes attempt to read a word from the standard input (`scanf`), showing competition for the input device.
- The child modifies the values of the variables and prints them.
- The parent waits for the child to finish, then prints the values again to show that changes in the child do not affect the parent (except for shared resources like stdin).

**Expected Results:**
- You will see two processes (parent and child) asking for input, demonstrating competition for stdin.
- The memory addresses of variables will be similar, but their values will differ after modification.
- The parent and child have independent memory spaces, but compete for the terminal input.

---

## e2.c
**Purpose:**
Show process creation, termination, orphan processes, and the use of `exec` to replace a process image.

**How it works:**
- The program creates two child processes.
- The first child prints a message and exits quickly with a specific exit code.
- The second child sleeps for 5 seconds, then replaces itself with an `echo` command using `execlp`.
- The parent waits for the first child and prints its exit code.
- The parent then terminates, potentially leaving the second child as an orphan (adopted by `init` or similar).

**Expected Results:**
- The first child prints and exits; the parent reports its exit code.
- The second child prints, sleeps, and then executes `echo`, showing how a process can be replaced.
- The parent may finish before the second child, demonstrating orphan process behavior.

---

## e3.c
**Purpose:**
Illustrate parallel computation using multiple processes and compare it to sequential execution.

**How it works:**
- The program receives two arguments: N (number of elements) and P (number of processes).
- It divides the computation of a mathematical average (using `tan(sqrt(i))`) among P child processes.
- Each child computes the average for its segment and prints the result.
- The parent waits for all children and then computes the average sequentially for comparison.

**Expected Results:**
- Each child prints its computed average for its segment.
- The parent prints the sequential average for all elements.
- You can compare parallel and sequential results and observe process concurrency.

---

## How to Compile and Run

Compile each program with:
```bash
gcc -o e1 e1.c
./e1

gcc -o e2 e2.c
./e2

gcc -o e3 e3.c
./e3 <N> <P>
```
Replace `<N>` and `<P>` with desired values for the third program.

---

## Objective
These exercises aim to help students understand:
- How processes are created and managed in Unix-like systems
- The difference between memory spaces of parent and child processes
- How processes compete for shared resources
- The behavior of orphan and replaced processes
- The benefits and challenges of parallel computation

---

## Expected Learning Outcomes
- Ability to use `fork`, `wait`, `exec`, and process control functions
- Understanding of process isolation and resource sharing
- Experience with concurrent programming and its effects
- Practical knowledge of process lifecycle and inter-process relationships