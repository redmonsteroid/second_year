#include <iostream> // 1
#include <string>

using namespace std;

struct StackNode {
    int data;
    StackNode* next;
    StackNode(int val) : data(val), next(nullptr) {}
};

struct Stack {
    StackNode* topNode;

    Stack() : topNode(nullptr) {}

    ~Stack() {
        while (!isEmpty()) pop();
    }

    void push(int value) {
        StackNode* newNode = new StackNode(value);
        newNode->next = topNode;
        topNode = newNode;
    }

    void pop() {
        if (isEmpty()) {
            cout << "error: stack underflow" << endl;
            exit(1);  // Завершение программы при ошибке
        }
        StackNode* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }

    int top() const {
        if (isEmpty()) {
            cout << "error: stack is empty" << endl;
            exit(1);  
        }
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }
};

//  арифметические операции между двумя числами
int applyOperation(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        default:
            cout << "error: invalid operation '" << op << "'" << endl;
            exit(1); 
    }
}

// приоритет
int precedence(char op) {
    if (op == '+' || op == '-') return 1;  
    if (op == '*') return 2;               
    return 0;
}

// Выполняет операцию из стека операторов над двумя верхними числами из стека значений
void performOperation(Stack& values, Stack& ops) {
    // Проверка наличия двух операндов для операции
    if (values.isEmpty() || values.topNode->next == nullptr) {
        cout << "error: insufficient operands for operation" << endl;
        exit(1);
    }
    int b = values.top(); values.pop(); // сначала второй опперанд потому что позде первого, но второй вершина стэка
    int a = values.top(); values.pop(); 
    char op = ops.top(); ops.pop();     
    values.push(applyOperation(a, b, op)); // Результат помещаем обратно в стек
}

// вычисление значения выражения
int evaluate(const string& expression) {
    Stack values; // числа
    Stack ops;    // оппер

    for (size_t i = 0; i < expression.length(); i++) {
        char ch = expression[i];

        if (ch == ' ') continue;

        
        if (isdigit(ch)) { // цифра
            int value = 0;
            while (i < expression.length() && isdigit(expression[i])) {
                value = value * 10 + (expression[i] - '0'); // Считываем многозначное число
                i++;
            }
            i--; // символ после числа
            values.push(value); // Добавляем число в стек значений
        }
        else if (ch == '(') {
            ops.push(ch); // Добавляем открывающую скобку в стек операторов
        }
        else if (ch == ')') {
            
            while (!ops.isEmpty() && ops.top() != '(') {
                performOperation(values, ops);
            }
            if (!ops.isEmpty()) ops.pop(); // Убираем ( из стека 
        }
        else if (ch == '+' || ch == '-' || ch == '*') {
            // Проверяем, что после оператора следует число или открывающая скобка
            if (i == expression.length() - 1 || (!isdigit(expression[i + 1]) && expression[i + 1] != '(')) {
                cout << "error: incomplete expression after '" << ch << "'" << endl;
                exit(1);
            }
            // Выполняем операции, пока текущий оператор имеет меньший приоритет
            while (!ops.isEmpty() && precedence(ops.top()) >= precedence(ch)) {
                performOperation(values, ops);
            }
            ops.push(ch); // Добавляем текущий оператор в стек операторов
        }
        else {
            // Если символ некорректный
            cout << "error: invalid character " << ch << endl;
            exit(1);
        }
    }

    // Выполняем оставшиеся операции в стеке операторов
    while (!ops.isEmpty()) {
        performOperation(values, ops);
    }

    // Проверяем, что в стеке остался один результат
    if (!values.isEmpty() && values.topNode->next == nullptr) {
        return values.top(); 
    } else {
        cout << "error: expression could not be evaluated" << endl;
        exit(1);
    }
}

int main() {
    string expression;
    cout << "Enter an expression: ";
    getline(cin, expression);

    int result = evaluate(expression);
    cout << "Result: " << result << endl;
    return 0;
}
