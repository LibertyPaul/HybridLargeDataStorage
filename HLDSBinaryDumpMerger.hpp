#ifndef HLDSBINARYDUMPMERGER_HPP
#define HLDSBINARYDUMPMERGER_HPP

#include "HLDSDump.hpp"

template<typename Key, typename Value>
class HLDSBinaryDumpMerger{
	HLDSDumpReader<Key, Value> reader1, reader2;
	HLDSDumpWriter<Key, Value> writer;

public:
	HLDSBinaryDumpMerger(std::istream &src1, std::istream &src2, std::ostream &dst): reader1(src1), reader2(src2), writer(dst){
		assert(reader1.getHeader().hldsId == reader2.getHeader().hldsId);
		assert(reader1.getHeader().keySize == reader2.getHeader().keySize);

		this->writer.writeHeader(reader1.getHeader());
	}

	void run(){
		while(this->reader1.hasNext() && this->reader2.hasNext()){
			if(this->reader1.peek().key == this->reader2.peek().key){
				const Key key = this->reader1.peek().key;
				const Value value = this->reader1.peek().value + this->reader2.peek().value;

				this->writer.write(HLDSDumpRecord<Key, Value>(key, value));

				this->reader1.read();
				this->reader2.read();
			}
			else if(this->reader1.peek().key < this->reader2.peek().key){
				this->writer.write(this->reader1.read());
			}
			else{
				this->writer.write(this->reader2.read());
			}
		}

		while(this->reader1.hasNext()){
			this->writer.write(this->reader1.read());
		}

		while(this->reader2.hasNext()){
			this->writer.write(this->reader2.read());
		}
	}

};

#endif // HLDSBINARYDUMPMERGER_HPP

