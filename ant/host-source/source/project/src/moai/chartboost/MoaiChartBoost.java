//----------------------------------------------------------------//
// Copyright (c) 2010-2011 Zipline Games, Inc. 
// All Rights Reserved. 
// http://getmoai.com
//----------------------------------------------------------------//

package com.ziplinegames.moai;

import android.app.Activity;
import android.view.View;

import com.chartboost.sdk.ChartBoost;
import com.chartboost.sdk.ChartBoostDelegate;

//================================================================//
// MoaiChartBoost
//================================================================//
public class MoaiChartBoost implements ChartBoostDelegate {

	private static Activity sActivity = null;
	
	//----------------------------------------------------------------//
	public static void onCreate ( Activity activity ) {
		
		MoaiLog.i ( "MoaiChartBoost onCreate: Initializing ChartBoost" );
		
		sActivity = activity;
	}

	//================================================================//
	// ChartBoost JNI callback methods
	//================================================================//

	public static void init ( String appId, String appSignature ) {
		
		ChartBoost.getSharedChartBoost ().setContext ( sActivity );
		ChartBoost.getSharedChartBoost ().setAppId ( appId );
		ChartBoost.getSharedChartBoost ().setAppSignature ( appSignature );
		ChartBoost.getSharedChartBoost ().setDelegate ( new MoaiChartBoost ());
	}
	
	public static void appInstalled () {
		
		ChartBoost.getSharedChartBoost ().install ();
	}
	
	public static void showInterstitial ( String location ) {
		
		if ( location != null ) {
			
			ChartBoost.getSharedChartBoost ().loadInterstitial ( location );
		} else {
			
			ChartBoost.getSharedChartBoost ().loadInterstitial ();
		}
	}

	//================================================================//
	// ChartBoostDelegateBase methods
	//================================================================//	

	//----------------------------------------------------------------//
	public boolean shouldRequestInterstitial () {
		
		return true;
	}

	//----------------------------------------------------------------//
	public boolean shouldDisplayInterstitial ( View interstitialView ) {
		
		return true;
	}

	//----------------------------------------------------------------//
	public void didDismissInterstitial ( View interstitialView ) {

	}

	//----------------------------------------------------------------//
	public void didCloseInterstitial ( View interstitialView ) {

	}

	//----------------------------------------------------------------//
	public void didClickInterstitial ( View interstitialView ) {
		
	}
	
	//----------------------------------------------------------------//
	public boolean shouldDisplayMoreApps ( View moreAppsView ) {
		
		return true;
	}
	
	//----------------------------------------------------------------//
	public void didDismissMoreApps ( View moreAppsView ) {
		
	}
	
	//----------------------------------------------------------------//
	public void didCloseMoreApps ( View moreAppsView ) {
		
	}
	
	//----------------------------------------------------------------//
	public void didClickMoreApps ( View moreAppsView ) {
		
	}
}