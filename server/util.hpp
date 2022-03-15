#ifndef DIGITAL_CURLING_SERVER_UTIL_HPP
#define DIGITAL_CURLING_SERVER_UTIL_HPP

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace digital_curling_server {

/// \brief mm:ss+xx:yy 形式の時刻を得る
/// \param t 時刻(デフォルトで現在時刻)
/// \return hh:mm:ss 形式の時刻
std::string GetTimeOfDay(boost::posix_time::ptime t = boost::posix_time::second_clock::local_time());

/// \brief YYYY-MM-DDThh:mm:ss+xx:yy 形式の時刻を得る
/// \param t 時刻(デフォルトで現在時刻)
/// \return YYYY-MM-DDThh:mm:ss+xx:yy 形式の時刻
std::string GetISO8601ExtendedString(boost::posix_time::ptime t = boost::posix_time::second_clock::local_time());

/// \brief YYYYMMDDThhmmss 形式の時刻を得る
/// \param t 時刻(デフォルトで現在時刻)
/// \return YYYYMMDDThhmmss 形式の時刻
std::string GetISO8601String(boost::posix_time::ptime t = boost::posix_time::second_clock::local_time());

} // namespace digital_curling_server

#endif
