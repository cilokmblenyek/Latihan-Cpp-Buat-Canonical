#include <bits/stdc++.h>

using namespace std;

class Solution {
public:
    vector<string> fizzBuzz(int n) {
        vector<string> output;

        output = vector<string>(n);

        for (int i = 1; i<=n; i++) {
            if (i % 3 == 0 && i % 5 == 0) output[i-1] = "FizzBuzz";
            else if (i % 3 == 0) output[i-1] = "Fizz";
            else if (i % 5 == 0) output [i-1] = "Buzz";
            else output[i-1] = to_string(i);
        }
        return output;
    }
};

int main() {
    Solution solution;
    int n;
    cin >> n;

    vector<string> result = solution.fizzBuzz(n);
    for (const string& str : result) {
        cout << str << endl;
    }
    return 0;
}