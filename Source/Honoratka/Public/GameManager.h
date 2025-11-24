#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

class ACustomer;
class AHonoratkaTable;
class ACustomerManager;

UCLASS()
class HONORATKA_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Selection system
	void SelectFirstCustomerGroup();
	void DeselectCustomers();
	bool HasSelectedCustomers() const { return SelectedCustomers.Num() > 0; }

	// Table assignment
	void OnTableClicked(AHonoratkaTable* Table);
	void OnCustomerClicked(ACustomer* Customer);

	// Customer manager reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TObjectPtr<ACustomerManager> CustomerManager;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

private:
	UPROPERTY()
	TArray<TObjectPtr<ACustomer>> SelectedCustomers;

	UPROPERTY()
	TObjectPtr<AHonoratkaTable> CurrentTable;

	void AssignCustomersToTable(AHonoratkaTable* Table);
	//void ReassignCustomerToTable(ACustomer* Customer, AHonoratkaTable* NewTable);
	void HighlightCustomers(bool bHighlight);
	AHonoratkaTable* FindCustomerTable(ACustomer* Customer) const;
};