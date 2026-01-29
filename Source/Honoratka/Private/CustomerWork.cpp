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

bool UCustomerWork::AdvanceWork(float Amount, TSubclassOf<UCustomerGreatWorkWidget> Widget)
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

        if (Widget == nullptr)
        {
            GreatWorkWidget = CreateWidget<UCustomerGreatWorkWidget>(GetWorld(), CustomerManager->GreatWorkWidgetClass);
        }
        else
        {
            GreatWorkWidget = CreateWidget<UCustomerGreatWorkWidget>(GetWorld(), Widget);
        }

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

FText UCustomerWork::GetDescription() const
{
    return Description;
}

void UCustomerWork::SetDescription(const FText& NewDescription)
{
    Description = NewDescription;
}
