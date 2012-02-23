----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

workingDir = MOAIFileSystem.getWorkingDirectory () 
MOAIFileSystem.setWorkingDirectory ( ".." )
require "testhelpers"
MOAIFileSystem.setWorkingDirectory ( workingDir )
MOAISim.openWindow ( "test", 512, 512 )

camera = MOAICameraFitter2D.new ()
runall = true

-- tests
function getSetFitModeTest ( flags )
	befflags = camera:getFitMode ()
	
	camera:setFitMode ( flags )
	
	test.evaluate ( camera:getFitMode () == flags,
		"Get/set fit mode to " .. flags )
	
	camera:setFitMode ( befflags )
end

if runall then
	getSetFitModeTest ( camera.FITTING_MODE_SEEK_LOC )
	getSetFitModeTest ( camera.FITTING_MODE_SEEK_SCALE )
	getSetFitModeTest ( camera.FITTING_MODE_APPLY_ANCHORS )
	getSetFitModeTest ( camera.FITTING_MODE_APPLY_BOUNDS )
	getSetFitModeTest ( camera.FITTING_MODE_DEFAULT )
	getSetFitModeTest ( camera.FITTING_MODE_MASK )
end

local results = MOAIThread.new ()
results:run ( test.printResults )
