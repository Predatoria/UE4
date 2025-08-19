// Copyright June Rhodes. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if defined(EOS_IS_FREE_EDITION)

class FUpgradeWindow
{
private:
    TSharedPtr<class SWindow> UpgradePromptWindow;

    FReply FollowUpgradePrompt();

public:
    FUpgradeWindow()
        : UpgradePromptWindow(nullptr){};
    bool Open();
};

#endif