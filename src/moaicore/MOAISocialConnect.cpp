/*
 *  MOAISocialConnect.cpp
 *  SteamPirates
 *
 *  Created by Christopher Kohnert on 6/6/11.
 *  Copyright 2011 UnboundFiction. All rights reserved.
 *
 */

#include <uslsext/uslsext.h>
#include <moaicore/moaiconf.h>
#include <moaicore/MOAILogMessages.h>
#include "MOAISocialConnect.h"



/** @name init
 @text Attempt to connect to the social API framework and authorize the given
   app (by its ID). You can also specify the permissions being requested (in case
   you do not want to use the default.)
 
 @in string appId The App ID to authorize with the social framework
 @opt table permissions The set of permissions being requested (e.g. {"email", "friends", etc} (may be nil).
 */
int MOAISocialConnect::_init( lua_State* L )
{
	MOAI_LUA_SETUP ( MOAISocialConnect, "US" );
	
	AKUSocialConnectInitFunc f = AKUGetFunc_SocialConnectInit();
	if( !f )
		return 0;

	STLArray<STLString> permsList;

	if( state.IsType(3, LUA_TTABLE) )
	{
		u32 itr = state.PushTableItr ( 3 );
		while ( state.TableItrNext ( itr ))
		{
			cc8* perm = lua_tostring(state, -1);
			if( perm )
			{
				permsList.push_back(perm);
			}
		}
	}
	
	cc8** permsArray = new cc8*[permsList.size()];
	for( size_t i = 0 ; i < permsList.size(); i++ )
		permsArray[i] = permsList[i].c_str();

	f(state.GetValue<cc8*>(2, ""), permsList.size(), permsArray, self);
	
	delete [] permsArray;
	
	return 0;
}

/** @name logout
 @text Attempt to log out and clean up any session information
 */
int MOAISocialConnect::_logout( lua_State* L )
{
	MOAI_LUA_SETUP ( MOAISocialConnect, "U" );
	
	AKUSocialConnectLogoutFunc f = AKUGetFunc_SocialConnectLogout();
	if( !f )
		return 0;

	f();
	
	return 0;
}

/** @name request
    @text Issue a request using the current session. The request must adhere to
          the API of choice and all arguments and response values are up to you
          to manage. The access_token and authorization glue will be added/inserted
          as needed, so simply provide the parameters pertinent to the request itself.
    
	@in string url The URL to issue the request to. e.g. https://graph.facebook.com/me/friends
    @opt table params The set of parameters for the request (may be nil).
    @opt string httpRequestMethod On of "GET", "POST", "DELETE"
*/
int MOAISocialConnect::_request( lua_State* L )
{
	MOAI_LUA_SETUP ( MOAISocialConnect, "US" );

	AKUSocialConnectRequestFunc f = AKUGetFunc_SocialConnectRequest();
	if( !f )
		return 0;
	
	STLArray<STLString> permsList;
	int args = state.GetTop() - state.AbsIndex(0);
	
	if( args >= 3 && state.IsType(3, LUA_TTABLE) )
	{
		u32 itr = state.PushTableItr ( 3 );
		while ( state.TableItrNext ( itr ))
		{
			cc8* key = lua_tostring(state, -2);
			cc8* value = lua_tostring(state, -1);
			if( key && value )
			{
				permsList.push_back(key);
				permsList.push_back(value);
			}
		}
	}
	
	cc8* requestMethod = state.GetValue<cc8*>(4, "GET");

	cc8** permsArray = new cc8*[permsList.size()];
	for( size_t i = 0 ; i < permsList.size(); i++ )
		permsArray[i] = permsList[i].c_str();
	
	u32 requestId = self->mNextRequestId++;
	
	f(requestId, state.GetValue<cc8*>(2, ""), requestMethod,
				permsList.size() / 2, permsArray);
	
	delete [] permsArray;
	
	// This will hold our request structure (both internally, and exposed as the return value)
	lua_newtable(state);
	lua_pushnumber(state, requestId);
	lua_setfield(state, -2, "requestId");
	lua_pushvalue(state, 2);
	lua_setfield(state, -2, "path");
	lua_pushstring(state, requestMethod);
	lua_setfield(state, -2, "method");
	if( args >= 3 )
	{
		lua_pushvalue(state, 3);
		lua_setfield(state, -2, "params");
	}
		
	// We'll be storing into this.
	self->mRequestsTable.PushRef(state);
	lua_pushnumber(state, requestId);
	// Copy our table (for the setfield)
	lua_pushvalue(state, -3);
	lua_settable(state, -3); // Pops key and value, stores in table
	state.Pop(1); // Pop the master requests table.

	// Leave the request table on the stack and return it
	return 1;
}

//----------------------------------------------------------------//
bool MOAISocialConnect::IsDone()
{
	return !mConnected;
}

//----------------------------------------------------------------//

MOAISocialConnect::MOAISocialConnect()
	: mConnected(false)
	, mNextRequestId(1)
{
	RTTI_BEGIN
		RTTI_EXTEND( MOAIAction )
	RTTI_END
	
	// Use a lua table to store these structures.
	USLuaStateHandle state = USLuaRuntime::Get().State();
	lua_newtable(state);
	mRequestsTable = state.GetStrongRef ( -1 );
	state.Pop(1);
}

//----------------------------------------------------------------//
MOAISocialConnect::~MOAISocialConnect()
{
	mRequestsTable.Clear();
}

//----------------------------------------------------------------//
void MOAISocialConnect::RegisterLuaClass( USLuaState& state )
{
	MOAIAction::RegisterLuaClass(state);
	
	state.SetField ( -1, "EVENT_LOGIN_STATUS", ( u32 )EVENT_SOCIALCONNECT_LOGIN_STATUS );
	state.SetField ( -1, "EVENT_REQUEST_STATUS", ( u32 )EVENT_SOCIALCONNECT_REQUEST_STATUS );
}

//----------------------------------------------------------------//
void MOAISocialConnect::RegisterLuaFuncs( USLuaState& state )
{
	MOAIAction::RegisterLuaFuncs(state);

	luaL_Reg regTable [] = {
		{ "init",		_init },
		{ "logout",		_logout },
		{ "request",	_request },
		{ NULL, NULL }
	};
	
	luaL_register( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAISocialConnect::PushRequest( USLuaState& state, u32 requestId )
{
	mRequestsTable.PushRef(state);
	state.GetField(-1, requestId);
	lua_remove(state, -2); // Pop the requests table, leaving the request
}

//----------------------------------------------------------------//
void MOAISocialConnect::DeleteRequest( USLuaState& state, u32 requestId )
{
	mRequestsTable.PushRef(state);
	lua_pushnumber(state, requestId);
	lua_pushnil(state);
	lua_settable(state, -3);
	state.Pop(1);
}

//----------------------------------------------------------------//

void MOAISocialConnect::OnLoginSuccess()
{
	mConnected = true;
	USLuaStateHandle state = USLuaRuntime::Get ().State ();
	if ( this->PushListenerAndSelf ( EVENT_SOCIALCONNECT_LOGIN_STATUS, state ) )
	{
		state.Push(mConnected);
		state.DebugCall ( 2, 0 );
	}
}

void MOAISocialConnect::OnLoginFailed(const STLString &errorMsg, bool cancelled)
{
	mConnected = false;
	USLuaStateHandle state = USLuaRuntime::Get ().State ();
	if ( this->PushListenerAndSelf ( EVENT_SOCIALCONNECT_LOGIN_STATUS, state ) )
	{
		state.Push(mConnected);
		state.Push(errorMsg);
		state.Push(cancelled);
		state.DebugCall ( 4, 0 );
	}
}

void MOAISocialConnect::OnLogout()
{
	mConnected = false;
	USLuaStateHandle state = USLuaRuntime::Get ().State ();
	if ( this->PushListenerAndSelf ( EVENT_SOCIALCONNECT_LOGIN_STATUS, state ) )
	{
		state.Push(mConnected);
		state.DebugCall ( 2, 0 );
	}
}

void MOAISocialConnect::OnRequestSuccess(u32 requestId, const STLString &data)
{
	USLuaStateHandle state = USLuaRuntime::Get ().State ();
	if ( this->PushListenerAndSelf ( EVENT_SOCIALCONNECT_REQUEST_STATUS, state ) )
	{
		PushRequest(state, requestId);
		state.Push(true);
		state.Push(data);
		state.DebugCall ( 4, 0 );
		DeleteRequest(state, requestId);
	}
}

void MOAISocialConnect::OnRequestFailed(u32 requestId, const STLString &errorMsg)
{
	USLuaStateHandle state = USLuaRuntime::Get ().State ();
	if ( this->PushListenerAndSelf ( EVENT_SOCIALCONNECT_REQUEST_STATUS, state ) )
	{
		PushRequest(state, requestId);
		state.Push(false);
		state.Push(errorMsg);
		state.DebugCall ( 4, 0 );
		DeleteRequest(state, requestId);
	}
}

void MOAISocialConnect::OnRequestPending(u32 requestId)
{
	// Let's skip this for now. It kind of confuses the API a bit, for no real
	// gain. Responses will not be super huge.

//	USLuaStateHandle state = USLuaRuntime::Get ().State ();
//	if ( this->PushListener ( EVENT_SOCIALCONNECT_REQUEST_STATUS, state ) )
//	{
//		PushRequest(state, requestId);
//		state.PushNil();
//		state.Push("pending");
//		state.DebugCall ( 3, 0 );
//	}
}


