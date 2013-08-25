/*
 * frame.h
 *
 *  Created on: 23/08/2013
 *      Author: alejo
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <vector>
#include <cstdlib>
#include <boost/smart_ptr.hpp>


namespace extmodem {

class frame;
typedef boost::shared_ptr<frame> frame_ptr;

class frame {
public:
	/**  Constructs a new frame without the CRC at the end of the buffer.
	 *
	 * @param buffer
	 * @param length
	 * @param crc
	 */
	explicit frame() : crc_(0) {};
	explicit frame(const unsigned char* buffer, std::size_t length, unsigned int crc);
	virtual ~frame();

	std::vector<unsigned char>& get_data() { return data_; }
	unsigned int get_crc() const { return crc_; }

	void print();

private:
	std::vector<unsigned char> data_;
	unsigned int crc_;
};

} /* namespace extmodem */
#endif /* FRAME_H_ */
