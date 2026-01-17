#include "PlayerCursor.h"
#include "Components/Image.h"

void UPlayerCursor::UpdateCursor(UTexture2D* CursorTexture)
{
    ensure(CursorImage);
    CursorImage->SetBrushFromTexture(CursorTexture, true);
}

void UPlayerCursor::UpdateHeldObjectIcon(UTexture2D* IconTexture)
{
    if (!IconTexture)
    {
        HeldObjectIcon->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    HeldObjectIcon->SetVisibility(ESlateVisibility::Visible);
    HeldObjectIcon->SetBrushFromTexture(IconTexture, true);
}

void UPlayerCursor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!GetOwningPlayer())
    {
        return;
    }

    float MouseX;
    float MouseY;
    if (GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
    {
        SetPositionInViewport(
            FVector2D(MouseX, MouseY),
            true // bRemoveDPIScale
        );
    }
}
