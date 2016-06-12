#ifndef TAILTREEITERATOR_HPP
#define TAILTREEITERATOR_HPP

#include "BaseNode.hpp"
#include "Node.hpp"
#include "ValueNode.hpp"
#include "TailTree.hpp"
#include "BranchHolder.hpp"

#include <vector>
#include <cassert>
#include <stdexcept>

template<typename Key, typename Value>
class BaseNode;

template<typename Key, typename Value>
class Node;

template<typename Key, typename Value>
class TailTree;

template<typename Key, typename Value>
class TailTreeIterator{
	Key tailKey;
	BranchHolder<Key, Value> branch;

protected:
	BaseNode<Key, Value> *root() const{
		return this->currentChain.front();
	}

private:
	TailTreeIterator(Key tailKey, BranchHolder<Key, Value> branch): tailKey(std::move(tailKey)), branch(std::move(branch)){
		assert(this->tailKey.size() + 1 == this->branch.size());

		for(size_t i = 0; i < this->branch.size() - 1; ++i){
			const Node<Key, Value> *currentNode = dynamic_cast<const Node<Key, Value> *>(this->branch.at(i));
			assert(currentNode != nullptr);

			const typename Key::value_type &currentKey = this->tailKey.at(i);

			assert(currentNode->tails.at(currentKey.toIndex()) == this->branch.at(i + 1));
		}
	}

	bool isValid() const{
		return branch.complete();
	}

public:
	TailTreeIterator(){}
	TailTreeIterator(const TailTreeIterator &tti): tailKey(tti.tailKey), branch(tti.branch){}
	TailTreeIterator(TailTreeIterator &&tti): tailKey(std::move(tti.tailKey)), branch(std::move(tti.branch)){}

	TailTreeIterator &operator=(const TailTreeIterator &tti){
		this->tailKey = tti.tailKey;
		this->branch = tti.branch;

		return *this;
	}

	TailTreeIterator &operator=(TailTreeIterator &&tti){
		std::swap(this->tailKey, tti.tailKey);
		std::swap(this->branch, tti.branch);

		return *this;
	}

	const Key &getKey() const{
		return this->tailKey;
	}

	bool operator==(const TailTreeIterator &it) const{
		return this->branch == it.branch;
	}

	bool operator!=(const TailTreeIterator &it) const{
		return !(*this == it);
	}

	const Value &operator*() const{
		if(this->isValid() == false){
			throw std::out_of_range("TailTreeIterator is invalid");
		}

		return this->branch.valueNode()->getValue();
	}

	Value &operator*(){
		if(this->isValid() == false){
			throw std::out_of_range("TailTreeIterator is invalid");
		}

		return this->branch.valueNode()->getValue();
	}

	TailTreeIterator &operator++(){
		if(this->isValid() == false){
			return *this; // why not? :)
		}

		constexpr typename Node<Key, Value>::Direction direction = Node<Key, Value>::Direction::higher;
		bool success = false;

		for(int i = this->branch.size() - 2; i >= 0; --i){
			Node<Key, Value> *current = dynamic_cast<Node<Key, Value> *>(this->branch.at(i));
			assert(current != nullptr);

			const typename Key::value_type &keyItem = this->tailKey.at(i);
			typename Node<Key, Value>::BranchInfo closestExistingBranchInfo = current->getClosestExistingBranch(direction, keyItem.toIndex());

			if(closestExistingBranchInfo.first != nullptr){
				success = true;
				this->branch.erase(this->branch.begin() + i + 1, this->branch.end());
				this->tailKey.erase(this->tailKey.begin() + i, this->tailKey.end());

				do{
					this->branch.push_back(closestExistingBranchInfo.first);
					this->tailKey.push_back(Key::value_type::fromIndex(closestExistingBranchInfo.second));

					Node<Key, Value> *node = dynamic_cast<Node<Key, Value> *>(this->branch.back());
					if(node != nullptr){
						closestExistingBranchInfo = node->getClosestExistingBranch(direction, -1);
					}
					else{
						break;
					}
				}while(closestExistingBranchInfo.first != nullptr);

				break;
			}
		}

		if(success == false){
			*this = TailTreeIterator<Key, Value>();
		}

		return *this;
	}



	friend void swap(TailTreeIterator &lhs, TailTreeIterator &rhs){
		std::swap(lhs.branch, rhs.branch);
		std::swap(lhs.tailKey, rhs.tailKey);
	}

	friend class TailTree<Key, Value>;
};


#endif // TAILTREEITERATOR_HPP

