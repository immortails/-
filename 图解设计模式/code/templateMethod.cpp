#include<iostream>
#include<vector>
#include<algorithm>
#include<map>
#include<queue>
#include<set>
#include<limits>
#include<string>
using namespace std;
class AbstractDisplay {
public:
	virtual void open() = 0;
	virtual void print() = 0;
	virtual void close() = 0;
	void display() {
		open();
		for (int i = 0; i < 5; i++) print();
		close();
	}
};

class charDisplay:public AbstractDisplay {
public:
	char ch;
	charDisplay(char _ch):ch(_ch) {

	}
	void open() override {
		cout << "<<" << endl;
	}
	void print() override {
		cout << ch << endl;
	}
	void close() override {
		cout << ">>" << endl;
	}
};

class stringDisplay :public AbstractDisplay {
public:
	string str;
	int width;
	stringDisplay(string _str) :str(_str) {
		width = str.size();
	}
	void open() override {
		printLine();
	}
	void print() {
		cout << "|" << str << "|" << endl;
	}
	void close() {
		printLine();
	}
private:
	void printLine() {
		cout << "+";
		for (int i = 0; i < width; i++) {
			cout << "-";
		}
		cout << "+"<<endl;
	}
};

int main() {
	AbstractDisplay* ab = new charDisplay('H');
	ab->display();
	AbstractDisplay* ab1 = new stringDisplay("123");
	ab1->display();
	return 0;
}