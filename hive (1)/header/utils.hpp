
/*******************************************************************************
  Name: Ahmad Agbaria
  Project: Utils
  Date: 05/11/23
  Version: 0.5
*******************************************************************************/

#ifndef __HRD_UTILS_H__
#define __HRD_UTILS_H__
/************************************(1)***************************************/

#define STATIC_ASSERT(NAME, SHOULD_BE_TRUE) typedef int NAME[1/(SHOULD_BE_TRUE)]
/*  example - to check if the ip_ty is 4 bytes : 
 STATIC_ASSERT(fourbytetype, (sizeof(ip_ty) >= 4)); */

/************************************(2)***************************************/
/*add DEBUG_ONLY define*/
#ifdef DEBUG
#define DEBUG_ONLY(code) code
#else
#define DEBUG_ONLY(code)
#endif

/************************************(3)***************************************/
/*!!(1&mask)+!!(1<<1&mask)*/

/************************************(4)***************************************/
#define AbortIfBad(good, msg) \
{\
if(!good)\
{\
	fprintf(stderr,"%s Creation:", msg);\
	exit(0);\
}\
}

#define ExitIfBad(good) \
{\
if(!good)\
{\
	exit(0);\
}\
}

#define ReturnIfBad(good, err) \
{\
if(!good)\
{\
	return err;\
}\
}


/************************************(5)***************************************/
#if __cplusplus<201103
#define OVERRIDE
#define NOEXCEPT throw()
#else//modern c++
#define OVERRIDE override
#define NOEXCEPT noexcept
#endif//#if __cplusplus<201103

/******************************************************************************/
#endif /*(__HRD_UTILITIES_H__)*/