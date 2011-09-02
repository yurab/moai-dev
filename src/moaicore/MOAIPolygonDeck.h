// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPOLYGONDECK_H
#define	MOAIPOLYGONDECK_H

#include <moaicore/MOAIDeck2D.h>

//================================================================//
// MOAIPolygonDeck
//================================================================//
/**	@name MOAIPolygonDeck
	@text Simple Polygon deck object.
*/
class MOAIPolygonDeck :
	public MOAIDeck2D {

public:

	struct Edge2D {
		int		mType;

		USVec2D mOrigin;
		USVec2D mDestination;

		Edge2D* mNext;
		Edge2D* mPrevious;

		Edge2D* mHelper;
	};

private:

	//----------------------------------------------------------------//
	struct Triangle {
		USVec2D mVerts [ 3 ];
	};	

	Triangle*	mTriangles;
	int			mNumTriangles;

	//----------------------------------------------------------------//
	enum {
		CHAINTOP,
		CHAINBOTTOM,
		CHAINBOTH,
	};

	enum {
		START,
		END,
		BOTTOM,
		TOP,
		MERGE,
		SPLIT,
	};

	struct ChainVertex {
		USVec2D mPosition;
		int		mChain;
	};

	USList < Edge2D* >	mEdges;
	USVec2D*			mVertices;
	int					mCurVertex;
	int					mNumVertices;

	USList < Edge2D* >	mDebugEdges;

	enum {
		TRIANGLES = 0x000001,
		WIREFRAME = 0x000002,
		OUTLINE   = 0x000004,
		MONOTONE  = 0x000008,
	};

	int					mDrawModes;
	
	int GetVertexType ( Edge2D& vertex );
	float GetAngle ( USVec2D vBegin, USVec2D vMid, USVec2D vEnd );
	void AddTriangle ( USVec2D point1, USVec2D point2, USVec2D point3 );

	bool LineLineIntersect ( USVec2D p1Start, USVec2D p1End, USVec2D p2Start, USVec2D p2End, USVec2D& pOut );
	bool LineRayIntersect ( USVec2D p1Start, USVec2D p1End, USVec2D p2Start, USVec2D p2End, USVec2D& pOut );

	USVec2D EvaluateCubicBezier ( float t, USVec2D p0, USVec2D p1, USVec2D p2, USVec2D p3 );

	USRect			mRect;

	USLuaRef		mOnDraw;
	USLuaRef		mOnRect;

	//----------------------------------------------------------------//
	static int		_addVertex			( lua_State* L );
	static int		_initVertices		( lua_State* L );

	static int		_setDrawCallback	( lua_State* L );
	
	static int		_setRect			( lua_State* L );
	static int		_setRectCallback	( lua_State* L );

	static int		_triangulate		( lua_State* L );

public:
	
	DECL_LUA_FACTORY ( MOAIPolygonDeck )
	
	//----------------------------------------------------------------//
	bool			Bind					();
	void			DrawPatch				( u32 idx, float xOff, float yOff, float xScale, float yScale );
	USRect			GetBounds				( u32 idx, MOAIDeckRemapper* remapper );
	void			LoadShader				();
					MOAIPolygonDeck			();
					~MOAIPolygonDeck		();
	void			RegisterLuaClass		( USLuaState& state );
	void			RegisterLuaFuncs		( USLuaState& state );

	//----------------------------------------------------------------//
	void InitEdges ();
	void Sweep ();
	void Triagulate ( );

	void DebugPrintTriangles ();
	void DebugPrintEdges ();
	void DebugPrintVertices ();

	Edge2D* FindEdgeAbove ( USList < Edge2D* > &sweepStatus, USVec2D position );

	Edge2D* InsertNewEdge ( Edge2D* start, Edge2D* end );
};

#endif
