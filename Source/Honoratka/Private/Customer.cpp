#include "Customer.h"

#include "GameManager.h"
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

	if (bMovingToTarget)
	{
		UpdateMovement(DeltaTime);
	}
}

void ACustomer::SetQueuePosition(int32 Position)
{
	QueuePosition = Position;
}

void ACustomer::MoveForward()
{
	if (CurrentState == ECustomerState::WaitingInQueue || CurrentState == ECustomerState::MovingForward)
	{
		SetCustomerState(ECustomerState::MovingForward);
	}
}

void ACustomer::LeaveRestaurant()
{
	SetCustomerState(ECustomerState::Leaving);
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

void ACustomer::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
	if (GameManager)
	{
		GameManager->OnCustomerClicked(this);
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
