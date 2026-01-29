#include "CustomerWork.h"

#include "CustomerGreatWorkWidget.h"
#include "CustomerManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

UCustomerWork::UCustomerWork()
{
    PrimaryComponentTick.bCanEverTick = true;
}

bool UCustomerWork::AdvanceWork(float Amount)
{
    if (bWorkFinished)
    {
        return false;
    }

    WorkProgress += WorkSpeed * Amount;

    if (WorkProgress >= 100.0f)
    {
        bWorkFinished = true;

        ACustomerManager* CustomerManager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
        GreatWorkWidget = CreateWidget<UCustomerGreatWorkWidget>(GetWorld(), CustomerManager->GreatWorkWidgetClass);

        GreatWorkWidget->AddToViewport();
        GreatWorkWidget->GetPosterImage()->SetBrushFromTexture(PosterTexture);
        GreatWorkWidget->GetDescriptionText()->SetText(Description);
    }

    return true;
}

const TArray<FGreatWorkRequirements>& UCustomerWork::GetRequirements() const
{
    return Requirements;
}

bool UCustomerWork::IsFinished() const
{
    return bWorkFinished;
}
