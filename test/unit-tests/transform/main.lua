----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

workingDir = MOAIFileSystem.getWorkingDirectory () 
MOAIFileSystem.setWorkingDirectory ( ".." )
require "testhelpers"
MOAIFileSystem.setWorkingDirectory ( workingDir )

local prop = MOAIProp2D.new ()

MOAISim.openWindow ( "test", 320, 480 )

-- addLoc
local function addLocTest ( xloc, yloc )
	local xexp, yexp = prop:getLoc ()
	xexp = xexp + xloc
	yexp = yexp + yloc
	
	prop:addLoc ( xloc, yloc )
	
	local x, y = prop:getLoc ()
	
	test.evaluate ( test.epsilon ( x, xexp ) and test.epsilon ( y, yexp ),
		"Moving Location by x = " .. xloc .. ", y = " .. yloc )
end

local values = { 0, 1, 5000, -1, -5000 }

for i = 1, 5 do
	for j = 1, 5 do
		addLocTest ( values [ i ], values [ j ] )
	end
end

-- addPiv
local function addPivTest ( xpiv, ypiv )
	local xexp, yexp = prop:getPiv ()
	xexp = xexp + xpiv
	yexp = yexp + ypiv
	
	prop:addPiv ( xpiv, ypiv )
	
	local x, y = prop:getPiv ()
	
	test.evaluate ( test.epsilon ( x, xexp ) and test.epsilon ( y, yexp ),
		"Moving Pivot by x = " .. xpiv .. ", y = " .. ypiv )
end

local values = { 0, 1, 5000, -1, -5000 }

for i = 1, 5 do
	for j = 1, 5 do
		addPivTest ( values [ i ], values [ j ] )
	end
end

-- addr
local function addRotTest ( r )
	local rexp = prop:getRot ()
	rexp = rexp + r
	
	prop:addRot ( r )
	
	local r = prop:getRot ()
	
	test.evaluate ( test.epsilon ( r, rexp ),
		"Adding ration by r = " .. r )
end

local values = { 0, 180, 500, -180, -500 }

for i = 1, 5 do
	addRotTest ( values [ i ] )
end

-- addScl
local function addSclTest ( xscl, ... )
	local xexp, yexp = prop:getScl ()
	
	if ... then
		xexp = xexp + xscl
		yexp = yexp + ...
		
		prop:addScl ( xscl, ... )
		
		local x, y = prop:getScl ()
		
		test.evaluate ( test.epsilon ( x, xexp ) and test.epsilon ( y, yexp ),
			"Adding to Scale by x = " .. xscl .. ", y = " .. ... )
	
	else
		xexp = xexp + xscl
		yexp = yexp + xscl
		
		prop:addScl ( xscl )
		
		local x, y = prop:getScl ()
		
		test.evaluate ( test.epsilon ( x, xexp ) and test.epsilon ( y, yexp ),
			"Adding to Scale by x = " .. xscl )
	end
end

local values = { 0, 1, 5000, -1, -5000 }

for i = 1, 5 do
	for j = 1, 5 do
		addSclTest ( values [ i ], values [ j ] )
	end
	addSclTest ( values [ i ] )
end

-- modelToWorld


-- move
local index = 0
local function moveTest ( xloc, yloc, r, xscl, yscl, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	local xlocexp, ylocexp = props [ i ]:getLoc ()
	local rexp = props [ i ]:getRot ()
	local xsclexp, ysclexp = props [ i ]:getScl ()
	
	xlocexp = xlocexp + xloc
	ylocexp = ylocexp + yloc
	rexp  = rexp  + r
	xsclexp = xsclexp + xscl
	ysclexp = ysclexp + yscl
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:move ( xloc, yloc, r, xscl, yscl, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xlocfin, ylocfin = props [ i ]:getLoc ()
				local rfin = props [ i ]:getRot ()
				local xsclfin, ysclfin = props [ i ]:getScl ()
				
				test.evaluate ( test.epsilon ( xlocexp, xlocfin ) and test.epsilon ( ylocexp, ylocfin )
						and test.epsilon ( rexp, rfin )
						and test.epsilon ( xsclexp, xsclfin ) and test.epsilon (ysclexp, ysclfin ),
					"Move using xloc = " .. xloc .. ", yloc = " .. yloc .. ", r = " .. r ..
						", xscl = " .. xscl .. ", yscl = " .. yscl .. ", timer = " .. length ..
						", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:move ( xloc, yloc, r, xscl, yscl, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xlocfin, ylocfin = props [ i ]:getLoc ()
				local rfin = props [ i ]:getRot ()
				local xsclfin, ysclfin = props [ i ]:getScl ()
				
				test.evaluate ( test.epsilon ( xlocexp, xlocfin ) and test.epsilon ( ylocexp, ylocfin )
						and test.epsilon ( rexp, rfin )
						and test.epsilon ( xsclexp, xsclfin ) and test.epsilon (ysclexp, ysclfin ),
					"Move using xloc = " .. xloc .. ", yloc = " .. yloc .. ", r = " .. r ..
						", xscl = " .. xscl .. ", yscl = " .. yscl .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end

local modes 	= { MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT, 
					MOAIEaseType.LINEAR, MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN,
					MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH }

moveTest ( 0, 0, 0, 0, 0, 1 )
moveTest ( 1, 1, 1, 1, 1, 0 )
moveTest ( 1, 1, 1, 1, 1, -1 )
for i = 1, 8 do
	moveTest ( 1, 1, 1, 1, 1, 0, modes [ i ] )
	moveTest ( 1, 1, 1, 1, 1, 5, modes [ i ] )
end

moveTest ( 5000, 0, 0, 0, 0, 5 )
moveTest ( 0, 5000, 0, 0, 0, 5 )
moveTest ( 0, 0, 5000, 0, 0, 5 )
moveTest ( 0, 0, 0, 5000, 0, 5 )
moveTest ( 0, 0, 0, 0, 5000, 5 )
moveTest ( -5000, 0, 0, 0, 0, 5 )
moveTest ( 0, -5000, 0, 0, 0, 5 )
moveTest ( 0, 0, -5000, 0, 0, 5 )
moveTest ( 0, 0, 0, -5000, 0, 5 )
moveTest ( 0, 0, 0, 0, -5000, 5 )
moveTest ( 5000, 5000, 5000, 5000, 5000, 5 )
moveTest ( -5000, -5000, -5000, -5000, -5000, 5 )

local results = MOAIThread.new ()
results:run ( test.printResults )

-- moveLoc
local index = 0
local function moveLocTest ( x, y, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	local xexp, yexp = props [ i ]:getLoc ()
	
	xexp = xexp + x
	yexp = yexp + y
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:moveLoc ( x, y, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getLoc ()
				
				test.evaluate ( test.epsilon ( xexp, xfin ) and test.epsilon ( yexp, yfin ),
					"Move Location using x = " .. x .. ", y = " .. y .. ", timer = " .. length .. ", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:moveLoc ( x, y, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getLoc ()
				
				test.evaluate ( test.epsilon ( xexp, xfin ) and test.epsilon ( yexp, yfin ),
					"Move Location using x = " .. x .. ", y = " .. y .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end

moveLocTest ( 0, 0, 1 )
moveLocTest ( 1, 1, 0 )
moveLocTest ( 1, 1, -1 )
for i = 1, 8 do
	moveLocTest ( 1, 1, 0, modes [ i ] )
	moveLocTest ( 1, 1, 5, modes [ i ] )
end
moveLocTest ( 5000, 0, 5 )
moveLocTest ( 0, 5000, 5 )
moveLocTest ( 5000, 5000, 5 )
moveLocTest ( -5000, 0, 5 )
moveLocTest ( 0, -5000, 5 )
moveLocTest ( -5000, -5000, 5 )

-- movePiv
local index = 0
local function movePivTest ( x, y, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	local xexp, yexp = props [ i ]:getPiv ()
	
	xexp = xexp + x
	yexp = yexp + y
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:movePiv ( x, y, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getPiv ()
				
				test.evaluate ( test.epsilon ( xexp, xfin ) and test.epsilon ( yexp, yfin ),
					"Move Pivot using x = " .. x .. ", y = " .. y .. ", timer = " .. length .. ", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:movePiv ( x, y, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getPiv ()
				
				test.evaluate ( test.epsilon ( xexp, xfin ) and test.epsilon ( yexp, yfin ),
					"Move Pivot using x = " .. x .. ", y = " .. y .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end

movePivTest ( 0, 0, 1 )
movePivTest ( 1, 1, 0 )
movePivTest ( 1, 1, -1 )
for i = 1, 8 do
	movePivTest ( 1, 1, 0, modes [ i ] )
	movePivTest ( 1, 1, 5, modes [ i ] )
end
movePivTest ( 5000, 0, 5 )
movePivTest ( 0, 5000, 5 )
movePivTest ( 5000, 5000, 5 )
movePivTest ( -5000, 0, 5 )
movePivTest ( 0, -5000, 5 )
movePivTest ( -5000, -5000, 5 )

-- moveRot
local index = 0
local function moveRotTest ( r, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	local rexp = props [ i ]:getRot ()
	
	rexp = rexp + r
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:moveRot ( r, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local rfin = props [ i ]:getRot ()
				
				test.evaluate ( test.epsilon ( rexp, rfin ),
					"Move Rotation using r = " .. r .. ", timer = " .. length .. ", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:moveRot ( r, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local rfin = props [ i ]:getRot ()
				
				test.evaluate ( test.epsilon ( rexp, rfin ),
					"Move Rotation using r = " .. r .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end


local values = { 0, 180, 500, -180, -500 }

for i = 1, 5 do
	moveRotTest ( values [ i ], i )
end
for i = 1, 8 do
	moveRotTest ( 360, 0, modes [ i ] )
	moveRotTest ( 360, 5, modes [ i ] )
end

-- moveScl
local index = 0
local function moveSclTest ( x, y, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	local xexp, yexp = props [ i ]:getScl ()
	
	xexp = xexp + x
	yexp = yexp + y
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:moveScl ( x, y, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getScl ()
				
				test.evaluate ( test.epsilon ( xexp, xfin ) and test.epsilon ( yexp, yfin ),
					"Move Scale using x = " .. x .. ", y = " .. y .. ", timer = " .. length .. ", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:moveScl ( x, y, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getScl ()
				
				test.evaluate ( test.epsilon ( xexp, xfin ) and test.epsilon ( yexp, yfin ),
					"Move Scale using x = " .. x .. ", y = " .. y .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end

moveSclTest ( 0, 0, 1 )
moveSclTest ( 1, 1, 0 )
moveSclTest ( 1, 1, -1 )
for i = 1, 8 do
	moveSclTest ( 1, 1, 0, modes [ i ] )
	moveSclTest ( 1, 1, 5, modes [ i ] )
end
moveSclTest ( 5000, 0, 5 )
moveSclTest ( 0, 5000, 5 )
moveSclTest ( 5000, 5000, 5 )
moveSclTest ( -5000, 0, 5 )
moveSclTest ( 0, -5000, 5 )
moveSclTest ( -5000, -5000, 5 )

