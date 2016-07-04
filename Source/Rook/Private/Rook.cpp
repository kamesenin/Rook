#include "RookPrivatePCH.h"
#include "Rook.h"
#include "OpenALSoft.h"

DEFINE_LOG_CATEGORY(RookLog)

IMPLEMENT_MODULE(FRook, Rook);

#define LOCTEXT_NAMESPACE "Rook"

void FRook::StartupModule() {
	
	if ( FParse::Param( FCommandLine::Get(), TEXT("TurnOffRook") ) ) {
		bIsRookEnabled = false;
	}

	if ( OpenALSoft::Instance().IsDLLLoaded() ) {
		UE_LOG( RookLog, Log, TEXT("OpenAL DLL was Loaded") );
	}	
}

void FRook::ShutdownModule() {
	OpenALSoft::Instance().CloseDeviceAndDestroyCurrentContext();
	UE_LOG(RookLog, Log, TEXT("Rook shutdown"));
}