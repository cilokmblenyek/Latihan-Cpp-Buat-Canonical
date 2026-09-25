# C Syntax Review — From Basics to Data Structures

A progressive refresher: start at the top if your C is very rusty, or jump
straight to whichever section you need. All examples target C11/C17 (the
common modern C standard), matching the GCC toolchain in your DevSkiller
environment.

---

## 1. Variables & Basic Data Types

```c
int a = 5;                  // integer
double d = 3.14;             // double-precision float
float f = 3.14f;             // single-precision float (note the f suffix)
char c = 'x';                 // single character
_Bool flag = 1;               // boolean (C99+); or #include <stdbool.h> for `bool`/`true`/`false`

#include <stdbool.h>
bool flag2 = true;

// Multiple declaration
int i = 1, j = 2, k = 3;

// Fixed-width integers (useful when exact size matters — common in systems code)
#include <stdint.h>
int32_t n32 = 100;
uint64_t n64 = 100;

// C has NO built-in string type — a string is just a char array (see Section 6)
```

**Important: C has no `auto` type inference and no `std::string`, `bool` isn't
built in pre-C99, and there is no function/operator overloading — every
function name must be unique.**

---

## 2. `const` and Pointer/Const Interactions

```c
const int MAX = 100;        // value cannot change after initialization
// Note: in C (unlike C++), a `const int` is NOT a true compile-time constant —
// you generally cannot use it as an array size without VLA support:
int arr[MAX];                 // legal in C99+ as a Variable Length Array, but
                               // NOT portable/standard the way C++'s constexpr is

// const with pointers — same "read right to left" rule as C++
const int* p1 = &MAX;         // pointer to const int: *p1 = 5 is ILLEGAL
int value = 5;
int* const p2 = &value;       // const pointer to int: p2 = &other is ILLEGAL
const int* const p3 = &value; // both illegal to change

// C has NO references (no `int&`) — pointers are the only way to alias a variable
```

---

## 3. Operators

```c
// Arithmetic
int sum = 5 + 3, diff = 5 - 3, prod = 5 * 3;
int quot = 5 / 2;      // == 2 (integer division truncates)
int rem  = 5 % 2;      // == 1 (modulo)
double dq = 5.0 / 2;   // == 2.5 (float division, since one operand is double)

// Comparison
int eq = (5 == 5);     // 1 (true) — C has no bool by default, uses int 0/1
int ne = (5 != 3);     // 1

// Logical
int andRes = 1 && 0;   // 0
int orRes  = 1 || 0;   // 1
int notRes = !1;       // 0

// Increment/decrement
int x = 5;
x++;    // post-increment, x is now 6
++x;    // pre-increment,  x is now 7

// Bitwise
int a = 6 & 3;   // AND  -> 2
int b = 6 | 3;   // OR   -> 7
int c = 6 ^ 3;   // XOR  -> 5
int d = ~6;      // NOT  -> -7 (two's complement)
int e = 6 << 1;  // left shift  -> 12
int f = 6 >> 1;  // right shift -> 3

// Ternary
int max = (a > b) ? a : b;
```

---

## 4. Control Flow

```c
// if / else if / else
if (x > 10) {
    // ...
} else if (x == 10) {
    // ...
} else {
    // ...
}

// switch
switch (x) {
    case 1:
        // ...
        break;              // don't forget break, or execution falls through
    case 2:
    case 3:                 // fall-through intentionally: 2 and 3 share this case
        // ...
        break;
    default:
        // ...
}

// while
int i = 0;
while (i < 5) {
    // ...
    i++;
}

// do-while (executes body at least once)
int j = 0;
do {
    // ...
    j++;
} while (j < 5);

// for
for (int k = 0; k < 5; k++) {
    // ...
}
// Note: declaring the loop variable inside for() requires C99+;
// strict C89 requires `int k; for (k = 0; ...)` declared beforehand.

// break / continue
for (int k = 0; k < 10; k++) {
    if (k == 3) continue;   // skip this iteration
    if (k == 7) break;      // exit the loop entirely
}

// C has NO range-based for loop — you always index manually or use pointers
```

---

## 5. Functions

```c
// Basic declaration (prototype) + definition
int add(int a, int b);         // prototype — put in a header or before use

int add(int a, int b) {         // definition
    return a + b;
}

// C has NO function overloading — every function name must be unique.
// Workarounds: different names (add_int, add_double), or _Generic macros (C11+)

// C has NO default arguments — every call must supply every parameter.

// Pass by value vs pointer (C has NO references — pointers are the only option
// for a function to modify the caller's variable)
void byValue(int x)    { x = 100; }              // caller's variable unaffected
void byPointer(int* x) { *x = 100; }              // caller's variable changed

int val = 5;
byValue(val);      // val is still 5
byPointer(&val);   // val is now 100

// Passing arrays: arrays decay to pointers when passed to functions
void printArray(int arr[], int size) {   // equivalent to `int* arr`
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
}
// You must ALWAYS pass the size separately — sizeof(arr) inside the function
// gives the pointer's size (usually 8), not the original array's size.

// Function pointers — useful for callbacks, common in C-style generic code
int square(int x) { return x * x; }
int cube(int x)   { return x * x * x; }

int applyFunc(int (*func)(int), int value) {
    return func(value);
}
applyFunc(square, 5);   // 25
applyFunc(cube, 5);     // 125

// Multiple "return values" via pointers (C has no std::pair/tuple)
void minMax(int* arr, int size, int* outMin, int* outMax) {
    *outMin = arr[0];
    *outMax = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] < *outMin) *outMin = arr[i];
        if (arr[i] > *outMax) *outMax = arr[i];
    }
}
int lo, hi;
int nums[] = {3, 1, 4, 1, 5};
minMax(nums, 5, &lo, &hi);
```

---

## 6. Arrays & Strings

```c
// Fixed-size array — the only kind C has natively (no std::vector, no growable array)
int arr[5] = {1, 2, 3, 4, 5};
int size = sizeof(arr) / sizeof(arr[0]);   // classic way to get element count
                                            // ONLY works if arr hasn't decayed to a pointer

// C strings — always a char array terminated by a null byte '\0'
char cstr[] = "hello";        // 6 bytes: 'h','e','l','l','o','\0'
                               // the '\0' is added automatically for string literals

#include <string.h>
strlen(cstr);                   // 5 (excludes the null terminator)
strcpy(dest, src);               // copies src into dest — dest must be big enough!
strcat(dest, src);               // appends src onto the end of dest
strcmp(s1, s2);                  // returns 0 if equal, <0 or >0 otherwise
strncpy(dest, src, n);           // safer bounded copy (still watch null-termination)

// Building a string manually (no operator+ like C++'s std::string)
char buffer[100];
sprintf(buffer, "%s %s", "hello", "world");   // buffer now holds "hello world"

// Common C string bug: buffer overflow — always ensure destination is large enough
char small[5];
strcpy(small, "this is too long");  // UNDEFINED BEHAVIOR — writes past the buffer!
```

---

## 7. Pointers & Memory Management

```c
int value = 42;
int* ptr = &value;     // ptr holds the address of value
*ptr = 100;               // dereference: changes value to 100
printf("%p", (void*)ptr);  // prints the address itself
printf("%d", *ptr);         // prints 100

// NULL pointer
int* p = NULL;              // C's null pointer constant (from <stddef.h> or <stdio.h>)
if (p == NULL) { /* safe check before dereferencing */ }

// Dynamic memory allocation — C has NO `new`/`delete`, only these functions:
#include <stdlib.h>

int* dynInt = (int*)malloc(sizeof(int));     // allocate a single int on the heap
*dynInt = 5;
free(dynInt);                                  // must free manually, or it leaks

int* dynArr = (int*)malloc(10 * sizeof(int));  // allocate an array of 10 ints
free(dynArr);                                    // one free() call, no [] needed (unlike C++'s delete[])

int* zeroed = (int*)calloc(10, sizeof(int));   // allocates AND zero-initializes
free(zeroed);

int* resized = (int*)realloc(dynArr, 20 * sizeof(int));  // grow/shrink existing allocation
// realloc may move the memory block — always reassign the pointer, and check for NULL

// Common pointer pitfalls to avoid (VERY commonly tested)
// 1. Dangling pointer: using a pointer after its memory has been freed
// 2. Memory leak: forgetting to call free() on everything you malloc/calloc/realloc
// 3. Double free: calling free() twice on the same pointer
// 4. NULL dereference: forgetting malloc can return NULL if allocation fails
// 5. Buffer overrun: writing past the bounds of an allocated block
```

---

## 8. Structs

```c
// Struct — groups related data together (C has no classes, no member functions)
struct Point {
    int x;
    int y;
};

struct Point p1;
p1.x = 3;
p1.y = 4;

// typedef to avoid writing "struct" everywhere
typedef struct {
    int x;
    int y;
} Point;

Point p2 = {3, 4};    // now you can just write `Point`, not `struct Point`

// Struct with a pointer member (common in linked structures)
typedef struct Node {
    int data;
    struct Node* next;   // note: must say `struct Node*` here, not just `Node*`,
                          // unless you use a forward-declared typedef pattern
} Node;

// Passing structs to functions — by value copies the whole struct (can be
// expensive for large structs); pass by pointer to avoid the copy or to modify it
void movePoint(Point* p, int dx, int dy) {
    p->x += dx;   // arrow operator: shorthand for (*p).x
    p->y += dy;
}
Point pt = {0, 0};
movePoint(&pt, 5, 5);   // pt is now {5, 5}
```

---

## 9. "Generic" Code via `void*`

C has no templates. The classic workaround for writing one function that
works with multiple types is `void*` plus an explicit element size, or
function pointers for custom comparison logic:

```c
#include <stdlib.h>  // for qsort

int compareInts(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);   // returns -1, 0, or 1
}

int arr[] = {5, 2, 8, 1, 9};
qsort(arr, 5, sizeof(int), compareInts);   // sorts arr in place, works for any type
                                            // if you write the right compare function
```

---

## 10. Data Structures From Scratch

These are the classic "build it yourself" implementations worth being able
to write cold in C — commonly tested precisely because C forces you to
manage the pointers and memory manually (no STL to hide behind).

### 10.1 Singly Linked List

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
} LinkedList;

void initList(LinkedList* list) {
    list->head = NULL;
}

void pushFront(LinkedList* list, int val) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = val;
    newNode->next = list->head;
    list->head = newNode;
}

void pushBack(LinkedList* list, int val) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = val;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
        return;
    }
    Node* current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
}

int contains(const LinkedList* list, int val) {
    Node* current = list->head;
    while (current != NULL) {
        if (current->data == val) return 1;
        current = current->next;
    }
    return 0;
}

void printList(const LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

// CRITICAL in C: you must free every node yourself, there's no destructor
void freeList(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
}
```

### 10.2 Stack (Array-Based, Fixed Capacity)

```c
#define MAX_STACK_SIZE 100

typedef struct {
    int data[MAX_STACK_SIZE];
    int top;   // index of the top element; -1 means empty
} Stack;

void initStack(Stack* s) {
    s->top = -1;
}

int isEmpty(const Stack* s) {
    return s->top == -1;
}

int isFull(const Stack* s) {
    return s->top == MAX_STACK_SIZE - 1;
}

void push(Stack* s, int val) {
    if (isFull(s)) {
        printf("Stack overflow\n");
        return;
    }
    s->data[++(s->top)] = val;
}

int pop(Stack* s) {
    if (isEmpty(s)) {
        printf("Stack underflow\n");
        return -1;   // sentinel error value — a real implementation would use
                      // an out-parameter or error code instead
    }
    return s->data[(s->top)--];
}

int peek(const Stack* s) {
    return s->data[s->top];
}
```

### 10.3 Queue (Array-Based Circular Queue)

```c
#define MAX_QUEUE_SIZE 100

typedef struct {
    int data[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;
} Queue;

void initQueue(Queue* q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int isQueueEmpty(const Queue* q) {
    return q->count == 0;
}

int isQueueFull(const Queue* q) {
    return q->count == MAX_QUEUE_SIZE;
}

void enqueue(Queue* q, int val) {
    if (isQueueFull(q)) {
        printf("Queue is full\n");
        return;
    }
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;   // wrap around circularly
    q->data[q->rear] = val;
    q->count++;
}

int dequeue(Queue* q) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty\n");
        return -1;
    }
    int val = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;
    return val;
}
```

### 10.4 Binary Search Tree (BST)

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
    int value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

TreeNode* createNode(int value) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

TreeNode* insert(TreeNode* node, int value) {
    if (node == NULL) return createNode(value);
    if (value < node->value) {
        node->left = insert(node->left, value);
    } else if (value > node->value) {
        node->right = insert(node->right, value);
    }
    // if value == node->value, typically ignore duplicates
    return node;
}

int search(TreeNode* node, int value) {
    if (node == NULL) return 0;
    if (node->value == value) return 1;
    return value < node->value ? search(node->left, value) : search(node->right, value);
}

void inorderTraversal(TreeNode* node) {     // left, root, right — sorted order for a BST
    if (node == NULL) return;
    inorderTraversal(node->left);
    printf("%d ", node->value);
    inorderTraversal(node->right);
}

void preorderTraversal(TreeNode* node) {    // root, left, right
    if (node == NULL) return;
    printf("%d ", node->value);
    preorderTraversal(node->left);
    preorderTraversal(node->right);
}

void postorderTraversal(TreeNode* node) {   // left, right, root
    if (node == NULL) return;
    postorderTraversal(node->left);
    postorderTraversal(node->right);
    printf("%d ", node->value);
}

int height(TreeNode* node) {
    if (node == NULL) return 0;
    int leftHeight = height(node->left);
    int rightHeight = height(node->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

// CRITICAL in C: recursively free every node, or the whole tree leaks
void freeTree(TreeNode* node) {
    if (node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

// Usage
int main(void) {
    TreeNode* root = NULL;
    root = insert(root, 5);
    root = insert(root, 3);
    root = insert(root, 8);
    root = insert(root, 1);
    root = insert(root, 4);

    printf("Inorder (sorted): ");
    inorderTraversal(root);   // 1 3 4 5 8
    printf("\n");

    printf("Search 4: %d\n", search(root, 4));   // 1 (true)
    printf("Height: %d\n", height(root));

    freeTree(root);
    return 0;
}
```

### 10.5 Level-Order Traversal (BFS) — Using an Array-Based Queue of Pointers

```c
#define MAX_NODES 1000

void levelOrder(TreeNode* root) {
    if (root == NULL) return;

    TreeNode* queue[MAX_NODES];
    int front = 0, rear = 0;
    queue[rear++] = root;

    while (front < rear) {
        TreeNode* current = queue[front++];
        printf("%d ", current->value);

        if (current->left != NULL)  queue[rear++] = current->left;
        if (current->right != NULL) queue[rear++] = current->right;
    }
}
```

---

## Quick Reference: C vs C++ — What's Missing in C

| Feature | C++ has it | C does not |
|---|---|---|
| References (`int&`) | ✅ | ❌ (use pointers) |
| Classes / member functions | ✅ | ❌ (use structs + free functions) |
| Function overloading | ✅ | ❌ (unique names, or `_Generic` in C11) |
| Templates | ✅ | ❌ (use `void*` + function pointers, or macros) |
| `std::string` | ✅ | ❌ (use `char[]` + `<string.h>` functions) |
| STL containers (`vector`, `map`, etc.) | ✅ | ❌ (implement your own, or use fixed arrays) |
| `new`/`delete` | ✅ | ❌ (use `malloc`/`calloc`/`realloc`/`free`) |
| Exceptions (`try`/`catch`) | ✅ | ❌ (use return codes / `errno` / `setjmp`/`longjmp`) |
| `bool` built-in | ✅ | Only via `<stdbool.h>` (C99+) |
| `auto` type inference | ✅ | ❌ (must write explicit types) |

---

Practice tip: rewrite the BST and linked-list sections from memory without
looking — pay special attention to remembering **every** `malloc`/`free`
pair, since forgetting to free is the single most common mistake in
C data-structure tests.