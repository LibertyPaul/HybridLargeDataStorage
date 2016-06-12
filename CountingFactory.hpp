#ifndef COUNTINGFACTORY_HPP
#define COUNTINGFACTORY_HPP

#include <cstddef>
#include <utility>

template<typename Product>
class CountingFactory{
	mutable size_t objectCount = 0;

public:
	template<typename...  ConstructorArgs>
	Product *create(ConstructorArgs... args) const{
		Product *product = new Product(std::forward<ConstructorArgs>(args)...);

		++this->objectCount;

		return product;
	}

	size_t producedItemsCount() const{
		return this->objectCount;
	}

	void reset(){
		objectCount = 0;
	}
};

#endif // COUNTINGFACTORY_HPP

