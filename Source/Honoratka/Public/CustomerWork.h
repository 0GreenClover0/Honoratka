#pragma once

#include "CoreMinimal.h"
#include "CustomerManager.h"
#include "Components/ActorComponent.h"
#include "CustomerWork.generated.h"

class ACustomer;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONORATKA_API UCustomerWork : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCustomerWork();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY()
	FGreatCustomerWork GreatWork;

protected:
	UPROPERTY()
	float WorkProgress = 0.0f;

	float DebugTime = 8.0f;

	UPROPERTY(EditAnywhere)
	float WorkSpeed = 10.0f;

	UPROPERTY()
	bool bWorkFinished = false;

	UPROPERTY()
	TObjectPtr<UCustomerGreatWorkWidget> GreatWorkWidget = nullptr;

private:
	UPROPERTY()
	TObjectPtr<ACustomer> Customer = nullptr;
};
