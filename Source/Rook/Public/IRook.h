/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "ModuleManager.h"
#include "Runtime/Core/Public/Delegates/DelegateSignatureImpl_Variadics.inl"

DECLARE_MULTICAST_DELEGATE( FEndPlay );

class IRook : public IModuleInterface {
public:
	static inline IRook& Get() {
		return FModuleManager::LoadModuleChecked< IRook >("Rook");
	}

	static inline bool IsAvailable() {
		return FModuleManager::Get().IsModuleLoaded("Rook");
	}

public:
	/* Helper boolean - indicates if plugin should play audio. It can be set be command line */
	bool			bIsRookEnabled = true;
	FEndPlay		OnEndPlay;

	TArray<TWeakObjectPtr<class URookListenerController>>	Listeners;
};