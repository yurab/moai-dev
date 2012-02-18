----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

require "testhelpers"

local prop = MOAIProp2D.new ()

prop:setLoc ( 5, 0 )
prop:setRot ( 0 )
prop:setScl ( 1, 1 )
prop:forceUpdate ()

print ( "Set location to x = 5, y = 0" )
print ( "Set rotation to 0" )
print ( "Set scale to x = 1, y = 1" )

x, y = prop:modelToWorld ( 0, 0 )

print ( "\nPerform modelToWorld function on x = 0, y = 0" )
print ( "Result x = " .. x .. ", y = " .. y )
