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

#include "DonNavigationManagerUnbound.h"
#include "DonAINavigationPrivatePCH.h"

ADonNavigationManagerUnbound::ADonNavigationManagerUnbound(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsUnbound = true;
}

void ADonNavigationManagerUnbound::BeginPlay()
{
	Super::BeginPlay();
}

bool ADonNavigationManagerUnbound::PrepareSolution(FDonNavigationQueryTask& Task)
{
	if (!bIsUnbound)
		return Super::PrepareSolution(Task);

	auto& data = Task.Data;

	// a rare edgecase, but worth handling gracefully in any case
	if (data.OriginVolumeCenter == data.DestinationVolumeCenter)
	{
		data.PathSolutionRaw.Add(data.Origin);
		data.PathSolutionRaw.Add(data.Destination);

		return true;
	}

	// The trajectory map operates in reverse, so start from the destination:
	data.PathSolutionRaw.Insert(data.Destination, 0);

	// Work our way back from destination to origin while generating a linear path solution list
	bool originFound = false;
	auto nextLocation = data.VolumeVsGoalTrajectoryMap_Unbound.Find(data.DestinationVolumeCenter);

	while (nextLocation)
	{
		if (data.PathSolutionRaw.Contains(*nextLocation))
			break; // Note:- this was introduced alongisde the bugfix to the bound manager to prevent infinte loops in PathSolutionFromVolumeTrajectoryMap

		data.PathSolutionRaw.Insert((*nextLocation), 0);

		if (*nextLocation == data.OriginVolumeCenter)
		{
			originFound = true;
			break;
		}

		nextLocation = data.VolumeVsGoalTrajectoryMap_Unbound.Find(*nextLocation);
	}

	return originFound;
}


void ADonNavigationManagerUnbound::TickNavigationSolver(FDonNavigationQueryTask& task)
{
	if (!bIsUnbound)
	{
		Super::TickNavigationSolver(task);

		return;
	}

	auto& data = task.Data;

	data.SolverIterationCount++;
	float MinZ = task.MinZ;
	float MaxZ = task.MaxZ;

	if (!data.Frontier_Unbound.empty())
	{
		// Move towards goal by fetching the "best neighbor" of the previous volume from the Frontier priority queue
		// The best neighbor is defined as the node most likely to lead us towards the goal
		FVector currentVector = data.Frontier_Unbound.get();

		// Have we reached the goal?
		if (currentVector == data.DestinationVolumeCenter)
		{
			data.bGoalFound = true;
			return;
		}

		// Discover all neighbors for current volume:
		const auto& neighbors = NeighborsAsVectors(currentVector);

		// Evaluate each neighbor for suitability, assign points, add to Frontier
		for (auto neighbor : neighbors)
		{
			if (neighbor.Z < MinZ || neighbor.Z > MaxZ)
			{
				continue;
			}

			ExpandFrontierTowardsTarget(task, currentVector, neighbor);
		}
	}
}

TArray<FVector> ADonNavigationManagerUnbound::NeighborsAsVectors(FVector Location)
{
	TArray<FVector> neighbors;
	neighbors.Reserve(Volume6DOF + VolumeImplicitDOF);

	// 6 DOF neighbors (Direct neighbors)
	for (int32 i = 0; i < Volume6DOF; i++)
	{
		FVector neighbor = Location + VoxelSize * FVector(x6DOFCoords[i], y6DOFCoords[i], z6DOFCoords[i]);

		neighbors.Add(neighbor);
	}

	// Implicit:
	
	// X		

	if (CanNavigate(LocationAtId(Location, 1, 0, 0)) && CanNavigate(LocationAtId(Location, 0, 0, 1)))
		neighbors.Add(LocationAtId(Location, 1, 0, 1));

	if (CanNavigate(LocationAtId(Location, -1, 0, 0)) && CanNavigate(LocationAtId(Location, 0, 0, 1)))
		neighbors.Add(LocationAtId(Location, -1, 0, 1));

	if (CanNavigate(LocationAtId(Location, 1, 0, 0)) && CanNavigate(LocationAtId(Location, 0, 0, -1)))
		neighbors.Add(LocationAtId(Location, 1, 0, -1));

	if (CanNavigate(LocationAtId(Location, -1, 0, 0)) && CanNavigate(LocationAtId(Location, 0, 0, -1)))
		neighbors.Add(LocationAtId(Location, -1, 0, -1));

	//Y
	if (CanNavigate(LocationAtId(Location, 0, 1, 0)) && CanNavigate(LocationAtId(Location, 0, 0, 1)))
		neighbors.Add(LocationAtId(Location, 0, 1, 1));

	if (CanNavigate(LocationAtId(Location, 0, -1, 0)) && CanNavigate(LocationAtId(Location, 0, 0, 1)))
		neighbors.Add(LocationAtId(Location, 0, -1, 1));

	if (CanNavigate(LocationAtId(Location, 0, 1, 0)) && CanNavigate(LocationAtId(Location, 0, 0, -1)))
		neighbors.Add(LocationAtId(Location, 0, 1, -1));

	if (CanNavigate(LocationAtId(Location, 0, -1, 0)) && CanNavigate(LocationAtId(Location, 0, 0, -1)))
		neighbors.Add(LocationAtId(Location, 0, -1, -1));

	//Z
	if (CanNavigate(LocationAtId(Location, 1, 0, 0)) && CanNavigate(LocationAtId(Location, 0, 1, 0)))
		neighbors.Add(LocationAtId(Location, 1, 1, 0));

	if (CanNavigate(LocationAtId(Location, -1, 0, 0)) && CanNavigate(LocationAtId(Location, 0, 1, 0)))
		neighbors.Add(LocationAtId(Location, -1, 1, 0));

	if (CanNavigate(LocationAtId(Location, 1, 0, 0)) && CanNavigate(LocationAtId(Location, 0, -1, 0)))
		neighbors.Add(LocationAtId(Location, 1, -1, 0));

	if (CanNavigate(LocationAtId(Location, -1, 0, 0)) && CanNavigate(LocationAtId(Location, 0, -1, 0)))
		neighbors.Add(LocationAtId(Location, -1, -1, 0));	

	return neighbors;
}

void ADonNavigationManagerUnbound::ExpandFrontierTowardsTarget(FDonNavigationQueryTask& Task, FVector Current, FVector Neighbor)
{
	if (!CanNavigateByCollisionProfile(Neighbor, Task.Data.VoxelCollisionProfile))
		return;

	float SegmentDist = VoxelSize;

	uint32 newCost = *Task.Data.VolumeVsCostMap_Unbound.Find(Current) + SegmentDist;
	uint32* volumeCost = Task.Data.VolumeVsCostMap_Unbound.Find(Neighbor);

	if (!volumeCost || newCost < *volumeCost)
	{
		Task.Data.VolumeVsGoalTrajectoryMap_Unbound.Add(Neighbor, Current);
		Task.Data.VolumeVsCostMap_Unbound.Add(Neighbor, newCost);

		float heuristic = FVector::Dist(Neighbor, Task.Data.Destination);
		uint32 priority = newCost + heuristic;

		Task.Data.Frontier_Unbound.put(Neighbor, priority);
	}
}

bool ADonNavigationManagerUnbound::GetClosestNavigableVector(FVector DesiredLocation, FVector& ResolvedLocation, UPrimitiveComponent* CollisionComponent, bool& bInitialPositionCollides, float CollisionShapeInflation, bool bShouldSweep, float MaxZ, float MinZ)
{
	bool bFoundSolution = Super::GetClosestNavigableVector(DesiredLocation, ResolvedLocation, CollisionComponent, bInitialPositionCollides, CollisionShapeInflation, bShouldSweep, MaxZ, MinZ);
	if (bFoundSolution)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ADonNavigationManager::GetClosestNavigableVector --- found solution regularly")));
		return true;
	}
	// 2 b) So we still haven't found an ideal neighbor. It's time to methodically scan for best neighbors:
	const int32 neighborSearchMaxDepth = 2;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ADonNavigationManager::GetClosestNavigableVector --- at neighbor part")));
	TSet<FVector> TriedLocs;

	bool bResult = GetBestVectorRecursive(DesiredLocation, ResolvedLocation, 0, neighborSearchMaxDepth, CollisionComponent, true, CollisionShapeInflation, bShouldSweep, TriedLocs, MaxZ, MinZ);

	if (bResult)
		return true;

	// No solution:

	UE_LOG(DoNNavigationLog, Error, TEXT("Error: GetClosestNavigableVolume failed to resolve a given volume."));

#if WITH_EDITOR
	DrawDebugSphere_Safe(GetWorld(), DesiredLocation, 64.f, 6.f, FColor::Magenta, true, -1.f);
	DrawDebugPoint_Safe(GetWorld(), DesiredLocation, 6.f, FColor::Yellow, true, -1.f);
#endif // WITH_EDITOR	

	return false;
}


bool ADonNavigationManagerUnbound::GetBestVectorRecursive(FVector StartLocation, FVector& ResultLocation, int32 CurrentDepth, int32 NeighborSearchMaxDepth, UPrimitiveComponent* CollisionComponent, bool bConsiderInitialOverlaps, float CollisionShapeInflation, bool bShouldSweep, TSet<FVector>& TriedLocs, float MaxZ, float MinZ)
{
	if (CurrentDepth > NeighborSearchMaxDepth)
		return false;

	FHitResult hit;
	TArray<FVector> neighbors = NeighborsAsVectors(StartLocation);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("ADonNavigationManager::GetBestNeighborRecursive --- at start")));
	for (FVector neighbor : neighbors)
	{
		if (TriedLocs.Contains(neighbor))
		{
			continue;
		}
		if (!CanNavigate(neighbor))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString::Printf(TEXT("Skip %s (delta = %s). Not Navigable Voxel"), *neighbor.ToString(), *(neighbor - StartLocation).ToString()));
			TriedLocs.Add(neighbor);
			continue;
		}
		if (neighbor.Z > MaxZ || neighbor.Z < MaxZ)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString::Printf(TEXT("Skip %s (delta = %s). Out of Z Bounds"), *neighbor.ToString(), *(neighbor - StartLocation).ToString()));
			TriedLocs.Add(neighbor);
			continue;
		}
		if (!bShouldSweep)
		{
			ResultLocation = neighbor;
			return true;
		}
		else if (IsDirectPathLineSweep(CollisionComponent, StartLocation, neighbor, hit, bConsiderInitialOverlaps, CollisionShapeInflation))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT("Return %s (delta = %s). Good location found!"), *neighbor.ToString(), *(neighbor - StartLocation).ToString()));
			ResultLocation = neighbor;
			return true;
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString::Printf(TEXT("Skip %s (delta = %s). Not in LoS"), *neighbor.ToString(), *(neighbor - StartLocation).ToString()));
			TriedLocs.Add(neighbor);
		}
	}

	// No suitable volume found, testing neighbors of neighbors:
	for (FVector neighbor : neighbors)
	{
		// need to optimize redundancy. A large number of voxels will get queried multiple times due to multi-neighbor relationships. Consider maintaining a hash (TSet) of visited neighbors
		// @Bug - the function below should actually use "neighbor" and not "Volume"! As this needs more testing, the change is reserved for a future update.
		if (TriedLocs.Contains(neighbor))
		{
			continue;
		}
		bool bFoundBestVolume = GetBestVectorRecursive(StartLocation, ResultLocation, CurrentDepth + 1, NeighborSearchMaxDepth, CollisionComponent, true, CollisionShapeInflation, bShouldSweep, TriedLocs, MaxZ, MinZ);
		if (bFoundBestVolume)
			return true;
	}

	return false;
}