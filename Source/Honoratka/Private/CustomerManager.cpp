#include "CustomerManager.h"

ACustomerManager::ACustomerManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACustomerManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!CustomerPrefab)
	{
		UE_LOG(LogTemp, Warning, TEXT("CustomerManager: CustomerPrefab not assigned!"));
	}

	TimeSinceLastSpawn = 0.0f;
	NextSpawnTime = FMath::FRandRange(MinSpawnInterval, MaxSpawnInterval);
}

void ACustomerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle spawning
	TimeSinceLastSpawn += DeltaTime;
	if (TimeSinceLastSpawn >= NextSpawnTime)
	{
		SpawnCustomerGroup();
		TimeSinceLastSpawn = 0.0f;
		NextSpawnTime = FMath::FRandRange(MinSpawnInterval, MaxSpawnInterval);
	}

	UpdateQueuePositions();

#if WITH_EDITOR
	if (bDebugDrawQueue)
	{
		DebugDrawQueue();
	}
#endif
}

void ACustomerManager::SetCustomerGroupPosition(ACustomer* Customer, const FVector& Position) const
{
	if (Customer->IsPaired())
	{
		Customer->SetTargetPosition(Position + FVector(0, PairSideOffset, 0));

		ACustomer* Pair = Customer->GetPairedCustomer();
		Pair->SetTargetPosition(Position - FVector(0, PairSideOffset, 0));
	}
	else
	{
		Customer->SetTargetPosition(Position);
	}
}

void ACustomerManager::SpawnCustomerGroup()
{
	if (!CustomerPrefab)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = CustomerSpawnPoint->GetComponentLocation();

	// Decide if we spawn a pair
	bool bSpawnPair = FMath::FRand() < PairSpawnChance;
	int32 CustomersToSpawn = bSpawnPair ? 2 : 1;

	TArray<ACustomer*> SpawnedCustomers;

	for (int32 i = 0; i < CustomersToSpawn; ++i)
	{
		FVector OffsetSpawnLocation = SpawnLocation;
		if (bSpawnPair && i == 1)
		{
			OffsetSpawnLocation = SpawnLocation + FVector(0, -PairSideOffset, 0);
		}

		ACustomer* NewCustomer = GetWorld()->SpawnActor<ACustomer>(CustomerPrefab, OffsetSpawnLocation, FRotator::ZeroRotator, SpawnParams);
		NewCustomer->SetPairOffset(PairSideOffset);

		if (NewCustomer)
		{
			SpawnedCustomers.Add(NewCustomer);
			NewCustomer->SetCustomerState(ECustomerState::WaitingInQueue);
		}
	}

	// Pair them up if spawned together
	if (SpawnedCustomers.Num() == 2)
	{
		SpawnedCustomers[0]->SetPairedCustomer(SpawnedCustomers[1]);
	}

	// Add to queue (only first customer represents the group in queue)
	if (SpawnedCustomers.Num() > 0)
	{
		FCustomerSlot Slot;
		Slot.Customer = SpawnedCustomers[0];
		Slot.QueuePosition = GetQueuePositionForIndex(CustomerQueue.Num());

		CustomerQueue.Add(Slot);
		SpawnedCustomers[0]->SetQueuePosition(CustomerQueue.Num() - 1);

		SetCustomerGroupPosition(SpawnedCustomers[0], Slot.QueuePosition);

		UE_LOG(LogTemp, Log, TEXT("Spawned %d customer(s). Queue length: %d"), SpawnedCustomers.Num(), CustomerQueue.Num());
	}
}

void ACustomerManager::UpdateQueuePositions()
{
	for (int32 i = 0; i < CustomerQueue.Num(); ++i)
	{
		if (CustomerQueue[i].Customer)
		{
			FVector TargetPos = GetQueuePositionForIndex(i);
			CustomerQueue[i].QueuePosition = TargetPos;

			// Only update target if customer is waiting or in queue
			if (CustomerQueue[i].Customer->GetCustomerState() == ECustomerState::WaitingInQueue ||
				CustomerQueue[i].Customer->GetCustomerState() == ECustomerState::MovingForward)
			{
				SetCustomerGroupPosition(CustomerQueue[i].Customer, TargetPos);
			}
		}
	}
}

FVector ACustomerManager::GetQueuePositionForIndex(int32 Index) const
{
	return CustomerQueueFrontPoint->GetComponentLocation() + (QueueDirection * QueueSpacing * Index);
}

void ACustomerManager::MoveQueueForward()
{
	if (CustomerQueue.Num() > 0)
	{
		// Remove from queue
		CustomerQueue.RemoveAt(0);

		// Update positions for remaining customers
		UpdateQueuePositions();
	}
}

ACustomer* ACustomerManager::GetFirstCustomerInQueue() const
{
	if (CustomerQueue.Num() > 0)
	{
		return CustomerQueue[0].Customer;
	}

	return nullptr;
}

void ACustomerManager::RemoveCustomerFromQueue(ACustomer* Customer)
{
	for (int32 i = 0; i < CustomerQueue.Num(); ++i)
	{
		if (CustomerQueue[i].Customer == Customer)
		{
			CustomerQueue.RemoveAt(i);
			UpdateQueuePositions();
			break;
		}
	}
}

void ACustomerManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up all customers
	for (const FCustomerSlot& Slot : CustomerQueue)
	{
		if (Slot.Customer)
		{
			Slot.Customer->Destroy();
		}
	}
	CustomerQueue.Empty();

	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void ACustomerManager::DebugDrawQueue() const
{
	for (int32 i = 0; i < CustomerQueue.Num(); ++i)
	{
		FVector Pos = GetQueuePositionForIndex(i);
		DrawDebugSphere(GetWorld(), Pos, 20.0f, 8, FColor::Green, false, -1.0f, 0, 2.0f);
	}
}
#endif
