#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class ACustomer;
class AHonoratkaTable;
class ACustomerManager;

UENUM(BlueprintType)
enum class EFoodType : uint8
{
    None = 0,
    Coffee = 1,
    Cake = 2,
};

USTRUCT(BlueprintType)
struct FFoodItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> Texture;

    UPROPERTY(EditAnywhere)
    EFoodType FoodType;
};

UCLASS()
class HONORATKA_API AGameManager : public AActor
{
    GENERATED_BODY()

public:
    AGameManager();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Selection system
    void DeselectCustomers();
    bool HasSelectedCustomers() const { return SelectedCustomers.Num() > 0; }

    // Table assignment
    void OnTableClicked(AHonoratkaTable* Table);
    void OnCustomerClicked(ACustomer* Customer);

    // Customer manager reference
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
    TObjectPtr<ACustomerManager> CustomerManager;

    UPROPERTY(EditAnywhere)
    TArray<FFoodItem> FoodItems;

private:
    UPROPERTY()
    TArray<TObjectPtr<ACustomer>> SelectedCustomers;

    UPROPERTY()
    TObjectPtr<AHonoratkaTable> PreviousTable;

    void AssignCustomersToTable(AHonoratkaTable* Table);
    void HighlightCustomers(bool bHighlight);
    AHonoratkaTable* FindCustomerTable(ACustomer* Customer) const;
};
