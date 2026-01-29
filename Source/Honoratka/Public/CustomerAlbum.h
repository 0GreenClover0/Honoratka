#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Customer.h"
#include "CustomerAlbum.generated.h"

UCLASS()
class HONORATKA_API ACustomerAlbum : public AActor
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer Spawning", meta = (ClampMin = 0, ClampMax = 64))
    int32 FamiliarCustomersNum = 12;

    TArray<FCustomerTypeInstance> GetAllFamiliarCustomers() const { return FamiliarCustomers; }
    bool IsCustomerFamiliar(const FCustomerTypeInstance& CustomerTypeInstance) const;
    void AddFamiliarCustomer(const FCustomerTypeInstance& CustomerTypeInstance);

private:
    UPROPERTY()
    TArray<FCustomerTypeInstance> FamiliarCustomers;
};
