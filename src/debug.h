#ifdef EX_DEBUG

#define EX_DEBUG_LOGLEVEL_NONE 0
#define EX_DEBUG_LOGLEVEL_FATAL 1
#define EX_DEBUG_LOGLEVEL_ERROR 2
#define EX_DEBUG_LOGLEVEL_WARNING 3
#define EX_DEBUG_LOGLEVEL_INFO 4
#define EX_DEBUG_LOGLEVEL_NOTICE 5
#define EX_DEBUG_LOGLEVEL_TRACE 6
#define EX_DEBUG_LOGLEVEL_VERBOSE 7

#ifdef EX_DEBUG
#ifndef EX_DEBUG_LOGLEVEL
#define EX_DEBUG_LOGLEVEL EX_DEBUG_LOGLEVEL_ERROR
#endif
#endif

#define EX_DEBUG_LOGLEVEL_IF(level) (level <= EX_DEBUG_LOGLEVEL)

namespace
{
#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_FATAL)
  static void
  EX_DBG_SETUP(unsigned long baud = 115200)
  {
    EX_DEBUG.begin(baud);
    while (!EX_DEBUG && !EX_DEBUG.available())
    {
    }
    delay(1000);
    EX_DEBUG.println("booting");
  }
#else
#define EX_DBG_SETUP(...)
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_FATAL)
  template <typename T>
  static void EX_DBG_PLAIN(T last)
  {
    EX_DEBUG.println(last);
  }
#else
#define EX_DBG_PLAIN(...)
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_FATAL)
  template <typename T, typename... Args>
  static void EX_DBG_PLAIN(T head, Args... tail)
  {
    EX_DEBUG.print(head);
    EX_DEBUG.print(' ');
    EX_DBG_PLAIN(tail...);
  }
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_FATAL)
  template <typename... Args>
  static void EX_DBG_F(Args... args)
  {
    EX_DBG_PLAIN(args...);
  }
#else
#define EX_DBG_F(...)
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_ERROR)
  template <typename... Args>
  static void EX_DBG_E(Args... args)
  {
    EX_DBG_PLAIN(args...);
  }
#else
#define EX_DBG_E(...)
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_WARNING)
  template <typename... Args>
  static void EX_DBG_W(Args... args)
  {
    EX_DBG_PLAIN(args...);
  }
#else
#define EX_DBG_W(...)
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_INFO)
  template <typename... Args>
  static void EX_DBG_I(Args... args)
  {
    EX_DBG_PLAIN(args...);
  }
#else
#define EX_DBG_I(...)
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_NOTICE)
  template <typename... Args>
  static void EX_DBG_N(Args... args)
  {
    EX_DBG_PLAIN(args...);
  }
#else
#define EX_DBG_N(...)
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_TRACE)
  template <typename... Args>
  static void EX_DBG_T(Args... args)
  {
    EX_DBG_PLAIN(args...);
  }
#else
#define EX_DBG_T(...)
#endif

#if EX_DEBUG_LOGLEVEL_IF(EX_DEBUG_LOGLEVEL_VERBOSE)
  template <typename... Args>
  static void EX_DBG_V(Args... args)
  {
    EX_DBG_PLAIN(args...);
  }
#else
#define EX_DBG_V(...)
#endif
} // namespace

#else // EX_DEBUG not defined
#define EX_DBG_SETUP(...)
#define EX_DBG_PLAIN(...)
#define EX_DBG_F(...)
#define EX_DBG_E(...)
#define EX_DBG_W(...)
#define EX_DBG_I(...)
#define EX_DBG_N(...)
#define EX_DBG_T(...)
#define EX_DBG_V(...)
#endif