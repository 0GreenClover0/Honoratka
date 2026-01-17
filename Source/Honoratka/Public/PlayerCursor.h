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

    // Pendulum physics parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Physics")
    float PendulumLength = 20.0f; // Distance from cursor to icon center

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Physics")
    float Damping = 2.0f; // Resistance to swinging

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Physics")
    float Stiffness = 9.0f; // How strongly it returns to center

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swing Physics")
    float MouseInfluence = 1.0f; // How much mouse movement affects swing

private:
    FVector2D PreviousMousePosition = FVector2D::ZeroVector;
    FVector2D CurrentPosition = FVector2D::ZeroVector; // Current position relative to cursor
    FVector2D Velocity = FVector2D::ZeroVector;
    bool bFirstFrame = true;
};