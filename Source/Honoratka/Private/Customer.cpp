#include "Customer.h"

#include "CustomerManager.h"
#include "GameManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ACustomer::ACustomer()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACustomer::BeginPlay()
{
    Super::BeginPlay();

    CurrentState = ECustomerState::Idle;
    Velocity = FVector::ZeroVector;
    bMovingToTarget = false;
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
	}

    SetCustomerState(ECustomerState::Leaving);
    SetTargetPosition(LeaveTargetPosition);
}

void ACustomer::SeatCustomer(AHonoratkaTable* TableToSeat, FVector const& Position)
{
    // If we've just been seated for the first time, reset angriness counter.
    if (CurrentState != ECustomerState::Seated)
    {
	    AngryCounter = 0.0f;
    }

	SetCustomerState(ECustomerState::Seated);
	SetShowingBubble();
	SetTargetPosition(Position);
	Table = TableToSeat;
}

void ACustomer::SetWidgetClass(const TSubclassOf<UUserWidget>& WidgetClass)
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

void ACustomer::NotifyActorOnClicked(FKey ButtonPressed)
{
    Super::NotifyActorOnClicked(ButtonPressed);

    AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
    if (GameManager)
    {
        GameManager->OnCustomerClicked(this);
    }
}

void ACustomer::UpdateAngriness(float DeltaTime)
{
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

    if (AngryCounter > AngryThreshold)
    {
	    LeaveRestaurant();
    }
}

void ACustomer::UpdateMovement(float DeltaTime)
{
    if (HasReachedTarget())
    {
        bMovingToTarget = false;
        Velocity = FVector::ZeroVector;
        AddActorWorldOffset(Velocity * DeltaTime);
        return;
    }

    FVector Direction = (TargetPosition - GetActorLocation()).GetSafeNormal();
    Velocity = Direction * WalkSpeed;
    AddActorWorldOffset(Velocity * DeltaTime);
}

bool ACustomer::HasReachedTarget() const
{
    return FVector::Dist(GetActorLocation(), TargetPosition) <= DistanceThreshold;
}

void ACustomer::SetShowingBubble()
{
    float InitialInterval = FMath::RandRange(3.1f, 10.0f);

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

void ACustomer::OnCustomerBubbleSpawned()
{
    TimerHandle.Invalidate();

    FVector WorldLocation = GetActorLocation();

    FVector2D ScreenPos;
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    Bubble = CreateWidget<UUserWidget>(GetWorld(), BubbleWidget);
    if (PC && PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPos))
    {
        Bubble->AddToViewport();
        Bubble->SetPositionInViewport(ScreenPos, true);
    }

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

void ACustomer::HideBubble()
{
    Bubble->RemoveFromParent();
    TimerHandle.Invalidate();
}
