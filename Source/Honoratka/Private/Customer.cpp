#include "Customer.h"

#include "CustomerBubbleWidget.h"
#include "CustomerManager.h"
#include "GameManager.h"
#include "Honoratka.h"
#include "HonoratkaTable.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ACustomer::ACustomer()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACustomer::BeginPlay()
{
    Super::BeginPlay();

    Mesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("MainBody")));
    OutlineMaterial = Mesh->GetOverlayMaterial();

    OutlineDynamicMaterial = Cast<UMaterialInstanceDynamic>(OutlineMaterial);
    OutlineDynamicMaterial = UMaterialInstanceDynamic::Create(OutlineMaterial, this);
    Mesh->SetOverlayMaterial(OutlineDynamicMaterial);

    CurrentState = ECustomerState::Idle;
    Velocity = FVector::ZeroVector;
    bMovingToTarget = false;

    GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
}

void ACustomer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == ECustomerState::Leaving && HasReachedTarget())
    {
        Destroy();
    }

    if (bMovingToTarget)
    {
        UpdateMovement(DeltaTime);
    }

    UpdateAngriness(DeltaTime);
    UpdatePresence(DeltaTime);

    if (Bubble)
    {
        UpdateBubblePosition();
    }
}

void ACustomer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (Bubble)
    {
        HideBubble();
    }
}

void ACustomer::SetQueuePosition(int32 Position)
{
    QueuePosition = Position;
}

void ACustomer::LeaveRestaurant()
{
    if (CurrentState == ECustomerState::WaitingInQueue)
    {
        CustomerManager->RemoveCustomerFromQueue(this);
        CustomerManager->NumberOfCustomersInQueue--;
    }
    else
    {
        CustomerManager->NumberOfCustomersInside--;
    }

    if (Table)
    {
        Table->RemoveCustomer(this);
    }

    SetCustomerState(ECustomerState::Leaving);
    SetTargetPosition(LeaveTargetPosition);
}

void ACustomer::SeatCustomer(AHonoratkaTable* TableToSeat, FVector const& Position)
{
    // If we've just been seated for the first time, reset angriness counter and select desired food item.
    if (CurrentState == ECustomerState::WaitingInQueue)
    {
        AngryCounter = 0.0f;

        SelectDesiredFoodItem();

        CustomerManager->NumberOfCustomersInQueue--;
        CustomerManager->NumberOfCustomersInside++;
    }

    SetCustomerState(ECustomerState::Seated);
    SetTargetPosition(Position);
    Table = TableToSeat;
}

void ACustomer::SetWidgetClass(const TSubclassOf<UCustomerBubbleWidget>& WidgetClass)
{
    BubbleWidget = WidgetClass;
}

void ACustomer::SetCustomerState(ECustomerState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
    }
}

void ACustomer::SetTargetPosition(const FVector& Target)
{
    CustomerManager->ChangeCustomerTexture(this, false, false);
    TargetPosition = Target;
    bMovingToTarget = true;
}

void ACustomer::SetPairedCustomer(ACustomer* InPair)
{
    PairedCustomer = InPair;
    if (InPair)
    {
        InPair->PairedCustomer = this;
    }
}

void ACustomer::SetPairOffset(float Offset)
{
    PairOffset = Offset;
}

void ACustomer::SetCustomerSelected(bool bIsSelected)
{
    bSelected = bIsSelected;
}

void ACustomer::SetLeaveTargetPosition(FVector const& Position)
{
    LeaveTargetPosition = Position;
}

void ACustomer::SetCustomerManager(ACustomerManager* NewCustomerManager)
{
    CustomerManager = NewCustomerManager;
}

void ACustomer::SetCustomerTypeInstance(const FCustomerTypeInstance& Instance)
{
    CustomerTypeInstance = Instance;
}

FCustomerTypeInstance ACustomer::GetCustomerTypeInstance() const
{
    return CustomerTypeInstance;
}

void ACustomer::GiveDish(EFoodType FoodType)
{
    if (bIsSatisfied)
    {
        return;
    }

    if (DesiredFoodType == FoodType)
    {
        ShowBubble(HappyTexture);

        AngryCounter = -999.0f;

        CustomerManager->ModifyHappiness(1.0f);

        return;
    }

    ShowBubble(MadTexture);
}

void ACustomer::NotifyActorOnClicked(FKey ButtonPressed)
{
    if (GameManager->IsPaused())
    {
        return;
    }

    Super::NotifyActorOnClicked(ButtonPressed);

    if (ButtonPressed == EKeys::LeftMouseButton)
    {
        GameManager->OnCustomerClicked(this);
    }

    if (ButtonPressed == EKeys::RightMouseButton)
    {
        ChangeHighlight();
    }
}

void ACustomer::UpdateAngriness(float DeltaTime)
{
    if (CurrentState == ECustomerState::Leaving)
    {
        return;
    }

    if (CurrentState == ECustomerState::WaitingInQueue)
    {
        // NOTE: Only the first customer (and their pair) in the queue are getting angry.
        ACustomer* FirstCustomer = CustomerManager->GetFirstCustomerInQueue();
        bool IsFirstCustomer = (this == FirstCustomer);
        bool IsPairOfFirst = (FirstCustomer->GetPairedCustomer() == this);

        if (IsFirstCustomer || IsPairOfFirst)
        {
            AngryCounter += DeltaTime;
        }
    }

    if (CurrentState == ECustomerState::Seated)
    {
        AngryCounter += DeltaTime;
    }

    if (AngryCounter > AngryThreshold)
    {
        CustomerManager->ModifyHappiness(-1.0f);

        ShowBubble(AngryTexture);

        LeaveRestaurant();
    }
}

void ACustomer::UpdatePresence(float DeltaTime)
{
    if (CurrentState == ECustomerState::Leaving)
    {
        return;
    }

    if (CurrentState == ECustomerState::Seated)
    {
        PresenceTimer += DeltaTime;
    }

    if (PresenceTimer > PresenceThreshold)
    {
        LeaveRestaurant();
    }
}

void ACustomer::UpdateMovement(float DeltaTime)
{
    if (HasReachedTarget())
    {
        if (CurrentState == ECustomerState::Seated)
        {
            FTableSeat Seat = Table->GetCustomerSeat(this);

            // Randomize sprite mirroring for top seat.
            bool bRandomLeft = false;
            if (Seat.SeatType == ESeatType::Top)
            {
                if (FMath::RandBool())
                {
                    bRandomLeft = true;
                }
            }

            if (Seat.SeatType == ESeatType::Left || bRandomLeft)
            {
                CustomerManager->ChangeCustomerTexture(this, true, false);
            }
            else
            {
                CustomerManager->ChangeCustomerTexture(this, true, true);
            }
        }

        bMovingToTarget = false;
        Velocity = FVector::ZeroVector;
        AddActorWorldOffset(Velocity * DeltaTime);
        return;
    }

    FVector Direction = (TargetPosition - GetActorLocation()).GetSafeNormal();
    Velocity = Direction * WalkSpeed;
    AddActorWorldOffset(Velocity * DeltaTime);
}

void ACustomer::UpdateBubblePosition()
{
    Bubble->SetPosition(GetActorLocation());
}

bool ACustomer::HasReachedTarget() const
{
    return FVector::Dist(GetActorLocation(), TargetPosition) <= DistanceThreshold;
}

void ACustomer::SetShowingBubble()
{
    float InitialInterval = 6.0f;

    GetWorld()->GetTimerManager().SetTimer
    (
        TimerHandle,                // handle to cancel timer at a later time
        this,                        // the owning object
        &ACustomer::OnCustomerBubbleSpawned,        // function to call on elapsed
        InitialInterval,                // float delay until elapsed
        false                        // looping?
    );

    bHasShownBubble = true;
}

void ACustomer::SelectDesiredFoodItem()
{
    // Choose random food item.
    int32 FoodIndex = FMath::RandRange(0, GameManager->FoodItems.Num() - 1);
    DesiredFoodType = GameManager->FoodItems[FoodIndex].FoodType;
    DesiredFoodTexture = GameManager->FoodItems[FoodIndex].Texture;

    SetShowingBubble();
}

void ACustomer::ShowBubble(UTexture2D* BubbleTexture)
{
    FVector WorldLocation = GetActorLocation();

    if (Bubble == nullptr)
    {
        Bubble = CreateWidget<UCustomerBubbleWidget>(GetWorld(), BubbleWidget);
        Bubble->SetVisible(true, WorldLocation);
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    }

    TimerHandle.Invalidate();

    Bubble->SetTexture(BubbleTexture);

    float TimerToShow = 3.0f;
    GetWorld()->GetTimerManager().SetTimer
    (
        TimerHandle,                // handle to cancel timer at a later time
        this,                        // the owning object
        &ACustomer::HideBubble,        // function to call on elapsed
        TimerToShow,                // float delay until elapsed
        false                        // looping?
    );
}

void ACustomer::OnCustomerBubbleSpawned()
{
    ShowBubble(DesiredFoodTexture);
}

void ACustomer::HideBubble()
{
    Bubble->RemoveFromParent();
    Bubble = nullptr;
    TimerHandle.Invalidate();
}

void ACustomer::ChangeHighlight()
{
    uint8 HighlightInt = (uint8)Highlight;
    HighlightInt++;

    if (HighlightInt > 2)
    {
        HighlightInt = 0;
    }

    Highlight = (ECustomerHighlight)HighlightInt;

    switch (HighlightInt)
    {
    case(0):
        OutlineDynamicMaterial->SetVectorParameterValue("Color", FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
        break;
    case(1):
        OutlineDynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Red);
        break;
    case(2):
        OutlineDynamicMaterial->SetVectorParameterValue("Color", FLinearColor::Green);
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("Customer: My Highlight is: %d"), HighlightInt);
}
