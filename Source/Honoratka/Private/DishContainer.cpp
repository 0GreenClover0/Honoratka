#include "DishContainer.h"

#include "Customer.h"
#include "PlayerManager.h"
#include "Kismet/GameplayStatics.h"

void ADishContainer::Interact_Implementation()
{
    Super::Interact_Implementation();

    PlayerManager->SelectObject(this, IconWhenGrabbed);
}

void ADishContainer::Uninteract_Implementation()
{
    Super::Uninteract_Implementation();

    PlayerManager->UnselectObject();

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    FHitResult Hit;
    bool bHitSuccessful = PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

    if (!bHitSuccessful || !Hit.GetActor())
    {
        // Drop the item.
        return;
    }

    ACustomer* Customer = Cast<ACustomer>(Hit.GetActor());

    if (!Customer)
    {
        // Drop the item.
        return;
    }

    Customer->GiveDish(FoodType);
}
