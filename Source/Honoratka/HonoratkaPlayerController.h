#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "HonoratkaPlayerController.generated.h"

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

protected:
    // Initialize input bindings
    virtual void SetupInputComponent() override;
    
    // Input handlers
    void OnLeftMouseClickStarted();
    void OnLeftMouseClickTriggered();
    void OnLeftMouseClickReleased();

protected:
    // FX Class that we will spawn when clicking
    UPROPERTY(EditAnywhere, Category="Input")
    UNiagaraSystem* FXCursor = nullptr;

    // MappingContext
    UPROPERTY(EditAnywhere, Category="Input")
    UInputMappingContext* DefaultMappingContext = nullptr;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* LeftMouseClickAction = nullptr;

private:
    UPROPERTY()
    AInteractable* CurrentInteractable = nullptr;
};
