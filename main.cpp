#include "HybridLargeDataStorage.hpp"

#include <iostream>
#include <list>
#include <cassert>
#include <initializer_list>

using namespace std;


enum class Alphabet{
	A = 0, T, G, C
};

class KeyItem{
	Alphabet index;
public:
	explicit KeyItem(const Alphabet index): index(index){}

	size_t toIndex() const{
		return static_cast<size_t>(index);
	}
};

class Key : public std::vector<KeyItem>{
public:
	static constexpr size_t alphabetSize = 4;

	Key(std::initializer_list<KeyItem> il): std::vector<KeyItem>(il){}


	KeyItem at(const size_t index) const{
		assert(index < this->size());
		return static_cast<const std::vector<KeyItem> *>(this)->at(index);
	}

	void pop_front(){
		this->erase(this->begin());
	}

	size_t size() const{
		return static_cast<const std::vector<KeyItem> *>(this)->size();
	}

	bool empty() const{
		return static_cast<const std::vector<KeyItem> *>(this)->empty();
	}
};

typedef uint64_t Value;

int main(){
	HybridLargeDataStorage<Key, Value> hlds(2, 2);
	Key key1 = {
		{KeyItem(Alphabet::A)},
		{KeyItem(Alphabet::A)},
		{KeyItem(Alphabet::A)},
		{KeyItem(Alphabet::A)}
	};
	Value value1 = 42;

	hlds.insert(key1, value1);

	const Value res = hlds.getValue(key1);
	assert(res == value1);



}

