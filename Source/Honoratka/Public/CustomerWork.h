#pragma once

#include "CoreMinimal.h"
#include "CustomerManager.h"
#include "Components/ActorComponent.h"
#include "CustomerWork.generated.h"

class ACustomer;

USTRUCT()
struct FGreatWorkRequirements
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int8 MaleActors;

    UPROPERTY(EditAnywhere)
    int8 FemaleActors;

    UPROPERTY(EditAnywhere)
    int8 Directors;

    UPROPERTY(EditAnywhere)
    int8 Screenwriters;

    bool operator==(const FGreatWorkRequirements& Requirements) const = default;
};

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HONORATKA_API UCustomerWork : public UActorComponent
{
    GENERATED_BODY()

public:    
    UCustomerWork();
    
    bool AdvanceWork(float Amount);

    const TArray<FGreatWorkRequirements>& GetRequirements() const;

protected:
    UPROPERTY()
    float WorkProgress = 0.0f;

    UPROPERTY()
    bool bWorkFinished = false;

    UPROPERTY(EditAnywhere, Category="Great Work")
    float WorkSpeed = 10.0f;

    UPROPERTY(EditAnywhere, Category="Great Work")
    TObjectPtr<UTexture2D> PosterTexture;

    UPROPERTY(EditAnywhere, meta = (MultiLine="true"), Category="Great Work")
    FText Description;

    UPROPERTY(EditAnywhere, Category="Great Work")
    TArray<FGreatWorkRequirements> Requirements;

    UPROPERTY()
    TObjectPtr<UCustomerGreatWorkWidget> GreatWorkWidget = nullptr;

private:
    UPROPERTY()
    TObjectPtr<ACustomer> Customer = nullptr;
};
