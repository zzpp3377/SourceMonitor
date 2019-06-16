#include "logger.hpp"

// static LogLevel logLevel=LOG_LEVEL_DEBUG;

int main(){
    log_fatal(logLevel,"log_fatal");
    log_err(logLevel,"log_err");
    log_warn(logLevel,"log_warn");
    log_info(logLevel,"log_info");
    log_debug(logLevel,"log_debug");
}