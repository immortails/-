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
#include<map>
using namespace std;
class Builder {
public:
	virtual void makeTitle(string title)=0;
	virtual void makeString(string str) = 0;
	virtual void makeItems(vector<string> items) = 0;
	virtual void close()=0;

};

class Director {
private:
	Builder* builder;
public:
	Director(Builder* _builder) :builder(_builder) {};
	void construct() {
		builder->makeTitle("Greeting");
		builder->makeString("从早上到晚上");
		builder->makeItems({ "晚上好","晚安","再见" });
		builder->close();
	}
};

class TextBuilder :public Builder {
private:
	string buff;
public:
	void makeTitle(string title) {
		buff+="==============\n";
		buff+="[" + title + "]\n";
		buff += "\n";
	}
	void makeString(string str) {
		buff+="_" + str;
		buff += "\n";
	}

	void makeItems(vector<string> items) {
		for (int i = 0; i < items.size(); i++) {
			buff+=" ." + items[i]+"\n";
		}
	}

	void close() {
		buff+="===============";
	}
	string getResult() {
		return buff;
	}
};

int main() {
	TextBuilder* b1 = new TextBuilder();
	Director* d1 = new Director(b1);
	d1->construct();
	string res = b1->getResult();
	cout << res << endl;
	return 0;
}