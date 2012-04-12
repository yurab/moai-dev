// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <aku/AKU-chartboost.h>
#include <moaiext-iphone/MOAIChartBoost.h>

//================================================================//
// AKU-chartboost
//================================================================//

//----------------------------------------------------------------//
void AKUChartBoostInit () {
	
	MOAIChartBoost::Affirm();
	REGISTER_LUA_CLASS ( MOAIChartBoost );
}