#include "Clock.h"

#include "Timer.h"
#include "Kismet/GameplayStatics.h"

AClock::AClock()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AClock::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (LevelTimer == nullptr)
    {
        LevelTimer = Cast<ATimer>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimer::StaticClass()));

        if (LevelTimer == nullptr)
        {
            return;
        }
    }

    ElapsedTime += DeltaTime;

    float Alpha = FMath::Clamp(ElapsedTime / LevelTimer->MaxTime, 0.0f, 1.0f);

    float Angle = Alpha * 360.0f;

    FRotator NewRotation(-Angle, 0.0f, 0.0f);
    MinuteHand->SetRelativeRotation(NewRotation);
}
