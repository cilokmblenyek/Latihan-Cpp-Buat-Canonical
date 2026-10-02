# Practice Task: EventNotifier (PIMPL + Subscribe/Unsubscribe/Notify)

This reconstructs the pattern you described: a class exposing
`constructor`, `destructor`, `subscribe()`, `unsubscribe()`, and
`notify(data, len)`, implemented using the **PIMPL idiom** (Pointer to
IMPLementation) with a handler/callback mechanism. This exact combination —
PIMPL + an observer-style notifier — is a genuinely common C++ interview
pattern because it tests several things at once: header/implementation
separation, smart pointer usage, callback storage, and a specific compiler
gotcha (explained below) that trips up a lot of candidates.

---

## 1. What PIMPL Is, and Why This Pattern Uses It

**PIMPL (Pointer to Implementation):** the public header only declares a
pointer to a forward-declared `Impl` class — the actual member data and
private logic live in the `.cpp` file, completely hidden from anyone
`#include`-ing the header.

**Why it's used here:**
- The header stays stable even if internal data structures change — no
  recompilation needed for code that just uses the class.
- Private implementation details (like *how* subscribers are stored) are
  fully hidden — nothing in the header leaks internal types like
  `std::unordered_map`.
- It's the standard way to give a class a stable public API while hiding
  volatile internals — exactly what a reusable "EventNotifier" component in
  a larger codebase would want.

---

## 2. The Header — `EventNotifier.h`

```cpp
#pragma once

#include <cstddef>   // for size_t
#include <cstdint>   // for uint8_t
#include <functional>
#include <memory>

class EventNotifier {
public:
    // Handler signature: receives a pointer to the event data and its length
    using Handler = std::function<void(const uint8_t* data, size_t len)>;

    // Opaque identifier returned by subscribe(), used later to unsubscribe
    using SubscriptionId = size_t;

    EventNotifier();
    ~EventNotifier();   // declared here, DEFINED in the .cpp — see note below

    // Non-copyable: a PIMPL class usually disables copy unless you
    // explicitly implement deep-copy semantics for the Impl
    EventNotifier(const EventNotifier&) = delete;
    EventNotifier& operator=(const EventNotifier&) = delete;

    // Movable is fine with unique_ptr (compiler-generated move works)
    EventNotifier(EventNotifier&&) noexcept = default;
    EventNotifier& operator=(EventNotifier&&) noexcept = default;

    // Register a handler; returns an ID you can use to unsubscribe later
    SubscriptionId subscribe(Handler handler);

    // Remove a previously registered handler by its ID
    void unsubscribe(SubscriptionId id);

    // Call every currently-subscribed handler with the given data
    void notify(const uint8_t* data, size_t len);

private:
    class Impl;                     // forward declaration only — incomplete type here
    std::unique_ptr<Impl> pImpl;    // the actual implementation lives in the .cpp
};
```

---

## 3. The Implementation — `EventNotifier.cpp`

```cpp
#include "EventNotifier.h"

#include <unordered_map>

// Full definition of Impl — only visible inside this .cpp file
class EventNotifier::Impl {
public:
    SubscriptionId subscribe(Handler handler) {
        SubscriptionId id = nextId_++;
        handlers_[id] = std::move(handler);
        return id;
    }

    void unsubscribe(SubscriptionId id) {
        handlers_.erase(id);
    }

    void notify(const uint8_t* data, size_t len) {
        for (auto& [id, handler] : handlers_) {
            if (handler) {
                handler(data, len);
            }
        }
    }

private:
    std::unordered_map<SubscriptionId, Handler> handlers_;
    SubscriptionId nextId_ = 0;
};

// --- EventNotifier method definitions ---

EventNotifier::EventNotifier()
    : pImpl(std::make_unique<Impl>()) {}

// IMPORTANT: this must be defined HERE, not in the header, and not with
// `= default` in the header either. See "The #1 Gotcha" below.
EventNotifier::~EventNotifier() = default;

EventNotifier::SubscriptionId EventNotifier::subscribe(Handler handler) {
    return pImpl->subscribe(std::move(handler));
}

void EventNotifier::unsubscribe(SubscriptionId id) {
    pImpl->unsubscribe(id);
}

void EventNotifier::notify(const uint8_t* data, size_t len) {
    pImpl->notify(data, len);
}
```

---

## 4. The #1 Gotcha: Why the Destructor MUST Be Defined in the `.cpp`

This is very likely the exact trap a PIMPL task is testing for, and it's a
genuine compiler error, not just a style preference.

**The problem:** `std::unique_ptr<Impl>`'s destructor needs to call
`delete` on the `Impl*` it owns. To call `delete` on a type, the compiler
needs to see that type's **complete definition** at the point where the
deletion code is generated.

If you write this in the header:
```cpp
// EventNotifier.h
~EventNotifier() = default;   // or even just omit it entirely and let the
                                // compiler generate one implicitly
```
...the compiler tries to generate the destructor's body **right there in
the header**, where `Impl` is only forward-declared (incomplete). This
fails to compile with an error like:
```
error: invalid application of 'sizeof' to incomplete type 'EventNotifier::Impl'
```

**The fix:** declare the destructor in the header (just `~EventNotifier();`
with no body), and define it in the `.cpp` — even if the definition is just
`= default;` — because by that point, `Impl`'s full definition is visible
(it's defined just above in the same file). This is exactly what the code
above does.

**This same issue applies to the move constructor/move assignment operator**
if you declare them as `= default` — they also need to see `Impl`'s
complete type, so if you default them, do it in the `.cpp`, not the header.
(The version above defaults them in the header, which actually only works
because `noexcept = default` move members don't themselves need to
instantiate `unique_ptr`'s deleter — but **if your compiler complains about
this**, the safe fix is to declare them in the header and `= default` them
in the `.cpp`, exactly like the destructor.)

---

## 5. A Test File to Self-Verify (GoogleTest style)

```cpp
#include "EventNotifier.h"
#include <gtest/gtest.h>
#include <vector>

TEST(EventNotifierTest, SingleSubscriberReceivesNotification) {
    EventNotifier notifier;
    std::vector<uint8_t> received;

    notifier.subscribe([&received](const uint8_t* data, size_t len) {
        received.assign(data, data + len);
    });

    uint8_t payload[] = {1, 2, 3};
    notifier.notify(payload, 3);

    EXPECT_EQ(received, (std::vector<uint8_t>{1, 2, 3}));
}

TEST(EventNotifierTest, MultipleSubscribersAllReceiveNotification) {
    EventNotifier notifier;
    int callCountA = 0;
    int callCountB = 0;

    notifier.subscribe([&](const uint8_t*, size_t) { callCountA++; });
    notifier.subscribe([&](const uint8_t*, size_t) { callCountB++; });

    uint8_t payload[] = {0xFF};
    notifier.notify(payload, 1);

    EXPECT_EQ(callCountA, 1);
    EXPECT_EQ(callCountB, 1);
}

TEST(EventNotifierTest, UnsubscribedHandlerIsNotCalled) {
    EventNotifier notifier;
    int callCount = 0;

    auto id = notifier.subscribe([&](const uint8_t*, size_t) { callCount++; });
    notifier.unsubscribe(id);

    uint8_t payload[] = {0x01};
    notifier.notify(payload, 1);

    EXPECT_EQ(callCount, 0);
}

TEST(EventNotifierTest, NotifyWithNoSubscribersDoesNotCrash) {
    EventNotifier notifier;
    uint8_t payload[] = {0x01};
    EXPECT_NO_THROW(notifier.notify(payload, 1));
}
```

A minimal `CMakeLists.txt` to build and run this locally:

```cmake
cmake_minimum_required(VERSION 3.18)
project(EventNotifierPractice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_executable(tests EventNotifier.cpp test_EventNotifier.cpp)
target_link_libraries(tests GTest::gtest GTest::gtest_main pthread)
```

---

## 6. Variations You Might Be Asked For

Be ready to adapt this pattern under a few common twists:

- **Thread safety:** if the task mentions handlers could be called from a
  different thread than `subscribe`/`unsubscribe`, you'd add a
  `std::mutex` inside `Impl` and lock it around all three operations.
  ```cpp
  std::mutex mutex_;
  void notify(const uint8_t* data, size_t len) {
      std::lock_guard<std::mutex> lock(mutex_);
      for (auto& [id, handler] : handlers_) { if (handler) handler(data, len); }
  }
  ```
- **`subscribe` returning `bool`/`void` instead of an ID**, with
  unsubscribe taking the same `Handler` back — this is actually a worse
  design (you can't compare two `std::function`s for equality), so if asked
  to implement it this way, mention this limitation if you get the chance.
- **A C-style handler instead of `std::function`:** e.g.,
  `void subscribe(void (*handler)(const uint8_t*, size_t, void* userData), void* userData)`
  — common if the task wants to stay closer to a C-compatible ABI. The
  storage inside `Impl` would hold a `struct { HandlerFn fn; void* userData; }`
  instead of a `std::function`.
- **Unsubscribe-all / clear() method** — trivial addition:
  `void clear() { handlers_.clear(); }`.

---

## 7. Quick Checklist for the Real Test

- [ ] `Impl` forward-declared in the header, fully defined in the `.cpp`
- [ ] Destructor **declared** in header, **defined** in `.cpp` (even if just `= default;`)
- [ ] Copy constructor/assignment deleted (standard for PIMPL unless you
      explicitly implement deep copy)
- [ ] `subscribe` returns something usable for `unsubscribe` later (an ID,
      not the handler itself)
- [ ] `notify` safely handles the zero-subscriber case (loop over an empty
      map just does nothing — no special-casing needed)
- [ ] Headers only include what's needed for the **interface**
      (`<memory>`, `<functional>`, `<cstdint>`, `<cstddef>`) — container
      headers like `<unordered_map>` belong in the `.cpp` only
