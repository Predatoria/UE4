// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

EOS_ENABLE_STRICT_WARNINGS

class ONLINESUBSYSTEMREDPOINTEOS_API FEOSTextureLoader
{
public:
    static UTexture *LoadTextureFromHttpResponse(const FHttpResponsePtr &Response);
};

EOS_DISABLE_STRICT_WARNINGS