#include<iostream>
#include<vector>
#include<algorithm>
#include<map>
#include<queue>
#include<set>
#include<limits>
#include<string>
using namespace std;
class Banner {
private:
    string str;
public:
    Banner(string _str):str(_str){}
    void showWithParen() {
        cout << "(" << str << ")" << endl;
    }
    void showWithAster() {
        cout << "*" << str << "*" << endl;
    }
};


//提供一个打印类的接口，真实的需求
class Print{
public:
    Print(){}
    virtual void printWeak() = 0;
    virtual void printStrong() = 0;
};

//一个适配器，将Banner转换成print
class PrintBanner:public Print {
private:
    Banner ban;
public:
    PrintBanner(string _str):ban(_str){

    }
    void printWeak() override {
        ban.showWithAster();
    }
    void printStrong() override {
        ban.showWithParen();
    }
};


int main() {
    Print* p = new PrintBanner("Hello");
    p->printWeak();
    p->printStrong();
    return 0;
}
