#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Customer.h"
#include "CustomerAlbum.h"
#include "CustomerManager.generated.h"

class UCustomerWork;
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

UENUM(BlueprintType)
enum class ECustomerType : uint8
{
    Actor = 0,
    Director = 1,
    Screenwriter = 2,
};

USTRUCT(BlueprintType)
struct FCustomerType
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    ECustomerType CustomerType;

    UPROPERTY(EditAnywhere)
    bool bIsMale;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> BaseImage;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> SitImage;

    UPROPERTY(EditAnywhere)
    TArray<TObjectPtr<UTexture2D>> Accessory1Images;

    UPROPERTY(EditAnywhere)
    TArray<TObjectPtr<UTexture2D>> Accessory2Images;

    UPROPERTY(EditAnywhere)
    TArray<TObjectPtr<UTexture2D>> Accessory3Images;

    UPROPERTY(EditAnywhere)
    TArray<TObjectPtr<UTexture2D>> Accessory2SitImages;
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

    void ChangeCustomerTexture(ACustomer* Customer, bool bIsSitting, bool bMirror);

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void SetCustomerGroupPosition(ACustomer* customer, const FVector& Position) const;
    void SpawnCustomerGroup();
    TObjectPtr<ACustomer> SpawnSingleCustomer(const FVector& OffsetSpawnLocation, const FVector& LeaveTargetLocation, const FActorSpawnParameters& SpawnParams, bool bFamiliar);
    FCustomerTypeInstance GenerateRandomCustomerInstance() const;
    void UpdateQueuePositions();
    FVector GetQueuePositionForIndex(int32 Index) const;
    int32 GetCustomerTypeIndex(const FCustomerTypeInstance& CustomerTypeInstance);

#if WITH_EDITOR
    void DebugDrawQueue() const;
#endif

public:
    UPROPERTY(EditAnywhere, Category = "Customer Types")
    TArray<FCustomerType> CustomerTypes = {};

    UPROPERTY(EditAnywhere, Category = "Customer Types")
    TObjectPtr<UTexture2D> TransparentImage = nullptr;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    TSubclassOf<ACustomer> CustomerPrefab;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    TObjectPtr<ACustomerAlbum> CustomerAlbumReference;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    float MinSpawnInterval = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    float MaxSpawnInterval = 8.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning")
    float PairSpawnChance = 0.5f;

    // Customer bubble reference
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UCustomerBubbleWidget> BubbleWidget;

    UPROPERTY(EditAnywhere)
    TArray<UCustomerWork*> GreatWorks;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UCustomerGreatWorkWidget> GreatWorkWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
    float QueueSpacing = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
    FVector QueueDirection = FVector(0, 1, 0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Queue")
    float PairSideOffset = 35.0f;

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

    UFUNCTION()
    UMaterialInstanceDynamic* CreateAndSetCustomerImage(UStaticMeshComponent* Mesh, UTexture2D* Texture);

    UFUNCTION()
    void SetCustomerImage(UMaterialInstanceDynamic* MaterialInstance, UTexture2D* Texture);

    UPROPERTY()
    TArray<FCustomerSlot> CustomerQueue;

    float TimeSinceLastSpawn;
    float NextSpawnTime;

    float Happiness = 0.0f;
};
