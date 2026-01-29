#include "Timer.h"

#include "GameManager.h"
#include "Kismet/GameplayStatics.h"

ATimer::ATimer()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATimer::SetTimer()
{
    GetWorld()->GetTimerManager().SetTimer
    (
        TimerHandle,                     // handle to cancel timer at a later time
        this,                               // the owning object
        &ATimer::OnTimerFired,              // function to call on elapsed
        TimerInterval,                      // float delay until elapsed
        true                                // looping?
    );

    TimeLeft = MaxTime;
}

FString ATimer::GetTimeLeftReadable()
{
    int32 TotalSeconds = FMath::FloorToInt(TimeLeft);
    int32 Minutes = TotalSeconds / 60;
    int32 Secs = TotalSeconds % 60;

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Secs);
}

float ATimer::GetTimeLeft() const
{
    return TimeLeft;
}

void ATimer::BeginPlay()
{
    Super::BeginPlay();

    GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));

    SetTimer();
}

void ATimer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATimer::OnTimerFired()
{
    TimeLeft -= TimerInterval;

    if (!bHasPlayedWarningSound && TimeLeft <= WarningTime)
    {
        bHasPlayedWarningSound = true;
        UGameplayStatics::PlaySound2D(GetWorld(), GameManager->ClockWarningSound);
    }

    if (TimeLeft <= TimerInterval)
    {
        FinishTimer();
    }
}

void ATimer::FinishTimer()
{
    OnTimerFinished.Broadcast();
    GameManager->FinishDay();
}
