#if defined(LOG_LOGLEVEL) || defined(LOGGER)

#define LOG_LOGLEVEL_NONE 0
#define LOG_LOGLEVEL_FATAL 1
#define LOG_LOGLEVEL_ERROR 2
#define LOG_LOGLEVEL_WARNING 3
#define LOG_LOGLEVEL_INFO 4
#define LOG_LOGLEVEL_NOTICE 5
#define LOG_LOGLEVEL_TRACE 6
#define LOG_LOGLEVEL_VERBOSE 7

#ifdef LOGGER
#ifndef LOG_LOGLEVEL
#define LOG_LOGLEVEL LOG_LOGLEVEL_ERROR
#endif
#endif

#define LOG_LOGLEVEL_IF(level) (level <= LOG_LOGLEVEL)

#define LOG_SHORT_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

namespace
{
#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_FATAL)
  static void
  LOG_SETUP(unsigned long baud = 115200)
  {
    LOGGER.begin(baud);
    while (!LOGGER && !LOGGER.available()) {} 
    LOGGER.println(F("\nStarting logging"));
  }
#else
#define LOG_SETUP(...)
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_FATAL)
  template <typename T>
  static void LOG_PLAIN(T last)
  {
    LOGGER.println(last);
  }
#else
#define LOG_PLAIN(...)
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_FATAL)
  template <typename T, typename... Args>
  static void LOG_PLAIN(T head, Args... tail)
  {
    LOGGER.print(head);
    LOGGER.print(' ');
    LOG_PLAIN(tail...);
  }
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_FATAL)
  template <typename... Args>
  static void LOG_F(Args... args)
  {
    LOGGER.print(F("FATAL:   "));
    LOG_PLAIN(args...);
  }
#else
#define LOG_F(...)
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_ERROR)
  template <typename... Args>
  static void LOG_E(Args... args)
  {
    LOGGER.print(F("ERROR:   "));
    LOG_PLAIN(args...);
  }
#else
#define LOG_E(...)
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_WARNING)
  template <typename... Args>
  static void LOG_W(Args... args)
  {
    LOGGER.print(F("Warning: "));
    LOG_PLAIN(args...);
  }
#else
#define LOG_W(...)
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_INFO)
  template <typename... Args>
  static void LOG_I(Args... args)
  {
    LOGGER.print(F("Info:    "));
    LOG_PLAIN(args...);
  }
#else
#define LOG_I(...)
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_NOTICE)
  template <typename... Args>
  static void LOG_N(Args... args)
  {
     LOGGER.print(F("Notice:  "));
   LOG_PLAIN(args...);
  }
#else
#define LOG_N(...)
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_TRACE)
  template <typename... Args>
  static void LOG_T(Args... args)
  {
    LOGGER.print(F("trace:   "));
    LOG_PLAIN(args...);
  }
#else
#define LOG_T(...)
#endif

#if LOG_LOGLEVEL_IF(LOG_LOGLEVEL_VERBOSE)
  template <typename... Args>
  static void LOG_V(Args... args)
  {
    LOGGER.print(F("verbose: "));
    LOG_PLAIN(args...);
  }
#else
#define LOG_V(...)
#endif
} // namespace

#else // LOGGER not defined
#define LOG_SETUP(...)
#define LOG_PLAIN(...)
#define LOG_F(...)
#define LOG_E(...)
#define LOG_W(...)
#define LOG_I(...)
#define LOG_N(...)
#define LOG_T(...)
#define LOG_V(...)
#endif