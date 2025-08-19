// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "../../Shared/EOSCommon.h"
#include "IPAddress.h"

EOS_ENABLE_STRICT_WARNINGS

class IInternetAddrEOS : public FInternetAddr
{
public:
    virtual ~IInternetAddrEOS(){};

    virtual void SetFromParameters(
        EOS_ProductUserId InUserId,
        const FString &InSymmetricSocketName,
        EOS_CHANNEL_ID_TYPE InSymmetricChannel) = 0;
    virtual EOS_ProductUserId GetUserId() const = 0;
};

EOS_DISABLE_STRICT_WARNINGS