#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HonoratkaTable.generated.h"

class ACustomer;

USTRUCT()
struct FTableSeat
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ACustomer> Customer = nullptr;

	UPROPERTY()
	FVector SeatPosition;

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
	bool IsTableFull() const { return GetAvailableSeats() == 0; }
	bool IsTableEmpty() const { return GetOccupiedSeats() == 0; }
	bool HasCustomer(ACustomer* Customer) const;

	// Click handling
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table")
	int32 MaxSeats = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Table")
	float SeatRadius = 60.0f;

private:
	UPROPERTY()
	TArray<FTableSeat> Seats;

	void InitializeSeats();
	FVector CalculateSeatPosition(int32 SeatIndex) const;
};
