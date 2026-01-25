#pragma once
#include <spdlog/spdlog.h>

void initLogger();

#define LOG_INFO(...)       spdlog::info(__VA_ARGS__)
#define LOG_WARN(...)       spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...)      spdlog::error(__VA_ARGS__)

#ifdef _DEBUG
#define LOG_DEBUG(...)      spdlog::debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...)      ((void)0)
#endif