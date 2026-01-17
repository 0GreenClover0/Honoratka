#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Customer.generated.h"

class AGameManager;
class UCustomerBubbleWidget;
enum class EFoodType : uint8;
class ACustomerManager;
class AHonoratkaTable;

UENUM(BlueprintType)
enum class ECustomerState : uint8
{
    Idle = 0,
    WaitingInQueue = 1,
    Seated = 2,
    Leaving = 3
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
    void LeaveRestaurant();
    void SeatCustomer(AHonoratkaTable* TableToSeat, FVector const& Position);

    // Bubble and Food
    void SetWidgetClass(const TSubclassOf<UCustomerBubbleWidget>& WidgetClass);
    void SetShowingBubble();
    void SelectDesiredFoodItem();
    void ShowBubble(UTexture2D* BubbleTexture);

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

    void SetLeaveTargetPosition(FVector const& Position);
    void SetCustomerManager(ACustomerManager* NewCustomerManager);

    void GiveDish(EFoodType FoodType);

    // Click handling
    virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

    // Public delegate others can bind to
    UPROPERTY(BlueprintAssignable)
    FSpawnBubbleDelegate OnBubbleSpawned;

public:
	UPROPERTY()
    TObjectPtr<AHonoratkaTable> Table = nullptr;

    UPROPERTY(EditAnywhere)
    UTexture2D* HappyTexture = nullptr;

    UPROPERTY(EditAnywhere)
    UTexture2D* MadTexture = nullptr;

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 150.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    float RotationSpeed = 5.0f;

    UPROPERTY(BlueprintReadWrite, Category = "Queue")
    int32 QueuePosition = -1;

private:
    UPROPERTY()
    AGameManager* GameManager = nullptr;

    UPROPERTY()
    ACustomerManager* CustomerManager;

    UPROPERTY()
    ECustomerState CurrentState;

    UPROPERTY()
    TObjectPtr<ACustomer> PairedCustomer = nullptr;

    TSubclassOf<UCustomerBubbleWidget> BubbleWidget;

    UPROPERTY()
    float PairOffset;

    UPROPERTY()
    FVector TargetPosition;

    UPROPERTY()
    FVector Velocity;

    UPROPERTY()
    bool bMovingToTarget;

    // Distance to the target threshold.
    UPROPERTY()
    float DistanceThreshold = 10.0f;

    UPROPERTY()
    bool bSelected = false;

    UPROPERTY()
    bool bHasShownBubble = false;

    UPROPERTY()
    TObjectPtr<UCustomerBubbleWidget> Bubble = nullptr;

    UPROPERTY()
    EFoodType DesiredFoodType;

    UPROPERTY()
    TObjectPtr<UTexture2D> DesiredFoodTexture;

    UPROPERTY()
    float AngryCounter = 0.0f;

    // Threshold of angriness (in seconds) for the customer to leave.
    UPROPERTY()
    float AngryThreshold = 30.0f;

    UPROPERTY()
    FVector LeaveTargetPosition;

    FTimerHandle TimerHandle;

    void UpdateAngriness(float DeltaTime);
    void UpdateMovement(float DeltaTime);
    bool HasReachedTarget() const;
    void OnCustomerBubbleSpawned();
    void HideBubble();
};
