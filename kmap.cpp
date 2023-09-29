//3rd prototype version of a program to utilize Karnaugh Maps to simplify truth tables or boolean expressions
//Desired functionality: can accept a user-entered boolean expression, truth table, or K-map with an unlimited number of variables
//Simplifies to SOP or POS form

#include <string>
#include <vector>
#include <iostream>
#include <math.h>
using namespace std;

//Program components listed below:

//A name and a sign representing a boolean variable in the input data
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
        // Cell *curr = first;
        // while(curr != nullptr) {
        //     cout << curr->name << flush;
        //     for (int x = 0; x < curr->boolVars.size(); x++) {
        //         curr->boolVars.at(x)->print();
        //     }
        //     cout << endl;
        //     curr = curr->next;
        // }
        // 
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
                    //New solution========================================================
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
};

//A collection of cells to form the kmap

//Program components listed above:

int main(int argc, char* argv[]) {
    //Instructions:  Enter truth table in command lne arguments
    //Format: (form) ("tt" for truth table) (list of variables) ('f' for function) (lis tout outputs if inputs are in ascending binary order)
    //Ex: for the following
    // a b c f
    // 0 0 0 1
    // 0 0 1 0
    // 0 1 0 0
    // 0 1 1 0
    // 1 0 0 1
    // 1 0 1 1
    // 1 1 0 1
    // 1 1 1 0
    //Command line arguments to execute and solve in SOP format would be:
    // ./a.exe sop tt a b c f 1 0 0 0 1 1 1 0

    //Find desired simplification method (SOP or POS)
    //Build truth table
    //Make k-map out of truth table
    //Solve k-map to find solution
    //Print solutiuon

    //Establish desired method (1 means SOP, 2 means POS)
    int method;
    if ((argv[1][0] == 's' || argv[1][0] == 'S') && (argv[1][1] == 'o' || argv[1][1] == 'O') && (argv[1][2] == 'p' || argv[1][2] == 'P')) {
        method = 1;
    }
    if ((argv[1][0] == 'p' || argv[1][0] == 'P') && (argv[1][1] == 'o' || argv[1][1] == 'O') && (argv[1][2] == 's' || argv[1][2] == 'S')) {
        method = 0;
    }
    
    //Build truth table out of either entered values or expression
    TruthTable *truthTable;

    //Build truth table from entered variables and outputs if prompted
    if (((argv[2][0] == 't' || argv[2][0] == 'T') && (argv[2][1] == 't' || argv[2][1] == 'T'))) {
    vector<string> inputNames;
    vector<int> outputs;
    bool passed = false;
    for (int x = 3; x < argc; x++) {
        if (passed) {
            if (*argv[x] == '1') {
                outputs.push_back(1);
            }
            else if (*argv[x] == 'd') {
                outputs.push_back(2);
            }
            else {
                outputs.push_back(0);
            }
        }
        else if (*argv[x] != 'f') {

            inputNames.push_back(argv[x]);
        }
        else {
            passed = true;
        }
    }
        truthTable = new TruthTable(inputNames, "f", outputs);
    }
    
    //Build truth table from boolean expression if entered
    if (((argv[2][0] == 'e' || argv[2][0] == 'E') && (argv[2][1] == 'x' || argv[2][1] == 'X'))) {
        string expression = "";
        for (int x = 3; x < argc; x++) {
            expression += argv[x];
        }
        cout << expression << endl;
        truthTable = new TruthTable(expression);
    }

    //Form kmap from truth table
    KarnaughMap *kmap = new KarnaughMap(truthTable, method);

    //Solve kmap
    kmap->solve();

    //Print solution
    kmap->printSolution();
};