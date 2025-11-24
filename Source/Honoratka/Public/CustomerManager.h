#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Customer.h"
#include "CustomerManager.generated.h"

USTRUCT()
struct FCustomerSlot
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ACustomer> Customer;

	UPROPERTY()
	FVector QueuePosition;
};

UCLASS()
class HONORATKA_API ACustomerManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ACustomerManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
	TSubclassOf<ACustomer> CustomerPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
	float MinSpawnInterval = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
	float MaxSpawnInterval = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
	float PairSpawnChance = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
	float QueueSpacing = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
	FVector QueueDirection = FVector(0, 1, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
	FVector CounterPosition = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
	float PairSideOffset = 50.0f;

	// Spawn point
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawning")
	TObjectPtr<USceneComponent> CustomerSpawnPoint;

	// First customer point
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Spawning")
	TObjectPtr<USceneComponent> CustomerQueueFrontPoint;

	// Queue management
	void MoveQueueForward();
	int32 GetQueueLength() const { return CustomerQueue.Num(); }

	// Debugging
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool bDebugDrawQueue = false;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TArray<FCustomerSlot> CustomerQueue;

	float TimeSinceLastSpawn;
	float NextSpawnTime;

	void SpawnCustomerGroup();
	void UpdateQueuePositions();
	void RemoveCustomerFromQueue(ACustomer* Customer);
	FVector GetQueuePositionForIndex(int32 Index) const;

#if WITH_EDITOR
	void DebugDrawQueue() const;
#endif
};
