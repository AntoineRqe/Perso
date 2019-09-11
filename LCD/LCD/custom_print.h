#pragma once

#include <stdio.h>

#ifndef CUSTOM_PRINT_H
#define CUSTOM_PRINT_H

#define PRINT_INFO(fmt, ...) do{fprintf(stdout, "[%s:%d] " fmt "\n", __func__, __LINE__, __VA_ARGS__);} while(0)
#define PRINT_DEBUG(fmt, ...) do{fprintf(stderr, "[%s:%d] " fmt "\n", __func__, __LINE__, __VA_ARGS__);} while(0)
#define PRINT_ERROR(fmt, ...) do{fprintf(stderr, "[%s:%d] " fmt "\n", __func__, __LINE__, __VA_ARGS__);} while(0)

#endif // !CUSTOM_PRINT_H
