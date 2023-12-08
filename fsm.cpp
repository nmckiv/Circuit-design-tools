//Program to provide input and output logic for a finite state machine
//A description of the desired states, inputs, outputs, and transition conditions must be hard-coded in main
//Prints out flip-flop input and output logic

using namespace std;
#include <string>
#include <iostream>
#include <vector>
#include <math.h>

//kmap soplver code below
struct BoolVar {
    string name;
    bool sign;
    bool include = true;//Indicates whether this variable is eliminated by the kmap
    BoolVar(string name, int sign) {
        this->name = name;
        this->sign = sign;
    }
    void print() {
        if (!sign) {
            cout << "~" << flush;
        }
        cout << name << flush;
    }
};

//A single cell in the kmap
struct Cell {
    //Cell pointers are used to represent adjacencies
    //A vector must be used to allow for an unknown number of variables
    string name;
    int value;
    Cell *next;
    int groupsIn = 0;
    vector<Cell*> adjacencies;
    vector<BoolVar*> boolVars;
    Cell(int value) {
        this->value = value;
    }
    void add(Cell *cell) {
        adjacencies.push_back(cell);
    }
    void addVar(BoolVar *boolVar) {
        boolVars.push_back(boolVar);
    }
};
struct Group {
    string expression;//Boolean expression required to represent the group
    bool necessary = false;
    vector<Cell*> elements;
    void Add(Cell *element) {
        elements.push_back(element);
    }
    Group(Cell *cell) {
        Add(cell);
    }
    Group() {
        
    }
    
    //For debugging purposes
    void Print() {
        int size = elements.size();
        for (int x = 0; x < size; x++) {
            cout << elements.at(x)->name << " " << flush;
            // cout << elements.at(x)->name << " " << elements.at(x)->groupsIn << flush;
        }
        cout << endl;
    }
};

//A single column in a truth table
struct TruthColumn {
    //A variable name and a vector of bools representing each case
    string name;
    vector<int> values;
    TruthColumn(string name) {
        this->name = name;
    }
    //Adds a value to the column
    void addValue(int value) {
        values.push_back(value);
    }
};

//Truth columns organized into a truth table
class TruthTable {
    //A vector of truth column inputs and a single output truth column
    public:
    vector<TruthColumn*> inputs;
    TruthColumn *output;
    //Constructor to build truth table out of a string representing a boolean function
    TruthTable(string expression) {

    }
    //Constructor to automatically generate input values, assumes output is in binary ascending order
    TruthTable(vector<string> inputNames, string outputName, vector<int> outputs) {
        TruthColumn *newTruthColumn;
        for (string name : inputNames) {
            inputs.push_back(new TruthColumn(name));
        }
        output = new TruthColumn(outputName);
        output->values = outputs;

        //Add input values in ascending binary order
        int size = pow(2, inputs.size());
        int inputNum = inputs.size();
        int step = size;
        bool value = true;
        for (int x = 0; x < inputNum; x++) {
            step /= 2;
            for (int y = 0; y < size; y++) {
                if (y % step == 0) {
                    value = !value;
                }
                inputs.at(x)->addValue(value);
            }
        }
    }
    //Returns value of output at an entered index
    int outputAt(int index) {
        return output->values.at(index);
    }
};

class KarnaughMap {
    Cell *first;//First cell in linked list of all cells in kmap
    vector<Cell*> cells;//Vector of all cells in kmap for easier access
    vector<Group*> groups;//Groups formed in kmap
    string solution = "";
    int method;
    public:

    //Method to make list of cells
    void makeCellList(TruthTable* truthTable) {
        first = new Cell(truthTable->outputAt(0));
        cells.push_back(first);
        int cellCount = pow(2, truthTable->inputs.size());
        for (int x = 1; x < cellCount; x++) {
            first->next = new Cell(truthTable->outputAt(x));
            first = first->next;
            cells.push_back(first);
        }
        first = cells.at(0);
    }

    //Adds boolean variables to each cell
    void addBools(TruthTable *truthTable) {
        bool varSign;
        string varName;
        Cell *curr = first;
        int cellCount = cells.size();
        int varCount = truthTable->inputs.size();
        for (int x = 0; x < cellCount; x++) {
            for (int y = 0; y < varCount; y++) {
                varSign = (bool) truthTable->inputs.at(y)->values.at(x);
                varName = truthTable->inputs.at(y)->name;
                curr->addVar(new BoolVar(varName, varSign));
            }
            curr = curr->next;
        }
    }

    //Rearranges according to kmap convention (i.e. swaps third and fourth row in each dimension)
    void rearrange() {
        vector<Cell*> bench;
        int cellCount = cells.size();
        int rearrangements = log10(cellCount) / log10(4);
        int interval;
        for (int x = 0; x < rearrangements; x++) {
            interval = 2 * int(pow(4, x));
            for (int y = 0; y < cellCount;) {
                y += interval;
                if (y >= cellCount) {break;}
                for (int z = 0; z < interval; z++) {
                    bench.push_back(cells.at(y + z));
                }
                for (int a = 0; a < interval / 2; a++) {
                    cells.at(y + a) = bench.at((interval / 2) + a);
                }
                for (int b = interval / 2; b < interval; b++) {
                    cells.at(y + b) = bench.at(b - (interval / 2));
                }
                bench.clear();
                y += interval;
            }
        }
    }

    //Forms adjacencies
    void formAdjacendies(TruthTable *truthTable) {
        vector<Cell*> cellsInverted;
        int cellCount = cells.size();
        int varCount = truthTable->inputs.size();
        for (int x = 0; x < cellCount; x++) {
            cellsInverted.push_back(cells.at(cellCount - 1 - x));
        }
        int interval;
        int step;
        int dimCount = ((varCount + 1) / 2);\
        for (int x = 0; x < dimCount; x++) {
        step = 0;
        for (int y = 0; y < cellCount; ) {
            for (int z = 0; z < pow(4, x); z++) {
                cells.at(y)->add(cells.at((step + ((int(y + pow(4, x))) % int (pow(4, x + 1)))) % cellCount));
                y++;
            }
            if (int((y) % (int(pow(4, x + 1)))) == 0) {
                step += pow(4, x + 1);
            }
        }
        step = 0;
        for (int y = 0; y < cellCount;) {
            for (int z = 0; z < pow(4, x); z++) {
                cellsInverted.at(y)->add(cellsInverted.at((step + ((int(y + pow(4, x))) % int (pow(4, x + 1)))) % cellCount));
                y++;
            }
            if (int((y) % (int(pow(4, x + 1)))) == 0) {
                step += pow(4, x + 1);
            }
        }
    }
    }

    //Names each cell according to the minterm/maxterm number it represents
    void nameCells() {
        int cellCount = cells.size();
        for(int x = 0; x < cellCount; x++) {
            cells.at(x)->name = to_string(x);
        }
    }

    //For debugging purposes
    void print() {
        for (int x = 0; x < cells.size(); x++) {
            cout << cells.at(x)->name << " = " << cells.at(x)->value << " -> " << flush;
            for (int y = 0; y < cells.at(x)->adjacencies.size(); y++) {
                cout << cells.at(x)->adjacencies.at(y)->name << " = " << cells.at(x)->adjacencies.at(y)->value << " -> " << flush;
            }
            cout << endl;
        }
    }
        KarnaughMap(TruthTable* truthTable, int method) {
            this->method = method;//Indicates whether to group 1's (SOP) or 0's (POS)
            makeCellList(truthTable);//Forms list of cells
            nameCells();//Names cells in minterm/maxterm order (i.e. 0, 1, 2...etc)
            addBools(truthTable);//Adds boolean variable representation to each cell in the kmap
            rearrange();//Rearranges cell order in kmap convention
            formAdjacendies(truthTable);//Forms appropriate adjacencies between cells
        }

        //For debugging purposes
        void printGroups(vector<Group*> groupsToPrint) {
            for (int x = 0; x < groupsToPrint.size(); x++) {
                groupsToPrint.at(x)->Print();
            }
        }

        bool checkExistence(Group *group, int direction, int target) {
            int groupSize = group->elements.size();
            Cell *currCell;
            for (int x = 0; x < groupSize; x++) {
                currCell = group->elements.at(x)->adjacencies.at(direction);
                if (currCell->value != target &&  currCell->value != 2) {
                    return false;
                }
            }
            return true;
        }

        bool checkExistence(Group *group, int direction, int target, string row) {
            int groupSize = group->elements.size();
            Cell *currCell;
            if (row == "row") {
                for (int x = 0; x < groupSize; x++) {
                    currCell = group->elements.at(x)->adjacencies.at(direction);
                    for (int y = 0; y < 3; y++) {
                        if (currCell->value != target &&  currCell->value != 2) {
                            return false;
                        }
                        currCell = currCell->adjacencies.at(direction);
                    }
                    //return true;
                }
                return true;
            }
            else {
                cout << "Invalid input" << endl;
                return false;
            }
            return false;
        }


        void makeGroups() {
            //Loop through every cell in the kmap
            //If the cell has not been grouped, make a new group of that singular cell
            //After the formation of the initial singular-cell group, try to extend that new group across a dimension to form a new group
            //repeat for each dimension
            vector<Group*> workingSet;
            Cell *curr = first;
            int workingSetSize = 0;
            int dimCount = cells.at(0)->adjacencies.size() / 2;
            Group *newGroup;
            bool twoLong;
            while (curr != nullptr) {
                if (curr->groupsIn == 0 && curr->value == method) {
                    workingSet.push_back((new Group(curr)));
                    curr->groupsIn++; workingSetSize++;
                    //Extend all groups in working set through every dimension

                    //For every dimension
                        //For every group in working set, extend through current dimension
                    for (int x = 0; x < dimCount; x++) {
                        for (int y = 0; y < workingSetSize; y++) {
                            twoLong = (workingSet.at(y)->elements.at(0)->name == workingSet.at(y)->elements.at(0)->adjacencies.at(2*x)->adjacencies.at(2*x)->name);
                            if (checkExistence(workingSet.at(y), 2*x + 1, method) && !twoLong) {
                                newGroup = new Group();
                                for (int z = 0; z < workingSet.at(y)->elements.size(); z++) {
                                    newGroup->Add(workingSet.at(y)->elements.at(z));
                                    workingSet.at(y)->elements.at(z)->groupsIn++;
                                    newGroup->Add(workingSet.at(y)->elements.at(z)->adjacencies.at(2*x + 1));
                                    workingSet.at(y)->elements.at(z)->adjacencies.at(2*x + 1)->groupsIn++;
                                }
                                workingSet.push_back(newGroup);
                            }
                            if (checkExistence(workingSet.at(y), 2*x, method)) {
                                newGroup = new Group();
                                for (int z = 0; z < workingSet.at(y)->elements.size(); z++) {
                                    newGroup->Add(workingSet.at(y)->elements.at(z));
                                    workingSet.at(y)->elements.at(z)->groupsIn++;
                                    newGroup->Add(workingSet.at(y)->elements.at(z)->adjacencies.at(2*x));
                                    workingSet.at(y)->elements.at(z)->adjacencies.at(2*x)->groupsIn++;
                                }
                                workingSet.push_back(newGroup);
                            }
                            if (checkExistence(workingSet.at(y), 2*x, method, "row") && !twoLong) {
                                newGroup = new Group();
                                for (int z = 0; z < workingSet.at(y)->elements.size(); z++) {
                                    //Need to fix 
                                    newGroup->Add(workingSet.at(y)->elements.at(z));
                                    workingSet.at(y)->elements.at(z)->groupsIn++;
                                    newGroup->Add(workingSet.at(y)->elements.at(z)->adjacencies.at(2*x));
                                    workingSet.at(y)->elements.at(z)->adjacencies.at(2*x)->groupsIn++;
                                    newGroup->Add(workingSet.at(y)->elements.at(z)->adjacencies.at(2*x)->adjacencies.at(2*x));
                                    workingSet.at(y)->elements.at(z)->adjacencies.at(2*x)->adjacencies.at(2*x)->groupsIn++;
                                    newGroup->Add(workingSet.at(y)->elements.at(z)->adjacencies.at(2*x)->adjacencies.at(2*x)->adjacencies.at(2*x));
                                    workingSet.at(y)->elements.at(z)->adjacencies.at(2*x)->adjacencies.at(2*x)->adjacencies.at(2*x)->groupsIn++;
                                    //Need to fix
                                }
                                workingSet.push_back(newGroup);
                            }
                        }
                        workingSetSize = workingSet.size();
                    }
                    //====================================================================
                for (int x = 0; x < workingSet.size(); x++) {
                    groups.push_back(workingSet.at(x));
                }
                workingSet.clear(); workingSetSize = 0;
                }
                curr = curr->next;
                
            }
            // printGroups(groups);
        }

        void removeUnnecessaryGroups() {
            vector<Group*> allGroups;
            int size = groups.size();
            for (int x = 0; x < size; x++) {
                allGroups.push_back(groups.at(x));
            }
            groups.clear();
            bool end = false;
            bool alreadyAdded = false;
            for (int x = 1; x > 0; x *= 2) {
                if (end) {break;}
                end = true;
                for (int y = 0; y < size; y++) {
                    if (allGroups.at(y)->elements.size() == x) {
                        alreadyAdded = false;
                        for (int z = 0; z < x; z++) {
                            if (allGroups.at(y)->elements.at(z)->groupsIn == 1 && !alreadyAdded) {
                                groups.push_back(allGroups.at(y));
                                alreadyAdded = true;
                            }
                        }
                        if (!alreadyAdded) {
                            for (int z = 0; z < x; z++) {
                                allGroups.at(y)->elements.at(z)->groupsIn--;
                            }
                        }
                        end = false;
                    }
                }
            }
            // printGroups(groups);
        }
        void getSolution() {
            int groupNum = groups.size();
            int varNum = cells.at(0)->boolVars.size();
            int groupSize;
            bool firstTerm;
            //If there are no groups, output is either always 0 or 1
            if (groups.size() == 0) {
                if (method == 1) {
                    solution = "0";
                }
                else if (method == 0) {
                    solution = "1";
                }
                return;
            }
            //If there is one group containing all the cells, the output is either always 0 or 1
            else if (groups.size() == 1 && groups.at(0)->elements.size() == cells.size()) {
                if (method == 1) {
                    solution = "1";
                }
                else if (method == 0) {
                    solution = "0";
                }
                return;
            }
            for (int x = 0; x < groupNum; x++) {
                firstTerm = true;
                if (method == 0) {
                    solution += "(";
                }
                for (int y = 0; y < varNum; y++) {
                    groupSize = groups.at(x)->elements.size();
                    for (int z = 0; z < groupSize; z++) {
                        if (groups.at(x)->elements.at(0)->boolVars.at(y)->sign != groups.at(x)->elements.at(z)->boolVars.at(y)->sign) {
                            break;
                        }
                        if (z == groupSize - 1) {
                            if (!firstTerm && method == 0) {
                                solution += " + ";
                            }
                            if ((!groups.at(x)->elements.at(z)->boolVars.at(y)->sign && method == 1) || (groups.at(x)->elements.at(z)->boolVars.at(y)->sign && method == 0)) {
                                solution += "~";
                            }
                            
                            firstTerm = false;
                            solution += groups.at(x)->elements.at(z)->boolVars.at(y)->name;
                        }
                    }
                    
                }
                if (x != groupNum - 1 && method == 1) {
                    solution += " + ";
                }
                if (method == 0) {
                    solution += ")";
                }                            
            }
        }
        void solve() {
            makeGroups();//Make all possible groups
            removeUnnecessaryGroups();//Removes unnecessary groups
            getSolution();//Uses essential groups to solve for solution
        }
        void printSolution() {
            cout << solution << endl;
        }
        string returnSolution() {
            return solution;
        }
};

string simplify(vector<string> inputs, string outputs) {
    vector<int> outputVector;
    for (int x = 0; x < outputs.size(); x++) {
        if (outputs.at(x) == '0') {
            outputVector.push_back(0);
        }
        else if (outputs.at(x) == '1') {
            outputVector.push_back(1);
        }
        else if (outputs.at(x) == '2') {
            outputVector.push_back(2);
        }
    }
    TruthTable *truthTable = new TruthTable(inputs, "f", outputVector);
    KarnaughMap *kmap = new KarnaughMap(truthTable, 1);
    kmap->solve();
    return kmap->returnSolution();
}
//kmap solver code above





//Boolean Expression solver code below
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

int eval(string expression, vector<string> inputNames, vector<bool> inputVals) {
    vector<int> inputs;
    for (int x = 0; x < inputVals.size(); x++) {
        if (inputVals.at(x)) {
            inputs.push_back(1);
        }
        else {
            inputs.push_back(0);
        }
    }
    return makeList(makeMathExp(expression, inputNames, inputs));
};
//Boolean Expression solver code above

struct State {
    string name;
    vector<int> qval;
    vector<vector<int>> outputVals;
    vector<string> outputConditions;//For use with mealy machines
    vector<string> destinationNames;
    vector<State*> destinations;
    vector<string> conditions;

    //Moore state constructor
    State(string name, vector<int> outputs, vector<string> destinationNames, vector<string> conditions) {
        this->name = name;
        outputVals = {outputs};
        outputConditions.push_back("1");
        this->destinationNames = destinationNames;
        this->conditions = conditions;
    }

    //Mealy state constructor
    State(string name, vector<vector<int>> outputs, vector<string> outputConditions, vector<string> destinationNames, vector<string> conditions) {
        this->name = name;
        outputVals = outputs;
        this->outputConditions = outputConditions;
        this->destinationNames = destinationNames;
        this->conditions = conditions;
    }
};

class BubbleDiagram {
    vector<State*> states;
    State *startState;

    public:
    BubbleDiagram(State* state) {
        startState = state;
        states.push_back(state);
    }
    void addState(State* state) {
        states.push_back(state);
    }
    void formTransitions() {
        for (int x = 0; x < states.size(); x++) {
            for (int a = 0; a < states.at(x)->destinationNames.size(); a++) {
                for (int y = 0; y < states.size(); y++) {
                    if (states.at(x)->destinationNames.at(a) == states.at(y)->name) {
                        states.at(x)->destinations.push_back(states.at(y));
                    }
                }
            }
        }
    }
    void print() {
        for (int x = 0; x < states.size(); x++) {
            for (int y = 0; y < states.at(x)->destinations.size(); y++) {
                cout << states.at(x)->name << " => " << states.at(x)->destinations.at(y)->name << " IF " << states.at(x)->conditions.at(y) << endl;
            }
        }
    }
};

int findValid(vector<string> conditions, vector<bool> inputs, vector<string> listOfInputs) {
    //Determine which condition evaluates to true given the inputs


    for (int x = 0; x < conditions.size(); x++) {
        if (eval(conditions.at(x), listOfInputs, inputs) == 1) {
            return x;
        }
    }
    return -1;
};

class StateTransitionTable {
    vector<string> inputs;//Input signal names
    vector<string> outputs;//Output signal names
    vector<State*> states;
    vector<string> ffdata;//Outputs to be kmapped into functions for flip flop inputs
    vector<string> outputData;//Truth table data for output signals to be kmapped
    vector<string> inputExpressions;
    vector<string> outputExpressions;
    public:
    int getNextStateQVal(int qNum, int stateNum, int condition) {
        vector<bool> inputValues;
        int inputNum = inputs.size();
        int posNum = pow(2, inputNum);
        int step = posNum / 2;
        bool value;
        int num;
        for (int x = 0; x < inputNum; x++) {
            value = true;
            num = -1;
            while (num < condition) {
                num += step;
                value = !value;
            }
            inputValues.push_back(value);
            step /= 2;
        }

        int result = states.at(stateNum)->destinations.at(findValid(states.at(stateNum)->conditions, inputValues, inputs))->qval.at(qNum);
        return result;

        // for (int x = 0; x < inputNum; x++) {
        //     cout << inputValues.at(x) << " " << flush;
        // }
        // // cout << endl;
        // cout << condition << endl;
    }
    void getInputDataD() {
        int ffnum = int(log2(states.size()));
        string data;
        //For every flip flop
        for (int x = 0; x < ffnum; x++) {
            data = "";
            //For every state
            for (int y = 0; y < states.size(); y++) {
                //For every possible combination of inputs
                for (int z = 0; z < pow(2, inputs.size()); z++) {
                    //If next state is a placeholder state, push back 2 (don't care) NEED TO IMPLEMENT
                    //Else push back Q(x) value of new state given transition represented by z (for)
                    data += to_string(getNextStateQVal(x, y, z));
                }
            }
            // cout << data << endl;
            ffdata.push_back(data);
        }
    }
    void getInputDataT() {
        int ffnum = ceil(log2(states.size()));
        int maxNumOfStates = pow(2, ffnum);
        string data = "";
        //For every flip flop
        for (int x = 0; x < ffnum; x++) {
            data = "";
            //For every state
            for (int y = 0; y < maxNumOfStates; y++) {
                //For every possible combination of inputs
                for (int z = 0; z < pow(2, inputs.size()); z++) {
                    try {
                        if (getNextStateQVal(x, y, z) == states.at(y)->qval.at(x)) {
                            data += "0";
                        }
                        else {
                            data += "1";
                        }
                    }
                    catch(exception e) {
                        data += 2;
                    }
                }
            }
            // cout << data << endl;
            ffdata.push_back(data);
        }
    }
    void getInputData(string ffType) {
        int ffnum = ceil(log2(states.size()));
        int maxNumOfStates = pow(2, ffnum);
        string data = "";
        string data2 = "";//Only needed for JK flip flops
        //For every flip flop
        for (int x = 0; x < ffnum; x++) {
            data = "";
            //For every state
            for (int y = 0; y < maxNumOfStates; y++) {
                //For every possible combination of inputs
                for (int z = 0; z < pow(2, inputs.size()); z++) {
                    if (ffType == "D" || ffType == "d") {
                        // cout << x << " " << y << " " << z << endl;
                        try {
                            data += to_string(getNextStateQVal(x, y, z));
                        }
                        catch (exception e) {
                            data += "2";
                        }
                    }
                    else if (ffType == "T" || ffType == "t") {
                        try {
                            if (getNextStateQVal(x, y, z) == states.at(y)->qval.at(x)) {
                                data += "0";
                            }
                            else {
                                data += "1";
                            }
                        }
                        catch (exception e) {
                            data += "2";
                        }
                    }
                    else if (ffType == "JK" || ffType == "jk") {
                        //Implementation of JKFF logic synthesis
                        try {
                            if (states.at(y)->qval.at(x) == 0 && getNextStateQVal(x, y, z) == 0) {
                                data += "0"; data2 += "2";
                            }
                            else if (states.at(y)->qval.at(x) == 0 && getNextStateQVal(x, y, z) == 1) {
                                data += "1"; data2 += "2";
                            }
                            else if (states.at(y)->qval.at(x) == 1 && getNextStateQVal(x, y, z) == 0) {
                                data += "2"; data2 += "1";
                            }
                            else if (states.at(y)->qval.at(x) == 1 && getNextStateQVal(x, y, z) == 1) {
                                data += "2"; data2 += "0";
                            }
                        }
                        catch (exception e) {
                            data += "2"; data2 += "2";
                        }
                    }
                }
            }
            ffdata.push_back(data);
            if (ffType == "JK" || ffType == "jk") {
                ffdata.push_back(data2);
            }
        }
    }

    //Method to get the value of a particular output signal
    int getOutputValue(int outputNum, int stateNum, int condition) {
        vector<bool> inputValues;
        int inputNum = inputs.size();
        int posNum = pow(2, inputNum);
        int step = posNum / 2;
        bool value;
        int num;
        for (int x = 0; x < inputNum; x++) {
            value = true;
            num = -1;
            while (num < condition) {
                num += step;
                value = !value;
            }
            inputValues.push_back(value);
            step /= 2;
        }
        int result = states.at(stateNum)->outputVals.at(findValid(states.at(stateNum)->outputConditions, inputValues, inputs)).at(outputNum);
        return result;
    }
    void getOutputData() {
        string data;
        int maxNumOfStates = pow(2, ceil(log2(states.size())));
        for (int x = 0; x < outputs.size(); x++) {  
            data = "";
            for (int y = 0; y < maxNumOfStates; y++) {
                for (int z = 0; z < states.at(0)->outputConditions.size(); z++) {
                    try {
                        //Data value exists
                        // data += to_string(states.at(y)->outputVals.at(z).at(x));
                        data += to_string(getOutputValue(x, y, z));
                    }
                    catch (exception e) {
                        //Data value was never entered by the user
                        //Assume this value doesn't matter (i.e. a don't care)
                        data += "2";
                    }
                }
            }
            outputData.push_back(data);
        }
    }
    void getInputExpressions() {
        vector<string> kmapInputs;
        int ffnum = ceil(log2(states.size()));
        // cout << ffnum << endl;
        for (int x = ffnum - 1; x >= 0; x--) {
            kmapInputs.push_back("Q" + to_string(x));
        }
        for (int x = 0; x < inputs.size(); x++) {
            kmapInputs.push_back(inputs.at(x));

        }
        for (int x = 0; x < ffdata.size(); x++) {
            inputExpressions.push_back(simplify(kmapInputs, ffdata.at(x)));
        }
    }
    void getOutputExpressions() {
        vector<string> kmapInputs;
        int ffnum = ceil(log2(states.size()));
        for (int x = ffnum - 1; x >= 0; x--) {
            kmapInputs.push_back("Q" + to_string(x));
        }
        for (int x = 0; x < inputs.size(); x++) {
            kmapInputs.push_back(inputs.at(x));
        }
        for (int x = 0; x < outputData.size(); x++) {
            outputExpressions.push_back(simplify(kmapInputs, outputData.at(x)));
        }
    }
    void printSolution(string ffType) {
        cout << "Input:" << endl;
        if (ffType == "JK" || ffType == "jk") {
            for (int x = 0; x < inputExpressions.size(); x++) {
                cout << "J" << inputExpressions.size() - 1 - x << ":\t" << inputExpressions.at(x) << endl;
                x++;
                cout << "K" << inputExpressions.size() - 1 - x + 1 << ":\t" << inputExpressions.at(x) << endl;
            }
        }
        else {
            for (int x = 0; x < inputExpressions.size(); x++) {
                cout << ffType << inputExpressions.size() - 1 - x << ":\t" << inputExpressions.at(x) << endl;
            }
        }
        cout << "Output:" << endl;
        for (int x = 0; x < outputExpressions.size(); x++) {
            cout << outputs.at(x) << ":\t" << outputExpressions.at(x) << endl;
        }
    }
    StateTransitionTable(vector<string> eninputs, vector<string> enOutputs, vector<State*> enstates) {
        inputs = eninputs;
        outputs = enOutputs;
        states = enstates;
    }

};

void make_fsm(vector<State*> states, vector<string> inputSignals, vector<string> outputSignals, string flipflip_type) {
    //Add q values to states
    int qvalnum = ceil(log2(states.size()));
    bool value = 0;
    int step = pow(2, qvalnum - 1);
    int step_reference = step;
    for (int a = 0; a < qvalnum; a++) {
        for (int x = 0; x < states.size(); x++) {
            if (step > 0) {
                step--;
            }
            else {
                step = step_reference - 1;
                value = !value;
            }
            states.at(x)->qval.push_back(value);
        }
        step = step_reference;
        step /= 2;
        step_reference /= 2;
        value = 0;
    }
    BubbleDiagram *bd = new BubbleDiagram(states.at(0));
    for (int x = 1; x < states.size(); x++) {
        bd->addState(states.at(x));
    }
    bd->formTransitions();
    StateTransitionTable *stt = new StateTransitionTable(inputSignals, outputSignals, states);
    stt->getInputData(flipflip_type);
    stt->getInputExpressions();
    stt->getOutputData();
    stt->getOutputExpressions();
    stt->printSolution(flipflip_type);
}

int main() {
    //Instructions====================================================================
    //Define each state:
        //Can be either mealy or moore style
        //For moore, simply add state name, output signal values (in same order as output signal names entered in make_fsm), next states, and the conditions to go to the next states (order dependant)
        //For mealy, include output values for each desired possibility of input values
    //Call 'make_fsm' on your states, input signal names, output signal names, and desired type of flip flop
    //Desired order of input and output signal names (i.e. to use binary to represent letters or numbers for ex) must line up
    //Instructions====================================================================
    //State argument format: (name), (output signals), (destinations), (transition conditions)
    //make_fsm arguments format: (states), (input signals), (output signals), (type of flip flop i.e. D, T, or JK)
    //Related arguments (ex. destinations and transition conditions) must appear in the same order when passed in as vectors
    //Example: basic up counter 
    State *a = new State("a", {0, 0, 0}, {"b"}, {"ab"});
    State *b = new State("b", {0, 0, 1}, {"c"}, {"1"});
    State *c = new State("c", {0, 1, 0}, {"e"}, {"1"});
    State *d = new State("d", {0, 1, 1}, {"e"}, {"1"});
    State *e = new State("e", {1, 0, 0}, {"f"}, {"1"});
    State *f = new State("f", {1, 0, 1}, {"g"}, {"1"});
    State *g = new State("g", {1, 1, 0}, {"h"}, {"1"});
    State *h = new State("h", {1, 1, 1}, {"a"}, {"1"});
    make_fsm({a, b, c, e, f, g, h}, {}, {"Z2", "Z1", "Z0"}, "D");

    //Example: mealy machine
    // State *x = new State("a", {{0}, {0}}, {"~in", "in"}, {"b"}, {"1"});
    // State *y = new State("b", {{0}, {0}}, {"~in", "in"}, {"c"}, {"1"});
    // State *z = new State("c", {{0}, {1}}, {"~in", "in"}, {"a"}, {"1"});
    // make_fsm({x, y, z}, {"in"}, {"Z0"}, "D");
};