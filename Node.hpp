#ifndef NODE_HPP
#define NODE_HPP

#include "BaseNode.hpp"
#include "TailTree.hpp"
#include "CountingFactory.hpp"

#include <array>
#include <utility>
#include <cassert>
#include <algorithm>
#include <functional>

template<typename Key, typename Value>
class BaseNode;

template<typename Key, typename Value>
class TailTree;

template<typename Key, typename Value>
class Node : public BaseNode<Key, Value>{
	typedef std::array<BaseNode<Key, Value> *, Key::value_type::alphabetSize> TailsStorage;
	TailsStorage tails;

	Node(){
		this->tails.fill(nullptr);
	}

	virtual ~Node(){
		for(const auto &tail : this->tails){
			delete tail;
		}
	}

public:

	enum class Direction{
		lower,
		higher
	};

	typedef std::pair<BaseNode<Key, Value> *, typename Key::value_type::index_t> BranchInfo;

	BranchInfo getClosestExistingBranch(const Direction direction, const int beginPos = -1){
		assert(beginPos >= -1 && beginPos <= static_cast<int>(this->tails.size()));
		int move = 0;

		switch(direction){
		case Direction::higher:
			move = 1;
			break;

		case Direction::lower:
			move = -1;
			break;
		}

		assert(move != 0);

		size_t pos;
		bool found = false;

		for(int i = beginPos + move; i < static_cast<int>(this->tails.size()) && i >= 0; i += move){
			if(this->tails.at(i) != nullptr){
				pos = static_cast<size_t>(i);
				found = true;
				break;
			}
		}

		if(found == false){
			return BranchInfo(nullptr, 0);
		}

		return BranchInfo(this->tails.at(pos), pos);
	}

	friend class TailTree<Key, Value>;
	friend typename TailTree<Key, Value>::iterator;
	friend class CountingFactory<Node>;

};

#endif // NODE_HPP

