#ifndef KEY_HPP
#define KEY_HPP

#include <vector>
#include <string>


template<typename KeyItem>
class Key_ : public std::vector<KeyItem>{
public:

	Key_(){}
	Key_(Key_ &&key): std::vector<KeyItem>(static_cast<std::vector<KeyItem> &&>(key)){}
	Key_(const Key_ &key): std::vector<KeyItem>(key){}
	Key_(const size_t size): std::vector<KeyItem>(size, KeyItem(KeyItem::min_value)){}
	Key_(std::initializer_list<KeyItem> il): std::vector<KeyItem>(il){}

	void resize(const size_t size, const typename KeyItem::value_type &fillValue = KeyItem::min_value){
		std::vector<KeyItem> &base = *this;
		base.resize(size, KeyItem(fillValue));
	}

	size_t toIndex() const{
		size_t result = 0;
		size_t multiplier = 1;

		for(auto it = this->crbegin(); it != this->crend(); ++it){
			result += it->toIndex() * multiplier;
			multiplier *= KeyItem::alphabetSize;
		}

		return result;
	}

	static Key_ fromIndex(size_t index, const size_t size){
		Key_ resultReversed;
		resultReversed.reserve(size);

		while(index != 0){
			const size_t currentIndex = index % KeyItem::alphabetSize;
			index /= KeyItem::alphabetSize;
			const KeyItem currentKeyItem = KeyItem::fromIndex(currentIndex);
			resultReversed.push_back(currentKeyItem);
		}

		if(resultReversed.size() > size){
			throw std::overflow_error("");
		}

		if(resultReversed.size() < size){
			resultReversed.resize(size, KeyItem::min_value);
		}

		Key_ result;
		result.resize(resultReversed.size());

		std::copy(resultReversed.crbegin(), resultReversed.crend(), result.begin());

		return result;
	}

	std::string toString() const{
		std::string result;
		result.reserve(this->size());

		for(const KeyItem &value : *this){
			result.push_back(value.toSymbol());
		}

		return result;
	}

	static Key_ fromString(const std::string &str){
		Key_ result;
		result.reserve(str.size());

		for(const char symbol : str){
			result.push_back(KeyItem::fromSymbol(symbol));
		}

		return result;
	}

	Key_ &operator=(Key_ key){
		std::vector<KeyItem> &base1 = *this;
		std::vector<KeyItem> &base2 = key;

		base1 = std::move(base2);

		return *this;
	}

	Key_ &operator++(){
		auto it = this->rbegin();
		while(it != this->rend()){
			if(*it == KeyItem::max_value){
				*it = KeyItem(KeyItem::min_value);
			}
			else{
				++*it;
				break;
			}

			++it;
		}

		if(it == this->rend()){
			std::fill(this->begin(), this->end(), KeyItem(KeyItem::min_value));
		}

		return *this;
	}

	void push_back(KeyItem value){
		std::vector<KeyItem> &base1 = *this;
		base1.push_back(std::move(value));
	}

	Key_ operator+(const Key_ &key) const{
		Key_ result(*this);
		result.reserve(this->size() + key.size());

		for(const auto val : key){
			result.push_back(val);
		}

		return result;
	}
};

template<typename KeyItem_>
bool operator<(const Key_<KeyItem_> &lhs, const Key_<KeyItem_> &rhs){
	assert(lhs.size() == rhs.size());

	for(size_t i = 0; i < lhs.size(); ++i){
		if(lhs[i] != rhs[i]){
			return lhs[i] < rhs[i];
		}
	}

	return false;
}

#endif // KEY_HPP

