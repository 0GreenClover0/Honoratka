#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Clock.generated.h"

class ATimer;

UCLASS()
class HONORATKA_API AClock : public AActor
{
    GENERATED_BODY()
    
public:    
    AClock();

    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<USceneComponent> MinuteHand = nullptr;

private:
    UPROPERTY()
    TObjectPtr<ATimer> LevelTimer = nullptr;

    float ElapsedTime = 0.0f;
};
