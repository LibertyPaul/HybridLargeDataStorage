#ifndef HLDSITERATOR_HPP
#define HLDSITERATOR_HPP

#include "HybridLargeDataStorage.hpp"
#include "HeadsIterator.hpp"

#include <utility>

template<typename Key, typename Value>
class HybridLargeDataStorage;

template<typename Key, typename Value>
class HLDSIterator{
	typename HeadsHolder<Key, Value>::iterator headsIterator;
	const typename HeadsHolder<Key, Value>::iterator headsEnd;
	typename TailTree<Key, Value>::iterator tailIterator;
	const typename TailTree<Key, Value>::iterator tailEnd;

	bool isSet;

protected:
	bool isValid() const{
		return isSet && this->headsIterator != this->headsEnd && this->tailIterator != this->tailEnd;
	}

public:
	HLDSIterator(): isSet(false){}

	HLDSIterator(
		const typename HeadsHolder<Key, Value>::iterator headsIterator,
		const typename HeadsHolder<Key, Value>::iterator headsEnd,
		const typename TailTree<Key, Value>::iterator tailIterator,
		const typename TailTree<Key, Value>::iterator tailEnd
	): headsIterator(std::move(headsIterator)), headsEnd(std::move(headsEnd)), tailIterator(std::move(tailIterator)), tailEnd(std::move(tailEnd)), isSet(true)
	{

	}

	operator bool() const{
		return this->tailIterator == this->tailEnd;
	}

	bool operator==(const HLDSIterator &o) const{
		if(this->isValid() == false){
			return !o.isValid();
		}

		return this->headsIterator == o.headsIterator && this->tailIterator == o.tailIterator;
	}

	Value &operator*(){
		return *this->tailIterator;
	}

	const Value &operator*() const{
		return *this->tailIterator;
	}

	Key getKey() const{
		return this->headsIterator.getKey() + this->tailIterator.getKey();
	}

	HLDSIterator &operator++(){
		++this->tailIterator;
		while(++this->headsIterator != this->headsEnd && this->tailIterator == this->headsIterator->end()){
			 this->tailIterator = this->headsIterator->begin();
		}

		if(this->headsIterator == this->headsEnd){
			this->tailIterator = this->tailEnd;
		}

		return *this;
	}
};

#endif // HLDSITERATOR_HPP
