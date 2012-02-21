----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

workingDir = MOAIFileSystem.getWorkingDirectory () 
MOAIFileSystem.setWorkingDirectory ( ".." )
require "testhelpers"
MOAIFileSystem.setWorkingDirectory ( workingDir )

local runall = true

MOAISim.openWindow ( "test", 512, 512 )

-- getWorldDir
function getWorldDirTest ()
	local base = MOAITransform.new ()
	
	base:addScl ( -2, -2 )
	base:forceUpdate ()
	x, y = base:getWorldDir ()
	test.evaluate ( test.epsilon ( x, -1 ) and test.epsilon ( y, 0 ),
		"getWorldDir test #1" )
		
	base:addRot ( 90 )
	base:forceUpdate ()
	x, y = base:getWorldDir ()
	test.evaluate ( test.epsilon ( x, 0 ) and test.epsilon ( y, -1 ),
		"getWorldDir test #2" )
		
	base:addRot ( -180 )
	base:forceUpdate ()
	x, y = base:getWorldDir ()
	test.evaluate ( test.epsilon ( x, 0 ) and test.epsilon ( y, 1 ),
		"getWorldDir test #3" )
		
	base:addRot ( -135 )
	base:forceUpdate ()
	x, y = base:getWorldDir ()
	test.evaluate ( test.epsilon ( x, .7071 ) and test.epsilon ( y, -.7071 ),
		"getWorldDir test #4" )
		
	x, y = base:getWorldDir ( 1 )
	test.evaluate ( test.epsilon ( x, .7071 ) and test.epsilon ( y, -.7071 ),
		"getWorldDir test #5" )
	
	base:addLoc ( 100, 100 )
	base:forceUpdate ()
	x, y = base:getWorldDir ()
	test.evaluate ( test.epsilon ( x, .7071 ) and test.epsilon ( y, -.7071 ),
		"getWorldDir test #6" )
		
	base:addScl ( -10, -10 )
	base:forceUpdate ()
	x, y = base:getWorldDir ()
	test.evaluate ( test.epsilon ( x, .7071 ) and test.epsilon ( y, -.7071 ),
		"getWorldDir test #7" )
end

-- getWorldLoc
function getWorldLocTest ()
	local base = MOAITransform.new ()
	
	x, y = base:getWorldLoc ()
	test.evaluate ( test.epsilon ( x, 0 ) and test.epsilon ( y, 0 ),
		"getWorldLoc test #1" )
	
	base:addRot ( 180 )
	base:forceUpdate ()
	x, y = base:getWorldLoc ()
	test.evaluate ( test.epsilon ( x, 0 ) and test.epsilon ( y, 0 ),
		"getWorldLoc test #2" )
		
	base:addScl ( 10, 10 )
	base:forceUpdate ()
	x, y = base:getWorldLoc ()
	test.evaluate ( test.epsilon ( x, 0 ) and test.epsilon ( y, 0 ),
		"getWorldLoc test #3" )
	
	base:addLoc ( 10, 10 )
	base:forceUpdate ()
	x, y = base:getWorldLoc ()
	test.evaluate ( test.epsilon ( x, 10 ) and test.epsilon ( y, 10 ),
		"getWorldLoc test #4" )
	
	base:addLoc ( -5000, -5000 )
	base:forceUpdate ()
	x, y = base:getWorldLoc ()
	test.evaluate ( test.epsilon ( x, -4990 ) and test.epsilon ( y, -4990 ),
		"getWorldLoc test #5" )
end

-- getWorldRot
function getWorldRotTest ()
	local base = MOAITransform.new ()
	
	r = base:getWorldRot ()
	test.evaluate ( test.epsilon ( r, 90 ),
		"getWorldRot test #1" )
	
	base:addLoc ( 5000, 5000 )
	base:forceUpdate ()
	r = base:getWorldRot ()
	test.evaluate ( test.epsilon ( r, 90 ),
		"getWorldRot test #2" )
	
	base:addScl ( 100, 100 )
	base:forceUpdate ()
	r = base:getWorldRot ()
	test.evaluate ( test.epsilon ( r, 90 ),
		"getWorldRot test #3" )
		
	base:addRot ( 180 )
	base:forceUpdate ()
	r = base:getWorldRot ()
	test.evaluate ( test.epsilon ( r, -90 ),
		"getWorldRot test #4" )
		
	base:addRot ( 405 )
	base:forceUpdate ()
	r = base:getWorldRot ()
	test.evaluate ( test.epsilon ( r, -135 ),
		"getWorldRot test #5" )
end

-- getWorldScl
function getWorldSclTest ()
	local base = MOAITransform.new ()
	
	x, y = base:getWorldScl ()
	test.evaluate ( test.epsilon ( x, 1 ) and test.epsilon ( y, 1 ),
		"getWorldLoc test #1" )
	
	base:addLoc ( 5000, 5000 )
	base:forceUpdate ()
	x, y = base:getWorldScl ()
	test.evaluate ( test.epsilon ( x, 1 ) and test.epsilon ( y, 1 ),
		"getWorldLoc test #2" )
	
	base:addRot ( 180 )
	base:forceUpdate ()
	x, y = base:getWorldScl ()
	test.evaluate ( test.epsilon ( x, 1 ) and test.epsilon ( y, 1 ),
		"getWorldLoc test #3" )
	
	base:addScl ( 10, 10 )
	base:forceUpdate()
	x, y = base:getWorldScl ()
	test.evaluate ( test.epsilon ( x, 11 ) and test.epsilon ( y, 11 ),
		"getWorldLoc test #4" )
	
	base:addScl ( -20, -20 )
	base:forceUpdate()
	x, y = base:getWorldScl ()
	test.evaluate ( test.epsilon ( x, 9 ) and test.epsilon ( y, 9 ),
		"getWorldLoc test #5" )
end

-- tests

if runall then

	getWorldDirTest ()
	getWorldLocTest ()
	getWorldRotTest ()
	getWorldSclTest ()

end

local results = MOAIThread.new ()
results:run ( test.printResults )
