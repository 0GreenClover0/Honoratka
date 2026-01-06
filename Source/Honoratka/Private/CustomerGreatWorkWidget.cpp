#include "CustomerGreatWorkWidget.h"

#include "GameManager.h"
#include "Kismet/GameplayStatics.h"

void UCustomerGreatWorkWidget::NativeConstruct()
{
    Super::NativeConstruct();

    AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
    if (GameManager)
    {
        GameManager->Pause();
    }
}

void UCustomerGreatWorkWidget::NativeDestruct()
{
    Super::NativeDestruct();

    AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
    if (GameManager)
    {
        GameManager->Resume();
    }
}

void UCustomerGreatWorkWidget::Close()
{
    RemoveFromParent();
}
