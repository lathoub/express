#ifdef DEBUG

namespace {
static void EX_DBG_SETUP(unsigned long baud) {
  DEBUG.begin(baud);
  while (!DEBUG){}
  delay(1500);
}

template <typename T>
static void EX_DBG_PLAIN(T last) {
  DEBUG.println(last);
}

template <typename T, typename... Args>
static void EX_DBG_PLAIN(T head, Args... tail) {
  DEBUG.print(head);
  DEBUG.print(' ');
  EX_DBG_PLAIN(tail...);
}

template <typename... Args>
static void EX_DBG(Args... args) {
  EX_DBG_PLAIN(args...);
}
}  // namespace

#else
#define EX_DBG_SETUP(...)
#define EX_DBG_PLAIN(...)
#define EX_DBG(...)

#endif
