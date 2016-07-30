/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "IRook.h"
DECLARE_LOG_CATEGORY_EXTERN( RookLog, Log, All );

class FRook : public IRook {
public:
	virtual void			StartupModule() override;
	virtual void			ShutdownModule() override;
};