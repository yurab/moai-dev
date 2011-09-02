// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moaicore/MOAIPolygonDeck.h>
#include <moaicore/MOAIDeckRemapper.h>
#include <moaicore/MOAIDraw.h>
#include <moaicore/MOAIGfxDevice.h>
#include <moaicore/MOAIGrid.h>
#include <moaicore/MOAILogMessages.h>
#include <moaicore/MOAIShaderMgr.h>
#include <moaicore/MOAISurfaceSampler2D.h>
#include <moaicore/MOAITransform.h>
#include <moaicore/MOAIVertexFormatMgr.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@name	setDrawCallback
	@text	Sets the callback to be issued when draw events occur.
			The callback's parameters are ( number index, number xOff, number yOff, number xScale, number yScale ).

	@in		MOAIPolygonDeck self
	@in		function callback
	@out	nil
*/
int MOAIPolygonDeck::_setDrawCallback ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIPolygonDeck, "UF" )
	
	self->mOnDraw.SetRef ( state, 2, false );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setRect
	@text	Set the model space dimensions of the deck's default rect.
	
	@in		MOAIPolygonDeck self
	@in		number xMin
	@in		number yMin
	@in		number xMax
	@in		number yMax
	@out	nil
*/
int MOAIPolygonDeck::_setRect ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIPolygonDeck, "UNNNN" )
	
	self->mRect = state.GetRect < float >( 2 );

	return 0;
}

//----------------------------------------------------------------//
/**	@name	setRectCallback
	@text	Sets the callback to be issued when the size of a deck item
			needs to be determined.
			The callback's parameters are ( number index ).

	@in		MOAIPolygonDeck self
	@in		function callback
	@out	nil
*/
int MOAIPolygonDeck::_setRectCallback ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIPolygonDeck, "UF" )
	
	self->mOnRect.SetRef ( state, 2, false );
	
	return 0;
}

int MOAIPolygonDeck::_addVertex ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIPolygonDeck, "UNN" )

	self->mVertices [ self->mCurVertex ]  = state.GetVec2D < float >( 2 );

	++self->mCurVertex;

	return 0;
}

int MOAIPolygonDeck::_initVertices ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIPolygonDeck, "UN" )

	self->mNumVertices = state.GetValue < int >( 2, 0 );
	self->mVertices = new USVec2D [ self->mNumVertices ];

	self->mCurVertex = 0;

	return 0;
}

int MOAIPolygonDeck::_triangulate ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIPolygonDeck, "U" )

	self->InitEdges ();
}

//================================================================//
// MOAIPolygonDeck
//================================================================//

//----------------------------------------------------------------//
bool MOAIPolygonDeck::Bind () {

	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get ();
	
	gfxDevice.SetTexture ();
	gfxDevice.SetVertexPreset ( MOAIVertexFormatMgr::XYC );

	return true;
}

//----------------------------------------------------------------//
void MOAIPolygonDeck::DrawPatch ( u32 idx, float xOff, float yOff, float xScale, float yScale ) {
	
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get ();

	if ( mDrawModes & TRIANGLES ) {

		gfxDevice.SetPenColor ( 0.25f, 0.25f, 0.25f, 1.0f );

		for ( int i = 0; i < mNumTriangles; ++i ) {
			gfxDevice.BeginPrim ( GL_TRIANGLES );

				gfxDevice.WriteVtx ( mTriangles [ i ].mVerts [ 0 ] );
				gfxDevice.WritePenColor4b ();

				gfxDevice.WriteVtx (  mTriangles [ i ].mVerts [ 1 ] );
				gfxDevice.WritePenColor4b ();

				gfxDevice.WriteVtx (  mTriangles [ i ].mVerts [ 2 ] );
				gfxDevice.WritePenColor4b ();

			gfxDevice.EndPrim ();
		}
	}
	
	if ( mDrawModes & WIREFRAME ) { 

		gfxDevice.SetPenColor ( 0.0f, 0.0f, 1.0f, 1.0f );

		for ( int i = 0; i < mNumTriangles; ++i ) {

			gfxDevice.BeginPrim ( GL_LINES );

				gfxDevice.WriteVtx ( mTriangles [ i ].mVerts [ 0 ] );
				gfxDevice.WritePenColor4b ();

				gfxDevice.WriteVtx (  mTriangles [ i ].mVerts [ 1 ] );
				gfxDevice.WritePenColor4b ();

			gfxDevice.EndPrim ();

			gfxDevice.BeginPrim ( GL_LINES );

				gfxDevice.WriteVtx (  mTriangles [ i ].mVerts [ 1 ] );
				gfxDevice.WritePenColor4b ();

				gfxDevice.WriteVtx (  mTriangles [ i ].mVerts [ 2 ] );
				gfxDevice.WritePenColor4b ();

			gfxDevice.EndPrim ();

			gfxDevice.BeginPrim ( GL_LINES );

				gfxDevice.WriteVtx (  mTriangles [ i ].mVerts [ 2 ] );
				gfxDevice.WritePenColor4b ();

				gfxDevice.WriteVtx (  mTriangles [ i ].mVerts [ 0 ] );
				gfxDevice.WritePenColor4b ();

			gfxDevice.EndPrim ();
		}
	}
	
	if ( mDrawModes & OUTLINE ) { 

		gfxDevice.SetPenColor ( 1.0f, 0.0f, 0.0f, 1.0f );

		for ( int i = 0; i < mNumVertices; ++i ) {

			gfxDevice.BeginPrim ( GL_LINES );

				gfxDevice.WriteVtx ( mVertices [ i ] );
				gfxDevice.WritePenColor4b ();

				gfxDevice.WriteVtx (  mVertices [ ( i + 1 ) % mNumVertices ] );
				gfxDevice.WritePenColor4b ();

			gfxDevice.EndPrim ();
		}

	}

	if ( mDrawModes & MONOTONE ) {

		USLeanLink < Edge2D* > *monotonePolys = mDebugEdges.Head ();
		for ( ; monotonePolys ; monotonePolys = monotonePolys->Next ()) {

			Edge2D *head = monotonePolys->Data ();
			Edge2D *iterate = head;

			do {
		
				gfxDevice.BeginPrim ( GL_LINES );

					gfxDevice.SetPenColor ( 1.0f, 0.5f, 0.0f, 1.0f );
					gfxDevice.WriteVtx ( iterate->mOrigin );
					gfxDevice.WritePenColor4b ();

					gfxDevice.SetPenColor ( 0.5f, 1.0f, 0.0f, 1.0f );
					gfxDevice.WriteVtx (  iterate->mDestination );
					gfxDevice.WritePenColor4b ();

				gfxDevice.EndPrim ();

				iterate = iterate->mNext;

			} while ( iterate != head );
		}
	}
	
}

//----------------------------------------------------------------//
USRect MOAIPolygonDeck::GetBounds ( u32 idx, MOAIDeckRemapper* remapper ) {
	
	if ( this->mOnRect ) {
	
		idx = remapper ? remapper->Remap ( idx ) : idx;
	
		USLuaStateHandle state = this->mOnRect.GetSelf ();
		lua_pushnumber ( state, idx );
		state.DebugCall ( 1, 4 );
		
		USRect rect;
		rect.mXMin = state.GetValue < float >( -4, 0.0f );
		rect.mYMin = state.GetValue < float >( -3, 0.0f );
		rect.mXMax = state.GetValue < float >( -2, 0.0f );
		rect.mYMax = state.GetValue < float >( -1, 0.0f );
		
		rect.Bless ();
		return rect;
	}
	return this->mRect;
}

//----------------------------------------------------------------//
void MOAIPolygonDeck::LoadShader () {

	if ( this->mShader ) {
		MOAIGfxDevice::Get ().SetShader ( this->mShader );
	}
	else {
		MOAIShaderMgr::Get ().BindShader ( MOAIShaderMgr::LINE_SHADER );
	}
}

//----------------------------------------------------------------//
MOAIPolygonDeck::MOAIPolygonDeck () {
	
	RTTI_SINGLE ( MOAIDeck2D )
	mDrawModes = 0;
	mDrawModes |= TRIANGLES;
	mDrawModes |= WIREFRAME;
	mDrawModes |= OUTLINE;
	mDrawModes |= MONOTONE;
	//InitTest ( );
}

//----------------------------------------------------------------//
MOAIPolygonDeck::~MOAIPolygonDeck () {
}

//----------------------------------------------------------------//
void MOAIPolygonDeck::RegisterLuaClass ( USLuaState& state ) {

	this->MOAIDeck2D::RegisterLuaClass ( state );
}

//----------------------------------------------------------------//
void MOAIPolygonDeck::RegisterLuaFuncs ( USLuaState& state ) {

	this->MOAIDeck2D::RegisterLuaFuncs ( state );

	luaL_Reg regTable [] = {
		{ "addVertex",				_addVertex },
		{ "initVertices",			_initVertices },
		{ "setDrawCallback",		_setDrawCallback },
		{ "setRect",				_setRect },
		{ "setRectCallback",		_setRectCallback },
		{ "triangulate",			_triangulate },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

void MOAIPolygonDeck::InitEdges () {

	float fMultiplier = 32.0f;

	//square
	/*const int dwVerts = 4;
	mVertices = new USVec2D[dwVerts];

	mVertices [ 0 ].mX = fMultiplier * 0.0f;
	mVertices [ 0 ].mY = fMultiplier * 0.0f;

	mVertices [ 1 ].mX = fMultiplier * 1.0f;
	mVertices [ 1 ].mY = fMultiplier * 0.0f;
 
	mVertices [ 2 ].mX = fMultiplier * 1.0f;
	mVertices [ 2 ].mY = fMultiplier * 1.0f;

	mVertices [ 3 ].mX = fMultiplier * 0.0f;
	mVertices [ 3 ].mY = fMultiplier * 1.0f;*/

	//diamond
	/*const int dwVerts = 4;
	mVertices = new USVec2D[dwVerts];

	mVertices [ 0 ].mX = fMultiplier * 0.0f;
	mVertices [ 0 ].mY = fMultiplier * 0.0f;

	mVertices [ 1 ].mX = fMultiplier * 1.0f;
	mVertices [ 1 ].mY = fMultiplier * 0.0f;
 
	mVertices [ 2 ].mX = fMultiplier * 1.1f;
	mVertices [ 2 ].mY = fMultiplier * 1.1f;

	mVertices [ 3 ].mX = fMultiplier * 0.1f;
	mVertices [ 3 ].mY = fMultiplier * 1.0f;*/

	//penta-thing
	/*const int dwVerts = 5;
	mVertices = new USVec2D [ dwVerts ];

	mVertices [ 0 ].mX = fMultiplier * 0.0f;
	mVertices [ 0 ].mY = fMultiplier * 0.0f;

	mVertices [ 1 ].mX = fMultiplier * 1.0f;
	mVertices [ 1 ].mY = fMultiplier * 0.0f;
 
	mVertices [ 2 ].mX = fMultiplier * 1.5f;
	mVertices [ 2 ].mY = fMultiplier * 1.5f;

	mVertices [ 3 ].mX = fMultiplier * 1.3f;
	mVertices [ 3 ].mY = fMultiplier * 1.2f;

	mVertices [ 4 ].mX = fMultiplier * 0.1f;
	mVertices [ 4 ].mY = fMultiplier * 1.0f;*/

	//merge
	/*const int dwVerts = 6;
	fMultiplier *= -1.0f;
	mVertices = new USVec2D [ dwVerts ];
	mEdges.Clear();

	mVertices [ 0 ].mX = -fMultiplier * 0.0f;
	mVertices [ 0 ].mY = -fMultiplier * 0.0f;

	mVertices [ 1 ].mX = -fMultiplier * 2.0f;
	mVertices [ 1 ].mY = -fMultiplier * 0.75f;

	mVertices [ 2 ].mX = -fMultiplier * 0.75f;
	mVertices [ 2 ].mY = -fMultiplier * 3.0f;

	mVertices [ 3 ].mX = -fMultiplier * 0.9f;
	mVertices [ 3 ].mY = -fMultiplier * 1.5f;

	mVertices [ 4 ].mX = -fMultiplier * 0.1f;
	mVertices [ 4 ].mY = -fMultiplier * 1.9f;

	mVertices [ 5 ].mX = -fMultiplier * 1.0f;
	mVertices [ 5 ].mY = -fMultiplier * 0.75f;*/

	//merge2
	/*const int dwVerts = 8;
	fMultiplier *= 1.0f;
	mVertices = new USVec2D [ dwVerts ];
	mEdges.Clear();

	mVertices [ 0 ].mX = -fMultiplier * 0.0f;
	mVertices [ 0 ].mY = -fMultiplier * 0.0f;

	//new
	mVertices [ 1 ].mX = -fMultiplier * 2.5f;
	mVertices [ 1 ].mY = -fMultiplier * 0.5f;

	mVertices [ 2 ].mX = -fMultiplier * 2.0f;
	mVertices [ 2 ].mY = -fMultiplier * 0.75f;

	//new
	mVertices [ 3 ].mX = -fMultiplier * 2.4f;
	mVertices [ 3 ].mY = -fMultiplier * 1.0f;

	mVertices [ 4 ].mX = -fMultiplier * 0.75f;
	mVertices [ 4 ].mY = -fMultiplier * 3.0f;

	mVertices [ 5 ].mX = -fMultiplier * 0.9f;
	mVertices [ 5 ].mY = -fMultiplier * 1.5f;

	mVertices [ 6 ].mX = -fMultiplier * 0.1f;
	mVertices [ 6 ].mY = -fMultiplier * 1.9f;

	mVertices [ 7 ].mX = -fMultiplier * 0.89f;
	mVertices [ 7 ].mY = -fMultiplier * 0.75f;*/

	//edges
	//mNumVertices = dwVerts;

	DebugPrintVertices ();

	Edge2D *edge = NULL;
	Edge2D *firstEdge = new Edge2D;
	Edge2D *previousEdge = firstEdge;

	firstEdge->mOrigin = mVertices [ 0 ];
	firstEdge->mDestination = mVertices [ 1 ];

	mEdges.PushFront ( firstEdge );

	for( int i = 1; i < mNumVertices; ++i ) {

		edge = new Edge2D;

		edge->mOrigin = mVertices [ i % mNumVertices ];
		edge->mDestination = mVertices [ ( i + 1 ) % mNumVertices ];

		edge->mPrevious =  previousEdge;
		previousEdge->mNext = edge;

		mEdges.PushBack ( edge );

		previousEdge = edge;
	}

	edge->mNext = firstEdge;
	firstEdge->mPrevious = edge;

	mTriangles = new Triangle [ mNumVertices - 2 ];

	Sweep ();
	printf("\n\n");

	mDebugEdges.Clear ();

	DebugPrintEdges ();

	mNumTriangles = 0;
	while( mEdges.Count () > 0 ) {

		printf ( "Triangulating %d\n", mEdges.Count ());
		Triagulate ();

		printf ( "\nEdges left:\n" );
		DebugPrintEdges ();
	}

	DebugPrintTriangles ();
}

//----------------------------------------------------------------//
//clockwise
float MOAIPolygonDeck::GetAngle ( USVec2D vBegin, USVec2D vMid, USVec2D vEnd ) 
{
	USVec2D v1 = vBegin;
	v1.Sub ( vMid );

	USVec2D v2 = vEnd;
	v2.Sub ( vMid );

	float fAngle = -( atan2 ( v2.mY, v2.mX ) - atan2 ( v1.mY, v1.mX ));

	if( fAngle < 0.0f )
	{
		fAngle = ( 2.0f * ( float )PI ) + fAngle;
	}

	return fAngle;
}

//----------------------------------------------------------------//
bool MOAIPolygonDeck::LineLineIntersect ( USVec2D p1Start, USVec2D p1End, USVec2D p2Start, USVec2D p2End, USVec2D& pOut )
{
	const float fEpsilon = 0.01f;

	float fDenominator  = ( p2End.mY - p2Start.mY ) * ( p1End.mX - p1Start.mX ) - ( p2End.mX - p2Start.mX ) * ( p1End.mY -p1Start.mY  );
	float fNumerator1   = ( p2End.mX - p2Start.mX ) * ( p1Start.mY - p2Start.mY ) - ( p2End.mY - p2Start.mY ) * ( p1Start.mX - p2Start.mX );
	float fNumerator2   = ( p1End.mX - p1Start.mX ) * ( p1Start.mY - p2Start.mY ) - ( p1End.mY - p1Start.mY ) * ( p1Start.mX - p2Start.mX );

	// return middle for identical lines
	if ( abs ( fNumerator1 ) < fEpsilon && abs ( fNumerator2 ) < fEpsilon && abs ( fDenominator ) < fEpsilon) {
		pOut.mX = ( p1Start.mX + p1End.mX ) / 2;
		pOut.mY = ( p1Start.mY + p1End.mY ) / 2;
		return true;
	}

	if ( abs ( fDenominator ) < fEpsilon ) {
		return false;
	}

	float fT1 = fNumerator1 / fDenominator;
	float fT2 = fNumerator2 / fDenominator;
	if ( fT1 < 0 || fT1 > 1 || fT2 < 0 || fT2 > 1 ) {
		return false;
	}

	pOut.mX = p1Start.mX + fT1 * ( p1End.mX - p1Start.mX );
	pOut.mY = p1Start.mY + fT1 * ( p1End.mY - p1Start.mY );

	return true;
}

//----------------------------------------------------------------//
bool MOAIPolygonDeck::LineRayIntersect ( USVec2D p1Start, USVec2D p1End, USVec2D p2Start, USVec2D p2Dir, USVec2D& pOut )
{
	const float fEpsilon = 0.01f;

	float fDenominator  = ( p2Dir.mY ) * ( p1End.mX - p1Start.mX ) - ( p2Dir.mX ) * ( p1End.mY -p1Start.mY  );
	float fNumerator1   = ( p2Dir.mX ) * ( p1Start.mY - p2Start.mY ) - ( p2Dir.mY ) * ( p1Start.mX - p2Start.mX );
	float fNumerator2   = ( p1End.mX - p1Start.mX ) * ( p1Start.mY - p2Start.mY ) - ( p1End.mY - p1Start.mY ) * ( p1Start.mX - p2Start.mX );

	// return middle for identical lines
	if ( abs ( fNumerator1 ) < fEpsilon && abs ( fNumerator2 ) < fEpsilon && abs ( fDenominator ) < fEpsilon) {
		pOut.mX = ( p1Start.mX + p1End.mX ) / 2;
		pOut.mY = ( p1Start.mY + p1End.mY ) / 2;
		return true;
	}

	if ( abs ( fDenominator ) < fEpsilon ) {
		return false;
	}

	float fT1 = fNumerator1 / fDenominator;
	float fT2 = fNumerator2 / fDenominator;
	if ( fT1 < 0 || fT1 > 1 || fT2 < 0 ) {
		return false;
	}

	pOut.mX = p1Start.mX + fT1 * ( p1End.mX - p1Start.mX );
	pOut.mY = p1Start.mY + fT1 * ( p1End.mY - p1Start.mY );

	return true;
}
//----------------------------------------------------------------//
void MOAIPolygonDeck::AddTriangle ( USVec2D point1, USVec2D point2, USVec2D point3 ) {
	 
	/*printf ( "Add Triangle ( %f, %f ), ( %f, %f ), ( %f, %f )\n", point1.mX, point1.mY,
																   point2.mX, point2.mY,
																   point3.mX, point3.mY );*/

	mTriangles [ mNumTriangles ].mVerts [ 0 ] = point1;
	mTriangles [ mNumTriangles ].mVerts [ 1 ] = point2;
	mTriangles [ mNumTriangles ].mVerts [ 2 ] = point3;
	++mNumTriangles;
}

void MOAIPolygonDeck::DebugPrintEdges () {

	USLeanLink < Edge2D* > *iterator =  mEdges.Head();

	for ( ; iterator ; iterator = iterator->Next ()) {

		printf ( "Edges ( %f, %f ) -> ( %f, %f ), this %p, next %p, previous %p \n", iterator->Data()->mOrigin.mX, iterator->Data()->mOrigin.mY,
																    iterator->Data()->mDestination.mX, iterator->Data()->mDestination.mY,
																    iterator->Data(), iterator->Data()->mNext, iterator->Data()->mPrevious );
	}
}

//----------------------------------------------------------------//
void MOAIPolygonDeck::DebugPrintTriangles () {

	printf ( "Printing Tris...\n" );
	for ( int i = 0; i < mNumTriangles; ++i )
	{
		printf ( "Triangles ( %f, %f ), ( %f, %f ), ( %f, %f )\n", mTriangles [ i ].mVerts [ 0 ].mX, mTriangles [ i ].mVerts [ 0 ].mY,
																   mTriangles [ i ].mVerts [ 1 ].mX, mTriangles [ i ].mVerts [ 1 ].mY,
																   mTriangles [ i ].mVerts [ 2 ].mX, mTriangles [ i ].mVerts [ 2 ].mY );
	}
	printf ( "...Done\n\n" );

}

void MOAIPolygonDeck::DebugPrintVertices () {

	printf ( "Printing Verts...\n" );
	for ( int i = 0; i < mNumVertices; ++i )
	{
		printf ( "Vertex %d ( %f, %f )\n", i, mVertices [ i ].mX, mVertices [ i ].mY );
	}
	printf ( "...Done\n\n" );

}
//----------------------------------------------------------------//
//qsort for sorting
void swap( MOAIPolygonDeck::Edge2D ** a, MOAIPolygonDeck::Edge2D ** b ) {
  MOAIPolygonDeck::Edge2D * t = *a; *a = *b; *b = t;
}

void qsortX( MOAIPolygonDeck::Edge2D ** arr, int begin, int end )
{
  if ( end > begin + 1 ) {

	int pivotI = ( begin + end ) / 2;
    float pivot = arr [ pivotI ]->mOrigin.mX;
	swap ( &arr [ pivotI ] , &arr [ begin ] );

	int left = begin + 1;
	int right = end;

    while ( left < right ) {
		if ( arr [ left ]->mOrigin.mX <= pivot ) {
			left++;
		}
		else {
			swap ( &arr [ left ], &arr [--right ] );
		}
    }
    swap ( &arr [--left ] , &arr [ begin ] );
    qsortX ( arr, begin, left );
    qsortX ( arr, right, end );
  }
}

//avl tree for edges
/*class AVLTree {
	struct AVLNode {
		float value;
		AVLNode *left;
		AVLNode *right;
		int height;
	};

	void insert ( AVLNode *node, float value ) {
		if ( node == NULL )
		{

		}
	}

	AVLNode *root;
};*/


int MOAIPolygonDeck::GetVertexType ( Edge2D& vertex ) {

	USVec2D previous;
	if ( vertex.mPrevious ) {
		previous = vertex.mPrevious->mOrigin;
	}
	else {
		//print error
		printf ( "ERROR: no previous vertex\n" );
		return -1;
	}

	float fAngle = GetAngle ( previous, vertex.mOrigin, vertex.mDestination );
	int type = -1;

	//printf( "angle %f, vert %f, %f, %f\n", fAngle, previous.mX, vertex.mOrigin.mX, vertex.mDestination.mX );
	if ( fabs ( fAngle ) > PI ) {

		if (( previous.mX < vertex.mOrigin.mX ) &&  ( vertex.mDestination.mX < vertex.mOrigin.mX )) {
			type = MERGE;
		}
		else if (( previous.mX > vertex.mOrigin.mX ) &&  ( vertex.mDestination.mX > vertex.mOrigin.mX )) {
			type = SPLIT;
		}
		else {

			if( previous.mX < vertex.mOrigin.mX ) {
				type = TOP;
			}
			else {
				type = BOTTOM;
			}
		}
	}
	else {
		if (( previous.mX < vertex.mOrigin.mX ) &&  ( vertex.mDestination.mX < vertex.mOrigin.mX )) {
			type = END;
		}
		else if (( previous.mX > vertex.mOrigin.mX ) &&  ( vertex.mDestination.mX > vertex.mOrigin.mX )) {
			type = START;
		}
		else {

			if( previous.mX < vertex.mOrigin.mX ) {
				type = TOP;
			}
			else {
				type = BOTTOM;
			}
		}
	}

	return type;
}

MOAIPolygonDeck::Edge2D* MOAIPolygonDeck::InsertNewEdge ( Edge2D* start, Edge2D* end ) {
		
	Edge2D* endPrevious = end->mPrevious;
	Edge2D* startPrevious = start->mPrevious;

	Edge2D *newEdge1 = new Edge2D;

	newEdge1->mOrigin = start->mOrigin;
	newEdge1->mDestination = end->mOrigin;

	newEdge1->mNext = end;
	newEdge1->mPrevious = startPrevious;

	startPrevious->mNext = newEdge1;
	end->mPrevious = newEdge1;

	mEdges.PushBack ( newEdge1 );

	//reverse direction
	Edge2D *newEdge2 = new Edge2D;

	newEdge2->mOrigin = end->mOrigin;
	newEdge2->mDestination = start->mOrigin;

	newEdge2->mNext = start;
	newEdge2->mPrevious = endPrevious;

	endPrevious->mNext = newEdge2;
	start->mPrevious = newEdge2;

	mEdges.PushBack ( newEdge2 );

	return newEdge1;
}

MOAIPolygonDeck::Edge2D* MOAIPolygonDeck::FindEdgeAbove ( USList < Edge2D* > &sweepStatus, USVec2D position ) {

	USLeanLink < Edge2D* > *iterator =  sweepStatus.Head();

	Edge2D *closestEdge = NULL;
	
	float lowestY = FLT_MAX;

	USVec2D above;
	above.mX = 0.0f;
	above.mY = 1.0f;

	USVec2D out;

	for ( ; iterator ; iterator = iterator->Next ()) {

		bool bIntersects = LineRayIntersect ( iterator->Data ()->mOrigin, iterator->Data ()->mDestination, position, above, out );
		
		if ( bIntersects && out.mY < lowestY ) {
			lowestY = out.mY;
			closestEdge = iterator->Data ();
		}
	}

	if ( !closestEdge ) {
		printf ( "ERROR: CLOSEST EDGE NOT FOUND\n" );
	}

	return closestEdge;
}

USVec2D MOAIPolygonDeck::EvaluateCubicBezier ( float t, USVec2D p0, USVec2D p1, USVec2D p2, USVec2D p3 ) {

	float cx = 3.0f * ( p1.mX - p0.mX );
	float bx = 3.0f * ( p2.mX - p1.mX ) - cx;
	float ax = p3.mX - p0.mX - cx - bx;

	float cy = 3.0f * ( p1.mY - p0.mY );
	float by = 3.0f * ( p2.mY - p1.mY ) - cy;
	float ay = p3.mY - p0.mY - cy - by;

	USVec2D position;

	float t2 = t * t;
	float t3 = t * t2;

	position.mX = ax * t3 + bx * t2 + cx * t + p0.mX;
	position.mY = ay * t3 + by * t2 + cy * t + p0.mY;

	return position;
}

void MOAIPolygonDeck::Sweep () {

	Edge2D **edgePointers = new Edge2D* [ mEdges.Count() ];

	USLeanLink < Edge2D* > *iterator =  mEdges.Head();
	int i = 0;
	for ( ; iterator ; iterator = iterator->Next ()) {
		edgePointers [ i ] = iterator->Data ();
		edgePointers [ i ]->mHelper = NULL;
		++i;
	}

	qsortX ( edgePointers, 0, mNumVertices );

	USList < Edge2D* > sweepStatus;

	for( int i = 0; i < mNumVertices; ++i ) {
		
		int type = GetVertexType ( *edgePointers [ i ] );
		edgePointers [ i ]->mType = type;

		printf ( "\nindex %d type = %d\n", i, type );
		printf ( "THIS EDGE (%f,%f)->(%f,%f) \n", edgePointers [ i ]->mOrigin.mX, 
				                                                   edgePointers [ i ]->mOrigin.mY, 
																   edgePointers [ i ]->mDestination.mX, 
																   edgePointers [ i ]->mDestination.mY );

		Edge2D* edge;

		switch ( type ) {
		case START:

			printf ( "handle START\n" );

			//insert cc edge (this edge) with this as helper
			edgePointers [ i ]->mPrevious->mHelper = edgePointers [ i ];
			sweepStatus.PushFront ( edgePointers [ i ]->mPrevious );

			printf ( "inserting: (%f,%f)->(%f,%f) with this as helper\n", edgePointers [ i ]->mPrevious->mOrigin.mX, 
				                                                          edgePointers [ i ]->mPrevious->mOrigin.mY, 
																		  edgePointers [ i ]->mPrevious->mDestination.mX, 
																		  edgePointers [ i ]->mPrevious->mDestination.mY );

			break;
		case END:

			printf ( "handle END\n" );
			//printf ( "helper type = %d\n", edgePointers [ i ]->mHelper->mType );
			if ( edgePointers [ i ]->mHelper && edgePointers [ i ]->mHelper->mType == MERGE ) {

				printf ( "Insert edge, from this to: (%f,%f)->(%f,%f) \n", edgePointers [ i ]->mHelper->mOrigin.mX, 
				                                                           edgePointers [ i ]->mHelper->mOrigin.mY, 
																		   edgePointers [ i ]->mHelper->mDestination.mX, 
																		   edgePointers [ i ]->mHelper->mDestination.mY );

				Edge2D* newEdge =  InsertNewEdge ( edgePointers [ i ], edgePointers [ i ]->mHelper );

			}

			sweepStatus.Remove( edgePointers [ i ] );

			//will lower ever be inserted?
			//sweepStatus.Remove( edgePointers [ i ]->mPrevious );

			break;

		//case BOTTOM:
		case TOP:

			printf ( "handle TOP\n" );
			//will lower ever be inserted?
			//sweepStatus.Remove( edgePointers [ i ]->mPrevious );
			
			edge = FindEdgeAbove ( sweepStatus, edgePointers [ i ]->mOrigin );

			if ( edge->mHelper && edge->mHelper->mType == MERGE ) {

				Edge2D* newEdge =  InsertNewEdge ( edgePointers [ i ], edge->mHelper );

				newEdge->mType = MERGE;
				edgePointers [ i ] = newEdge;
			}


			edge->mHelper = edgePointers [ i ];

			break;

		//case TOP:
		case BOTTOM:
			printf ( "handle BOTTOM\n" );
			sweepStatus.Remove( edgePointers [ i ] );

			if ( edgePointers [ i ]->mHelper && edgePointers [ i ]->mHelper->mType == MERGE ) {

				Edge2D* newEdge = InsertNewEdge ( edgePointers [ i ], edgePointers [ i ]->mHelper );

				newEdge->mType = MERGE;
				edgePointers [ i ] = newEdge;
			}

			edgePointers [ i ]->mPrevious->mHelper = edgePointers [ i ];
			sweepStatus.PushFront ( edgePointers [ i ]->mPrevious );

			break;
		case MERGE:

			printf ( "handle MERGE\n" );
			//will bottom ever be inserted?
			//sweepStatus.Remove( edgePointers [ i ] );

			sweepStatus.Remove( edgePointers [ i ] );

			if ( edgePointers [ i ]->mHelper->mType == MERGE ) {

				printf ( "Insert edge, from this to: (%f,%f)->(%f,%f) \n", edgePointers [ i ]->mHelper->mOrigin.mX, 
				                                                           edgePointers [ i ]->mHelper->mOrigin.mY, 
																		   edgePointers [ i ]->mHelper->mDestination.mX, 
																		   edgePointers [ i ]->mHelper->mDestination.mY );

				Edge2D* newEdge = InsertNewEdge ( edgePointers [ i ], edgePointers [ i ]->mHelper );

				newEdge->mType = MERGE;
				edgePointers [ i ] = newEdge;
			}

			//printf ( "helper type = %d\n", edgePointers [ i ]->mHelper->mType );
			
			edge = FindEdgeAbove ( sweepStatus, edgePointers [ i ]->mOrigin );

			if ( edge->mHelper->mType == MERGE ) {

				printf ( "Insert edge, from this to: (%f,%f)->(%f,%f) \n", edgePointers [ i ]->mHelper->mOrigin.mX, 
				                                                           edgePointers [ i ]->mHelper->mOrigin.mY, 
																		   edgePointers [ i ]->mHelper->mDestination.mX, 
																		   edgePointers [ i ]->mHelper->mDestination.mY );

				Edge2D* newEdge = InsertNewEdge ( edgePointers [ i ], edge->mHelper);

				newEdge->mType = MERGE;
				edgePointers [ i ] = newEdge;
			}

			edge->mHelper = edgePointers [ i ];

			break;
		case SPLIT:
			
			printf ( "handle SPLIT\n" );
			edge = FindEdgeAbove ( sweepStatus, edgePointers [ i ]->mOrigin );

			/*printf ( "split inserting: (%f,%f)->(%f,%f) with this as helper\n", edgePointers [ i ]->mPrevious->mOrigin.mX, 
				                                                          edgePointers [ i ]->mPrevious->mOrigin.mY, 
																		  edgePointers [ i ]->mPrevious->mDestination.mX, 
																		  edgePointers [ i ]->mPrevious->mDestination.mY );*/

			edgePointers [ i ]->mPrevious->mHelper = edgePointers [ i ];
			sweepStatus.PushFront ( edgePointers [ i ]->mPrevious );
			
			InsertNewEdge ( edgePointers [ i ], edge->mHelper );
			//no helper for upper? 
			//sweepStatus.PushFront ( edgePointers [ i ] );

			edge->mHelper = edgePointers [ i ];

			break;
		}

	}

}

//----------------------------------------------------------------//
void MOAIPolygonDeck::Triagulate () {
	
	Edge2D *head = mEdges.Head ()->Data ();
	mDebugEdges.PushBack ( mEdges.Head ()->Data () );

	Edge2D *minEdge = head;
	Edge2D *maxEdge = head;

	Edge2D *iterate = head->mNext;

	//find min and max indices
	while ( iterate != head ) {
		
		if ( iterate->mOrigin.mX < minEdge->mOrigin.mX ) {
			minEdge = iterate;
		}

		if ( iterate->mOrigin.mX > maxEdge->mOrigin.mX ) {
			maxEdge = iterate;
		}

		iterate = iterate->mNext;
	}

	//merge vertices to sorted list
	USList < ChainVertex > sortedVertices;

	ChainVertex newVertex;

	//insert first vertex (min)
	newVertex.mChain = CHAINBOTH;
	newVertex.mPosition = minEdge->mOrigin;

	sortedVertices.PushFront ( newVertex );

	mEdges.Remove ( minEdge );

	Edge2D *forwardChainI = minEdge->mNext;
	Edge2D *backwardChainI = minEdge->mPrevious;

	while ( forwardChainI != maxEdge || backwardChainI != maxEdge ) {

		if ( forwardChainI->mOrigin.mX < backwardChainI->mOrigin.mX ) {

			if ( forwardChainI != maxEdge ) {

				newVertex.mChain = CHAINTOP;
				newVertex.mPosition = forwardChainI->mOrigin;

				mEdges.Remove ( forwardChainI );
				sortedVertices.PushFront ( newVertex );

				forwardChainI = forwardChainI->mNext;
			}
		}
		else {

			if ( backwardChainI != maxEdge ) {

				newVertex.mChain = CHAINBOTTOM;
				newVertex.mPosition = backwardChainI->mOrigin;

				mEdges.Remove ( backwardChainI );
				sortedVertices.PushFront ( newVertex );	

				backwardChainI = backwardChainI->mPrevious;
			}
		}
	}

	//push last vertex (max)
	newVertex.mChain = CHAINBOTH;
	newVertex.mPosition = maxEdge->mOrigin;

	sortedVertices.PushFront ( newVertex );

	mEdges.Remove ( maxEdge );

	//triangulate monotone poly
	USList < ChainVertex > reflexVertices;
	
	reflexVertices.Clear ();

	//get first two verts add in
	reflexVertices.PushFront ( sortedVertices.Back ());
	sortedVertices.PopBack ();

	int stackChain = sortedVertices.Back ().mChain;

	reflexVertices.PushFront ( sortedVertices.Back ());
	sortedVertices.PopBack ();

	while ( sortedVertices.Count () > 0 ) {

		printf ( "\nvertex %f, %f\n", sortedVertices.Back ().mPosition.mX, sortedVertices.Back ( ).mPosition.mY );
		ChainVertex curVertex = sortedVertices.Back ();
		sortedVertices.PopBack ();

		if ( stackChain == CHAINBOTTOM ) {
			printf (  "stack chain = BOTTOM\n" );
		}
		else {
			printf (  "stack chain = TOP\n" );
		}
		if ( curVertex.mChain != stackChain ) {
			
			printf ( "Does not match stack chain\n" );
			ChainVertex first; 
			ChainVertex second;

			while ( reflexVertices.Count () > 1 ) {

				first = reflexVertices.Tail ()->Data ();
				second = reflexVertices.Tail ()->Prev ()->Data ();

				AddTriangle ( first.mPosition, second.mPosition, curVertex.mPosition );

				reflexVertices.PopBack ();
			}
			reflexVertices.PushFront ( curVertex );
		}
		else {

			//TODO, clean this up
			printf ( "Matches stack chain\n" );
			ChainVertex last = reflexVertices.Head ()->Data ();
			ChainVertex secondToLast = reflexVertices.Head ()->Next ()->Data ();	 
			
			float angle;
			if ( curVertex.mChain == CHAINBOTTOM ) {
				angle = GetAngle( curVertex.mPosition, last.mPosition, secondToLast.mPosition );
				printf ( "angle = %f \nbetween %f, %f, %f\n", ( angle / PI ) * 180, curVertex.mPosition.mX, last.mPosition.mX, secondToLast.mPosition.mX );
			}
			else {
				angle = GetAngle( secondToLast.mPosition, last.mPosition, curVertex.mPosition );
				printf ( "angle = %f \n", ( angle / PI ) * 180 );
			}

			
			while ( abs ( angle ) < PI && reflexVertices.Count () > 1 ) {

				AddTriangle ( last.mPosition, secondToLast.mPosition, curVertex.mPosition );
				reflexVertices.PopFront ();

				if ( reflexVertices.Count () > 1 ) {
					last = reflexVertices.Head ()->Data ();
					secondToLast = reflexVertices.Head ()->Next ()->Data ();

					if ( curVertex.mChain == CHAINBOTTOM ) {
						angle = GetAngle( curVertex.mPosition, last.mPosition, secondToLast.mPosition );
						printf ( "angle = %f \nbetween %f, %f, %f\n", ( angle / PI ) * 180, curVertex.mPosition.mX, last.mPosition.mX, secondToLast.mPosition.mX );
					}
					else {
						angle = GetAngle( secondToLast.mPosition, last.mPosition, curVertex.mPosition );
						printf ( "angle = %f \n", ( angle / PI ) * 180 );
					}
				}
			}
			reflexVertices.PushFront ( curVertex );
		}

		stackChain = curVertex.mChain;
	}
}
