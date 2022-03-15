#include "util.hpp"

#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace digital_curling_server {

std::string GetTimeOfDay(boost::posix_time::ptime t)
{
    return boost::posix_time::to_simple_string(t.time_of_day());
}

std::string GetISO8601ExtendedString(boost::posix_time::ptime t)
{
    const auto utc_timestamp = boost::posix_time::second_clock::universal_time();
    const auto local_timestamp = boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(utc_timestamp);
    const auto diff_from_utc = local_timestamp - utc_timestamp;

    std::ostringstream oss;
    oss << boost::posix_time::to_iso_extended_string(t);
    oss << (diff_from_utc.is_negative() ? '-' : '+');
    oss << std::setw(2) << std::setfill('0');
    oss << boost::date_time::absolute_value(diff_from_utc.hours());
    oss << ':';
    oss << std::setw(2) << std::setfill('0');
    oss << boost::date_time::absolute_value(diff_from_utc.minutes());
    return oss.str();

}

std::string GetISO8601String(boost::posix_time::ptime t)
{
    return boost::posix_time::to_iso_string(t);
}

} // namespace digital_curling_server
