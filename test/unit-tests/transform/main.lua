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
local runall = false

local modes 	= { MOAIEaseType.EASE_IN, MOAIEaseType.EASE_OUT, MOAIEaseType.FLAT,
					MOAIEaseType.LINEAR, MOAIEaseType.SMOOTH, MOAIEaseType.SOFT_EASE_IN,
					MOAIEaseType.SOFT_EASE_OUT, MOAIEaseType.SOFT_SMOOTH }

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

-- addr
local function addRotTest ( r )
	local rexp = prop:getRot ()
	rexp = rexp + r
	
	prop:addRot ( r )
	
	local r = prop:getRot ()
	
	test.evaluate ( test.epsilon ( r, rexp ),
		"Adding ration by r = " .. r )
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

-- modelToWorld
local function modelToWorldTest ( xin, yin, xout, yout )
	x, y = prop:modelToWorld ( xin, yin )
	
	test.evaluate ( test.epsilon ( x, xout ) and test.epsilon ( y, yout ),
		"Model to World with x = " .. xin .. ", y = " .. yin )
end

-- move
local index = 1
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

-- moveLoc
local index = 1
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

-- movePiv
local index = 1
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

-- moveRot
local index = 1
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

-- moveScl
local index = 1
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

-- seekLoc
local index = 1
local function seekLocTest ( x, y, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:seekLoc ( x, y, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getLoc ()
				
				test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
					"Seek Location x = " .. x .. ", y = " .. y .. ", timer = " .. length .. ", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:seekLoc ( x, y, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getLoc ()
				
				test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
					"Seek Location x = " .. x .. ", y = " .. y .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end

-- seekPiv
local index = 1
local function seekPivTest ( x, y, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:seekPiv ( x, y, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getPiv ()
				
				test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
					"Seek Pivot x = " .. x .. ", y = " .. y .. ", timer = " .. length .. ", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:seekPiv ( x, y, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getPiv ()
				
				test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
					"Seek Pivot x = " .. x .. ", y = " .. y .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end

-- seekRot
local index = 1
local function seekRotTest ( r, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:seekRot ( r, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local rfin = props [ i ]:getRot ()
				
				test.evaluate ( test.epsilon ( r, rfin ),
					"Seek Rotation r = " .. r .. ", timer = " .. length .. ", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:seekRot ( r, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local rfin = props [ i ]:getRot ()
				
				test.evaluate ( test.epsilon ( r, rfin ),
					"Seek Rotation r = " .. r .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end

-- seekScl
local index = 1
local function seekSclTest ( x, y, length, ... )
	local i = index
	local props = {}
	props [ i ] = MOAIProp2D.new ()
	
	if ... then
		local thread = test.requestThread ()
		local mode = ...
		thread:run (
			function ()
				local action = props [ i ]:seekScl ( x, y, length, mode )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getScl ()
				
				test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
					"Seek Scale x = " .. x .. ", y = " .. y .. ", timer = " .. length .. ", mode = " .. mode )
			end
		)
	else
		local thread = test.requestThread ()
		thread:run (
			function ()
				local action = props [ i ]:seekScl ( x, y, length )
				if action ~= nil then
					while action:isBusy () do
						coroutine.yield ()
					end
				end
				
				local xfin, yfin = props [ i ]:getScl ()
				
				test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
					"Seek Scale x = " .. x .. ", y = " .. y .. ", timer = " .. length )
			end
		)
		
	end
	
	index = index + 1
end

-- setLoc
local index = 1
local function setLocTest ( x, y )
	prop:setLoc ( x, y )
	
	local xfin, yfin = prop:getLoc ()
	
	test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
		"Setting Location to x = " .. x .. ", y = " .. y )
end

-- setParent

-- setPiv
local index = 1
local function setPivTest ( x, y )
	prop:setPiv ( x, y )
	
	local xfin, yfin = prop:getPiv ()
	
	test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
		"Setting Pivot to x = " .. x .. ", y = " .. y )
end

-- setRot
local index = 1
local function setRotTest ( r )
	prop:setRot ( r )
	
	local rfin = prop:getRot ()
	
	test.evaluate ( test.epsilon ( r, rfin ),
		"Setting Rotation to r = " .. r )
end

-- setScl
local index = 1
local function setSclTest ( x, y )
	prop:setScl ( x, y )
	
	local xfin, yfin = prop:getScl ()
	print ( xfin, yfin )
	
	test.evaluate ( test.epsilon ( x, xfin ) and test.epsilon ( y, yfin ),
		"Setting Scale to x = " .. x .. ", y = " .. y )
end

-- worldToModel
local function worldToModelTest ( xin, yin, xout, yout )
	x, y = prop:worldToModel ( xin, yin )
	
	test.evaluate ( test.epsilon ( x, xout ) and test.epsilon ( y, yout ),
		"World to Model with x = " .. xin .. ", y = " .. yin )
end

-- tests

prop:setLoc ( 0, 0 )
prop:setRot ( 0 )
prop:setScl ( 1, 1 )
prop:forceUpdate ()
modelToWorldTest ( 1, 1, 1, 1 )
modelToWorldTest ( 5000, 1, 5000, 1 )
modelToWorldTest ( -5000, 5000, -5000, 5000 )
modelToWorldTest ( 5000, -5000, 5000, -5000 )

prop:setLoc ( 5000, 1 )
prop:setRot ( 180 )
prop:setScl ( 3, 3 )
prop:forceUpdate ()
modelToWorldTest ( 1, 1, 4997, -2 )
modelToWorldTest ( 5000, 1, -10000, -2 )
modelToWorldTest ( -5000, 5000, 20000, -14999 )
modelToWorldTest ( 5000, -5000, -10000, 15001 )

prop:setLoc ( -10, -5000 )
prop:setRot ( -400 )
prop:setScl ( 10 )
prop:forceUpdate ()
modelToWorldTest ( 1, 1, -4990, -2 )
modelToWorldTest ( 5000, 1, -10000, -2 )
modelToWorldTest ( -5000, 5000, 20000, -14999 )
modelToWorldTest ( 5000, -5000, -10000, 15001 )

prop:setLoc ( 0, 0 )
prop:setRot ( 0 )
prop:setScl ( 1, 1 )
prop:forceUpdate ()
worldToModelTest ( 1, 1, 1, 1 )
worldToModelTest ( 5000, 1, 5000, 1 )
worldToModelTest ( -5000, 5000, -5000, 5000 )
worldToModelTest ( 5000, -5000, 5000, -5000 )

prop:setLoc ( 5000, 1 )
prop:setRot ( 180 )
prop:setScl ( 3, 3 )
prop:forceUpdate ()
worldToModelTest ( 4997, -2, 1, 1 )
worldToModelTest ( -10000, -2, 5000, 1 )
worldToModelTest ( 20000, -14999, -5000, 5000 )
worldToModelTest ( -10000, 15001, 5000, -5000 )

prop:setLoc ( -10, -5000 )
prop:setRot ( -400 )
prop:setScl ( 10 )
prop:forceUpdate ()
worldToModelTest ( 4997, -2, 1, 1 )
worldToModelTest ( -10000, -2, 5000, 1 )
worldToModelTest ( 20000, -14999, -5000, 5000 )
worldToModelTest ( -10000, 15001, 5000, -5000 )

if runall then

	-- addLoc
	local values = { 0, 1, 5000, -1, -5000 }

	for i = 1, 5 do
		for j = 1, 5 do
			addLocTest ( values [ i ], values [ j ] )
		end
	end
	
	-- addPiv
	local values = { 0, 1, 5000, -1, -5000 }

	for i = 1, 5 do
		for j = 1, 5 do
			addPivTest ( values [ i ], values [ j ] )
		end
	end
	
	-- addRot
	local values = { 0, 180, 500, -180, -500 }

	for i = 1, 5 do
		addRotTest ( values [ i ] )
	end
	
	-- addScl
	local values = { 0, 1, 5000, -1, -5000 }

	for i = 1, 5 do
		for j = 1, 5 do
			addSclTest ( values [ i ], values [ j ] )
		end
		addSclTest ( values [ i ] )
	end
	
	-- modelToWorld
	
	-- move
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
	
	-- moveLoc
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
	local values = { 0, 180, 500, -180, -500 }

	for i = 1, 5 do
		moveRotTest ( values [ i ], i - 2 )
	end
	for i = 1, 8 do
		moveRotTest ( 360, 0, modes [ i ] )
		moveRotTest ( 360, 5, modes [ i ] )
	end
	
	-- moveScl
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
	
	-- seekLoc
	seekLocTest ( 0, 0, 1 )
	seekLocTest ( 1, 1, 0 )
	seekLocTest ( 1, 1, -1 )
	for i = 1, 8 do
		seekLocTest ( 1, 1, 0, modes [ i ] )
		seekLocTest ( 1, 1, 5, modes [ i ] )
	end
	seekLocTest ( 5000, 0, 5 )
	seekLocTest ( 0, 5000, 5 )
	seekLocTest ( 5000, 5000, 5 )
	seekLocTest ( -5000, 0, 5 )
	seekLocTest ( 0, -5000, 5 )
	seekLocTest ( -5000, -5000, 5 )
	
	-- seekPiv
	seekPivTest ( 0, 0, 1 )
	seekPivTest ( 1, 1, 0 )
	seekPivTest ( 1, 1, -1 )
	for i = 1, 8 do
		seekPivTest ( 1, 1, 0, modes [ i ] )
		seekPivTest ( 1, 1, 5, modes [ i ] )
	end
	seekPivTest ( 5000, 0, 5 )
	seekPivTest ( 0, 5000, 5 )
	seekPivTest ( 5000, 5000, 5 )
	seekPivTest ( -5000, 0, 5 )
	seekPivTest ( 0, -5000, 5 )
	seekPivTest ( -5000, -5000, 5 )
	
	-- seekRot
	local values = { 0, 180, 500, -180, -500 }

	for i = 1, 5 do
		seekRotTest ( values [ i ], i - 2 )
	end
	for i = 1, 8 do
		seekRotTest ( 360, 0, modes [ i ] )
		seekRotTest ( 360, 5, modes [ i ] )
	end
	
	-- seekScl
	seekSclTest ( 0, 0, 1 )
	seekSclTest ( 1, 1, 0 )
	seekSclTest ( 1, 1, -1 )
	for i = 1, 8 do
		seekSclTest ( 1, 1, 0, modes [ i ] )
		seekSclTest ( 1, 1, 5, modes [ i ] )
	end
	seekSclTest ( 5000, 0, 5 )
	seekSclTest ( 0, 5000, 5 )
	seekSclTest ( 5000, 5000, 5 )
	seekSclTest ( -5000, 0, 5 )
	seekSclTest ( 0, -5000, 5 )
	seekSclTest ( -5000, -5000, 5 )
	
	-- setLoc
	local values = { 0, 1, 5000, -1, -5000 }

	for i = 1, 5 do
		for j = 1, 5 do
			setLocTest ( values [ i ], values [ j ] )
		end
	end
	
	-- setParent
	
	-- setPiv
	local values = { 0, 1, 5000, -1, -5000 }

	for i = 1, 5 do
		for j = 1, 5 do
			setPivTest ( values [ i ], values [ j ] )
		end
	end
	
	-- setRot
	local values = { 0, 180, 500, -180, -500 }

	for i = 1, 5 do
		setRotTest ( values [ i ] )
	end
	
	-- setScl
	local values = { 0, 1, 5000, -1, -5000 }

	for i = 1, 5 do
		for j = 1, 5 do
			setSclTest ( values [ i ], values [ j ] )
		end
	end
	
	-- worldToModel
	
end

local results = MOAIThread.new ()
results:run ( test.printResults )
