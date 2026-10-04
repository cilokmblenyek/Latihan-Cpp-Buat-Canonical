# C Code Review Checklist — Full Examples & Explanations

Each item below shows: a **buggy snippet** (the kind of thing a code-review
task might plant), **why it's a problem**, and **the fix**. Read through
these once, then try to spot them cold in new code without re-reading the
explanations.

---

## Memory Management

### 1. Every `malloc`/`calloc`/`realloc` matched with a `free`?

```c
void process() {
    char *buf = malloc(100);
    strcpy(buf, "hello");
    printf("%s\n", buf);
    // missing free(buf) — leaked every time this function runs
}
```
**Why it matters:** every call to `process()` leaks 100 bytes. In a
long-running program (a server, a daemon), this accumulates until the
process runs out of memory.
**Fix:** add `free(buf);` before the function returns.

---

### 2. Is the return value of `malloc`/`calloc` checked for `NULL`?

```c
int *arr = malloc(n * sizeof(int));
arr[0] = 5;   // crashes (NULL pointer dereference) if malloc returned NULL
```
**Why it matters:** `malloc` returns `NULL` if allocation fails (e.g., out
of memory). Using the pointer without checking causes undefined behavior —
typically a crash, but not guaranteed to fail safely.
**Fix:**
```c
int *arr = malloc(n * sizeof(int));
if (arr == NULL) {
    // handle the error — return, log, abort gracefully
    return -1;
}
arr[0] = 5;
```

---

### 3. Any use-after-free?

```c
char *p = malloc(10);
strcpy(p, "hello");
free(p);
printf("%s\n", p);   // use-after-free: p's memory may already be reused
```
**Why it matters:** once freed, that memory can be reallocated to something
else at any time. Reading or writing through `p` after `free(p)` is
undefined behavior — it might print garbage, might crash, might silently
corrupt unrelated data.
**Fix:** don't use `p` after `free(p)`. As a defensive habit, set
`p = NULL;` right after freeing, so any accidental later use crashes
immediately and loudly (NULL deref) instead of silently corrupting memory.

---

### 4. Any double-free?

```c
free(p);
// ... some code later, possibly in a different branch ...
free(p);   // double-free — undefined behavior, can corrupt the heap
```
**Why it matters:** freeing the same pointer twice corrupts the heap's
internal bookkeeping, which can cause crashes or, in the worst case,
exploitable memory corruption.
**Fix:** set `p = NULL;` after the first `free(p);`. Calling `free(NULL)`
is explicitly defined by the C standard as a safe no-op, so this makes a
second accidental `free(p)` harmless.

---

### 5. Any memory leak in an early-return path?

```c
char *process(int flag) {
    char *buf = malloc(50);
    if (flag) {
        return NULL;      // buf is leaked here — never freed on this path
    }
    free(buf);
    return buf;
}
```
**Why it matters:** the early `return` skips the `free(buf)` entirely on
that code path. Every call with `flag` truthy leaks 50 bytes.
**Fix:** free before every return point, or restructure with a single
cleanup path:
```c
char *process(int flag) {
    char *buf = malloc(50);
    if (flag) {
        free(buf);
        return NULL;
    }
    return buf;
}
```

---

### 6. Is `realloc`'s result reassigned correctly?

```c
p = realloc(p, newSize);   // if realloc fails, it returns NULL —
                            // and the ORIGINAL block p pointed to is now
                            // lost (leaked), because p was just overwritten
```
**Why it matters:** `realloc` returns `NULL` on failure *without freeing
the original block*. If you directly overwrite `p` with the result, a
failed `realloc` both loses your only reference to the original memory
(leak) and leaves `p` as `NULL` (anything using `p` afterward crashes).
**Fix:**
```c
char *temp = realloc(p, newSize);
if (temp == NULL) {
    // realloc failed — p is still valid and usable here
    // handle the error, p is NOT leaked
} else {
    p = temp;
}
```

---

## Buffer & Bounds Issues

### 7. Use of unsafe functions (`gets`, unchecked `strcpy`/`strcat`/`sprintf`)?

```c
char buf[10];
gets(buf);                    // NEVER safe — no bounds, period
strcpy(buf, userInput);        // no check that userInput fits in buf
sprintf(buf, "%s", longString); // no size limit on the write
```
**Why it matters:** all three can write past the end of `buf` if the
source is longer than the destination — a classic stack buffer overflow,
potentially exploitable for arbitrary code execution. `gets()` was so
inherently unsafe it was formally removed from the C11 standard.
**Fix:**
```c
fgets(buf, sizeof(buf), stdin);           // bounded read
strncpy(buf, userInput, sizeof(buf) - 1);  // bounded copy
buf[sizeof(buf) - 1] = '\0';                // strncpy doesn't guarantee null-termination
snprintf(buf, sizeof(buf), "%s", longString); // bounded, always null-terminates
```

---

### 8. Off-by-one errors in loop bounds?

```c
int arr[5];
for (int i = 0; i <= 5; i++) {
    arr[i] = i;   // valid indices are 0-4; i==5 writes one past the array
}
```
**Why it matters:** `<=` instead of `<` writes/reads one element past the
array's valid range — undefined behavior, often silently corrupts adjacent
memory rather than crashing immediately, which makes it hard to debug later.
**Fix:** `for (int i = 0; i < 5; i++)`.

---

### 9. Does a loop or copy write past the end of a fixed-size buffer?

```c
char dest[5];
int len = strlen(src);
for (int i = 0; i <= len; i++) {   // also off-by-one AND no bounds check vs dest
    dest[i] = src[i];
}
```
**Why it matters:** this copies `strlen(src) + 1` bytes into `dest`
regardless of `dest`'s actual size — if `src` is longer than 4 characters,
this overflows `dest`.
**Fix:** bound the loop by `dest`'s capacity, not just `src`'s length:
```c
for (int i = 0; i < sizeof(dest) - 1 && i < len; i++) {
    dest[i] = src[i];
}
dest[i] = '\0';
```

---

### 10. `scanf("%s", buf)` without a width specifier?

```c
char buf[10];
scanf("%s", buf);   // unbounded — if input is longer than 9 characters,
                     // this overflows buf
```
**Why it matters:** `%s` with `scanf` reads until whitespace with no
length limit — classic overflow vector if the input is attacker-controlled
or just longer than expected.
**Fix:** `scanf("%9s", buf);` (width = buffer size minus 1, to leave room
for the null terminator).

---

## Pointer Safety

### 11. Any pointer dereferenced without a prior `NULL` check?

```c
int *p = find_value(arr, n, target);   // may return NULL if not found
printf("%d\n", *p);                     // crashes if p is NULL
```
**Why it matters:** functions that can fail or "not find" something
commonly signal that with `NULL`. Dereferencing without checking is an
immediate crash risk whenever the "not found" case actually happens.
**Fix:**
```c
if (p != NULL) {
    printf("%d\n", *p);
} else {
    printf("not found\n");
}
```

---

### 12. Any dangling pointer (pointing to freed or out-of-scope stack memory)?

```c
int* getPointer() {
    int local = 5;
    return &local;   // local's storage is destroyed when the function returns
}
// caller:
int *p = getPointer();
printf("%d\n", *p);    // undefined behavior — p points to a dead stack frame
```
**Why it matters:** `local` is a stack variable — its memory is reclaimed
the instant the function returns. The returned address is immediately
invalid, even though it might *look* like it still works by accident
(stack memory not yet overwritten).
**Fix:** allocate on the heap and transfer ownership, or have the caller
pass in a buffer:
```c
int* getPointer() {
    int* p = malloc(sizeof(int));
    *p = 5;
    return p;   // caller is now responsible for free()ing it
}
```

---

### 13. Returning the address of a local (stack) variable from a function?

```c
char* getMessage() {
    char msg[20] = "hello";
    return msg;   // same bug as above, but with an array — msg decays to a
                   // pointer to stack memory that's gone once the function returns
}
```
**Why it matters:** identical root cause to #12, just with a buffer
instead of a single variable — very common in string-building helper
functions specifically.
**Fix options:** use a `static` buffer (not thread-safe, has other
trade-offs), have the caller provide the buffer, or `malloc` and document
that the caller owns the returned memory.

---

## Correctness / Logic

### 14. Integer overflow potential?

```c
int n = 100000;
int *arr = malloc(n * n * sizeof(int));
// n * n = 10,000,000,000 — overflows a 32-bit int, wraps to some smaller
// (or negative) number. malloc then succeeds with a MUCH smaller buffer
// than the code thinks it has, and later writes overflow it.
```
**Why it matters:** the overflow happens silently — no crash, no warning
at the `malloc` call itself. The bug only shows up later as out-of-bounds
writes, making it hard to trace back to the real cause.
**Fix:** use `size_t` for sizes, and check for overflow before
multiplying, or compute the size in a way the compiler/sanitizers can
catch:
```c
size_t total;
if (__builtin_mul_overflow(n, n, &total) || __builtin_mul_overflow(total, sizeof(int), &total)) {
    // handle overflow error
}
int *arr = malloc(total);
```

---

### 15. Signed/unsigned comparison bugs?

```c
size_t count = 0;
for (size_t i = count - 1; i >= 0; i--) {
    // count - 1 when count == 0 UNDERFLOWS (size_t is unsigned) to SIZE_MAX
    // i >= 0 is ALWAYS true for an unsigned type — this is an infinite loop
    // that will eventually crash when i indexes way out of bounds
    process(arr[i]);
}
```
**Why it matters:** mixing signed and unsigned types in comparisons is a
very common, very subtle bug class in C. Unsigned integers can't go
negative — they wrap around instead — so a condition like `i >= 0` on an
unsigned `i` is either always true (infinite loop) or masks an underflow
that already happened.
**Fix:** use a signed loop counter when counting down, or guard against
the zero case explicitly:
```c
for (int i = (int)count - 1; i >= 0; i--) { process(arr[i]); }
// or:
if (count > 0) {
    for (size_t i = count; i-- > 0; ) { process(arr[i]); }  // safe unsigned pattern
}
```

---

### 16. Division without a zero-check on the divisor?

```c
int average(int sum, int count) {
    return sum / count;   // undefined behavior if count == 0
}
```
**Why it matters:** integer division by zero is undefined behavior in C —
on most platforms it causes a crash (SIGFPE), but it's not guaranteed to
fail safely or consistently.
**Fix:**
```c
int average(int sum, int count) {
    if (count == 0) {
        // handle appropriately — return an error code, 0, or similar per spec
        return 0;
    }
    return sum / count;
}
```

---

### 17. Missing `break` in a `switch` causing unintended fallthrough?

```c
switch (level) {
    case 1:
        printf("Low\n");
        // missing break — falls through into case 2 unintentionally
    case 2:
        printf("Medium\n");
        break;
    case 3:
        printf("High\n");
        break;
}
// calling with level == 1 prints BOTH "Low" and "Medium" — almost
// certainly not what was intended
```
**Why it matters:** C's `switch` falls through by default unless you
explicitly `break`. Forgetting one `break` silently executes extra cases —
a very common, easy-to-miss bug, especially in longer `switch` blocks.
**Fix:** add `break;` after each case's logic (or, if fallthrough is
genuinely intentional, add a comment like `// fallthrough` so a reviewer
knows it's deliberate, not a mistake).

---

### 18. Off-by-one in loop bounds (array initialization variant)?

```c
int arr[10];
for (int i = 1; i <= 10; i++) {
    arr[i] = 0;   // starts at index 1 (skips arr[0], leaving it
                   // uninitialized) AND writes arr[10], one past the end
}
```
**Why it matters:** this is a double bug — it both skips a valid index
*and* overflows the array. `arr[0]` is left with garbage/uninitialized
data, and `arr[10]` is an out-of-bounds write.
**Fix:** `for (int i = 0; i < 10; i++) { arr[i] = 0; }`.

---

## Error Handling

### 19. Are return values of `fopen`, `malloc`, etc. checked?

```c
FILE *f = fopen("data.txt", "r");
fscanf(f, "%d", &value);   // crashes if the file doesn't exist (f == NULL)
```
**Why it matters:** `fopen` returns `NULL` if the file can't be opened
(doesn't exist, no permission, etc.) — a very common real-world condition,
not an edge case. Not checking it turns a routine, recoverable situation
into a crash.
**Fix:**
```c
FILE *f = fopen("data.txt", "r");
if (f == NULL) {
    perror("Failed to open data.txt");
    return -1;
}
fscanf(f, "%d", &value);
```

---

### 20. Is a file handle ever left un-closed?

```c
void readConfig() {
    FILE *f = fopen("config.txt", "r");
    // ... read data ...
    // missing fclose(f) — the file descriptor is never released
}
```
**Why it matters:** every process has a limited number of file descriptors
it can have open at once. A long-running program that calls this
repeatedly without closing the file will eventually exhaust that limit and
start failing to open *any* files.
**Fix:** `fclose(f);` before the function returns (and on every early-exit
path, same principle as the memory-leak-on-early-return case above).

---

### 21. Are error paths handled gracefully, or does the code just continue?

```c
char *buf = malloc(size);
strcpy(buf, "data");   // proceeds even if malloc returned NULL — this is
                        // actually a NULL-dereference crash disguised as
                        // "continuing as if nothing happened"
```
**Why it matters:** this combines #2 (unchecked malloc) with a broader
pattern worth calling out separately in review: code that has *no error
handling branch at all* — it just assumes every call succeeds and marches
forward. In review, flag this as a pattern, not just a one-off missing
check, since it usually means several other calls nearby have the same gap.
**Fix:** add an explicit check-and-handle branch for every fallible call,
even if "handling" just means returning an error code up the call stack.

---

## Style, Readability, Maintainability

### 22. Magic numbers that should be named constants?

```c
if (status == 3) {
    archiveRecord();
}
```
**Why it matters:** `3` means nothing to a reader without digging through
the rest of the codebase to find out what status code 3 represents. This
makes the code harder to maintain and easy to get wrong when someone adds
a new status value later.
**Fix:**
```c
#define STATUS_COMPLETE 3
if (status == STATUS_COMPLETE) {
    archiveRecord();
}
```

---

### 23. Inconsistent naming conventions or unclear variable names?

```c
int x = calculateTotal();
int tmp = x * 2;
int data1 = tmp - 5;
```
**Why it matters:** `x`, `tmp`, and `data1` carry no information about
what they represent. Six months later (or to a reviewer today), it's
unclear what `data1` actually *is* without tracing back through every line.
**Fix:**
```c
int orderTotal = calculateTotal();
int doubledTotal = orderTotal * 2;
int adjustedTotal = doubledTotal - DISCOUNT_AMOUNT;
```

---

### 24. Functions doing too many unrelated things?

```c
void processOrder(Order *o) {
    // validates input
    // calculates total
    // writes to database
    // sends confirmation email
    // logs the transaction
}
```
**Why it matters:** a function with five unrelated responsibilities is
hard to test (you can't test "calculate total" without also triggering a
database write and an email), hard to reuse, and hard to reason about when
something goes wrong — which of the five things failed?
**Fix:** split into focused functions: `validateOrder()`,
`calculateTotal()`, `saveOrderToDatabase()`, `sendConfirmationEmail()`,
`logTransaction()`, each independently testable, called in sequence from a
thin orchestrating function.

---

### 25. Missing comments on genuinely non-obvious logic?

```c
int result = (a ^ b) & ~(a & b) | (a & b & c);
```
**Why it matters:** bit manipulation like this is opaque without context —
a reviewer (or future maintainer) has no way to know *what problem* this
is solving, so they can't verify it's even correct, let alone safely
modify it later.
**Fix:**
```c
// Computes the majority-vote bit pattern: for each bit position, result
// bit is 1 if at least two of a, b, c have a 1 in that position.
int result = (a ^ b) & ~(a & b) | (a & b & c);
```
(Note: the *lack* of a comment is the issue here, not the logic itself —
don't over-comment obvious lines like `i++; // increment i`, but DO
comment anything a reader can't quickly re-derive on their own.)

---

### 26. Dead code, unreachable code, or unused variables?

```c
int unused = 5;          // never referenced anywhere else

if (0) {
    doSomething();        // unreachable — condition is always false
}

return result;
cleanup();                 // unreachable — this line is after a return
```
**Why it matters:** dead code adds noise, confuses readers about what
actually executes, and can hide the fact that `cleanup()` was *supposed*
to run but a `return` was added above it later without anyone noticing it
now never executes.
**Fix:** remove unused variables and unreachable code; if `cleanup()` was
meant to run before returning, move it above the `return` statement.

---

### 27. Missing `const` where a parameter is never modified?

```c
void printArray(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);   // arr is only read, never written
    }
}
```
**Why it matters:** without `const`, nothing in the function's signature
tells a caller (or a reviewer) that `arr` is guaranteed not to be
modified. `const`-correctness is a form of documentation enforced by the
compiler — it also lets the compiler catch accidental modifications.
**Fix:** `void printArray(const int *arr, int size)`.

---

## Security

### 28. Format string vulnerabilities?

```c
void logMessage(char *userInput) {
    printf(userInput);   // userInput is used AS the format string itself
}
```
**Why it matters:** if `userInput` ever contains `%s`, `%x`, or especially
`%n`, this is a real, exploitable vulnerability — `%n` can be used to
**write** to arbitrary memory addresses via `printf`'s format-string
mechanism. This is a classic, well-documented C vulnerability class
(CWE-134), not a theoretical concern.
**Fix:** never pass user-controlled data as a format string — always use
a fixed format string with the data as an argument:
```c
printf("%s", userInput);
```

---

### 29. Trusting unchecked user input length before copying into a fixed buffer?

```c
char buffer[50];
printf("Enter your name: ");
gets(buffer);   // or: scanf("%s", buffer); — no length enforcement at all
```
**Why it matters:** this is the root cause behind many of the buffer
issues above, called out separately because it's worth recognizing as a
*pattern*: any time user-supplied data flows into a fixed-size buffer
without an explicit length check or bound, that's a potential overflow —
whether it's `gets`, unbounded `scanf("%s", ...)`, `strcpy`, or manual
copy loops.
**Fix:** always bound reads/copies to the destination buffer's actual
size: `fgets(buffer, sizeof(buffer), stdin);` or
`scanf("%49s", buffer);` (width = size - 1).

---

## How to Use This While Practicing

1. Take any C file (your own old project code works well) and go through
   it item by item from this checklist — even if you don't find a real
   issue, the practice of *actively scanning* for each category builds
   the pattern-recognition speed you'll need under time pressure.
2. Then deliberately introduce 3-4 of these bugs into a working file and
   have someone else (or yourself, after a break) try to find them — this
   simulates the actual review task much more closely than just reading.
3. For the real test, work through categories in roughly this order of
   priority: **Memory Management → Pointer Safety → Buffer/Bounds →
   Security → Correctness/Logic → Error Handling → Style**, since the
   first four are the categories most likely to represent genuinely
   serious, "this would crash or be exploitable in production" issues —
   which should anchor your review before you spend time on style nits.
