#include <iostream>

using namespace std;

struct GrandClass {
    float q;
    bool a;
    bool f;
    bool d;
};

struct ParentClass : public GrandClass{

};

struct SonClass1 : public ParentClass{

};

struct SonClass2 : public ParentClass{

};

int main () {
    cout << sizeof(GrandClass) << endl;
    return 0;
}