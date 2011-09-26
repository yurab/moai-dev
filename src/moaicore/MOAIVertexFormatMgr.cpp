// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moaicore/MOAIGfxDevice.h>
#include <moaicore/MOAIVertexFormat.h>
#include <moaicore/MOAIVertexFormatMgr.h>

//================================================================//
// MOAIVertexFormatMgr
//================================================================//

//----------------------------------------------------------------//
const MOAIVertexFormat& MOAIVertexFormatMgr::GetPreset ( u32 presetID ) {

	assert ( presetID < TOTAL_PRESETS );
	return this->mFormats [ presetID ];
}

//----------------------------------------------------------------//
MOAIVertexFormatMgr::MOAIVertexFormatMgr () {
	
	MOAIVertexFormat* format;
	
	format = &this->mFormats [ XYC ];

#if USE_OPENGLES1	
	format->DeclareAttribute ( XYC_POSITION, GL_FLOAT, 2, GL_VERTEX_ARRAY, false );
	format->DeclareAttribute ( XYC_COLOR, GL_UNSIGNED_BYTE, 4, GL_COLOR_ARRAY, true );
#else
	format->DeclareAttribute ( XYC_POSITION, GL_FLOAT, 2, MOAIVertexFormat::ARRAY_VERTEX, false );
	format->DeclareAttribute ( XYC_COLOR, GL_UNSIGNED_BYTE, 4, MOAIVertexFormat::ARRAY_COLOR, true );
#endif
	
	format = &this->mFormats [ XYUVC ];

#if USE_OPENGLES1
	format->DeclareAttribute ( XYUVC_POSITION, GL_FLOAT, 2, GL_VERTEX_ARRAY, false );
	format->DeclareAttribute ( XYUVC_TEXCOORD, GL_FLOAT, 2, GL_TEXTURE_COORD_ARRAY, false );
	format->DeclareAttribute ( XYUVC_COLOR, GL_UNSIGNED_BYTE, 4, GL_COLOR_ARRAY, true );
#else
	format->DeclareAttribute ( XYUVC_POSITION, GL_FLOAT, 2, MOAIVertexFormat::ARRAY_VERTEX, false );
	format->DeclareAttribute ( XYUVC_TEXCOORD, GL_FLOAT, 2, MOAIVertexFormat::ARRAY_TEX_COORD, false );
	format->DeclareAttribute ( XYUVC_COLOR, GL_UNSIGNED_BYTE, 4, MOAIVertexFormat::ARRAY_COLOR, true );
#endif

}

//----------------------------------------------------------------//
MOAIVertexFormatMgr::~MOAIVertexFormatMgr () {
}
