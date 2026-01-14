#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "DishContainer.generated.h"

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
};
