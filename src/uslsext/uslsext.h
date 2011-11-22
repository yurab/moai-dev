// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef USLSEXT_H
#define USLSEXT_H

#define MOAI_OS_NACL 1
#ifdef MOAI_OS_NACL
	#define USE_BOX2D 1
	#define USE_CHIPMUNK 0
	#define USE_FREETYPE 0
	#define USE_CURL 0
	#define USE_SQL 0
	#define USE_EXPAT 0
	#define USE_OPENGLES1 0
#else
	#define USE_CURL 1
	#define USE_SQL 1
	#define USE_EXPAT 1
	#define USE_OPENGLES1 1
#endif

#include <uslsext/pch.h>

#include <uslsext/USAdapterInfo.h>
#include <uslsext/USAffine2D.h>
#include <uslsext/USBinarySearch.h>
#include <uslsext/USBox.h>
#include <uslsext/USCgt.h>
#include <uslsext/USColor.h>
#include <uslsext/USCurve.h>
#include <uslsext/USData.h>
#include <uslsext/USDataIOTask.h>
#include <uslsext/USDelegate.h>
#include <uslsext/USDistance.h>
#include <uslsext/USHexDump.h>
//#include <uslsext/USFsm.h>
//#include <uslsext/USFsmStateFactory.h>
#if USE_CURL
#include <uslsext/USHttpTask.h>
#include <uslsext/USHttpTask_impl.h>
#endif
#include <uslsext/USInterpolate.h>
#include <uslsext/USIntersect.h>
#include <uslsext/USLexStream.h>
#include <uslsext/USMathConsts.h>
#include <uslsext/USMatrix.h>
#include <uslsext/USMatrix3x3.h>
#include <uslsext/USMatrix4x4.h>
#include <uslsext/USMercator.h>
#include <uslsext/USMutex.h>
#include <uslsext/USMutex_posix.h>
#include <uslsext/USMutex_win32.h>
#include <uslsext/USParser.h>
#include <uslsext/USPlane.h>
#include <uslsext/USPolar.h>
#include <uslsext/USPolygon2D.h>
#include <uslsext/USPolyScanner.h>
#include <uslsext/USQuad.h>
#include <uslsext/USQuadCoord.h>
#include <uslsext/USRadixSort16.h>
#include <uslsext/USRadixSort32.h>
#include <uslsext/USSqlBinding.h>
#include <uslsext/USSqlColumn.h>
#include <uslsext/USSqlRecord.h>
#include <uslsext/USSqlStatement.h>
#include <uslsext/USSqlValue.h>
#include <uslsext/USSurface2D.h>
#include <uslsext/USSyntaxNode.h>
#include <uslsext/USSyntaxScanner.h>
#include <uslsext/USTask.h>
#include <uslsext/USTaskThread.h>
#include <uslsext/USThread.h>
#include <uslsext/USThread_posix.h>
#include <uslsext/USThread_win32.h>
#include <uslsext/USTrig.h>
#include <uslsext/USTypedPtr.h>
//#if USE_EXPAT
#include <uslsext/USXmlReader.h>
//#endif

//================================================================//
// uslsext
//================================================================//
namespace uslsext {

	//----------------------------------------------------------------//
	void			Init			();
}

#endif
