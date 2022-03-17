#include<iostream>
#include<vector>
#include<algorithm>
#include<map>
#include<queue>
#include<set>
#include<limits>
#include<string>
class BookShelf;
template<typename T>
class Iterator {
public:
	virtual bool hasNext() = 0;
	virtual T* next() = 0;
};


//这里加模板是因为这里是一个抽象的基类，我不知道元素是什么
template<typename T>
class Aggregate {
public:
	virtual Iterator<T>* iterator() = 0;
};

//书，也就是一个元素
class Book {
private:
	std::string name;
public:
	Book(std::string _name) :name(_name) {

	}
	std::string getName() {
		return name;
	}
};

//这里由于iterator要返回，所以直接上模板，返回对应的元素
template<typename T>
class BookShelfIterator :public Iterator<T> {
private:
	BookShelf* bookShelf;
	int idx;
public:
	BookShelfIterator(BookShelf* _bookShelf) :bookShelf(_bookShelf), idx(0) {}
	bool hasNext() {
		if (idx < bookShelf->size()) {
			return true;
		}
		else {
			return false;
		}
	}

	T* next() {
		T* book = bookShelf->getBookAt(idx);
		idx++;
		return book;
	}
};

//书架，这里明确了元素就是书，所以这一步无需模板
class BookShelf : public Aggregate<Book> {
private:
	std::vector<Book*> books;
public:
	BookShelf() {

	};
	Book* getBookAt(int idx) {
		return books[idx];
	}

	void appendBook(Book* book) {
		books.emplace_back(book);
	}
	int size() {
		return books.size();
	}
	Iterator<Book>* iterator() {
		Iterator<Book>* it = new BookShelfIterator<Book>(this);
		return it;
	}
};

int main() {
	BookShelf* bookShelf = new BookShelf();
	bookShelf->appendBook(new Book("Bible"));
	bookShelf->appendBook(new Book("Cinderlla"));
	bookShelf->appendBook(new Book("harry potter"));
	Iterator<Book>* it = bookShelf->iterator();
	while (it->hasNext()) {
		Book* book = it->next();
		std::cout << book->getName() << std::endl;
	}
	return 0;
}


