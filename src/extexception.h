/*
 * extexception.h
 *
 *  Created on: 18/11/2013
 *      Author: alejo
 */

#ifndef EXTEXCEPTION_H_
#define EXTEXCEPTION_H_

#include <exception>
#include <string>


namespace extmodem {

class extexception : public std::exception {
public:
	explicit extexception() {}
	explicit extexception(const std::string& msg) throw() : msg_(msg) {}
	virtual ~extexception() throw(){}

	virtual const char* what() const throw() { return msg_.c_str(); }

private:
	std::string msg_;
};

class audiosourceexception : public std::exception {
public:
	explicit audiosourceexception(const std::string& msg) throw() : msg_(msg) {}
	virtual ~audiosourceexception() throw() {}
	virtual const char* what() const throw() { return msg_.c_str(); }

private:
	std::string msg_;
};


} /* namespace extmodem */
#endif /* EXTEXCEPTION_H_ */
