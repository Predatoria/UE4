// Copyright June Rhodes. All Rights Reserved.

#include "RedpointEOSEditorTestsModule.h"

#if defined(EOS_HANDLE_BUGGY_OCULUSEDITOR_MODULE) && defined(WITH_ENGINE) && WITH_ENGINE &&                            \
    defined(WITH_EDITORONLY_DATA) && WITH_EDITORONLY_DATA
#include "Misc/CoreDelegates.h"
#endif

DEFINE_LOG_CATEGORY(LogEOSEditorTestsModule);

void FRedpointEOSEditorTestsModule::StartupModule()
{
    UE_LOG(LogEOSEditorTestsModule, Verbose, TEXT("Module is starting up."));

#if defined(EOS_HANDLE_BUGGY_OCULUSEDITOR_MODULE) && defined(WITH_ENGINE) && WITH_ENGINE &&                            \
    defined(WITH_EDITORONLY_DATA) && WITH_EDITORONLY_DATA
    FCoreDelegates::OnPreExit.AddLambda([]() {
        // OculusEditor calls FOculusBuildAnalytics::Shutdown when the OculusEditor module shuts down, but if the plugin
        // wrapper instance in the OculusHMD module isn't loaded or doesn't exist, then it crashes.
        //
        // The OculusEditor skips all that shutdown logic if it thinks we're running in a commandlet, so just pretend
        // that we're running in a commandlet for the shutdown process if we're running in UE 4.27.
        if (GIsEditor)
        {
            UE_LOG(
                LogEOSEditorTestsModule,
                Verbose,
                TEXT("Working around buggy OculusVR module in Unreal Engine 4.27 and later..."));
            PRIVATE_GIsRunningCommandlet = true;
        }
    });
#endif
}

void FRedpointEOSEditorTestsModule::ShutdownModule()
{
    UE_LOG(LogEOSEditorTestsModule, Verbose, TEXT("Module is shutting down."));
}

IMPLEMENT_MODULE(FRedpointEOSEditorTestsModule, RedpointEOSEditorTests);
