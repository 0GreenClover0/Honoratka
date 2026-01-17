#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomerBubbleWidget.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class HONORATKA_API UCustomerBubbleWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetVisible(bool bVisible, const FVector& WorldLocation = FVector::ZeroVector);
    void SetPosition(const FVector& WorldLocation);
    void SetTexture(UTexture2D* NewTexture);

protected:
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UImage> BubbleImage;

    UPROPERTY()
    TObjectPtr<UTexture2D> Texture;
};
