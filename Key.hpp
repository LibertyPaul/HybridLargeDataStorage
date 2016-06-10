#ifndef KEY_HPP
#define KEY_HPP

#include <vector>

enum class Alphabet{
	A = 0, T, G, C, N
};

class KeyItem{
	Alphabet index;
public:
	typedef Alphabet value_type;
	typedef size_t index_t;
	static constexpr size_t alphabetSize = 5;
	static constexpr Alphabet min_value = Alphabet::A;
	static constexpr Alphabet max_value = Alphabet::N;

	explicit KeyItem(const Alphabet index): index(index){}

	size_t toIndex() const{
		return static_cast<size_t>(index);
	}

	static KeyItem fromIndex(const size_t index){
		if(index > static_cast<size_t>(max_value)){
			throw std::overflow_error("");
		}

		return KeyItem(static_cast<Alphabet>(index));
	}

	KeyItem &operator=(const KeyItem &keyItem){
		this->index = keyItem.index;
		return *this;
	}

	bool operator==(const Alphabet &alphabet) const{
		return this->index == alphabet;
	}

	bool operator==(const KeyItem &o) const{
		return *this == o.index;
	}

	bool operator!=(const KeyItem &o) const{
		return !(*this == o);
	}

	bool operator<(const KeyItem &o) const{
		return this->index < o.index;
	}

	KeyItem &operator++(){
		*this = KeyItem::fromIndex(this->toIndex() + 1);
		return *this;
	}
};

constexpr size_t KeyItem::alphabetSize;
constexpr Alphabet KeyItem::min_value;
constexpr Alphabet KeyItem::max_value;

class Key : public std::vector<KeyItem>{
public:

	Key(){}
	Key(Key &&key): std::vector<KeyItem>(static_cast<std::vector<KeyItem> &&>(key)){}
	Key(const Key &key): std::vector<KeyItem>(key){}
	Key(const size_t size): std::vector<KeyItem>(size, KeyItem(value_type::min_value)){}
	Key(std::initializer_list<KeyItem> il): std::vector<KeyItem>(il){}

	void resize(const size_t size, const value_type::value_type &fillValue = value_type::min_value){
		std::vector<KeyItem> &base = *this;
		base.resize(size, KeyItem(fillValue));
	}

	size_t toIndex() const{
		size_t result = 0;
		size_t multiplier = 1;

		for(auto it = this->crbegin(); it != this->crend(); ++it){
			result += it->toIndex() * multiplier;
			multiplier *= value_type::alphabetSize;
		}

		return result;
	}

	static Key fromIndex(size_t index, const size_t size){
		Key resultReversed;
		resultReversed.reserve(size);

		while(index != 0){
			const size_t currentIndex = index % value_type::alphabetSize;
			index /= value_type::alphabetSize;
			const KeyItem currentKeyItem = KeyItem::fromIndex(currentIndex);
			resultReversed.push_back(currentKeyItem);
		}

		if(resultReversed.size() > size){
			throw std::overflow_error("");
		}

		if(resultReversed.size() < size){
			resultReversed.resize(size, value_type::min_value);
		}

		Key result;
		result.resize(resultReversed.size());

		std::copy(resultReversed.crbegin(), resultReversed.crend(), result.begin());

		return result;
	}

	Key &operator=(Key key){
		std::vector<KeyItem> &base1 = *this;
		std::vector<KeyItem> &base2 = key;

		base1 = std::move(base2);

		return *this;
	}

	Key &operator++(){
		Key keyCopy = *this;
		auto it = keyCopy.rbegin();
		while(it != keyCopy.rend()){
			if(*it == value_type::max_value){
				*it = KeyItem(value_type::min_value);
			}
			else{
				++*it;
				break;
			}

			++it;
		}

		if(it == keyCopy.rend()){
			throw std::overflow_error("");
		}

		*this = std::move(keyCopy);

		return *this;
	}

	void push_back(value_type value){
		std::vector<KeyItem> &base1 = *this;
		base1.push_back(std::move(value));
	}

	Key operator+(const Key &key) const{
		Key result(*this);
		result.reserve(this->size() + key.size());

		for(const auto val : key){
			result.push_back(val);
		}

		return result;
	}
};

bool operator<(const Key &lhs, const Key &rhs){
	assert(lhs.size() == rhs.size());

	for(size_t i = 0; i < lhs.size(); ++i){
		if(lhs[i] != rhs[i]){
			return lhs[i] < rhs[i];
		}
	}

	return false;
}

#endif // KEY_HPP

