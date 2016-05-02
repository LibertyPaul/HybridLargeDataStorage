#ifndef BASENODE_HPP
#define BASENODE_HPP

template<typename Key, typename Value>
class BaseNode{
public:
	BaseNode(){}
	virtual ~BaseNode(){}

	virtual void addTail(Key &&key, const Value &value) = 0;
	virtual void addTail(const Key &key, const Value &value) = 0;


	virtual Value getValue(Key &&key) const = 0;
	virtual Value getValue(const Key &key) const = 0;
};

#endif // BASENODE_HPP

