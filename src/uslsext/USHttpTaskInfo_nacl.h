// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef USHTTPTASKINFO_NACL_H
#define USHTTPTASKINFO_NACL_H
#ifdef MOAI_OS_NACL

//================================================================//
// USHttpTaskInfo
//================================================================//
class USHttpTaskInfo {
public:

	//----------------------------------------------------------------//
	void			Finish					();
	void			InitForGet				( cc8* url, cc8* useragent, bool verbose );
	void			InitForPost				( cc8* url, cc8* useragent, const void* buffer, u32 size, bool verbose );
					USHttpTaskInfo			();
					~USHttpTaskInfo			();
};

#endif
#endif
