----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, -480 )

mouseX = 0
mouseY = 0

currentX = 0
currentY = 0

velX = 0
velY = 0

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

fla = dofile ( 'multi.fla.lua' )

polygon = MOAIPolygonDeck.new ()

edges = table.getn( fla )
polygon:initEdges ( edges )

for i=1,edges do
	polygon:addEdge( fla [ i ].x1, fla [ i ].y1, fla [ i ].x2, fla [ i ].y2, fla [ i ].x3, fla [ i ].y3 )
end

polygon:triangulate ()

polygon:setRect ( -64, -64, 64, 64 )

prop = MOAIProp2D.new ()
prop:setDeck ( polygon )
layer:insertProp ( prop )

gfxQuad = MOAIGfxQuad2D.new ()
gfxQuad:setTexture ( "cathead.png" )
gfxQuad:setRect ( fla [ 1 ].x1 + -5, fla [ 1 ].y1 + -5, fla [ 1 ].x1 + 5, fla [ 1 ].y1 + 5 )
gfxQuad:setUVRect ( 0, 0, 1, 1 )

prop2 = MOAIProp2D.new ()
prop2:setDeck ( gfxQuad )
layer:insertProp ( prop2 )

function onPointerEvent ( x, y )
	mouseX, mouseY = layer:wndToWorld ( x, y )
	
end

function onMouseLeftEvent ( down )

	if down then
		--prop:moveLoc ( 1.0, 1.0, 0.1 )
		--prop:setLoc ( mouseX, mouseY )
	end
end

MOAIInputMgr.device.pointer:setCallback ( onPointerEvent )
MOAIInputMgr.device.mouseLeft:setCallback ( onMouseLeftEvent )

function main ()

	while true == true do
	
	velX = ( velX + 0.005 * ( mouseX - currentX )) * 0.95
	velY = ( velY + 0.005 * ( mouseY - currentY )) * 0.95
	
	currentX = currentX + velX;
	currentY = currentY + velY;
	
	prop:setLoc ( currentX , currentY )
	
	prop2:setLoc ( mouseX, mouseY )
	
	coroutine.yield ()
	end
	
end

spawnThread = MOAIThread.new ()
spawnThread:run ( main )

