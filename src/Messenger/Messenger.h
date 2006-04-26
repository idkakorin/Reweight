//____________________________________________________________________________
/*!

\class    genie::Messenger

\brief    A more convenient interface to the log4cpp Message Service

\author   Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
          CCLRC, Rutherford Appleton Laboratory

\created  June 16, 2004

\cpright  Copyright (c) 2003-2006, GENIE Neutrino MC Generator Collaboration
          All rights reserved.
          For the licensing terms see $GENIE/USER_LICENSE.
*/
//____________________________________________________________________________

#ifndef _MESSENGER_H_
#define _MESSENGER_H_

#include <iostream>
#include <string>
#include <map>

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"

using std::string;

// comment defined priority levels for the document generator
/*! \def pFATAL  \brief Defines the FATAL priority level */
/*! \def pALERT  \brief Defines the ALERT priority level */
/*! \def pCRIT   \brief Defines the ALERT priority level */
/*! \def pERROR  \brief Defines the ALERT priority level */
/*! \def pWARN   \brief Defines the ALERT priority level */
/*! \def pNOTICE \brief Defines the ALERT priority level */
/*! \def pINFO   \brief Defines the ALERT priority level */
/*! \def pDEBUG  \brief Defines the ALERT priority level */

#define pFATAL  log4cpp::Priority::FATAL
#define pALERT  log4cpp::Priority::ALERT
#define pCRIT   log4cpp::Priority::CRIT
#define pERROR  log4cpp::Priority::ERROR
#define pWARN   log4cpp::Priority::WARN
#define pNOTICE log4cpp::Priority::NOTICE
#define pINFO   log4cpp::Priority::INFO
#define pDEBUG  log4cpp::Priority::DEBUG

/*! \def ENDL  \brief A shortcut for log4cpp's CategoryStream::ENDLINE */

#define ENDL log4cpp::CategoryStream::ENDLINE

/*! \def _GCLASS \brief Define a class name macro */

#define __P1__ __PRETTY_FUNCTION__
#define __P2__ string(__P1__).erase( string(__P1__).find_first_of("("),  string(__P1__).size() )
#define __P3__ string(__P2__).erase( (string(__P2__).find_last_of(":") != string::npos) ? string(__P2__).find_last_of(":")-1 : 0, string(__P2__).size() )
#define __P4__ string(__P3__).erase( 0, string(__P3__).find_last_of(" ")+1)
#define __GCLASS __P4__

/*!
  \def   SLOG(stream, priority)
  \brief A macro that returns the requested log4cpp::Category
         appending a short string (using the __FUNCTION__ and __LINE__ macros)
         with information for the calling method [produces short message].
*/

#define SLOG(stream, priority) \
           (*Messenger::Instance())(stream) \
               << priority << "[s] <" \
               << __FUNCTION__ << " (" << __LINE__ << ")> : "

/*!
  \def   LOG(stream, priority)
  \brief A macro that returns the requested log4cpp::Category
         appending a string (using the __GCLASS, __FUNCTION__ and __LINE__ macros)
         with information for the calling method [produces normal messages].
*/

#define LOG(stream, priority) \
           (*Messenger::Instance())(stream) \
               << priority << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

#define LOG_FATAL(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::FATAL << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

#define LOG_ALERT(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::ALERT << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

#define LOG_CRIT(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::CRIT << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

#define LOG_ERROR(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::ERROR << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

#define LOG_WARN(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::WARN << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

#define LOG_NOTICE(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::NOTICE << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

#define LOG_INFO(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::INFO << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

#define LOG_DEBUG(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::DEBUG << "[n] <" \
               << __GCLASS << "::" << __FUNCTION__ << " (" << __LINE__ << ")> : "

/*!
  \def   LLOG(stream, priority)
  \brief A macro that returns the requested log4cpp::Category
         appending a string (using the __PRETTY_FUNCTION__ and __LINE__ macros)
         with information for the calling method [produces long messages].
*/

#define LLOG(stream, priority) \
           (*Messenger::Instance())(stream) \
               << priority << "[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

#define LLOG_FATAL(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::FATAL << "[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

#define LLOG_ALERT(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::ALERT << "[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

#define LLOG_CRIT(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::CRIT << "[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

#define LLOG_ERROR(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::ERROR << "[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

#define LLOG_WARN(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::WARN << "'[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

#define LLOG_NOTICE(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::NOTICE << "[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

#define LLOG_INFO(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::INFO << "[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

#define LLOG_DEBUG(stream) \
          (*Messenger::Instance())(stream) \
               << log4cpp::Priority::DEBUG << "[l] <" \
               << __PRETTY_FUNCTION__ << " (" << __LINE__ << ")> : "

namespace genie {

class Messenger
{
public:

  static Messenger * Instance(void);

  log4cpp::Category & operator () (const char * stream);

  void SetPriorityLevel(const char * stream,
                                 log4cpp::Priority::Value priority);

private:

  Messenger();
  Messenger(const Messenger & config_pool);
  virtual ~Messenger();

  static Messenger * fInstance;

  // configuration methods
  void Configure(void);
  bool SetPrioritiesFromXmlFile(string filename);
  log4cpp::Priority::Value PriorityFromString(string priority);

  struct Cleaner {
      void DummyMethodAndSilentCompiler() { }
      ~Cleaner() {
         if (Messenger::fInstance !=0) {
            delete Messenger::fInstance;
            Messenger::fInstance = 0;
         }
      }
  };
  friend struct Cleaner;
};

}      // genie namespace

#endif // _MESSENGER_H_
