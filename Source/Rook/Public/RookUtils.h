#pragma once
#include "RookEnums.h"
#include "../OpenALSoft/include/AL/efx-presets.h"
#include "../OpenALSoft/include/AL/efx-creative.h"

class RookUtils {
public:
	static RookUtils& Instance();
	~RookUtils() {};
	TMap< EEAX, EFXEAXREVERBPROPERTIES > EAXReverb;
private:
	void				SetUpEAXReverbMap();
	static RookUtils	Utils;
	RookUtils();
	RookUtils(const RookUtils&);
	RookUtils& operator= (const RookUtils&);
};