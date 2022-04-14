#include<iostream>
#include<vector>
#include<algorithm>
#include<map>
#include<deque>
#include<queue>
#include<set>
#include<limits>
#include<string>
using namespace std;


//饿汉模式，不管用不用，先创建一个对象
class A {
private:
	A() {};
	static A a;
public:
	static A* getInstance() {
		return &a;
	}

	void print() {
		cout << "word" << endl;
	}
};

//懒汉模式,调用时再创建这个对象
class B {
private:
	B() {};
	static B* b;
public:
	void print() {
		cout << "OK" << endl;
	}
	static B* getInstance() {
		//因为在类中所以拥有访问私有作用域的权限，而且构造函数是可以直接调用的。
		if (b == nullptr) b = new B();
	}
};

//这里需要声明
A A::a;
B* B::b;
int main() {
	A::getInstance()->print();
	return 0;
}
