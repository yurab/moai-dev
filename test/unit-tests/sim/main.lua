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

runall = true

-- setLoopFlags
function setGetLoopFlagsTest ( flags )
	befflags = MOAISim.getLoopFlags ()

	MOAISim.setLoopFlags ( flags )
	
	test.evaluate ( MOAISim.getLoopFlags () == flags,
		"Set/get loop flags " .. flags )
	
	MOAISim.setLoopFlags ( befflags )
end

-- tests

if runall then
	setGetLoopFlagsTest ( MOAISim.SIM_LOOP_FORCE_STEP )
	setGetLoopFlagsTest ( MOAISim.SIM_LOOP_ALLOW_BOOST )
	setGetLoopFlagsTest ( MOAISim.SIM_LOOP_ALLOW_SPIN )
	setGetLoopFlagsTest ( MOAISim.SIM_LOOP_NO_DEFICIT )
	setGetLoopFlagsTest ( MOAISim.SIM_LOOP_NO_SURPLUS )
	setGetLoopFlagsTest ( MOAISim.SIM_LOOP_LONG_DELAY )
	setGetLoopFlagsTest ( MOAISim.SIM_LOOP_RESET_CLOCK )
end

local results = MOAIThread.new ()
results:run ( test.printResults )
