#include "HonoratkaPlayerController.h"

#include "CustomerManager.h"
#include "GameFramework/Pawn.h"
#include "HonoratkaCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "Honoratka.h"
#include "HonoratkaHUDWidget.h"
#include "Interactable.h"
#include "TimerWidget.h"

AHonoratkaPlayerController::AHonoratkaPlayerController()
{
    bShowMouseCursor = false;
    DefaultMouseCursor = EMouseCursor::Default;
}

void AHonoratkaPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Only set up input on local player controllers.
    if (IsLocalPlayerController())
    {
        // Add Input Mapping Context.
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }

        // Set up action bindings.
        if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
        {
            // Setup mouse input events.
            EnhancedInputComponent->BindAction(LeftMouseClickAction, ETriggerEvent::Started, this, &AHonoratkaPlayerController::OnLeftMouseClickStarted);
            EnhancedInputComponent->BindAction(LeftMouseClickAction, ETriggerEvent::Triggered, this, &AHonoratkaPlayerController::OnLeftMouseClickTriggered);
            EnhancedInputComponent->BindAction(LeftMouseClickAction, ETriggerEvent::Completed, this, &AHonoratkaPlayerController::OnLeftMouseClickReleased);
            EnhancedInputComponent->BindAction(LeftMouseClickAction, ETriggerEvent::Canceled, this, &AHonoratkaPlayerController::OnLeftMouseClickReleased);
        }
        else
        {
            UE_LOG(LogHonoratka, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
        }
    }
}

void AHonoratkaPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && HUDWidgetClass)
    {
        HUDWidget = Cast<UHonoratkaHUDWidget>(CreateWidget<UUserWidget>(this, HUDWidgetClass));

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ATimer* TimerActor = GetWorld()->SpawnActor<ATimer>(
            ATimer::StaticClass(),
            FTransform(),
            Params
        );

        HUDWidget->SetTimer(TimerActor);

        // Pass known customers to album
        for (const FCustomerTypeInstance Customer : CustomerManagerPtr->CustomerAlbumReference->GetAllFamiliarCustomers())
        {
            HUDWidget->RegisterCustomer(Customer);
        }

        HUDWidget->AddToViewport();
    }
}

void AHonoratkaPlayerController::OnLeftMouseClickStarted()
{
    // We look for the location in the world where the player has pressed the input.
    FHitResult Hit;
    bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

    if (!bHitSuccessful || !Hit.GetActor())
    {
        return;
    }

    AInteractable* Interactable = Cast<AInteractable>(Hit.GetActor());

    if (!Interactable)
    {
        return;
    }

    CurrentInteractable = Interactable;
    Interactable->Interact();
}

void AHonoratkaPlayerController::OnLeftMouseClickTriggered()
{
}

void AHonoratkaPlayerController::OnLeftMouseClickReleased()
{
    if (CurrentInteractable)
    {
        CurrentInteractable->Uninteract();
    }

    CurrentInteractable = nullptr;
}
