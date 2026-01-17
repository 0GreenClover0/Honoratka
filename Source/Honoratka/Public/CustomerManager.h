#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Customer.h"
#include "CustomerManager.generated.h"

class UCustomerBubbleWidget;
class UCustomerGreatWorkWidget;

USTRUCT()
struct FCustomerSlot
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<ACustomer> Customer;

    UPROPERTY()
    FVector QueuePosition;
};

USTRUCT(BlueprintType)
struct FGreatCustomerWork
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> PosterTexture;

    UPROPERTY(EditAnywhere)
    FText Description;
};

UCLASS()
class HONORATKA_API ACustomerManager : public AActor
{
    GENERATED_BODY()

public:
    ACustomerManager();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Queue management
    void MoveQueueForward();
    void RemoveCustomerFromQueue(ACustomer* Customer);
    int32 GetQueueLength() const { return CustomerQueue.Num(); }
    ACustomer* GetFirstCustomerInQueue() const;

    void ModifyHappiness(float Amount);

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void SetCustomerGroupPosition(ACustomer* customer, const FVector& Position) const;
    void SpawnCustomerGroup();
    void UpdateQueuePositions();
    FVector GetQueuePositionForIndex(int32 Index) const;

#if WITH_EDITOR
    void DebugDrawQueue() const;
#endif

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Works")
    TArray<FGreatCustomerWork> GreatCustomerWorks = {};

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    TSubclassOf<ACustomer> CustomerPrefab;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    float MinSpawnInterval = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    float MaxSpawnInterval = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    float PairSpawnChance = 0.5f;

    // Customer bubble reference
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UCustomerBubbleWidget> BubbleWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UCustomerGreatWorkWidget> GreatWorkWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
    float QueueSpacing = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
    FVector QueueDirection = FVector(0, 1, 0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
    float PairSideOffset = 50.0f;

    // Spawn point
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawning")
    TObjectPtr<USceneComponent> CustomerSpawnPoint;

    // First customer point
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawning")
    TObjectPtr<USceneComponent> CustomerQueueFrontPoint;

    // Debugging
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
    bool bDebugDrawQueue = false;

    // Number of customers
    int NumberOfCustomersInQueue = 0;
    int NumberOfCustomersInside = 0;

private:
    UPROPERTY()
    TArray<FCustomerSlot> CustomerQueue;

    float TimeSinceLastSpawn;
    float NextSpawnTime;

    float Happiness = 0.0f;
};
