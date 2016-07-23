#pragma once
#include "ModuleManager.h"

class IRook : public IModuleInterface {
public:
	/* Helper boolean - indicates if plugin should play audio. It can be set be command line */
	bool			bIsRookEnabled = false;
	
	static inline IRook& Get() {
		return FModuleManager::LoadModuleChecked< IRook >("Rook");
	}

	static inline bool IsAvailable() {
		return FModuleManager::Get().IsModuleLoaded("Rook");
	}
};