#pragma once
#include <iostream>

#define ERROR_FILEOPEN "Unable to open file.\n"
#define ERROR_WRITE "Unable to write file.\n"
#define ERROR_NULLPTR "Pointer is equal to nullptr.\n"

#define PRINT(a) cout << "ERROR at (" << __FILE__ << ", " << __LINE__ << ", " << __func__ << "): " << a;

#define myassert( isOK ) \
 if ( !(isOK) ) { \
   std::cout << "ERROR at (filename: " << __FILE__ << ", line no: " << __LINE__ << ", function: " << __func__ << ") with condition: " << #isOK << std::endl;  __debugbreak();  }