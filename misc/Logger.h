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
#ifndef ECCELOGGER_H_
#define ECCELOGGER_H_

#include <typeinfo>
#include <iostream>
#include <iomanip>
#include <map>
#include <cxxabi.h>

#include <stdlib.h>

namespace ecce {
//=============================================================================
/* Sole purpose of this class is to discard all that goes into it */
struct NullStream: std::ostream
{
	struct nullbuf: std::streambuf
	{
		int overflow(int c)
		{
			return traits_type::not_eof(c);
		}
	} m_sbuf;
	NullStream() :
		std::ios(&m_sbuf), std::ostream(&m_sbuf)
	{
	}
};
//=============================================================================
class Logger
{
public:
	typedef enum { logAll, logDebug, logInfo, logWarn, logError } logLevel;
	typedef struct {
		logLevel CurrentLevel;
		logLevel DefaultLevel;
	} logConfig;

private:
	static const unsigned int wClass = 20;
	static const unsigned int wMeth = 25;
	static const unsigned int wFunc = wClass + wMeth + 2; // <Class::Func>

	typedef std::pair<std::string, logConfig> loggerDesc_t;
	std::string itsLogname;
	/**
	 * @brief Protects us from static initialization fiasco- Logger objects can
	 * be created stati, despite having static field
	 * @return Instance of loggers description map
	 */
	static std::map<std::string, logConfig>& getLoggers();
	/**
	 * @brief Protects us from static initialization fiasco- Logger objects can
	 * be created stati, despite having static field
	 * @return NullStream reference
	 */
	static NullStream& getNullStream();
	std::string doTrim(std::string s, unsigned int w);
	std::string StripPrettyFunction(const char* fname);
	std::string doGetClassName(std::string name);
	template<class T> std::string GetClassName(T* obj)
	{
		// XXX make it exception safe
		int status;
		const std::type_info  &ti = typeid(obj);
		char* realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
		std::string name(realname);
		free(realname);

		return doGetClassName(name);
	}
public:
	Logger(std::string logname, int logelvel = -1);
	virtual ~Logger();

	static void ForceLoglevel(logLevel level);
	static void ForceLoglevel(logLevel level, std::string module);
	static void RestoreLoglevel();
	static void RestoreLoglevel(std::string);

	template<class T>
	std::ostream& out(logLevel loglevel, const char* func, T* classobj)
	{
		return doPrint(loglevel, func, Logger::GetClassName(classobj));
	}
	std::ostream& doPrint(logLevel loglevel, const char* func, std::string classname);
	std::ostream& out(logLevel loglevel, const char* func);
	std::ostream& out(logLevel loglevel);
};
//=============================================================================
#define LOG_MARKER_DBG_CON 	" dbg + "
#define LOG_MARKER_DBG_DES 	" dbg ~ "
#define LOG_MARKER_DBG 		" dbg   "
#define LOG_MARKER_NFO 		" nfo . "
#define LOG_MARKER_WRN 		" wrn ? "
#define LOG_MARKER_ERR 		" ERR ! "

#define LOG_PRINT_CLASS(lvl) 	logger.out(lvl, __func__, this)
#define LOG_PRINT_FUNC(lvl) 	logger.out(lvl, __func__)

//========================= debug logs =========================================
#define DBG_CONSTRUCTOR	LOG_PRINT_CLASS(Logger::logDebug) << LOG_MARKER_DBG_CON << this << std::endl
#define DBG_DESTRUCTOR	LOG_PRINT_CLASS(Logger::logDebug) << LOG_MARKER_DBG_DES << this << std::endl
#define DBG				LOG_PRINT_CLASS(Logger::logDebug) << LOG_MARKER_DBG
#define DBG_FUNC		LOG_PRINT_FUNC(Logger::logDebug) << LOG_MARKER_DBG
//========================= info logs ==========================================
#define NFO 			LOG_PRINT_CLASS(Logger::logInfo) << LOG_MARKER_NFO
#define NFO_FUNC		LOG_PRINT_FUNC(Logger::logInfo) << LOG_MARKER_NFO
//========================= warning logs =======================================
#define WRN 			LOG_PRINT_CLASS(Logger::logWarn) << LOG_MARKER_WRN
#define WRN_FUNC		LOG_PRINT_FUNC(Logger::logWarn) << LOG_MARKER_WRN
//========================= error logs =========================================
#define ERR 			LOG_PRINT_CLASS(Logger::logError) << LOG_MARKER_ERR
#define ERR_FUNC		LOG_PRINT_FUNC(Logger::logError) << LOG_MARKER_ERR

} //ecce
#endif /* LOGGER_H_ */
