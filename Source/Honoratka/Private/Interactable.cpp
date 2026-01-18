#include "Interactable.h"

#include "PlayerManager.h"
#include "Kismet/GameplayStatics.h"
#include <Components/BoxComponent.h>

AInteractable::AInteractable()
{
    PrimaryActorTick.bCanEverTick = true;
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Game/Honoratka/Materials/M_OutlineShader.M_OutlineShader"));
    OutlineMaterial = MatFinder.Object;
}

void AInteractable::BeginPlay()
{
    Super::BeginPlay();

    PlayerManager = Cast<APlayerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerManager::StaticClass()));

    MainMesh = Cast<UMeshComponent>(GetDefaultSubobjectByName(TEXT("MainMesh")));
    Collider = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collider")));

    Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collider->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    Collider->OnBeginCursorOver.AddDynamic(this, &AInteractable::OnHoverBegin);
    Collider->OnEndCursorOver.AddDynamic(this, &AInteractable::OnHoverEnd);
}

void AInteractable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AInteractable::Interact_Implementation()
{
}

void AInteractable::Uninteract_Implementation()
{
}

void AInteractable::OnHoverBegin(UPrimitiveComponent* TouchedComponent)
{
    MainMesh->SetOverlayMaterial(OutlineMaterial);

    if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(MainMesh))
    {
        StaticMesh->bDisallowNanite = true;
    }
}

void AInteractable::OnHoverEnd(UPrimitiveComponent* TouchedComponent)
{
    MainMesh->SetOverlayMaterial(nullptr);

    if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(MainMesh))
    {
        StaticMesh->bDisallowNanite = false;
    }
}
