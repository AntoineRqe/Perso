#pragma once

#include <stdio.h>

#ifndef CUSTOM_PRINT_H
#define CUSTOM_PRINT_H

#define DEBUG 1

#define PRINT_INFO(fmt, ...) do{fprintf(stdout,fmt "\n", __VA_ARGS__);} while(0)
#define PRINT_DEBUG(fmt, ...) do{if(DEBUG){fprintf(stderr, "[%s:%d] " fmt "\n", __func__, __LINE__, __VA_ARGS__);}} while(0)
#define PRINT_ERROR(fmt, ...) do{fprintf(stderr, "[%s:%d] " fmt "\n", __func__, __LINE__, __VA_ARGS__);} while(0)

#endif // !CUSTOM_PRINT_H
