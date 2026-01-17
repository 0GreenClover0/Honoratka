#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "DishContainer.generated.h"

enum class EFoodType : uint8;

/**
 * Parent class for all containers that the Player can take the dish out from.
 */
UCLASS()
class HONORATKA_API ADishContainer : public AInteractable
{
    GENERATED_BODY()

public:
    virtual void Interact_Implementation() override;
    virtual void Uninteract_Implementation() override;

public:
    UPROPERTY(EditAnywhere)
    EFoodType FoodType;
};
