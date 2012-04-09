// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAICHARTBOOST_H
#define	MOAICHARTBOOST_H

#ifndef DISABLE_CHARTBOOST

#include <moaicore/moaicore.h>

//================================================================//
// MOAIChartBoost
//================================================================//
class MOAIChartBoost :
	public MOAIGlobalClass < MOAIChartBoost, MOAILuaObject > {
private:

	//----------------------------------------------------------------//
	static int	_init 				( lua_State* L );
	static int	_install 			( lua_State* L );
	static int	_showInterstitial 	( lua_State* L );
	
public:

	DECL_LUA_SINGLETON ( MOAIChartBoost );
	
					MOAIChartBoost		();
					~MOAIChartBoost		();
	void			RegisterLuaClass	( MOAILuaState& state );
};

#endif  //DISABLE_CHARTBOOST

#endif  //MOAICHARTBOOST_H
