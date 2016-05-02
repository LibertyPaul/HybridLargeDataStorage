#include "HybridLargeDataStorage.hpp"

#include <iostream>
#include <list>
#include <cassert>
#include <initializer_list>
#include <boost/optional.hpp>

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

	const boost::optional<Value &> res = hlds.getValue(key1);
	assert(res.get() == value1);


	Key key2 = {
		{KeyItem(Alphabet::T)},
		{KeyItem(Alphabet::T)},
		{KeyItem(Alphabet::G)},
		{KeyItem(Alphabet::C)}
	};

	const boost::optional<Value &> invRes = hlds.getValue(key2);

	assert(!invRes);


	hlds.insert(key2, 100);

	boost::optional<Value &> res1 = hlds.getValue(key1);
	boost::optional<Value &> res2 = hlds.getValue(key2);
	assert(res1.get() != res2.get());

	res1.get() = 200;
	res2.get() = 200;

	assert(hlds.getValue(key1).get() == 200);
	assert(hlds.getValue(key2).get() == 200);


}

