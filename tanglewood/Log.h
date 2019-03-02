#include <ion/core/debug/Debug.h>

#define DBG_LOG_LEVEL 1

#if DBG_LOG_LEVEL >= 1
#define DBG_LOG_FUNC_LV1(a) ion::debug::log(#a); a
#define DBG_LOG_LV1(a) ion::debug::log << a << ion::debug::end
#else
#define DBG_LOG_FUNC_LV1(a) a
#define DBG_LOG_LV1(a)
#endif

#if DBG_LOG_LEVEL >= 2
#define DBG_LOG_FUNC_LV2(a) ion::debug::Log(#a); a
#define DBG_LOG_LV2(a) ion::debug::log << a << ion::debug::end
#else
#define DBG_LOG_FUNC_LV2(a) a
#define DBG_LOG_LV2(a)
#endif

#if DBG_LOG_LEVEL >= 3
#define DBG_LOG_FUNC_LV3(a) ion::debug::Log(#a); a
#define DBG_LOG_LV3(a) ion::debug::log << a << ion::debug::end
#else
#define DBG_LOG_FUNC_LV3(a) a
#define DBG_LOG_LV3(a)
#endif