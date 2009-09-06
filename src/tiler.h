/*
 * tilermgr.h
 *
 * DMM library support functions for TI OMAP processors.
 *
 * Copyright (C) 2009-2010 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _TILERMGR_H_
#define _TILERMGR_H_

/*****************************

 TILER Public API    
 
 It is the public TILER API for users of the TILER resource manager.
 It is not the private API for implementors of the TILER resource manager.
 Private API is defining RPC, PAT programming etc ...   
 
 This API is made of several parts:
 1 - Allocation of Tiled buffers in System space
 2 - Allocation of page mode buffers in System space ;
 3 - Conversion of addresses to Tiler space ;
 4 - Map/Unmap functions ;
 5 - Miscellaneous utility functions ;
*/
 
 
/*******************************************
     
 Type definitions
 
*/

/** 

 System Space Pointer

 A pointer to a tiled buffer in System Space (MPU space)
 A tiled buffer is a buffer in TILER container which is bi-dimensional (not using page mode)

*/
typedef void* SSPtr;


/**

  Tiler Space Pointer
  
  A pointer to a tiled buffer in Tiler Space (33 bit adressing)
  33rd bit is not contained in Tiler Space Pointer since this bit is always 1 for Tiler Space addressing
  and is generated by the hardware.

*/
typedef void* TSPtr;

/**

  A pointer to a page mode buffer in TILER container in System space.
  
*/
typedef void* SSPageModePtr;

/**

  A pointer to a page mode buffer in TILER container in Tiler space (33 bit addressing)
  
*/
typedef void* TSPageModePtr;

/**

 Buffer length in bytes
 
*/
typedef unsigned long bytes_t;

/**

 Length in pixels
 
*/
typedef unsigned short pixels_t;

/**

 Generic type of a list of physical pages.
 Since it is OS dependent, the API is relying on casting and utility functions to be provided by the user
 
*/
 
typedef void* pageListElem_t;

/**

 Page list iterator.
 NULL is returned for last page
 
*/
typedef pageListElem_t (*pageIterator)(pageListElem_t currentPage);  

/**

 Page analysis to get the Physical pointer for the page
 
*/
typedef void* (*pagePointer)(pageListElem_t currentPage);

/**

 Pixel format
 
 Page mode is managed with a different set of APIs so it is not encoded in the pixel format.
 
*/
enum kPixelFormat {
	k8bit=1,
	k16bit=2,
	k32bit=3
};

/**
 NOTE : The SW description used for rotation and mirroring is redundant contrary to the HW encoding.
 Different rotation + mirroring settings can be equivalent.
 For instance : horizontal + vertical mirroring is equivalent to a rotation by 180 degree
*/

/**

 Rotation
 
*/
enum kRotation {
	k0degrees=1,
	k90degrees=2,
	k180degrees=4,
	k270degrees=8
};

/**

  Mirroring
  
*/
enum kMirroring {
	kNoMirroring=0,
	kHorizontalMirroring=16,
	kVerticalMirroring=32
};

/**

 Reference corner
 
 The corner pointed at with the Tiler Space Address
 
*/
 
enum kRefCorner {
    kTopLeft,
    kTopRight,
    kBottomLeft,
    kBottomRight
};

/*******************************************
     
 Utility functions
 
*/

/**

Get reference corner for a given orientation

*/


enum kRefCorner tilerGetRefCorner(TSPtr tiledBuffer);


/*******************************************
     
 Allocations
 
*/
 
/**

 tilerAlloc
 
 Allocate a tiled buffer in TILER container and return a System Space address. So, no rotation is assumed.
 The securityZone is for future developments and can be set to 0.
 A NULL pointer is returned in case of error.
 Allocation is always using full 4k pages. A 4k page partially used by the buffer will be fully reserved by the buffer.
*/
extern SSPtr tilerAlloc(enum kPixelFormat pixelFormat,pixels_t width, pixels_t height, short securityZone);

/**
 tilerRealloc

 Similar to the tilerAlloc function and used to resize a buffer. The same pixel format and security zone is assumed.
*/
extern SSPtr tilerRealloc(SSPtr oldBuffer,pixels_t newWidth, pixels_t newHeight);

/**
 tilerPageModeAlloc
 
 Allocate a page mode buffer in TILER container and returns a system space address.
 A NULL pointer is returned in case of error. 
*/
extern SSPageModePtr tilerPageModeAlloc(bytes_t length);

/**
 tilerPageModeRealloc
 
 Similar to tilerPageModeAlloc and used to resize a buffer.
*/
extern SSPageModePtr tilerPageModeRealloc(SSPageModePtr oldBuffer, bytes_t newLength);

/**
 tilerFree
 
 Free a tiled buffer from the TILER container.
*/
extern int tilerFree(SSPtr tilerBuffer);

/**
 tilerPageModeFree

 Free a page mode buffer from the TILER container.
*/
extern int tilerPageModeFree(SSPageModePtr tilerBuffer);


/*******************************************
     
 Convertion to TILER space
 
*/

/**
 convertToTilerSpace

 Convert a System Space address for a tiled buffer to a Tiler Space Adress using a Rotation and Mirroring setting.
 A NULL pointer is converted to a NULL pointer.
*/
extern TSPtr convertToTilerSpace(SSPtr systemPointer,short rotationAndMirroring);

/**
 convertPageModeToTilerSpace

 Convert a System Space address for a page mode buffer to a Tiler Space Adress.
 A NULL pointer is converted to a NULL pointer.
*/
extern TSPageModePtr convertPageModeToTilerSpace(SSPageModePtr systemPointer);

/*******************************************
     
 Map/Unmap
 
*/
 
/**
 mapInPageMode

 Map a list of pages to TILER container in page mode.
 The function pointers are used to iterate the list of page and get the physical pointer to each page.
 The list of page structure is OS dependent.
*/ 
SSPageModePtr mapInPageMode(pageListElem_t* listOfPages, pageIterator nextPage, pagePointer getPagePhysicalPointer);
void unmapFromPageMode(SSPageModePtr buffer,unsigned short nbPages);

/**
 mapInTiledMode

 Map a list of pages to TILER container in tiled mode.
*/ 
SSPtr mapInTiledMode(pageListElem_t* listOfPages,enum kPixelFormat pixelFormat,short securityZone, unsigned short  horizontalPages, unsigned short  verticalPages, pageIterator nextPage, pagePointer getPagePhysicalPointer);
void unmapFromTiledMode(SSPtr buffer,unsigned short horizontalPages, unsigned short verticalPages);

extern int tilerInit();
extern int tilerDeinit();
extern unsigned long *tilervirt2phys(unsigned long *v);

#endif