#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Customer.generated.h"

class AHonoratkaTable;

UENUM(BlueprintType)
enum class ECustomerState : uint8
{
    Idle = 0,
    WaitingInQueue = 1,
    MovingForward = 2,
    Seated = 3,
    Leaving = 4
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpawnBubbleDelegate);

UCLASS()
class HONORATKA_API ACustomer : public ACharacter
{
    GENERATED_BODY()

public:
    ACustomer();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Queue management
    void SetQueuePosition(int32 Position);
    void MoveForward();
    void LeaveRestaurant();

    // Bubble
    void SetWidgetClass(const TSubclassOf<UUserWidget>& WidgetClass);
    void SetShowingBubble();

    // State management
    void SetCustomerState(ECustomerState NewState);
    ECustomerState GetCustomerState() const { return CurrentState; }

    // Position target
    void SetTargetPosition(const FVector& Target);
    FVector GetTargetPosition() const { return TargetPosition; }

    // Pair management
    void SetPairedCustomer(ACustomer* InPair);
    ACustomer* GetPairedCustomer() const { return PairedCustomer; }
    bool IsPaired() const { return PairedCustomer != nullptr; }
    void SetPairOffset(float Offset);

    // Selection
    void SetCustomerSelected(bool bIsSelected);
    bool IsCustomerSelected() const { return bSelected; }

    // Click handling
    virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

    // Public delegate others can bind to
    UPROPERTY(BlueprintAssignable)
    FSpawnBubbleDelegate OnBubbleSpawned;

public:
	UPROPERTY()
    TObjectPtr<AHonoratkaTable> Table = nullptr;

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 150.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    float RotationSpeed = 5.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Queue")
    int32 QueuePosition = -1;

private:
    UPROPERTY()
    ECustomerState CurrentState;

    UPROPERTY()
    TObjectPtr<ACustomer> PairedCustomer = nullptr;

    TSubclassOf<UUserWidget> BubbleWidget;

    UPROPERTY()
    float PairOffset;

    UPROPERTY()
    FVector TargetPosition;

    UPROPERTY()
    FVector Velocity;

    UPROPERTY()
    bool bMovingToTarget;

    UPROPERTY()
    float DistanceThreshold = 10.0f;

    UPROPERTY()
    bool bSelected = false;

    UPROPERTY()
    bool bHasShownBubble = false;

    UPROPERTY()
    TObjectPtr<UUserWidget> Bubble;

    FTimerHandle TimerHandle;

    void UpdateMovement(float DeltaTime);
    bool HasReachedTarget() const;
    void OnCustomerBubbleSpawned();
    void HideBubble();
};
