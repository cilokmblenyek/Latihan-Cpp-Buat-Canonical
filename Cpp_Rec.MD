# C++ Syntax Review — From Basics to Data Structures

A progressive refresher: start at the top if your C++ is very rusty, or jump
straight to whichever section you need. All examples target C++17 (matches
GCC 11, the compiler version in your DevSkiller environment).

---

## 1. Variables & Basic Data Types

```cpp
int a = 5;                 // integer
double d = 3.14;           // double-precision float
float f = 3.14f;           // single-precision float (note the f suffix)
char c = 'x';               // single character
bool flag = true;           // boolean
std::string s = "hello";    // string (needs #include <string>)

// Type inference
auto x = 42;                 // x is int
auto y = 3.14;                // y is double

// Multiple declaration
int i = 1, j = 2, k = 3;

// Fixed-width integers (useful when exact size matters)
#include <cstdint>
int32_t n32 = 100;
uint64_t n64 = 100;
```

---

## 2. `const`, `constexpr`, and References

```cpp
const int MAX = 100;        // value cannot change after initialization
constexpr int SIZE = 10;    // evaluated at compile time, usable as array size
int arr[SIZE];               // legal because SIZE is constexpr

int value = 5;
int& ref = value;            // reference: an alias for `value`, not a copy
ref = 10;                    // this changes `value` too — now value == 10

const int& cref = value;     // reference that can't modify what it refers to

// const with pointers — read right to left
const int* p1 = &value;      // pointer to const int: *p1 = 5 is ILLEGAL, p1 = &other is OK
int* const p2 = &value;      // const pointer to int: *p2 = 5 is OK, p2 = &other is ILLEGAL
const int* const p3 = &value; // both are illegal to change
```

---

## 3. Operators

```cpp
// Arithmetic
int sum = 5 + 3, diff = 5 - 3, prod = 5 * 3;
int quot = 5 / 2;      // == 2 (integer division truncates)
int rem  = 5 % 2;      // == 1 (modulo)
double dq = 5.0 / 2;   // == 2.5 (float division, since one operand is double)

// Comparison
bool eq = (5 == 5);    // true
bool ne = (5 != 3);    // true

// Logical
bool andRes = true && false;  // false
bool orRes  = true || false;  // true
bool notRes = !true;          // false

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

```cpp
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

// range-based for (C++11+)
std::vector<int> nums = {1, 2, 3};
for (int n : nums)        { /* read-only copy of each element */ }
for (int& n : nums)       { n *= 2; /* modifies the vector in place */ }
for (const int& n : nums) { /* read-only, no copy */ }

// break / continue
for (int k = 0; k < 10; k++) {
    if (k == 3) continue;   // skip this iteration
    if (k == 7) break;      // exit the loop entirely
}
```

---

## 5. Functions

```cpp
// Basic declaration
int add(int a, int b) {
    return a + b;
}

// Default arguments
int multiply(int a, int b = 2) {
    return a * b;
}
multiply(5);       // uses default b=2, returns 10
multiply(5, 3);    // returns 15

// Function overloading — same name, different parameter types/counts
int square(int x)    { return x * x; }
double square(double x) { return x * x; }

// Pass by value vs reference vs pointer
void byValue(int x)  { x = 100; }              // caller's variable unaffected
void byRef(int& x)   { x = 100; }              // caller's variable changed
void byPointer(int* x) { *x = 100; }           // caller's variable changed, via dereference

int val = 5;
byValue(val);    // val is still 5
byRef(val);      // val is now 100
byPointer(&val); // val is now 100 again

// Passing containers by const reference (avoids expensive copies)
double average(const std::vector<int>& nums) {
    double sum = 0;
    for (int n : nums) sum += n;
    return sum / nums.size();
}

// Returning multiple values via std::pair or structured bindings (C++17)
#include <utility>
std::pair<int, int> minMax(const std::vector<int>& v) {
    return {*std::min_element(v.begin(), v.end()), *std::max_element(v.begin(), v.end())};
}
auto [lo, hi] = minMax({3, 1, 4, 1, 5});  // structured binding
```

---

## 6. Arrays & Strings

```cpp
// C-style array — fixed size, known at compile time
int arr[5] = {1, 2, 3, 4, 5};
int size = sizeof(arr) / sizeof(arr[0]);  // classic way to get element count

// std::array — fixed size, but STL-friendly (knows its own size)
#include <array>
std::array<int, 5> stdArr = {1, 2, 3, 4, 5};
stdArr.size();      // 5, no manual division needed

// std::vector — dynamic size, most commonly used
#include <vector>
std::vector<int> v = {1, 2, 3};
v.push_back(4);      // v is now {1, 2, 3, 4}
v.pop_back();         // removes last element
v.size();             // current element count
v[0];                 // access (no bounds check)
v.at(0);              // access with bounds check (throws if out of range)

// C-style string (char array)
char cstr[] = "hello";       // 6 bytes: 'h','e','l','l','o','\0'
strlen(cstr);                  // needs #include <cstring>, returns 5 (excludes null terminator)

// std::string — safer, preferred in modern C++
#include <string>
std::string s = "hello";
s += " world";            // s is now "hello world"
s.length();                 // 11
s.substr(0, 5);              // "hello"
s.find("world");             // returns index 6, or std::string::npos if not found
s[0];                         // 'h'
```

---

## 7. Pointers & Memory Management

```cpp
int value = 42;
int* ptr = &value;     // ptr holds the address of value
*ptr = 100;              // dereference: changes value to 100
std::cout << ptr;         // prints the address itself
std::cout << *ptr;        // prints 100

// nullptr (C++11+) — prefer over NULL or 0
int* p = nullptr;
if (p == nullptr) { /* safe check before dereferencing */ }

// Dynamic memory — manual (C++ style)
int* dynInt = new int(5);      // allocate a single int on the heap
delete dynInt;                    // must free manually, or it leaks

int* dynArr = new int[10];      // allocate an array
delete[] dynArr;                  // must use delete[] for arrays, not delete

// Smart pointers (C++11+) — PREFERRED over raw new/delete in modern C++
#include <memory>

std::unique_ptr<int> up = std::make_unique<int>(5);
// automatically freed when up goes out of scope; cannot be copied, only moved

std::shared_ptr<int> sp1 = std::make_shared<int>(5);
std::shared_ptr<int> sp2 = sp1;   // both share ownership; reference-counted
// memory freed automatically when the last shared_ptr referencing it is destroyed

// Common pointer pitfalls to avoid
// 1. Dangling pointer: pointer to memory that's already been freed
// 2. Memory leak: forgetting to delete/delete[] what you `new`'d
// 3. Double free: calling delete twice on the same pointer
// 4. Mixing new/delete with malloc/free — always match the pair
```

---

## 8. STL Containers Overview

```cpp
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <stack>
#include <queue>

// vector — dynamic array, contiguous memory, fast random access
std::vector<int> v = {3, 1, 2};

// map — ordered key-value pairs (sorted by key, implemented as a tree)
std::map<std::string, int> ages;
ages["Alice"] = 30;
ages["Bob"] = 25;
for (const auto& [name, age] : ages) { /* iterates in sorted key order */ }

// unordered_map — hash table version, faster average lookup, no ordering
std::unordered_map<std::string, int> fastAges;
fastAges["Alice"] = 30;

// set — sorted unique elements
std::set<int> s = {3, 1, 2, 2};  // stored as {1, 2, 3} — duplicates removed

// stack — LIFO (last in, first out)
std::stack<int> st;
st.push(1);
st.push(2);
st.top();    // 2
st.pop();    // removes 2

// queue — FIFO (first in, first out)
std::queue<int> q;
q.push(1);
q.push(2);
q.front();   // 1
q.pop();     // removes 1
```

---

## 9. Iterators

```cpp
std::vector<int> v = {1, 2, 3, 4};

// Explicit iterator loop
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";   // dereference the iterator to get the value
}

// auto makes this much cleaner
for (auto it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
}

// Common algorithms that work with iterators (#include <algorithm>)
#include <algorithm>
std::sort(v.begin(), v.end());                       // sorts in place
auto it = std::find(v.begin(), v.end(), 3);           // returns iterator or v.end()
int count = std::count(v.begin(), v.end(), 3);        // counts occurrences
std::reverse(v.begin(), v.end());                     // reverses in place
```

---

## 10. Classes & Objects (OOP Basics)

```cpp
class Rectangle {
private:
    double width;
    double height;

public:
    // Constructor
    Rectangle(double w, double h) : width(w), height(h) {}

    // Destructor (called automatically when object is destroyed)
    ~Rectangle() {
        // cleanup code here, if needed (e.g., freeing manually managed resources)
    }

    // Member function
    double area() const {          // const: promises not to modify the object
        return width * height;
    }

    // Getter / setter
    double getWidth() const { return width; }
    void setWidth(double w) { width = w; }
};

// Usage
Rectangle r(5.0, 3.0);
std::cout << r.area();   // 15.0

// Access specifiers:
// public    - accessible from anywhere
// private   - accessible only within the class itself
// protected - accessible within the class and its derived classes

// Struct vs class: identical except struct defaults to public, class to private
struct Point {
    int x, y;   // public by default in a struct
};
```

---

## 11. Inheritance & Polymorphism

```cpp
// Base class
class Shape {
public:
    virtual double area() const {   // virtual: allows override in derived classes
        return 0.0;
    }
    virtual ~Shape() {}              // virtual destructor — important when using
                                      // polymorphism with pointers, prevents leaks
};

// Derived class
class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}

    double area() const override {   // override: explicit, catches typos at compile time
        return 3.14159 * radius * radius;
    }
};

class Square : public Shape {
private:
    double side;
public:
    Square(double s) : side(s) {}
    double area() const override {
        return side * side;
    }
};

// Polymorphism in action — treating different shapes uniformly via base pointer
std::vector<std::unique_ptr<Shape>> shapes;
shapes.push_back(std::make_unique<Circle>(5.0));
shapes.push_back(std::make_unique<Square>(4.0));

for (const auto& shape : shapes) {
    std::cout << shape->area() << std::endl;   // calls the correct derived area()
}

// Abstract class — a class with at least one pure virtual function (= 0)
// cannot be instantiated directly, only through derived classes
class AbstractShape {
public:
    virtual double area() const = 0;   // pure virtual — no implementation here
};
```

---

## 12. Templates (Generic Programming)

```cpp
// Function template — works with any type that supports the operations used
template <typename T>
T maxValue(T a, T b) {
    return (a > b) ? a : b;
}
maxValue(3, 7);          // T deduced as int
maxValue(3.5, 2.1);      // T deduced as double

// Class template
template <typename T>
class Box {
private:
    T value;
public:
    Box(T v) : value(v) {}
    T get() const { return value; }
};

Box<int> intBox(5);
Box<std::string> strBox("hello");
```

---

## 13. Exception Handling

```cpp
#include <stdexcept>

double divide(double a, double b) {
    if (b == 0) {
        throw std::invalid_argument("division by zero");
    }
    return a / b;
}

try {
    double result = divide(10, 0);
} catch (const std::invalid_argument& e) {
    std::cout << "Caught: " << e.what() << std::endl;
} catch (...) {
    std::cout << "Caught an unknown exception" << std::endl;   // catch-all
}
```

---

## 14. Data Structures From Scratch

These are the classic "build it yourself" implementations worth being able
to write cold — common in interview-style tests.

### 14.1 Singly Linked List

```cpp
struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
public:
    LinkedList() : head(nullptr) {}

    ~LinkedList() {                     // clean up all nodes to avoid leaks
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    void pushFront(int val) {
        Node* newNode = new Node(val);
        newNode->next = head;
        head = newNode;
    }

    void pushBack(int val) {
        Node* newNode = new Node(val);
        if (head == nullptr) {
            head = newNode;
            return;
        }
        Node* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }

    bool contains(int val) const {
        Node* current = head;
        while (current != nullptr) {
            if (current->data == val) return true;
            current = current->next;
        }
        return false;
    }

    void print() const {
        Node* current = head;
        while (current != nullptr) {
            std::cout << current->data << " -> ";
            current = current->next;
        }
        std::cout << "nullptr" << std::endl;
    }
};
```

### 14.2 Stack (Custom Implementation, using std::vector internally)

```cpp
template <typename T>
class Stack {
private:
    std::vector<T> data;
public:
    void push(const T& val) { data.push_back(val); }

    void pop() {
        if (empty()) throw std::runtime_error("pop from empty stack");
        data.pop_back();
    }

    T& top() {
        if (empty()) throw std::runtime_error("top from empty stack");
        return data.back();
    }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};
```

### 14.3 Queue (Custom Implementation, using std::deque internally)

```cpp
#include <deque>

template <typename T>
class Queue {
private:
    std::deque<T> data;
public:
    void push(const T& val) { data.push_back(val); }

    void pop() {
        if (empty()) throw std::runtime_error("pop from empty queue");
        data.pop_front();
    }

    T& front() {
        if (empty()) throw std::runtime_error("front from empty queue");
        return data.front();
    }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};
```

### 14.4 Binary Search Tree (BST)

```cpp
struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int v) : value(v), left(nullptr), right(nullptr) {}
};

class BST {
private:
    TreeNode* root;

    TreeNode* insertHelper(TreeNode* node, int value) {
        if (node == nullptr) return new TreeNode(value);
        if (value < node->value) {
            node->left = insertHelper(node->left, value);
        } else if (value > node->value) {
            node->right = insertHelper(node->right, value);
        }
        // if value == node->value, typically ignore duplicates
        return node;
    }

    bool searchHelper(TreeNode* node, int value) const {
        if (node == nullptr) return false;
        if (node->value == value) return true;
        return value < node->value
            ? searchHelper(node->left, value)
            : searchHelper(node->right, value);
    }

    void inorderHelper(TreeNode* node, std::vector<int>& result) const {
        if (node == nullptr) return;
        inorderHelper(node->left, result);   // left
        result.push_back(node->value);       // root
        inorderHelper(node->right, result);  // right
    }

    void preorderHelper(TreeNode* node, std::vector<int>& result) const {
        if (node == nullptr) return;
        result.push_back(node->value);       // root
        preorderHelper(node->left, result);  // left
        preorderHelper(node->right, result); // right
    }

    void postorderHelper(TreeNode* node, std::vector<int>& result) const {
        if (node == nullptr) return;
        postorderHelper(node->left, result);  // left
        postorderHelper(node->right, result); // right
        result.push_back(node->value);        // root
    }

    void destroyHelper(TreeNode* node) {
        if (node == nullptr) return;
        destroyHelper(node->left);
        destroyHelper(node->right);
        delete node;
    }

    int heightHelper(TreeNode* node) const {
        if (node == nullptr) return 0;
        return 1 + std::max(heightHelper(node->left), heightHelper(node->right));
    }

public:
    BST() : root(nullptr) {}
    ~BST() { destroyHelper(root); }

    void insert(int value) {
        root = insertHelper(root, value);
    }

    bool search(int value) const {
        return searchHelper(root, value);
    }

    std::vector<int> inorderTraversal() const {
        std::vector<int> result;
        inorderHelper(root, result);
        return result;   // for a BST, inorder traversal is always sorted ascending
    }

    std::vector<int> preorderTraversal() const {
        std::vector<int> result;
        preorderHelper(root, result);
        return result;
    }

    std::vector<int> postorderTraversal() const {
        std::vector<int> result;
        postorderHelper(root, result);
        return result;
    }

    int height() const {
        return heightHelper(root);
    }
};

// Usage
BST tree;
tree.insert(5);
tree.insert(3);
tree.insert(8);
tree.insert(1);
tree.insert(4);

tree.search(4);                 // true
auto sorted = tree.inorderTraversal();   // {1, 3, 4, 5, 8} — always sorted for a BST
tree.height();                   // tree depth
```

### 14.5 Breadth-First Traversal (Level Order) — Using a Queue

```cpp
std::vector<int> levelOrder(TreeNode* root) {
    std::vector<int> result;
    if (root == nullptr) return result;

    std::queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();
        result.push_back(current->value);

        if (current->left != nullptr)  q.push(current->left);
        if (current->right != nullptr) q.push(current->right);
    }
    return result;
}
```

---

## Quick Reference: When to Use What

| Need | Use |
|---|---|
| Fixed-size collection, fast random access | `std::array` |
| Growable collection, fast random access | `std::vector` |
| Fast insert/remove at both ends | `std::deque` |
| Key-value pairs, sorted by key | `std::map` |
| Key-value pairs, fastest average lookup, no order | `std::unordered_map` |
| Unique sorted elements | `std::set` |
| LIFO behavior | `std::stack` |
| FIFO behavior | `std::queue` |
| Automatic memory management, single owner | `std::unique_ptr` |
| Automatic memory management, shared ownership | `std::shared_ptr` |
| Sorted data with fast search/insert/delete (average) | Binary Search Tree |

---

Practice tip: rewrite the BST section from memory without looking, then
compare against this file — that's a much stronger test of readiness than
just reading it once.