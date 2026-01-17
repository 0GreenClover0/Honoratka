#include "CustomerBubbleWidget.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

void UCustomerBubbleWidget::SetVisible(bool bVisible, const FVector& WorldLocation)
{
    FVector2D ScreenPos;
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PC && PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPos))
    {
        AddToViewport();
        SetPositionInViewport(ScreenPos, true);
    }
}

void UCustomerBubbleWidget::SetPosition(const FVector& WorldLocation)
{
    FVector2D ScreenPos;
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PC && PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPos))
    {
        SetPositionInViewport(ScreenPos, true);
    }
}

void UCustomerBubbleWidget::SetTexture(UTexture2D* NewTexture)
{
    Texture = NewTexture;

    BubbleImage->SetBrushFromTexture(Texture, true);
}
