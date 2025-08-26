#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "sdkconfig.h"

#define MYLOG_LOG_LEVEL_NONE (0)
#define MYLOG_LOG_LEVEL_ERROR (1)
#define MYLOG_LOG_LEVEL_WARN (2)
#define MYLOG_LOG_LEVEL_INFO (3)
#define MYLOG_LOG_LEVEL_DEBUG (4)
#define MYLOG_LOG_LEVEL_VERBOSE (5)

#ifndef CONFIG_MYLOG_LOG_DEFAULT_LEVEL
#define CONFIG_MYLOG_LOG_DEFAULT_LEVEL MYLOG_LOG_LEVEL_NONE
#endif

#ifndef MYLOG_LOG_LEVEL
#define MYLOG_LOG_LEVEL MYLOG_LOG_LEVEL_NONE
#endif

#define MYLOG_LOG_COLOR_E
#define MYLOG_LOG_COLOR_W
#define MYLOG_LOG_COLOR_I
#define MYLOG_LOG_COLOR_D
#define MYLOG_LOG_COLOR_V
#define MYLOG_LOG_RESET_COLOR

const char *pathToFileName(const char *path);
int log_printf(const char *fmt, ...);

#define MYLOG_SHORT_LOG_FORMAT(letter, format) MYLOG_LOG_COLOR_##letter format MYLOG_LOG_RESET_COLOR "\r\n"
#define MYLOG_LOG_FORMAT(letter, format) MYLOG_LOG_COLOR_##letter "[" #letter "][%s:%u] %s(): " format MYLOG_LOG_RESET_COLOR "\r\n", pathToFileName(__FILE__), __LINE__, __FUNCTION__

#if MYLOG_LOG_LEVEL >= MYLOG_LOG_LEVEL_VERBOSE
#define myLog_v(format, ...) log_printf(MYLOG_LOG_FORMAT(V, format), ##__VA_ARGS__)
#define myIsr_log_v(format, ...) ets_printf(MYLOG_LOG_FORMAT(V, format), ##__VA_ARGS__)
#else
#define myLog_v(format, ...)
#define myIsr_log_v(format, ...)
#endif

#if MYLOG_LOG_LEVEL >= MYLOG_LOG_LEVEL_DEBUG
#define myLog_d(format, ...) log_printf(MYLOG_LOG_FORMAT(D, format), ##__VA_ARGS__)
#define myIsr_log_d(format, ...) ets_printf(MYLOG_LOG_FORMAT(D, format), ##__VA_ARGS__)
#else
#define myLog_d(format, ...)
#define myIsr_log_d(format, ...)
#endif

#if MYLOG_LOG_LEVEL >= MYLOG_LOG_LEVEL_INFO
#define myLog_i(format, ...) log_printf(MYLOG_LOG_FORMAT(I, format), ##__VA_ARGS__)
#define myIsr_log_i(format, ...) ets_printf(MYLOG_LOG_FORMAT(I, format), ##__VA_ARGS__)
#else
#define myLog_i(format, ...)
#define myIsr_log_i(format, ...)
#endif

#if MYLOG_LOG_LEVEL >= MYLOG_LOG_LEVEL_WARN
#define myLog_w(format, ...) log_printf(MYLOG_LOG_FORMAT(W, format), ##__VA_ARGS__)
#define myIsr_log_w(format, ...) ets_printf(MYLOG_LOG_FORMAT(W, format), ##__VA_ARGS__)
#else
#define myLog_w(format, ...)
#define myIsr_log_w(format, ...)
#endif

#if MYLOG_LOG_LEVEL >= MYLOG_LOG_LEVEL_ERROR
#define myLog_e(format, ...) log_printf(MYLOG_LOG_FORMAT(E, format), ##__VA_ARGS__)
#define myIsr_log_e(format, ...) ets_printf(MYLOG_LOG_FORMAT(E, format), ##__VA_ARGS__)
#else
#define myLog_e(format, ...)
#define myIsr_log_e(format, ...)
#endif

#if MYLOG_LOG_LEVEL >= MYLOG_LOG_LEVEL_NONE
#define myLog_n(format, ...) log_printf(MYLOG_LOG_FORMAT(E, format), ##__VA_ARGS__)
#define myIsr_log_n(format, ...) ets_printf(MYLOG_LOG_FORMAT(E, format), ##__VA_ARGS__)
#else
#define myLog_n(format, ...)
#define myIsr_log_n(format, ...)
#endif

#include "esp_log.h"

#ifdef CONFIG_MYLOG_ESP_LOG
#undef ESP_LOGE
#undef ESP_LOGW
#undef ESP_LOGI
#undef ESP_LOGD
#undef ESP_LOGV
#undef ESP_EARLY_LOGE
#undef ESP_EARLY_LOGW
#undef ESP_EARLY_LOGI
#undef ESP_EARLY_LOGD
#undef ESP_EARLY_LOGV

#define ESP_LOGE(tag, ...) log_e(__VA_ARGS__)
#define ESP_LOGW(tag, ...) log_w(__VA_ARGS__)
#define ESP_LOGI(tag, ...) log_i(__VA_ARGS__)
#define ESP_LOGD(tag, ...) log_d(__VA_ARGS__)
#define ESP_LOGV(tag, ...) log_v(__VA_ARGS__)
#define ESP_EARLY_LOGE(tag, ...) isr_log_e(__VA_ARGS__)
#define ESP_EARLY_LOGW(tag, ...) isr_log_w(__VA_ARGS__)
#define ESP_EARLY_LOGI(tag, ...) isr_log_i(__VA_ARGS__)
#define ESP_EARLY_LOGD(tag, ...) isr_log_d(__VA_ARGS__)
#define ESP_EARLY_LOGV(tag, ...) isr_log_v(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MY_LOG_H__ */
