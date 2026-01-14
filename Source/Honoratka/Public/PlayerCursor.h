#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerCursor.generated.h"

class UImage;

UCLASS()
class HONORATKA_API UPlayerCursor : public UUserWidget
{
    GENERATED_BODY()

public:
    void UpdateCursor(UTexture2D* CursorTexture);

    void UpdateHeldObjectIcon(UTexture2D* IconTexture);

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UImage> CursorImage;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UImage> HeldObjectIcon;
};
