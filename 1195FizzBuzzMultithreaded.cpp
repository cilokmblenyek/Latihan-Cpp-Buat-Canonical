#include <bits/stdc++.h>

using namespace std;

vector<string> output;
mutex outputMutex;

class FizzBuzz {
private:
    int n;
    int current;
    mutex currentMutex;
    condition_variable condition;

    bool isFizz() const {
        return current % 3 == 0 && current % 5 != 0;
    }

    bool isBuzz() const {
        return current % 5 == 0 && current % 3 != 0;
    }

    bool isFizzBuzz() const {
        return current % 3 == 0 && current % 5 == 0;
    }

    bool isNumber() const {
        return current % 3 != 0 && current % 5 != 0;
    }

    void addOutput(const string& value) {
        lock_guard<mutex> lock(outputMutex);
        output.push_back(value);
    }

public:
    explicit FizzBuzz(int n) : n(n), current(1) {}

    // printFizz() outputs "fizz".
    void fizz(function<void()> printFizz) {
        while (true) {
            unique_lock<mutex> lock(currentMutex);

            condition.wait(lock, [this] {
                return current > n || isFizz();
            });

            if (current > n) {
                return;
            }

            printFizz();
            ++current;

            lock.unlock();
            condition.notify_all();
        }
    }

    // printBuzz() outputs "buzz".
    void buzz(function<void()> printBuzz) {
        while (true) {
            unique_lock<mutex> lock(currentMutex);

            condition.wait(lock, [this] {
                return current > n || isBuzz();
            });

            if (current > n) {
                return;
            }

            printBuzz();
            ++current;

            lock.unlock();
            condition.notify_all();
        }
    }

    // printFizzBuzz() outputs "fizzbuzz".
    void fizzbuzz(function<void()> printFizzBuzz) {
        while (true) {
            unique_lock<mutex> lock(currentMutex);

            condition.wait(lock, [this] {
                return current > n || isFizzBuzz();
            });

            if (current > n) {
                return;
            }

            printFizzBuzz();
            ++current;

            lock.unlock();
            condition.notify_all();
        }
    }

    // printNumber(x) outputs "x", where x is an integer.
    void number(function<void(int)> printNumber) {
        while (true) {
            unique_lock<mutex> lock(currentMutex);

            condition.wait(lock, [this] {
                return current > n || isNumber();
            });

            if (current > n) {
                return;
            }

            printNumber(current);
            ++current;

            lock.unlock();
            condition.notify_all();
        }
    }
};

class runthread {
public:
    explicit runthread(int n) {
        FizzBuzz fizzBuzz(n);

        thread t1(&FizzBuzz::fizz, &fizzBuzz, [] {
            lock_guard<mutex> lock(outputMutex);
            output.push_back("fizz");
        });

        thread t2(&FizzBuzz::buzz, &fizzBuzz, [] {
            lock_guard<mutex> lock(outputMutex);
            output.push_back("buzz");
        });

        thread t3(&FizzBuzz::fizzbuzz, &fizzBuzz, [] {
            lock_guard<mutex> lock(outputMutex);
            output.push_back("fizzbuzz");
        });

        thread t4(&FizzBuzz::number, &fizzBuzz, [](int x) {
            lock_guard<mutex> lock(outputMutex);
            output.push_back(to_string(x));
        });

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
};

int main() {
    runthread(150);

    cout << "[";
    for (size_t i = 0; i < output.size(); ++i) {
        if (i > 0) {
            cout << ",";
        }
        cout << output[i];
    }
    cout << "]\n";
}