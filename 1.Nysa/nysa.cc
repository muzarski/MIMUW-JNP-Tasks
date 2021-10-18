// Authors: Daniel Mastalerz, Mikolaj Uzarski

#include <iostream>
#include <set>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

using namespace std;

enum {AND, NAND, OR, NOR, NOT, XOR};
static unordered_map<string, int> logicOP {{"AND", 0}, {"NAND", 1}, {"OR", 2}, {"NOR", 3}, {"NOT", 4}, {"XOR", 5}};

// Regular expressions used to validate user's input.
static const regex regNot ("^[ \t\v\f\r]*NOT([ \t\v\f\r]+0*[1-9][0-9]{0,8}){2}[ \t\v\f\r]*$");
static const regex regXor ("^[ \t\v\f\r]*XOR([ \t\v\f\r]+0*[1-9][0-9]{0,8}){3}[ \t\v\f\r]*$");
static const regex regRest ("^[ \t\v\f\r]*(AND|NAND|OR|NOR)([ \t\v\f\r]+0*[1-9][0-9]{0,8}){3,}[ \t\v\f\r]*$");

static bool checkRegex(const string &s) {
    return regex_match(s, regNot) || regex_match(s, regXor) || regex_match(s, regRest);
}

// Checks whether the graph contains a cycle.
static void dfs(int &curOutSignal, const unordered_multimap<int, int> &edges,
                unordered_set<int> &visited, unordered_set<int> &currentPath, bool &hasCycle) {

    // Range of input signals that are assigned to given output signal.
    auto inputSignalsRange = edges.equal_range(curOutSignal);

    if (inputSignalsRange.first != inputSignalsRange.second) {
        visited.insert(curOutSignal);
        currentPath.insert(curOutSignal);
    }

    for (auto inputsIter = inputSignalsRange.first; inputsIter != inputSignalsRange.second; inputsIter++) {
        if (currentPath.contains(inputsIter->second)) {
            hasCycle = true;
            return;
        }

        int curValue = inputsIter->second;
        if (!visited.contains(curValue)) {
            dfs(curValue, edges, visited, currentPath, hasCycle);
            if (hasCycle) return;
        }
    }

    // Since we finished visiting all neighbours of the current output signal,
    // we can remove it from the 'currentPath'.
    currentPath.erase(curOutSignal);
}

// Calculates logic values of the signals.
// If 'signal' has already a value assigned, the function simply returns it.
// Otherwise, it means that 'signal' is an output signal, so the function
// calculates its value based upon the values of input signals that are
// connected to the 'signal'.
static bool findValues(int signal, unordered_multimap<int, int>& edges, unordered_map<int,
        string>& operations, unordered_map<int,bool>& logicValues) {

    auto it = logicValues.find(signal);
    if (it != logicValues.end()) {
        return it->second;
    }
    string op = operations[signal];
    auto signals = edges.equal_range(signal);
    bool val = true;
    switch(logicOP[op]) {
        case AND:
            val = all_of(signals.first, signals.second,
                         [&edges, &operations, &logicValues](pair<const int, int> &p)
                         {return findValues(p.second, edges, operations, logicValues);});
            break;
        case NAND:
            val = !all_of(signals.first, signals.second,
                          [&edges, &operations, &logicValues](pair<const int, int> &p)
                          {return findValues(p.second, edges, operations, logicValues);});
            break;
        case OR:
            val = any_of(signals.first, signals.second,
                         [&edges, &operations, &logicValues](pair<const int, int> &p)
                         {return findValues(p.second, edges, operations, logicValues);});
            break;
        case NOR:
            val = !any_of(signals.first, signals.second,
                          [&edges, &operations, &logicValues](pair<const int, int> &p)
                          {return findValues(p.second, edges, operations, logicValues);});
            break;
        case NOT:
            val = !findValues(signals.first->second, edges, operations, logicValues);
            break;
        case XOR:
            int inSignal1 = signals.first->second;
            signals.first++;
            int inSignal2 = signals.first->second;
            val = findValues(inSignal1, edges, operations, logicValues) ^ findValues(inSignal2, edges, operations, logicValues);
            break;
    }

    logicValues.insert(make_pair(signal, val));
    return val;
}

// Recursively generates consecutive binary numbers representing logic values of the input signals.
// When a certain combination is calculated, the function calculates the logic values of the remaining signals.
// Results are stored in the 'output' vector.
static void generateAllCombinations(unordered_set <int>::iterator a, unordered_set <int>::iterator b,
                                    unordered_multimap<int, int>& edges, unordered_map<int, string>& operations,
                                    unordered_map<int,bool>& logicValues, unordered_set <int>& outSignals,
                                    set<int>& signals, vector<pair<string, string>>& output, unordered_set<int>& inSignals) {

    // Combination is not generated yet.
    if (a != b) {
        unordered_map<int, bool> logic1 = logicValues;
        unordered_map<int, bool> logic2 = logicValues;
        auto c = a;
        logic1.insert(make_pair(*a, 0));
        generateAllCombinations(++a, b, edges, operations, logic1,outSignals, signals, output, inSignals);
        logic2.insert(make_pair(*c, 1));
        generateAllCombinations(++c, b, edges, operations, logic2,outSignals, signals, output, inSignals);
    }
    else {
        string str;
        string str2;
        for (auto& elem: outSignals) {
            findValues(elem, edges, operations, logicValues);
        }
        for (auto& elem: signals) {
            string x = to_string(logicValues[elem]);
            str += x;
            if (inSignals.contains(elem)) {
                str2 += x;
            }
        }
        output.emplace_back(str2, str);
    }

}

int main() {

    // Ordered set of all signals that appear.
    set<int> signals;

    // Set of the output signals.
    unordered_set<int> outputSignals;

    // Set of signals that are only input signals (basically 'signals \setminus outputSignals').
    unordered_set<int> inputSignals;

    // Edges of a directed graph whose vertices are signals: (output signal) -> (input signal).
    unordered_multimap<int, int> edges;

    // Map: (output signal) -> (logic gate that the output signal is assigned to).
    unordered_map<int, string> operations;

    // Current logic gate
    string operation;

    int curSignal, curOutSignal, lineNumber = 1;
    string s;
    bool hasError = false;

    while (getline(cin, s)) {
        if (checkRegex(s)) {
            stringstream strStream;
            strStream.str(s);
            strStream >> operation;
            int i = 0;

            while(strStream >> curSignal) {

                signals.insert(curSignal);

                if (i == 0) {
                    if (outputSignals.contains(curSignal)) {
                        cerr << "Error in line " << lineNumber << ": signal " << curSignal << " is assigned to multiple outputs." << endl;
                        hasError = true;
                    }

                    outputSignals.insert(curSignal);
                    curOutSignal = curSignal;
                    operations.insert(make_pair(curOutSignal, operation));

                }
                else if (!hasError) {

                    edges.insert(make_pair(curOutSignal, curSignal));
                }
                i++;
            }
        }
        else {
            cerr << "Error in line " << lineNumber << ": " << s << endl;
            hasError = true;
        }
        lineNumber++;
    }


    if (!hasError) {
        bool hasCycle = false;

        unordered_set<int> visited;
        for (int curValue : outputSignals) {
            if (!visited.contains(curValue)) {

                // Set of vertices that were visited in the current traversal.
                // If we visit a vertex, and 'currentPath' already contains its value => our graph is not the DAG.
                unordered_set<int> currentPath;
                dfs(curValue, edges, visited, currentPath, hasCycle);
                if (hasCycle) {
                    cerr << "Error: sequential logic analysis has not yet been implemented." << endl;
                    return 0;
                }
            }

        }
    }
    else {
        return 0;
    }

    for (auto& elem: signals) {
        if (!outputSignals.contains(elem)) {
            inputSignals.insert(elem);
        }
    }

    // Vector of pairs: (binary number represented by logic values of the input signals) -> (logic values of all signals)
    vector<pair<string, string> > output;
    unordered_map<int, bool> logicValues;
    generateAllCombinations(inputSignals.begin(), inputSignals.end(), edges, operations,
                            logicValues, outputSignals, signals, output, inputSignals);

    // Sorting lexicographically based upon the first element of the pair (consecutive binary numbers).
    sort(output.begin(), output.end());
    for (auto& elem : output) {
        cout << elem.second << "\n";
    }
    return 0;
}
