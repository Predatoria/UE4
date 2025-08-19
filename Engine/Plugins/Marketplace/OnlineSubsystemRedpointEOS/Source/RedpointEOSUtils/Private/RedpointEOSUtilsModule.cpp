// Copyright June Rhodes. All Rights Reserved.

#include "RedpointEOSUtilsModule.h"

#include "OnlineEngineInterfaceEOS.h"
#include "UObject/Package.h"

void FRedpointEOSUtilsModule::StartupModule()
{
    if (IsRunningCommandlet())
    {
        // If this hack is active when IoStore packaging runs, it causes a crash. Turn off this hack if running as a
        // commandlet (since EOS will also be turned off as well, as per the check in
        // OnlineSubsystemRedpointEOSModule.cpp).
        return;
    }

    UClass *OnlineEngineInterfaceClass = StaticLoadClass(
        UOnlineEngineInterfaceEOS::StaticClass(),
        NULL,
        TEXT("/Script/RedpointEOSUtils.OnlineEngineInterfaceEOS"),
        NULL,
        LOAD_Quiet,
        NULL);
    if (OnlineEngineInterfaceClass)
    {
        UOnlineEngineInterfaceEOS *Obj =
            NewObject<UOnlineEngineInterfaceEOS>(GetTransientPackage(), OnlineEngineInterfaceClass);
        Obj->DoHack();
    }
}

IMPLEMENT_MODULE(FRedpointEOSUtilsModule, RedpointEOSUtils)