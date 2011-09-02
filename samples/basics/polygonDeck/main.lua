----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, -480 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

fla = dofile ( 'test2.fla.lua' )

polygon = MOAIPolygonDeck.new ()

verts = table.getn( fla )
polygon:initVertices ( verts )

for i=1,verts do
	polygon:addVertex ( fla [ i ].x, fla [ i ].y )
end

polygon:triangulate ()

polygon:setRect ( -64, -64, 64, 64 )

prop = MOAIProp2D.new ()
prop:setDeck ( polygon )
layer:insertProp ( prop )

gfxQuad = MOAIGfxQuad2D.new ()
gfxQuad:setTexture ( "cathead.png" )
gfxQuad:setRect ( fla [ 1 ].x + -5, fla [ 1 ].y + -5, fla [ 1 ].x + 5, fla [ 1 ].y + 5 )
gfxQuad:setUVRect ( 0, 0, 1, 1 )

prop2 = MOAIProp2D.new ()
prop2:setDeck ( gfxQuad )
layer:insertProp ( prop2 )

prop:moveRot ( 360, 1.5 )
