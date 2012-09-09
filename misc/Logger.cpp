/*
	Copyright 2011, Michal Stawinski <michal.stawinski@gmail.com>

	This file is part of libsockets- C++ wrapper over OpenSSL and raw sockets.

	libsockets is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libsockets is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <misc/Logger.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <iostream>

using namespace ecce;
namespace ecce {
Logger::Logger(std::string logname, int loglevel) :
		itsLogname(logname)
{
	if (loglevel == -1)
		loglevel = LOGLEVEL;
//		loglevel = logDebug;

	logConfig config;
	config.CurrentLevel = static_cast<logLevel>(loglevel);
	config.DefaultLevel = static_cast<logLevel>(loglevel);

	getLoggers().insert(loggerDesc_t(logname, config));
}

Logger::~Logger()
{
}

std::map<std::string, Logger::logConfig>& Logger::getLoggers()
{
	static std::map<std::string, Logger::logConfig>* loggersDescs = new std::map<std::string, Logger::logConfig>();
	return *loggersDescs;
}

NullStream& Logger::getNullStream()
{
	static NullStream* nstream = new NullStream();
	return *nstream;
}

void Logger::ForceLoglevel(logLevel level)
{
	std::map<std::string, logConfig>::iterator it;
	it = getLoggers().begin();

	for (; it != getLoggers().end(); ++it) {
		(*it).second.CurrentLevel = level;
	}
}

void Logger::ForceLoglevel(logLevel level, std::string module)
{
	std::map<std::string, logConfig>::iterator it;
	it = getLoggers().find(module);
	assert(it != getLoggers().end());
	(*it).second.CurrentLevel = level;
}

void Logger::RestoreLoglevel()
{
	std::map<std::string, logConfig>::iterator it;
	it = getLoggers().begin();

	for (; it != getLoggers().end(); ++it) {
		(*it).second.CurrentLevel = (*it).second.DefaultLevel;
	}
}

void Logger::RestoreLoglevel(std::string module)
{
	std::map<std::string, logConfig>::iterator it;
	it = getLoggers().find(module);
	assert(it != getLoggers().end());
	(*it).second.CurrentLevel = (*it).second.DefaultLevel;

}


std::string Logger::StripPrettyFunction(const char* fname)
{
	// e.g.: std::ostream& osock::Logger::out(osock::Logger::logLevel);
	std::string name(fname);
	// get rid of space
	name = name.substr(name.find(' '));
	// get rid of arguments
	name = name.erase(name.find('('));
	return name;
}

std::string Logger::doGetClassName(std::string name)
{
	// e.g.: osock::Logger*
	// get rid of '*'
	name.erase(name.find('*'));
	//strip out all namespaces
	for(size_t pos = name.find("::"); pos != std::string::npos; pos = name.find("::")) {
		name = name.substr(pos + 2);
	}
	return name;
}

std::string Logger::doTrim(std::string s, unsigned int w)
{
	if (s.length() > w) {
		s = s.substr(s.length() - w + 3);
		s = "..." + s;
	}

	return s;
}

std::ostream& Logger::doPrint(logLevel loglevel, const char* func, std::string c)
{
	std::string m = doTrim(func, wMeth);
	c = doTrim(c, wClass);

	return out(loglevel) << "<" << std::setw(wClass) << std::right << c
			<< "::" << std::setw(wMeth) << std::left << m << "> ";
}

std::ostream& Logger::out(logLevel loglevel, const char* func)
{
	std::string f = doTrim(func, wFunc);
	unsigned int pad = wFunc - f.length();
	unsigned int beg = (pad / 2);
	unsigned int end = (pad / 2) + (pad % 2);
	return out(loglevel)<< std::setw(beg + 1) << std::left << "<"
						<< f
						<< std::setw(end + 2) << std::right << "> ";
}

std::ostream& Logger::out(logLevel loglevel)
{
	std::map<std::string, logConfig>::const_iterator it;

	//XXX consecutive accesses to map can result in data corruption if we were rescheduled
	it = getLoggers().find(itsLogname);
	assert(it != getLoggers().end());

//	std::cout << "YYY "<< ExtractClass(this)
//			<< "FFF " << __func__ << std::endl;
//			<< "XXX " << SimplifyFuncname(__PRETTY_FUNCTION__) << std::endl;

//	std::cout <<  ti << std::endl;
	if(loglevel < (*it).second.CurrentLevel) {
		return getNullStream();
	} else {

		// XXX this is a nasty workaround to make printouts shorter
		if (loglevel <= logInfo)
			return std::cout << std::hex << ((pthread_self() >> 7) & 0xFFF) << std::dec << " " ;
		else
			return std::cerr << std::hex << ((pthread_self() >> 7) & 0xFFF) << std::dec << " " ;
	}
}
}
