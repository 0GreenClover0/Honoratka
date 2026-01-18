#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HonoratkaTable.generated.h"

class AGameManager;
class ACustomer;

UENUM()
enum class ESeatType
{
    Right,
    Bottom,
    Left,
    Top,
};

USTRUCT()
struct FTableSeat
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<ACustomer> Customer = nullptr;

    UPROPERTY()
    FVector SeatPosition;

    UPROPERTY()
    ESeatType SeatType;

    bool IsOccupied() const { return Customer != nullptr; }
};

UCLASS()
class HONORATKA_API AHonoratkaTable : public AActor
{
    GENERATED_BODY()
    
public:
    AHonoratkaTable();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Table management
    bool CanSeatCustomers(int32 Count) const;
    bool SeatCustomers(TArray<ACustomer*> Customers);
    void RemoveCustomer(ACustomer* Customer);
    void ClearTable();

    // Getters
    int32 GetAvailableSeats() const;
    int32 GetOccupiedSeats() const;
    TArray<ACustomer*> GetCustomers() const;
    bool IsTableFull() const { return GetAvailableSeats() == 0; }
    bool IsTableEmpty() const { return GetOccupiedSeats() == 0; }
    bool HasCustomer(ACustomer* Customer) const;
    FTableSeat GetCustomerSeat(ACustomer* Customer) const;

    // Click handling
    virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

private:
    void InitializeSeats();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table")
    int32 MaxSeats = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table")
    float SeatRadius = 70.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
    TArray<USceneComponent*> SeatComponents;

private:
    UPROPERTY()
    AGameManager* GameManager = nullptr;

    UPROPERTY()
    TArray<FTableSeat> Seats;
};
