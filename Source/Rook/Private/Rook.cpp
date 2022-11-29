/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "Rook.h"
#include "RookPrivatePCH.h"
#include "OpenALSoft.h"

DEFINE_LOG_CATEGORY(RookLog)

IMPLEMENT_MODULE(FRook, Rook);

#define LOCTEXT_NAMESPACE "Rook"

void FRook::StartupModule() 
{
	bIsRookEnabled = !(FParse::Param(FCommandLine::Get(), TEXT("TurnOffRook")));
	
	if (OpenALSoft::Instance().IsDLLLoaded()) { UE_LOG(RookLog, Log, TEXT("OpenAL DLL was Loaded")); }		
}

void FRook::ShutdownModule() 
{
	OpenALSoft::Instance().CloseDeviceAndDestroyCurrentContext();
	RookUtils::Instance().CleanData();
	Listeners.Empty();
	UE_LOG(RookLog, Log, TEXT("Rook shutdown"));
}