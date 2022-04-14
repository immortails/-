#include<iostream>
#include<vector>
#include<algorithm>
#include<map>
#include<deque>
#include<queue>
#include<set>
#include<limits>
#include<string>
#include<unordered_map>
using namespace std;

class Product {
public:
	virtual void use(string s) = 0;
	virtual Product* createClone() = 0;
};

class Manager {
private:
	unordered_map<string, Product*> showcase;		//记录所有的case的
public:
	void Register(string name, Product* proto) {
		showcase[name] = proto;
	}
	Product* create(string protoname) {				//工厂模式来复制实例
		Product* p = showcase[protoname];
		return p->createClone();
	}
};

class messageBox:public Product {
private:
	char decochar;
public:
	messageBox(char _decochar):decochar(_decochar) {}
	messageBox(messageBox& _data) :decochar(_data.decochar) {
		cout << "Prototype copy!" << endl;
	}
	void use(string s) {
		int len = s.size();
		for (int i = 0; i < len + 4; i++) {
			cout << decochar;
		}
		cout << endl;
		cout << decochar << " " << s<<" " << decochar << endl;
		for (int i = 0; i < len + 4; i++) {
			cout << decochar;
		}
		cout << endl;
	}
	Product* createClone() {					//调用拷贝构造函数来进行复制
		return new messageBox(*this);
	}

};

int main() {
	Manager* mang = new Manager();
	messageBox* msgBox = new messageBox('*');
	mang->Register("messageBox", msgBox);
	Product* a=mang->create("messageBox");
	a->use("123");
	return 0;
}

