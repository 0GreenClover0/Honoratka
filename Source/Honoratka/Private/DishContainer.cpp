#include "DishContainer.h"

#include "PlayerManager.h"

void ADishContainer::Interact_Implementation()
{
    Super::Interact_Implementation();

    PlayerManager->SelectObject(this, IconWhenGrabbed);
}

void ADishContainer::Uninteract_Implementation()
{
    Super::Uninteract_Implementation();

    PlayerManager->UnselectObject();
}
