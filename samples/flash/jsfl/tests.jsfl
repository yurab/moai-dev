//----------------------------------------------------------------//
// Copyright (c) 2010-2011 Zipline Games, Inc. 
// All Rights Reserved. 
// http://getmoai.com
//----------------------------------------------------------------//

//----------------------------------------------------------------//
function exportShapeToLua ( ) {
	
	var document = fl.getDocumentDOM ();
	
	if ( document.selection [ 0 ].elementType == "shape" ) {
		fl.trace ( "Found a shape1 " );
		
		for ( int i = 0; i < document.selection [ 0 ].contours.length; ++i ) {
			fl.trace ( "Found contour" + i );
		}
	}
	else {
		fl.trace ( "Selecting: " + document.selection [ 0 ].elementType );
	}
}

exportShapeToLua ( );