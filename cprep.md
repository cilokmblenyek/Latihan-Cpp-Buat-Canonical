# DevSkiller Code Review Section — C — Prep Guide

## What This Task Type Actually Is (confirmed from DevSkiller's own platform docs)

This is fundamentally different from your other two parts. Key facts,
straight from DevSkiller/SkillPanel's own guidebook:

- You'll be shown an existing piece of C code (uploaded by Canonical as part
  of the test setup) and asked to **write review comments** on it — exactly
  like reviewing a GitHub/GitLab pull request.
- **It is NOT auto-graded by compiling code or running a test suite.**
  DevSkiller's own docs state: *"Code review tasks require manual evaluation
  because the answer is subjective... it cannot be said if it is right or
  wrong by the machine that automatically marks it."*
- Practically: the recruiter/reviewer set up **reference comments** (an
  answer key of the issues they expect a strong candidate to catch), and a
  human compares your submitted comments against that reference — checking
  whether you spotted the same issues, how you explained them, and how you
  communicated them.
- This means you're being evaluated on **two separate skills at once**:
  (1) can you actually spot real bugs/issues in C code, and (2) can you
  communicate them clearly and professionally, the way a senior engineer
  reviewing a colleague's PR would.

**What this means for prep:** memorizing algorithms won't help here. What
helps is (a) having a mental checklist of common C bug categories so you
don't miss things under time pressure, and (b) practicing *writing clear,
specific, well-structured review comments*, not just silently spotting bugs.

---

## The C-Specific Checklist — What to Scan For

Given this is a C test, expect the sample code to have some combination of
these planted issues. Go through code with this checklist actively in mind
rather than just reading passively:

### Memory Management (very likely to appear — C's classic weak spot)
- [ ] Is every `malloc`/`calloc`/`realloc` matched with a `free`?
- [ ] Is the return value of `malloc`/`calloc` checked for `NULL` before use?
- [ ] Any **use-after-free** (pointer used after it was freed)?
- [ ] Any **double-free** (freeing the same pointer twice)?
- [ ] Any **memory leak** in an early-return path (function returns before
      reaching its `free()` call)?
- [ ] `realloc` result reassigned correctly? (If `realloc` fails, the
      original pointer is lost if you overwrite it directly: `p = realloc(p, n)`
      loses `p` on failure — should assign to a temp variable first.)

### Buffer & Bounds Issues
- [ ] Any use of unsafe functions: `gets()` (never safe), `strcpy`/`strcat`
      without checking destination size, `sprintf` without a size limit
      (`snprintf` is the safer alternative)?
- [ ] Array indexing — any off-by-one errors (`<=` vs `<` in a loop bound)?
- [ ] Does a loop or copy ever write past the end of a fixed-size buffer?
- [ ] `scanf("%s", buf)` without a width specifier (unbounded read into buf)?

### Pointer Safety
- [ ] Any pointer dereferenced without a prior `NULL` check?
- [ ] Any dangling pointer (pointing to a freed or out-of-scope stack variable)?
- [ ] Returning the address of a local (stack) variable from a function?

### Correctness / Logic
- [ ] Integer overflow potential (e.g., multiplying sizes before `malloc`
      without checking for overflow)?
- [ ] Signed/unsigned comparison bugs (comparing a signed int that could be
      negative against an unsigned value/size)?
- [ ] Division without a zero-check on the divisor?
- [ ] Missing `break` in a `switch` causing unintended fallthrough?
- [ ] Off-by-one in loop bounds (`for (i = 0; i <= n; i++)` on an array of
      size `n`)?

### Error Handling
- [ ] Are return values of `fopen`, `malloc`, or other fallible calls checked?
- [ ] Is a file handle (`fopen`) ever left un-closed (`fclose` missing)?
- [ ] Are error paths handled gracefully, or does the code just continue as
      if nothing went wrong?

### Style, Readability, Maintainability
- [ ] Magic numbers that should be named constants/`#define`s
- [ ] Inconsistent naming conventions or unclear variable names (`x`, `tmp`,
      `data1`, `data2` with no explanation)
- [ ] Functions doing too many unrelated things (violates single responsibility)
- [ ] Missing comments on genuinely non-obvious logic (not over-commenting
      obvious lines)
- [ ] Dead code / unreachable code / unused variables
- [ ] Missing `const` where a parameter is never modified

### Security (given Canonical's Ubuntu/infrastructure focus, this is plausible)
- [ ] Format string vulnerabilities: `printf(userInput)` instead of
      `printf("%s", userInput)` — if `userInput` contains `%s` or `%n`, this
      is a real vulnerability
- [ ] Trusting unchecked user input length before copying into a fixed buffer

---

## How to Write a Good Review Comment

A weak comment just names the bug. A strong comment:
1. **Points to the specific location** (line number / function name / variable)
2. **States what's wrong**, concretely
3. **Explains why it matters** (what could go wrong — crash, security issue,
   leak, undefined behavior)
4. **Suggests a fix or direction**, without necessarily writing the whole fix

**Weak example:**
> "This has a memory leak."

**Strong example:**
> "Line 42: `buffer` is allocated with `malloc` but the function returns
> early on line 47 if `count == 0`, without freeing `buffer` first. This
> leaks memory every time this function is called with `count == 0`.
> Suggest freeing `buffer` before the early return, or restructuring with a
> single cleanup path at the end of the function."

**Tone matters too** — this is implicitly testing whether you'd be a good
collaborator. Keep it constructive and professional, the way you'd want a
senior engineer to review *your* code, not dismissive or harsh.

---

## Practice Exercise

Here's a short C function with several planted issues, similar in spirit to
what a code review task might show you. Try writing your own review comments
before reading the answer key below.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* buildGreeting(char* name) {
    char buffer[20];
    strcpy(buffer, "Hello, ");
    strcat(buffer, name);

    char* result = malloc(strlen(buffer));
    strcpy(result, buffer);

    return result;
}

void printUserMessage(char* input) {
    printf(input);
}

int processData(int* data, int size) {
    int sum = 0;
    for (int i = 0; i <= size; i++) {
        sum += data[i];
    }
    return sum;
}
```

<details>
<summary>Click to see a model review (try writing your own first)</summary>

**`buildGreeting`:**
- Line 6-7 (`buffer[20]` + `strcpy`/`strcat`): `buffer` is only 20 bytes. If
  `name` is longer than about 12 characters, `strcat` will write past the
  end of `buffer` — a stack buffer overflow. This is undefined behavior and
  a potential security issue. Consider using `snprintf` with a bounded size
  instead, or dynamically sizing the buffer based on `strlen(name)`.
- Line 9 (`malloc(strlen(buffer))`): this allocates exactly `strlen(buffer)`
  bytes, but `strcpy` on line 10 will also copy the null terminator, which
  needs one extra byte. This is an off-by-one buffer overflow. Should be
  `malloc(strlen(buffer) + 1)`.
- Line 9: the return value of `malloc` is never checked for `NULL` before
  being used on line 10.
- The caller of `buildGreeting` is responsible for calling `free()` on the
  returned pointer, but nothing here documents that — worth a comment, since
  it's easy for a caller to leak this.

**`printUserMessage`:**
- Line 15 (`printf(input)`): this passes `input` directly as the format
  string. If `input` contains `%` specifiers (e.g., from user input), this
  is a classic **format string vulnerability** — it can crash the program or
  leak stack memory. Should be `printf("%s", input);`.

**`processData`:**
- Line 20 (`for (int i = 0; i <= size; i++)`): this is an off-by-one error.
  For an array of `size` elements (valid indices `0` to `size-1`), using
  `<=` reads one element past the end of `data` — undefined behavior. Should
  be `i < size`.
- No check that `data` is non-`NULL` or that `size` is non-negative before
  use.

</details>

---

## Time Management for 45 Minutes

- **First pass (~10 min):** read the whole file(s) once for overall
  structure and purpose — don't write comments yet, just understand what the
  code is supposed to do.
- **Second pass (~25 min):** go function by function (or line by line) with
  the checklist above actively in mind. Write comments as you find issues —
  don't try to hold everything in memory for later.
- **Final pass (~10 min):** review your own comments — are they specific
  enough? Did you explain *why* each issue matters, not just *what* it is?
  Prioritize: make sure you haven't buried a serious bug (buffer overflow,
  use-after-free) under a pile of minor style nitpicks — lead with
  correctness/security issues, then style/readability.

## One More Thing Worth Checking

Since this task is manually graded against reference comments, it's worth
re-reading your test's own instructions (or the welcome page, if there's a
separate one for this section) for **exactly how you're expected to submit
comments** — inline in a code editor with a comment feature, in a separate
text box, referencing line numbers manually, etc. The mechanics of *how* you
submit matter here since there's no compiler to save you if your comment
doesn't clearly point to the right place.
