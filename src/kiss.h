/*
 * kiss.h
 *
 *  Created on: 21/08/2013
 *      Author: alejo
 */

#ifndef KISS_H_
#define KISS_H_

#include <vector>
#include <cstdlib>

/*
 * KISS specs:
 *  - http://www.ka9q.net/papers/kiss.html
 *  - http://www.ax25.net/kiss.aspx
 */

#define KISS_FEND   0xC0 /* Frame  End */
#define KISS_FESC   0xDB /* Frame  Escape */
#define KISS_TFEND  0xDC /* Transposed Frame End */
#define KISS_TFESC  0xDD /* Transposed Frame Escape */

namespace extmodem {

void kiss_encode(const unsigned char* buffer, std::size_t length, std::vector<char>* dst);

}

#endif /* KISS_H_ */
