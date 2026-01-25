#pragma once

#include "CoreMinimal.h"
#include "Timer.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "HonoratkaPlayerController.generated.h"

class ACustomerManager;
class UHonoratkaHUDWidget;
class AInteractable;
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Player controller for a top-down perspective game.
 *  Implements point and click based controls.
 */
UCLASS(abstract)
class AHonoratkaPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AHonoratkaPlayerController();

    UFUNCTION()
    UHonoratkaHUDWidget* GetHonoratkaHUD() const { return HUDWidget; }

    UPROPERTY()
    TObjectPtr<ACustomerManager> CustomerManagerPtr;

protected:
    // Initialize input bindings
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;

    // Input handlers
    void OnLeftMouseClickStarted();
    void OnLeftMouseClickTriggered();
    void OnLeftMouseClickReleased();

    // FX Class that we will spawn when clicking
    UPROPERTY(EditAnywhere, Category = "Input")
    UNiagaraSystem* FXCursor = nullptr;

    // MappingContext
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* DefaultMappingContext = nullptr;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LeftMouseClickAction = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY()
    TObjectPtr<UHonoratkaHUDWidget> HUDWidget;

private:

    UPROPERTY()
    TSubclassOf<ATimer> Timer;

    UPROPERTY()
    AInteractable* CurrentInteractable = nullptr;
};
