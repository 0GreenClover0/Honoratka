#include "PlayerCursor.h"

#include "Components/Image.h"

void UPlayerCursor::UpdateCursor(UTexture2D* CursorTexture)
{
    if (CursorImage == nullptr)
    {
        return;
    }

    CursorImage->SetBrushFromTexture(CursorTexture, true);
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
