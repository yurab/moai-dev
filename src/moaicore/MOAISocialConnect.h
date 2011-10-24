/*
 *  MOAISocialConnect.h
 *  SteamPirates
 *
 *  Created by Christopher Kohnert on 6/6/11.
 *  Copyright 2011 UnboundFiction. All rights reserved.
 *
 */


#ifndef	MOAISOCIALCONNECT_H
#define	MOAISOCIALCONNECT_H

#include <AKU/AKU.h>
#include <moaicore/MOAIAction.h>

/*
 
	Basic LUA API for interacting with Facebook.
 
    -- Set up a top level "session" event handler that will respond to session
    -- events (i.e. login and out)
 
	sc = SocialConnect.new()
    sc:setListener(SocialConnect.EVENT_LOGIN_STATUS, function(connected, [optional errorMsg]))	
	sc:setListener(SocialConnect.EVENT_REQUEST_PENDING, function())
	sc:setListener(SocialConnect.EVENT_REQUEST_COMPLETE, function(data))
	sc:setListener(SocialConnect.EVENT_REQUEST_FAILED, function(errorMsg))
 
 
	-- Initiate the FB connection, requesting the specified permissions. This will
    -- ask the user to login if necessary (showing the standard FB login dialog).
    -- Note: this is asynchronous. It returns immediately. You have to listen for
    -- the LOGIN_STATUS event to do anything useful after logging in.
	sc:init("APP_ID", { "email", "post", "blah"} )

    -- Perform an authorized request using a logged in session. The request is a
    -- well-formed REST style request to the API of your choice.
    sc:request("/me/friends", { foo=1 } )
 
*/

//================================================================//
// MOAISocialConnect
//================================================================//
/**	@name	MOAISocialConnect
 @text	A callback interface to the social connect API.
*/
class MOAISocialConnect : public MOAIAction {
private:
	
	static int		_init					( lua_State* L );
	static int		_request		        ( lua_State* L );
	static int		_logout					( lua_State* L );
	
	//----------------------------------------------------------------//
	bool mConnected;
	u32 mNextRequestId;
	USLuaRef mRequestsTable;
	
	void            PushRequest             ( USLuaState& state, u32 requestId );
	void            DeleteRequest           ( USLuaState& state, u32 requestId );
	
public:
	
	DECL_LUA_FACTORY ( MOAISocialConnect )
	
	enum {
		EVENT_SOCIALCONNECT_LOGIN_STATUS = MOAIAction::TOTAL_EVENTS,
		EVENT_SOCIALCONNECT_REQUEST_STATUS,
		TOTAL_EVENTS,
	};
	
	char  mAppId [ 128 ];

	void            OnLoginSuccess();
	void            OnLoginFailed(const STLString &errorMsg, bool cancelled);
	void            OnLogout();
	void            OnRequestSuccess(u32 requestId, const STLString &data);
	void            OnRequestFailed(u32 requestId, const STLString &errorMsg);
	void            OnRequestPending(u32 requestId);
	
	//----------------------------------------------------------------//
	bool			IsDone					();
	MOAISocialConnect			();
	~MOAISocialConnect			();
	void			RegisterLuaClass		( USLuaState& state );
	void			RegisterLuaFuncs		( USLuaState& state );
//	STLString		ToString				();
};

#endif
