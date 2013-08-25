/*
 * ptt.h
 *
 *  Created on: 25/08/2013
 *      Author: alejo
 */

#ifndef PTT_H_
#define PTT_H_

#include <boost/shared_ptr.hpp>

namespace extmodem {

class ptt;
typedef boost::shared_ptr<ptt> ptt_ptr;

class ptt {
public:
	ptt();
	virtual ~ptt();

	virtual int init(const char* fname) = 0;
	virtual void set_tx(int tx) = 0;
	virtual int get_tx() = 0;
};

class ptt_serial_unix: public ptt {
public:
	ptt_serial_unix() : fd_(-1) {}
	virtual ~ptt_serial_unix();

	virtual int init(const char* fname);
	virtual void set_tx(int tx);
	virtual int get_tx();

	void set_dtr(int tx);
	void set_rts(int tx);

private:
	int fd_;
	int state_;
};
} /* namespace extmodem */
#endif /* PTT_H_ */
