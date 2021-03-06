//
//  BE2DTextureData.h
//  Sapindus
//
//  Created by YuliSun on 24/02/2017.
//  Copyright © 2017 oreisoft. All rights reserved.
//

#ifndef BE2DTextureData_h
#define BE2DTextureData_h

#include "monkc_export.h"
#include "BE2DTextureData.h"

//handle image, no GL related code
class(BE2DTextureData, MCObject,
      //const types
      unsigned AUTO;
      unsigned L;
      unsigned LA;
      unsigned RGB;
      unsigned RGBA;
      
      //input
      //unsigned type;
      char* path;
      
      //output
      unsigned char* raw;
      int width;
      int height;
      int channels;
);

util(BE2DTextureData, BE2DTextureData*, newWithPathnameType, const char* path, unsigned type);
util(BE2DTextureData, BE2DTextureData*, newWithPathname, const char* path);
util(BE2DTextureData, BE2DTextureData*, newWithFilename, const char* file);

fun(BE2DTextureData, void, bye, voida);

#endif /* BE2DTextureData_h */
