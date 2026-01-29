#include "CustomerManager.h"

#include "CustomerWork.h"
#include "HonoratkaHUDWidget.h"
#include "HonoratkaPlayerController.h"
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
        AHonoratkaPlayerController* PC = Cast<AHonoratkaPlayerController>(
            GetWorld()->GetFirstPlayerController());

        PC->CustomerManagerPtr = this;

        for (int32 i = 0; i < CustomerAlbumReference->FamiliarCustomersNum; ++i)
        {
            const FCustomerTypeInstance CustomerTypeInstance = GenerateRandomCustomerInstance();
            CustomerAlbumReference->AddFamiliarCustomer(CustomerTypeInstance);
        }
    }

    TimeSinceLastSpawn = 0.0f;
    NextSpawnTime = FMath::FRandRange(MinSpawnInterval, MaxSpawnInterval);

    TInlineComponentArray<UCustomerWork*> CustomerWorkComponents(GetOwner(), false);
    GetComponents(CustomerWorkComponents);

    GreatWorks.Reserve(CustomerWorkComponents.Num());
    GreatWorks.Append(CustomerWorkComponents);
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
        // Likely unfamiliar but there IS a chance for a familiar guy, leaving it like that
        CustomerTypeInstance = GenerateRandomCustomerInstance();
    }

    FCustomerWidgetPayload Payload;

    for (int32 k = 0; k < CustomerTypes.Num(); ++k)
    {
        if (CustomerTypes[k].CustomerType != static_cast<ECustomerType>(CustomerTypeInstance.Type) || CustomerTypes[k].bIsMale != static_cast<bool>(CustomerTypeInstance.IsMale))
        {
            continue;
        }

        UStaticMeshComponent* MainBody = Cast<UStaticMeshComponent>(NewCustomer->GetDefaultSubobjectByName(TEXT("MainBody")));
        NewCustomer->MainBodyMat = CreateAndSetCustomerImage(MainBody, CustomerTypes[k].BaseImage);
        Payload.MainBody = CustomerTypes[k].BaseImage;

        UStaticMeshComponent* Accessory1 = Cast<UStaticMeshComponent>(NewCustomer->GetDefaultSubobjectByName(TEXT("Accessory1")));
        if (CustomerTypes[k].Accessory1Images.Num() > 0 && CustomerTypeInstance.Accessory1 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory1;
            NewCustomer->Accessory1Mat = CreateAndSetCustomerImage(Accessory1, CustomerTypes[k].Accessory1Images[AccessoryId]);
            Payload.Accessory1 = CustomerTypes[k].Accessory1Images[AccessoryId];
        }
        else
        {
            NewCustomer->Accessory1Mat = CreateAndSetCustomerImage(Accessory1, TransparentImage);
            Payload.Accessory1 = TransparentImage;
        }

        UStaticMeshComponent* Accessory2 = Cast<UStaticMeshComponent>(NewCustomer->GetDefaultSubobjectByName(TEXT("Accessory2")));
        if (CustomerTypes[k].Accessory2Images.Num() > 0 && CustomerTypeInstance.Accessory2 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory2;
            NewCustomer->Accessory2Mat = CreateAndSetCustomerImage(Accessory2, CustomerTypes[k].Accessory2Images[AccessoryId]);
            Payload.Accessory2 = CustomerTypes[k].Accessory2Images[AccessoryId];
        }
        else
        {
            NewCustomer->Accessory2Mat = CreateAndSetCustomerImage(Accessory2, TransparentImage);
            Payload.Accessory2 = TransparentImage;
        }

        UStaticMeshComponent* Accessory3 = Cast<UStaticMeshComponent>(NewCustomer->GetDefaultSubobjectByName(TEXT("Accessory3")));
        if (CustomerTypes[k].Accessory3Images.Num() > 0 && CustomerTypeInstance.Accessory3 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory3;
            NewCustomer->Accessory3Mat = CreateAndSetCustomerImage(Accessory3, CustomerTypes[k].Accessory3Images[AccessoryId]);
            Payload.Accessory3 = CustomerTypes[k].Accessory3Images[AccessoryId];
        }
        else
        {
            NewCustomer->Accessory3Mat = CreateAndSetCustomerImage(Accessory3, TransparentImage);
            Payload.Accessory3 = TransparentImage;
        }
    }

    NewCustomer->CustomerWidgetPayload = Payload;
    NewCustomer->SetCustomerTypeInstance(CustomerTypeInstance);
    NewCustomer->SetLeaveTargetPosition(LeaveTargetLocation);
    NewCustomer->SetCustomerManager(this);
    NewCustomer->SetPairOffset(PairSideOffset);
    NewCustomer->SetWidgetClass(BubbleWidget);

    return NewCustomer;
}

FCustomerTypeInstance ACustomerManager::GenerateRandomCustomerInstance() const
{
    FCustomerTypeInstance CustomerTypeInstance = {};

    // NOTE: Choose the customer (type + gender) randomly, but equally to the combinations of clothes that each of them has.
    //       Ex. If we only had a male actor that has 9 combinations and an actress that has 27 combinations, the chance to choose an actor would be 9/36 = 25% and for actress it would be 27/36 = 75%.
    //       This is done so that each combination shows up equally frequently.

    // 
    // HACK: Number from 1 to 9 is an actor.
    //       Number from 10 to 36 is an actress.
    //       Number from 37 to 45 is a director.
    //       Number from 46 to 72 is a writer.
    int32 Random = FMath::RandRange(1, 72);
    ECustomerType RandomType;
    bool bIsMale;

    if (Random <= 9)
    {
        RandomType = ECustomerType::Actor;
        bIsMale = true;
    }
    else if (Random <= 36)
    {
        RandomType = ECustomerType::Actor;
        bIsMale = false;
    }
    else if (Random <= 45)
    {
        RandomType = ECustomerType::Director;
        bIsMale = true;
    }
    else
    {
        RandomType = ECustomerType::Screenwriter;
        bIsMale = true;
    }

    CustomerTypeInstance.IsMale = bIsMale;
    CustomerTypeInstance.Type = static_cast<int32>(RandomType);

    for (int32 k = 0; k < CustomerTypes.Num(); ++k)
    {
        if (CustomerTypes[k].CustomerType != RandomType || CustomerTypes[k].bIsMale != bIsMale)
        {
            continue;
        }

        if (CustomerTypes[k].Accessory1Images.Num() > 0)
        {
            int32 RandAccessory = FMath::RandRange(0, CustomerTypes[k].Accessory1Images.Num() - 1);
            CustomerTypeInstance.Accessory1 = RandAccessory;
        }
        else
        {
            CustomerTypeInstance.Accessory1 = INDEX_NONE;
        }

        if (CustomerTypes[k].Accessory2Images.Num() > 0)
        {
            int32 RandAccessory = FMath::RandRange(0, CustomerTypes[k].Accessory2Images.Num() - 1);
            CustomerTypeInstance.Accessory2 = RandAccessory;
        }
        else
        {
            CustomerTypeInstance.Accessory2 = INDEX_NONE;
        }

        if (CustomerTypes[k].Accessory3Images.Num() > 0)
        {
            int32 RandAccessory = FMath::RandRange(0, CustomerTypes[k].Accessory3Images.Num() - 1);
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

bool ACustomerManager::IsCustomerFamiliar(ACustomer* Customer) const
{
    return CustomerAlbumReference->IsCustomerFamiliar(Customer->GetCustomerTypeInstance());
}

bool ACustomerManager::IsCustomerFamiliar(const FCustomerTypeInstance& CustomerTypeInstance) const
{
    return CustomerAlbumReference->IsCustomerFamiliar(CustomerTypeInstance);
}

float ACustomerManager::GetHappiness() const
{
    return Happiness;
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
        Cast<USceneComponent>(Customer->GetDefaultSubobjectByName(TEXT("BodyRoot")))->SetRelativeScale3D(FVector(-1.0f, 1.0f, 1.0f));
    }
    else
    {
        Cast<USceneComponent>(Customer->GetDefaultSubobjectByName(TEXT("BodyRoot")))->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    }

    FCustomerTypeInstance CustomerTypeInstance = Customer->GetCustomerTypeInstance();
    int32 CustomerTypeIndex = GetCustomerTypeIndex(CustomerTypeInstance);

    if (bIsSitting)
    {
        SetCustomerImage(Customer->MainBodyMat, CustomerTypes[CustomerTypeIndex].SitImage);
        Customer->OutlineDynamicMaterial->SetTextureParameterValue("Texture", CustomerTypes[CustomerTypeIndex].SitImage);

        if (CustomerTypes[CustomerTypeIndex].Accessory2SitImages.Num() > 0)
        {
            SetCustomerImage(Customer->Accessory2Mat, CustomerTypes[CustomerTypeIndex].Accessory2SitImages[CustomerTypeInstance.Accessory2]);
        }
    }
    else
    {
        SetCustomerImage(Customer->MainBodyMat, CustomerTypes[CustomerTypeIndex].BaseImage);
        Customer->OutlineDynamicMaterial->SetTextureParameterValue("Texture", CustomerTypes[CustomerTypeIndex].BaseImage);

        if (CustomerTypes[CustomerTypeIndex].Accessory2Images.Num() > 0)
        {
            SetCustomerImage(Customer->Accessory2Mat, CustomerTypes[CustomerTypeIndex].Accessory2Images[CustomerTypeInstance.Accessory2]);
        }
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

UMaterialInstanceDynamic* ACustomerManager::CreateAndSetCustomerImage(UStaticMeshComponent* Mesh, UTexture2D* Texture)
{
    UMaterialInstanceDynamic* DynamicMat = Mesh->CreateAndSetMaterialInstanceDynamic(0);
    SetCustomerImage(DynamicMat, Texture);
    return DynamicMat;
}

void ACustomerManager::SetCustomerImage(UMaterialInstanceDynamic* MaterialInstance, UTexture2D* Texture)
{
    if (MaterialInstance)
    {
        MaterialInstance->SetTextureParameterValue(FName("MaterialImage"), Texture);
    }
}
