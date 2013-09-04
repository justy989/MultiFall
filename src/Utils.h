#ifndef UTILS_H
#define UTILS_H

//Defines
#define LIMIT_60_FPS 1.0f / 60.0f

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
#define SafeDelete(x) { delete x; x = 0; }

#define CLAMP(x, min, max) if( x < min ){x = min;} if (x > max){x = max;}

#define BYTE_MAX 255

//Typedefs
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;

#endif