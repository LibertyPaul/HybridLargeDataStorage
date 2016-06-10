#ifndef HEADSITERATOR_HPP
#define HEADSITERATOR_HPP

#include "HybridLargeDataStorage.hpp"
#include "HeadsHolder.hpp"

template<typename Key, typename Value>
class HeadsHolder;

template<typename Key, typename Value>
class HeadsIterator : protected HeadsContainer<Key, Value>::iterator{
	Key headsKey;

public:
	explicit HeadsIterator(){}

	HeadsIterator(typename HeadsContainer<Key, Value>::iterator it, Key headsKey):
		HeadsContainer<Key, Value>::iterator(std::move(it)),
		headsKey(std::move(headsKey))
	{

	}

	HeadsIterator(const HeadsIterator &headsIterator):
		HeadsContainer<Key, Value>::iterator(headsIterator),
		headsKey(headsIterator.headsKey)
	{

	}

	HeadsIterator(HeadsIterator &&headsIterator):
		HeadsContainer<Key, Value>::iterator(headsIterator),
		headsKey(std::move(headsIterator.headsKey))
	{

	}

	const Key &getKey() const{
		return this->headsKey;
	}

	HeadsIterator &operator=(HeadsIterator headsIterator){
		this->headsKey = std::move(headsIterator.headsKey);

		typename HeadsContainer<Key, Value>::iterator &base = *this;
		base = std::move(headsIterator);

		return *this;
	}

	HeadsIterator &operator++(){
		++headsKey;

		typename HeadsContainer<Key, Value>::iterator &base = *this;
		++base;

		return *this;
	}

	HeadsIterator operator+(const size_t distance) const{
		HeadsIterator result(*this);
		result.headsKey += distance;

		typename HeadsContainer<Key, Value>::iterator &base = result;
		base += distance;

		return result;
	}

	bool operator==(const HeadsIterator &o) const{
		const typename HeadsContainer<Key, Value>::iterator &thisRef = *this;
		const typename HeadsContainer<Key, Value>::iterator &oRef = o;
		return thisRef == oRef;
	}

	bool operator!=(const HeadsIterator &o) const{
		return !(*this == o);
	}

	typename HeadsHolder<Key, Value>::value_type &operator*(){
		typename HeadsContainer<Key, Value>::iterator &base = *this;
		return *base;
	}

	const typename HeadsHolder<Key, Value>::value_type &operator*() const{
		const typename HeadsContainer<Key, Value>::iterator &base = *this;
		return *base;
	}

	typename HeadsHolder<Key, Value>::value_type *operator->(){
		typename HeadsContainer<Key, Value>::iterator &base = *this;
		return &*base;
	}

	const typename HeadsHolder<Key, Value>::value_type *operator->() const{
		const typename HeadsContainer<Key, Value>::iterator &base = *this;
		return &*base;
	}

};

#endif // HEADSITERATOR_HPP

