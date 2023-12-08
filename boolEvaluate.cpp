//Program to evaluate a boolean expression given the input values for each variable
//Expression to solve for must be hard-coded in main
//Prints result of the expression

using namespace std;
#include <string>
#include <iostream>
#include <vector>
#include <math.h>

struct charElement {
    char value;
    charElement *next;
    charElement *prev;
    charElement(char enValue) {
        value = enValue;
    }
};


int solve(charElement* start, charElement* end) {
    charElement *one = start;
    charElement *curr = start;
    int total = 1;
    int parenCount = 0;
    bool add = false;
    bool tilde = false;
    charElement *closeParen;
    while (curr != end->next) {
        if (curr->value == '+') {
            add = true;
            curr = curr->next;
        }
        if (curr->value == '~') {
            tilde = true;
            curr = curr->next;
        }
        if (curr->value == '(') {
            closeParen = curr;
            while (closeParen != nullptr) {
                if (closeParen->value == '(') {
                    parenCount++;
                }
                else if (closeParen->value == ')') {
                    parenCount--;
                }
                if (parenCount == 0) {
                    if (add) {
                        if (tilde) {
                            total += ((solve(curr->next, closeParen->prev) + 1) % 2);
                        }
                        else {
                            total += solve(curr->next, closeParen->prev);
                        }
                    }
                    else {
                        if (tilde) {
                            total *= ((solve(curr->next, closeParen->prev) + 1) % 2);
                        }
                        else {
                            total *= solve(curr->next, closeParen->prev);
                        }
                    }
                    curr->next = closeParen->next;
                    break;
                }
                closeParen = closeParen->next;
            }
        }
        else {
            if (curr->value == '1') {
                if (add) {
                    if (tilde) {
                        total += 0;
                    }
                    else {
                        total += 1;
                    }
                 }
                else {
                    if (tilde) {
                        total *= 0;
                    }
                    else {
                        total *= 1;
                    }
                }
            }
            else if (curr->value == '0') {
                if (add) {
                    if (tilde) {
                        total += 1;
                    }
                    else {
                        total += 0;
                    }
                }
                else {
                    if (tilde) {
                        total *= 1;
                    }
                    else {
                        total *= 0;
                    }
                }
            }
        }
        curr = curr->next;
        add = false;
        tilde = false;
    }
    if (total > 0) {
        return 1;
    }
    else {
        return 0;
    }
}

int makeList(string expression) {
    charElement *curr;
    charElement *prevCurr;
    charElement *first;
    charElement *last;
    first = new charElement(expression.at(0));
    curr = first;
    for (int x = 1; x < expression.size(); x++) {        
        curr->next = new charElement(expression.at(x));
        curr = curr->next;
    }
    curr->next = nullptr;
    last = curr;

    curr = first;
    first->prev = nullptr;
    prevCurr = curr->next;
    while (prevCurr != nullptr) {
        prevCurr->prev = curr;
        prevCurr = prevCurr->next;
        curr = curr->next;
    }
    return solve(first, last);
}

string makeMathExp(string expression, vector<string> inputNames, vector<int> inputVals) {
    //First turn the boolean expression into a math expression using the given inputs
    string mathExpression = "";
    
        //Eliminate whitespace
        for (int y = 0; y < expression.size(); y++) {
            if(expression.at(y) == ' ') {
                mathExpression += "";
            }
            else {
                mathExpression += expression.at(y);
            }
        }

        string currVar;
        string newExpression = "";
        for (int x = 0; x < mathExpression.size(); x++) {  
            if (mathExpression.at(x) != '(' && mathExpression.at(x) != ')' && mathExpression.at(x) != '1' && mathExpression.at(x) != '0' && mathExpression.at(x) != '+' && mathExpression.at(x) != '~') {
                currVar += mathExpression.at(x);
                for (int y = 0; y < inputNames.size(); y++) {
                    if (currVar == inputNames.at(y)) {
                        newExpression += to_string(inputVals.at(y));
                        currVar = "";
                        break;
                    }
                }
            }
            else {
                newExpression += mathExpression.at(x);
            }
        }
        
        return newExpression;
};

int eval(string expression, vector<string> inputNames, vector<int> inputVals) {
    return makeList(makeMathExp(expression, inputNames, inputVals));
};

int main() {
    //Argument order: (expression), (variables), (corresponding variable values)
    cout << eval("~ab(c+d)", {"a", "b", "c", "d"}, {0, 1, 1, 0}) << endl;
}