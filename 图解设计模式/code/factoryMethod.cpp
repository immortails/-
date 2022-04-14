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
class Product {
public:
	virtual void use()=0;
};


//采用template method模式
class Factory {
public:
	//create函数是生成逻辑
	Product* create(string owner) {
		Product* p = createProject(owner);
		registerProject(p);
		return p;
	}
	//资源创建交给派生类去干
	virtual Product* createProject(string owner) = 0;
	virtual void registerProject(Product* p) = 0;
};

class IDcard: public Product{
private:
	string owner;
public:
	IDcard(string _owner) :owner(_owner) {
		cout << "制作" << owner << "的ID卡。" << endl;
	};

	void use() {
		cout << "使用" << owner << "的ID卡。" << endl;
	}
	string getOwner(){
		return owner;
	}
};

class IDcardFactory :public Factory {
private:
	deque<string> owners;
public:
	Product* createProject(string owner) {
		Product* p = new IDcard(owner);
		return p;
	}
protected:
	void registerProject(Product* product) {
		IDcard* card = dynamic_cast<IDcard*>(product);
		owners.emplace_back(card->getOwner());
	}
};
int main() {
	Factory* factory = new IDcardFactory();
	Product* card1 = factory->create("harry");
	Product* card2 = factory->create("potter");
	Product* card3 = factory->create("amy");
	card1->use();
	card2->use();
	card3->use();
	return 0;
}