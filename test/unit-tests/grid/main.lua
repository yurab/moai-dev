----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

workingDir = MOAIFileSystem.getWorkingDirectory () 
MOAIFileSystem.setWorkingDirectory ( ".." )
require "testhelpers"
MOAIFileSystem.setWorkingDirectory ( workingDir )

runall = true

MOAISim.openWindow ( "test", 512, 512 )

viewport = MOAIViewport.new ()
viewport:setSize ( 512, 512 )
viewport:setScale ( 512, 512 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

grid = MOAIGrid.new ()
grid:initDiamondGrid ( 4, 4, 128, 64 )
grid:setRepeat ( true )

grid:setRow ( 1, 	0x01, 0x02, 0x01, 0x02 )
grid:setRow ( 2,	0x03, 0x04, 0x03, 0x04 )
grid:setRow ( 3,	0x01, 0x02, 0x01, 0x02 )
grid:setRow ( 4,	0x03, 0x04, 0x03, 0x04 )

tileDeck = MOAITileDeck2D.new ()
tileDeck:setTexture ( "diamond-tiles.png" )
tileDeck:setSize ( 4, 4 )

prop = MOAIProp2D.new ()
prop:setDeck ( tileDeck )
prop:setGrid ( grid )
prop:setLoc ( -256, -256 )
prop:forceUpdate ()
layer:insertProp ( prop )

-- tests

-- setTileFlags
-- getTileFlags
function setGetTileFlags ( x, y, flags )
	befflags = grid:getTileFlags ( x, y )
	
	grid:setTileFlags ( x, y, flags )
	
	test.evaluate ( grid:getTileFlags ( x, y ) == flags,
		"Set/get tile flags " .. flags )
		
	grid:setTileFlags ( x, y, befflags )
end

if runall then
	setGetTileFlags ( 1, 1, MOAIGridSpace.TILE_X_FLIP )
	setGetTileFlags ( 1, 2, MOAIGridSpace.TILE_Y_FLIP )
	setGetTileFlags ( 2, 1, MOAIGridSpace.TILE_XY_FLIP )
	setGetTileFlags ( 2, 2, MOAIGridSpace.TILE_HIDE )
	
end

local results = MOAIThread.new ()
results:run ( test.printResults )
