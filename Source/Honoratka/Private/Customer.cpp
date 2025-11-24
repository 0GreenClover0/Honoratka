#include "Customer.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		TurnTowardTarget(DeltaTime);
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

	// If paired, move partner to an offset position
	if (PairedCustomer)
	{
		TargetPosition = Target + FVector(0, PairOffset, 0);

		FVector PartnerOffset = Target - FVector(0, PairOffset, 0); // Offset to the side
		PairedCustomer->TargetPosition = PartnerOffset;
		PairedCustomer->bMovingToTarget = true;
	}
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

void ACustomer::TurnTowardTarget(float DeltaTime)
{
	FVector ToTarget = (TargetPosition - GetActorLocation()).GetSafeNormal();
	FVector CurrentForward = GetActorForwardVector();
	
	FRotator TargetRotation = ToTarget.Rotation();
	FRotator CurrentRotation = GetActorRotation();
	
	FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, RotationSpeed * DeltaTime);
	SetActorRotation(NewRotation);
}

bool ACustomer::HasReachedTarget() const
{
	return FVector::Dist(GetActorLocation(), TargetPosition) <= DistanceThreshold;
}
