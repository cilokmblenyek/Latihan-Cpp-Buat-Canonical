# Canonical Graduate SWE — DevSkiller C/C++ Test Prep Guide

## Your Test Structure (from what you posted)
- **Part 1:** 6 multiple-choice questions, C & C++ knowledge — 8 min
- **Part 2:** 1 C++ programming task — 30 min
  - Environment: **CMake 3.18.4, Boost 1.74.0.3ubuntu5, GCC 11.1.0-1ubuntu1~21.04,
    Google Test 1.10.0** — this is an Ubuntu 21.04-based toolchain, unsurprising
    given Canonical makes Ubuntu.

---

## 1. GitHub Repos to Re-Learn C/C++ Syntax

**Primary — modern C++ feature cheatsheet (start here):**
[`AnthonyCalandra/modern-cpp-features`](https://github.com/AnthonyCalandra/modern-cpp-features)
(21k+ stars) — Short, example-driven refresher of C++11 through C++20 features:
smart pointers, lambdas, `auto`, move semantics, `constexpr`, structured
bindings, etc. If your C++ is rusty, skim `CPP11.md` and `CPP14.md` first —
those are the features most likely to matter in a test written against GCC 11.

**Algorithm/syntax reference (compare your solutions against these):**
[`TheAlgorithms/C-Plus-Plus`](https://github.com/TheAlgorithms/C-Plus-Plus)
(34k+ stars) and its sibling [`TheAlgorithms/C`](https://github.com/TheAlgorithms/C)
— clean, tested implementations of sorting, searching, strings, and data
structures in both languages. Use these the same way as before: attempt a
problem yourself first, then compare.

**Bonus — if broader Linux/systems questions come up later in the process:**
[`angrave/SystemProgramming`](https://github.com/angrave/SystemProgramming)
(via its GitHub wiki) — a widely used systems programming reference covering C,
pointers, memory, and Linux internals. Not essential for this specific
DevSkiller test, but useful given Canonical's later technical rounds reportedly
cover Linux system skills too.

---

## 2. What Canonical's DevSkiller C/C++ Test Actually Looks Like

**From DevSkiller's own C/C++ test documentation:** their tests follow the same
**"RealLifeTesting"** methodology as the Python test — realistic, self-contained
programming tasks rather than abstract puzzles. Their own published example for
a "medium" C++ task is implementing **run-length encoding** for an ASCII string —
i.e., a contained algorithm/string-processing function with a clear input →
output spec, not a huge system to build.

**From a candidate who took Canonical's DevSkiller test (Glassdoor, verified
report):**
> "The coding questions were huge and difficult, they claimed it could be
> finished in 30 minutes each but it took 45 mins for me to finish the first one!"
> ... "The problems were your standard medium difficulty leetcode problems."

So: expect a **medium-difficulty algorithmic/data-structure task**, generous on
paper but tight in practice — budget your 30 minutes carefully (see Tips below).

**Given the environment specifics (CMake + Boost + GoogleTest):**
- The project is almost certainly scaffolded for you — a CMake project with
  header/source files and **GoogleTest test cases already written**, similar to
  the Python test's "some tests prepopulated, not exhaustive" model. Read the
  given `.cpp` test file first — it **is** your spec.
- Boost being listed doesn't necessarily mean you must use advanced Boost
  features — it may just be available/linked in case the scaffold or your
  solution needs it (e.g., `boost::lexical_cast`, `boost::optional`). Don't
  panic if you don't know Boost deeply; standard C++/STL is usually enough
  unless the task explicitly calls for a Boost utility.
- As with the Python test, **hidden/additional tests exist beyond what you can
  see** — passing the visible GTest cases doesn't guarantee full marks. Think
  about edge cases explicitly (empty input, single element, max values,
  negative numbers) even if no visible test covers them.

---

## 3. Tutorial: C/C++ Gotchas Likely to Show Up in the MCQ Section

With only 6 MCQs in 8 minutes, expect core language-difference and
correctness questions rather than obscure trivia:

```cpp
// 1. C vs C++ struct differences
// In C, you must write `struct Point p;` — in C++, `Point p;` works directly.

// 2. malloc/free vs new/delete
int* a = (int*)malloc(sizeof(int));  // C-style, no constructor call
free(a);
int* b = new int(5);                  // C++-style, calls constructor if applicable
delete b;
// Mixing them (new + free, or malloc + delete) is undefined behavior.

// 3. Pass by value vs reference vs pointer
void byValue(int x) { x = 10; }        // caller's variable unchanged
void byRef(int& x)  { x = 10; }        // caller's variable changed
void byPtr(int* x)  { *x = 10; }       // caller's variable changed, via dereference

// 4. const correctness
void print(const std::string& s);     // won't modify s, avoids a copy
const int* p1;   // pointer to const int — can't modify *p1
int* const p2 = &x; // const pointer to int — can't reassign p2 itself

// 5. Array decay to pointer
void f(int arr[10]) { /* arr is actually int*, sizeof(arr) here is 8, not 40 */ }

// 6. Uninitialized variables — undefined behavior, not always zero
int x;
std::cout << x; // garbage value, not guaranteed 0 in C++ (unlike some assumptions)

// 7. Integer overflow and implicit conversions
unsigned int u = 0;
u = u - 1;              // wraps to UINT_MAX, not -1 (no signed/unsigned warning by default)
int i = 3 / 2;           // integer division = 1, not 1.5
double d = 3 / 2;        // still 1.0! division happens as int first, then converts

// 8. String handling: C-style vs std::string
char cstr[] = "hello";           // mutable, fixed buffer, needs manual null-termination care
std::string s = "hello";         // manages its own memory, has .length(), .substr(), etc.

// 9. Smart pointers (likely relevant given GCC 11 / modern C++ context)
std::unique_ptr<int> up = std::make_unique<int>(5); // exclusive ownership, no manual delete
std::shared_ptr<int> sp = std::make_shared<int>(5); // reference-counted ownership

// 10. Range-based for and const-ness
std::vector<int> v = {1, 2, 3};
for (int& x : v) x *= 2;         // modifies v's elements
for (const int& x : v) { /* read-only, no copy */ }
for (int x : v) { /* read-only, copies each element */ }

// 11. Header guards / extern "C"
extern "C" {
    // tells the C++ compiler to use C linkage — relevant if mixing C and C++ code
}

// 12. Operator overloading pitfall
// Overloading == doesn't automatically give you !=; you must define both
// (pre-C++20; C++20 can auto-generate != from == via `= default`/rewritten candidates)
```

---

## 4. Practice Problems in This Test's Style

Try these using the **exact environment**: write a CMake project, put your
function under test, write/extend GoogleTest cases, and build with
`cmake -S . -B build && cmake --build build && ctest`.

1. **Run-length encoding** (DevSkiller's own published example) — encode
   `"aaabbc"` → `"3a2b1c"`, and write the matching decode function.
2. **Reverse a string in place** — using raw `char[]`, then again using
   `std::string`, to practice both C-style and C++-style approaches.
3. **Check balanced parentheses** — `"(a(b)c)"` → true, `"(a(b)c"` → false,
   using a `std::stack`.
4. **Find the first non-repeating character** in a string, using
   `std::unordered_map<char, int>` for counting.
5. **Merge two sorted vectors** into one sorted vector without using
   `std::sort` afterward (classic two-pointer merge).
6. **Implement a simple `Stack` class template** (`template <typename T>`)
   with `push`, `pop`, `top`, `empty`, using `std::vector<T>` internally.
7. **Word frequency counter** — split a sentence into words and count
   occurrences using `std::map<std::string, int>`.

**Worked example (shows expected style — clean, uses STL, self-tested):**

```cpp
#include <string>
#include <cctype>
#include <cassert>

std::string runLengthEncode(const std::string& input) {
    std::string result;
    size_t n = input.size();
    for (size_t i = 0; i < n; ) {
        char current = input[i];
        size_t count = 0;
        while (i < n && input[i] == current) {
            ++count;
            ++i;
        }
        result += std::to_string(count) + current;
    }
    return result;
}

int main() {
    assert(runLengthEncode("aaabbc") == "3a2b1c");
    assert(runLengthEncode("") == "");
    assert(runLengthEncode("a") == "1a");
    return 0;
}
```

---

## 5. Tips & Tricks — Including Real Reported Gotchas

- **Use your own local editor if the test offers that option.** A candidate
  who took Canonical's DevSkiller test twice reported: *"DON'T use the
  in-browser code editors as they take about 15 seconds to run your code and
  I've had the website throw an error and refuse to launch... You're much
  better off using your own code editor and the git submission system."*
  Check your test's welcome page for a local-dev-environment option (the
  Python version of this test explicitly offered one) — if available, set up
  the **exact versions** listed (CMake 3.18.4, GCC 11.1.0, Boost 1.74.0,
  GTest 1.10.0) beforehand so you're not fighting environment issues during
  the timed test.
- **Quick Ubuntu setup to mirror their environment**, if you have access to an
  Ubuntu 21.04-ish box or a Docker container:
  ```bash
  sudo apt-get update
  sudo apt-get install -y cmake g++ libboost-all-dev libgtest-dev build-essential
  ```
  (Exact versions may differ slightly depending on your distro — the point is
  to rehearse the CMake + GTest build/run loop, not match every patch version.)
- **The practice problem may count against your time.** The same candidate
  reported a platform bug where the practice/warm-up problem's time was
  deducted from the total, even though it's meant to just verify your setup.
  Move through it quickly and don't treat it as "free" time.
- **Read the given GTest file before writing any code.** It's your spec.
  Note the exact function signature, parameter types, and return type expected
  — a mismatched signature won't compile against their tests at all.
- **Compile clean.** Canonical is a systems/infrastructure company; don't
  ignore compiler warnings if you can help it — `-Wall` habits (no unused
  variables, no implicit narrowing conversions) reflect well on submitted code
  even if not explicitly graded.
- **Time budget for the 30-minute task:** ~5 min reading the spec/given tests,
  ~20 min implementing and running tests locally, ~5 min buffer for edge cases
  (empty input, single element, boundary values) and a final clean build.
- **For the 6 MCQs (8 min):** these are meant to be fast. If one stumps you,
  flag your best guess and move on rather than burning your 30-minute coding
  budget on an 80-second question.
- **Don't mix C and C++ idioms carelessly** in your submission (e.g., `malloc`
  with `delete`, or C-style casts instead of `static_cast`) — using idiomatic,
  consistent C++ is a subtle but real signal of comfort with the language.

Good luck — you've now got the Python guide and this one both in hand if the process asks for more than one language.