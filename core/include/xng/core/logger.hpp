#pragma once

#include "singleton.hpp"
#include "string_convert.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <ostream>
#include <sstream>
#include <mutex>

#define XNG_LOG_DEBUG_LABEL            BOOST_PP_CAT(BOOST_PP_CAT(__FUNCTION__, " @ "), BOOST_PP_CAT(BOOST_PP_CAT(__FILE__, ":"), BOOST_PP_CAT(, BOOST_PP_STRINGIZE(__LINE__))))
#define XNG_LOG(Label, Message)        xng::core::logger::get_singleton()->log(Label, Message);
#define XNG_LOG_DEBUG(Message)         XNG_LOG(XNG_LOG_DEBUG_LABEL, Message)
#define XNG_LOG_STREAM()               std::stringstream()
#define XNG_LOG_IF(Condition, Message) { if (Condition) XNG_LOG_DEBUG(Message); }

namespace xng
{
	namespace core
	{
		class logger :
		public singleton<logger>
		{
	
		public:
	
			logger(std::basic_ostream<char> * stream) : m_logStream(stream) {}

			void log(const char * label, const char * message) const;
			void log(const char * label, const decltype(XNG_LOG_STREAM()) & stream) const;
			void log(const char * label, const std::basic_ostream<char> & stream) const;
	
			template <typename SourceLabel, typename SourceMessage>
			void log(const SourceLabel & label, const SourceMessage & message) const
			{
				std::string labelStr, messageStr;
				string_convert(label, labelStr);
				string_convert(message, messageStr);
				log(labelStr.c_str(), messageStr.c_str());
			}

		private:
	
			std::basic_ostream<char> * m_logStream;
			mutable std::mutex m_mutex;
	
		};		
	}
}