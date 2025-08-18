// The MIT License(MIT)
//
// Copyright(c) 2015 Venugopalan Sreedharan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), 
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "DonNavigationHelper.h"
#include "DonAINavigationPrivatePCH.h"

ADonNavigationManager* UDonNavigationHelper::DonNavigationManager(UObject* WorldContextObject)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (!World)
		return NULL;

	for (TActorIterator<ADonNavigationManager> It(World, ADonNavigationManager::StaticClass()); It; ++It)
	{
		return *It;
	}

	return NULL;
}

ADonNavigationManager* UDonNavigationHelper::DonNavigationManagerForActor(const AActor *Actor)
{
	UE_LOG(LogTemp, Display, TEXT("UDonNavigationHelper::DonNavigationManagerForActor start"));
	if (!Actor)
		return nullptr;

	UE_LOG(LogTemp, Display, TEXT("UDonNavigationHelper::DonNavigationManagerForActor %s"), *UKismetSystemLibrary::GetDisplayName(Actor));
	for (TActorIterator<ADonNavigationManager> It(Actor->GetWorld(), ADonNavigationManager::StaticClass()); It; ++It) {
		const ADonNavigationManager *Mgr = *It;

		UE_LOG(LogTemp, Display, TEXT("UDonNavigationHelper::DonNavigationManagerForActor checking %s, IsLocationWithinNavigableWorld:%i, Loc:%s"), *UKismetSystemLibrary::GetDisplayName(Mgr), Mgr->IsLocationWithinNavigableWorld(Actor->GetActorLocation()), 
			*Actor->GetActorLocation().ToString());
		if (Mgr->IsLocationWithinNavigableWorld(Actor->GetActorLocation()))
			return *It;
	}

	UE_LOG(LogTemp, Display, TEXT("UDonNavigationHelper::DonNavigationManagerForActor end return nullptr"));
	return nullptr;
}
