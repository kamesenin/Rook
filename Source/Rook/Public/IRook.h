#pragma once
#include "ModuleManager.h"

class IRook : public IModuleInterface {
public:
	bool			bIsRookEnabled = false;
	
	static inline IRook& Get() {
		return FModuleManager::LoadModuleChecked< IRook >("Rook");
	}

	static inline bool IsAvailable() {
		return FModuleManager::Get().IsModuleLoaded("Rook");
	}
};