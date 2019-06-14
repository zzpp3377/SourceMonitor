#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>

#define __DEBUG    //日志模块总开关，注释掉将关闭日志输出

#define DEBUG(format, ...) printf (format, ##__VA_ARGS__)

//定义日志级别
typedef enum LOG_LEVEL {    
    LOG_LEVEL_OFF=0,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} LogLevel;

#ifdef __DEBUG
    #define log_fatal(level,format, ...) \
        do { \
            if(level>=LOG_LEVEL_FATAL)\
            DEBUG( "=>FATAL FUNC:%s FILE:%s LINE:%d \t" format "\n" ,\
                        __func__, __FILE__, __LINE__, ##__VA_ARGS__ );\
        } while (0)
#else 
    #define log_fatal(level,format, ...)
#endif

#ifdef __DEBUG
    #define log_err(level,format, ...) \
        do { \
            if(level>=LOG_LEVEL_ERR)\
            DEBUG( "=>ERR  FUNC:%s FILE:%s LINE:%d \t" format "\n" ,\
                        __func__, __FILE__, __LINE__, ##__VA_ARGS__ );\
        } while (0)
#else 
    #define log_err(level,format, ...)
#endif

#ifdef __DEBUG
    #define log_warn(level,format, ...) \
        do { \
            if(level>=LOG_LEVEL_WARN)\
            DEBUG( "=>WARN  FUNC:%s \t" format "\n" ,__func__, ##__VA_ARGS__ );\
        } while (0)
#else 
    #define log_warn(level,format, ...) 
#endif

#ifdef __DEBUG
    #define log_info(level,format, ...) \
        do { \
            if(level>=LOG_LEVEL_INFO)\
            DEBUG( "=>INFO  FUNC:%s \t"format"\n" ,__func__,##__VA_ARGS__ );\
        } while (0)
#else 
    #define log_info(level,format, ...)
#endif

#ifdef __DEBUG
    #define log_debug(level,format, ...) \
        do { \
            if(level>=LOG_LEVEL_DEBUG)\
            DEBUG( "=>DEBUG FUNC:%s \t"format"\n" ,__func__,##__VA_ARGS__ );\
        } while (0)
#else 
    #define log_debug(level,format, ...)
#endif


#endif 