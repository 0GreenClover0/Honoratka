#include "CustomerManager.h"

#include "CustomerWork.h"
#include "Components/CapsuleComponent.h"

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

    // Get album and initialize with random familiar customers
    if (CustomerAlbumReference)
    {
        for (int32 i = 0; i < CustomerAlbumReference->FamiliarCustomersNum; ++i)
        {
            const FCustomerTypeInstance CustomerTypeInstance = GenerateRandomCustomerInstance();
            CustomerAlbumReference->AddFamiliarCustomer(CustomerTypeInstance);
        }
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
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

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

        const bool bFamiliar = FMath::RandBool();

        if (ACustomer* NewCustomer = SpawnSingleCustomer(OffsetSpawnLocation, SpawnLocation, SpawnParams, bFamiliar))
        {
            SpawnedCustomers.Add(NewCustomer);
            NewCustomer->SetCustomerState(ECustomerState::WaitingInQueue);

            NumberOfCustomersInQueue++;
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

TObjectPtr<ACustomer> ACustomerManager::SpawnSingleCustomer(const FVector& OffsetSpawnLocation, const FVector& LeaveTargetLocation, const FActorSpawnParameters& SpawnParams, bool bFamiliar)
{
    ACustomer* NewCustomer = GetWorld()->SpawnActor<ACustomer>(CustomerPrefab, OffsetSpawnLocation, FRotator::ZeroRotator, SpawnParams);

    check(CustomerAlbumReference);

    // Generate CustomerTypeInstance
    FCustomerTypeInstance CustomerTypeInstance = {};
    if (bFamiliar)
    {
        TArray<FCustomerTypeInstance> FamilarCustomers = CustomerAlbumReference->GetAllFamiliarCustomers();
        const int32 Index = FMath::RandRange(0, FamilarCustomers.Num() - 1);

        check(!FamilarCustomers.IsEmpty());

        CustomerTypeInstance = FamilarCustomers[Index];
    }
    else
    {
        // Likely unfamiliar but there IS a very tiny chance for a familiar guy, leaving it like that
        CustomerTypeInstance = GenerateRandomCustomerInstance();
    }

    for (int32 k = 0; k < CustomerTypes.Num(); ++k)
    {
        if (CustomerTypes[k].CustomerType != static_cast<ECustomerType>(CustomerTypeInstance.Type) || CustomerTypes[k].bIsMale != static_cast<bool>(CustomerTypeInstance.IsMale))
        {
            continue;
        }

        UStaticMeshComponent* MainBody = Cast<UStaticMeshComponent>(NewCustomer->GetDefaultSubobjectByName(TEXT("MainBody")));
        MainBody->SetMaterial(0, CustomerTypes[k].BaseMaterial);

        UStaticMeshComponent* Accessory1 = Cast<UStaticMeshComponent>(NewCustomer->GetDefaultSubobjectByName(TEXT("Accessory1")));
        if (CustomerTypes[k].Accessory1Materials.Num() > 0 && CustomerTypeInstance.Accessory1 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory1;
            Accessory1->SetMaterial(0, CustomerTypes[k].Accessory1Materials[AccessoryId]);
        }
        else
        {
            Accessory1->SetMaterial(0, TransparentMaterial);
        }

        UStaticMeshComponent* Accessory2 = Cast<UStaticMeshComponent>(NewCustomer->GetDefaultSubobjectByName(TEXT("Accessory2")));
        if (CustomerTypes[k].Accessory2Materials.Num() > 0 && CustomerTypeInstance.Accessory2 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory2;
            Accessory2->SetMaterial(0, CustomerTypes[k].Accessory2Materials[AccessoryId]);
        }
        else
        {
            Accessory2->SetMaterial(0, TransparentMaterial);
        }

        UStaticMeshComponent* Accessory3 = Cast<UStaticMeshComponent>(NewCustomer->GetDefaultSubobjectByName(TEXT("Accessory3")));
        if (CustomerTypes[k].Accessory3Materials.Num() > 0 && CustomerTypeInstance.Accessory3 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory3;
            Accessory3->SetMaterial(0, CustomerTypes[k].Accessory3Materials[AccessoryId]);
        }
        else
        {
            Accessory3->SetMaterial(0, TransparentMaterial);
        }
    }

    NewCustomer->SetCustomerTypeInstance(CustomerTypeInstance);
    NewCustomer->SetLeaveTargetPosition(LeaveTargetLocation);
    NewCustomer->SetCustomerManager(this);
    NewCustomer->SetPairOffset(PairSideOffset);
    NewCustomer->SetWidgetClass(BubbleWidget);
    NewCustomer->GetComponentByClass<UCustomerWork>()->GreatWork = GreatCustomerWorks[FMath::RandRange(0, GreatCustomerWorks.Num() - 1)];
    return NewCustomer;
}

FCustomerTypeInstance ACustomerManager::GenerateRandomCustomerInstance() const
{
    FCustomerTypeInstance CustomerTypeInstance = {};

    ECustomerType RandomType = static_cast<ECustomerType>(FMath::RandRange(0, 0));
    bool bIsMale = FMath::RandBool();

    CustomerTypeInstance.IsMale = bIsMale;
    CustomerTypeInstance.Type = static_cast<int32>(RandomType);

    for (int32 k = 0; k < CustomerTypes.Num(); ++k)
    {
        if (CustomerTypes[k].CustomerType != RandomType || CustomerTypes[k].bIsMale != bIsMale)
        {
            continue;
        }

        if (CustomerTypes[k].Accessory1Materials.Num() > 0)
        {
            int32 RandAccessory = FMath::RandRange(0, CustomerTypes[k].Accessory1Materials.Num() - 1);
            CustomerTypeInstance.Accessory1 = RandAccessory;
        }
        else
        {
            CustomerTypeInstance.Accessory1 = INDEX_NONE;
        }

        if (CustomerTypes[k].Accessory2Materials.Num() > 0)
        {
            int32 RandAccessory = FMath::RandRange(0, CustomerTypes[k].Accessory2Materials.Num() - 1);
            CustomerTypeInstance.Accessory2 = RandAccessory;
        }
        else
        {
            CustomerTypeInstance.Accessory2 = INDEX_NONE;
        }

        if (CustomerTypes[k].Accessory3Materials.Num() > 0)
        {
            int32 RandAccessory = FMath::RandRange(0, CustomerTypes[k].Accessory3Materials.Num() - 1);
            CustomerTypeInstance.Accessory3 = RandAccessory;
        }
        else
        {
            CustomerTypeInstance.Accessory3 = INDEX_NONE;
        }
    }

    return CustomerTypeInstance;
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
            if (CustomerQueue[i].Customer->GetCustomerState() == ECustomerState::WaitingInQueue)
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

int32 ACustomerManager::GetCustomerTypeIndex(const FCustomerTypeInstance& CustomerTypeInstance)
{
    for (int32 i = 0; i < CustomerTypes.Num(); ++i)
    {
        if (CustomerTypes[i].CustomerType != static_cast<ECustomerType>(CustomerTypeInstance.Type) || CustomerTypes[i].bIsMale != static_cast<bool>(CustomerTypeInstance.IsMale))
        {
            continue;
        }

        return i;
    }

    ensure(false);
    return -1;
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

void ACustomerManager::ModifyHappiness(float Amount)
{
    if (Amount > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Increase happiness by %f"), Amount);
    }
    else if (Amount < 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Decrease happiness by %f"), Amount);
    }

    Happiness += Amount;
}

void ACustomerManager::ChangeCustomerTexture(ACustomer* Customer, bool bIsSitting, bool bMirror)
{
    if (bMirror)
    {
        Cast<UCapsuleComponent>(Customer->GetDefaultSubobjectByName(TEXT("CollisionCylinder")))->SetRelativeScale3D(FVector(-1.0f, 1.0f, 1.0f));
    }
    else
    {
        Cast<UCapsuleComponent>(Customer->GetDefaultSubobjectByName(TEXT("CollisionCylinder")))->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    }

    UStaticMeshComponent* MainBody = Cast<UStaticMeshComponent>(Customer->GetDefaultSubobjectByName(TEXT("MainBody")));
    UStaticMeshComponent* Accessory2 = Cast<UStaticMeshComponent>(Customer->GetDefaultSubobjectByName(TEXT("Accessory2")));
    FCustomerTypeInstance CustomerTypeInstance = Customer->GetCustomerTypeInstance();
    int32 CustomerTypeIndex = GetCustomerTypeIndex(CustomerTypeInstance);

    if (bIsSitting)
    {
        MainBody->SetMaterial(0, CustomerTypes[CustomerTypeIndex].SitMaterial);
        Accessory2->SetMaterial(0, CustomerTypes[CustomerTypeIndex].Accessory2SitMaterials[CustomerTypeInstance.Accessory2]);
    }
    else
    {
        MainBody->SetMaterial(0, CustomerTypes[CustomerTypeIndex].BaseMaterial);
        Accessory2->SetMaterial(0, CustomerTypes[CustomerTypeIndex].Accessory2Materials[CustomerTypeInstance.Accessory2]);
    }
}

void ACustomerManager::RemoveCustomerFromQueue(ACustomer* Customer)
{
    if (!Customer)
    {
        return;
    }

    // Check if this customer is a paired customer (secondary)
    // If so, we need to find their pair in the queue
    for (int32 i = 0; i < CustomerQueue.Num(); ++i)
    {
        if (CustomerQueue[i].Customer == Customer)
        {
            // This is the primary customer in queue
            // Check if they have a pair
            ACustomer* PairedCustomer = Customer->GetPairedCustomer();

            if (PairedCustomer)
            {
                // Unpair them
                Customer->SetPairedCustomer(nullptr);
                PairedCustomer->SetPairedCustomer(nullptr);

                // Make the secondary customer the new primary in this slot
                CustomerQueue[i].Customer = PairedCustomer;

                // Recenter the now-solo customer
                SetCustomerGroupPosition(PairedCustomer, CustomerQueue[i].QueuePosition);
            }
            else
            {
                // No pair, just remove from queue
                CustomerQueue.RemoveAt(i);
            }

            UpdateQueuePositions();
            return;
        }
        else if (CustomerQueue[i].Customer && CustomerQueue[i].Customer->GetPairedCustomer() == Customer)
        {
            // This is the secondary customer - their pair is in the queue
            // Unpair them and recenter the primary
            CustomerQueue[i].Customer->SetPairedCustomer(nullptr);
            Customer->SetPairedCustomer(nullptr);

            // Recenter the primary customer
            SetCustomerGroupPosition(CustomerQueue[i].Customer, CustomerQueue[i].QueuePosition);
            return;
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
