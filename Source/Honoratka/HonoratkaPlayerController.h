#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "HonoratkaPlayerController.generated.h"

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

protected:
    // FX Class that we will spawn when clicking
    UPROPERTY(EditAnywhere, Category="Input")
    UNiagaraSystem* FXCursor;

    // MappingContext
    UPROPERTY(EditAnywhere, Category="Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, Category="Input")
    UInputAction* LeftMouseClickAction;

public:
    AHonoratkaPlayerController();

protected:
    // Initialize input bindings
    virtual void SetupInputComponent() override;
    
    // Input handlers
    void OnLeftMouseClickStarted();
    void OnLeftMouseClickTriggered();
    void OnLeftMouseClickReleased();
};
