#include "PlayerCursor.h"
#include "Components/Image.h"

void UPlayerCursor::UpdateCursor(UTexture2D* CursorTexture)
{
    ensure(CursorImage);
    CursorImage->SetBrushFromTexture(CursorTexture, true);
}

void UPlayerCursor::UpdateHeldObjectIcon(UTexture2D* IconTexture)
{
    if (!IconTexture)
    {
        HeldObjectIcon->SetVisibility(ESlateVisibility::Hidden);
        // Reset physics when hiding
        CurrentPosition = FVector2D(0, PendulumLength); // Start at resting position (below cursor)
        Velocity = FVector2D::ZeroVector;
        bFirstFrame = true;
        return;
    }

    HeldObjectIcon->SetVisibility(ESlateVisibility::Visible);
    HeldObjectIcon->SetBrushFromTexture(IconTexture, true);
}

void UPlayerCursor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!GetOwningPlayer())
    {
        return;
    }

    float MouseX;
    float MouseY;
    if (GetOwningPlayer()->GetMousePosition(MouseX, MouseY))
    {
        FVector2D CurrentMousePosition(MouseX, MouseY);

        // Initialize on first frame
        if (bFirstFrame)
        {
            PreviousMousePosition = CurrentMousePosition;
            CurrentPosition = FVector2D(0, PendulumLength); // Start hanging down
            bFirstFrame = false;
        }

        // Update cursor position
        SetPositionInViewport(CurrentMousePosition, true);

        // Apply pendulum physics to held object icon if visible
        if (HeldObjectIcon && HeldObjectIcon->GetVisibility() == ESlateVisibility::Visible)
        {
            // Calculate mouse movement
            FVector2D MouseDelta = CurrentMousePosition - PreviousMousePosition;
            
            // Add mouse influence to velocity (the mouse "pushes" the pendulum)
            Velocity += MouseDelta * MouseInfluence;

            // Get current position
            float Distance = CurrentPosition.Size();
            
            if (Distance > 0.01f)
            {
                FVector2D Direction = CurrentPosition / Distance;
                
                // Calculate tangent vector (perpendicular to radius)
                FVector2D Tangent(-Direction.Y, Direction.X);
                
                // The torque (rotational force) pushes toward the down position
                // When icon is to the right (Direction.X > 0), we want to push it left (negative tangent)
                // When icon is to the left (Direction.X < 0), we want to push it right (positive tangent)
                // So we use Direction.X (positive sign) to make it hang DOWN instead of UP
                float TorqueMagnitude = Direction.X * Stiffness * 100.0f;
                
                FVector2D GravityForce = Tangent * TorqueMagnitude;
                
                // Damping opposes velocity
                FVector2D DampingForce = -Velocity * Damping;
                
                // Total acceleration
                FVector2D Acceleration = GravityForce + DampingForce;
                
                // Update velocity and position
                Velocity += Acceleration * InDeltaTime;
                CurrentPosition += Velocity * InDeltaTime;

                // Constrain to circular path (pendulum stays at fixed distance)
                float NewDistance = CurrentPosition.Size();
                if (NewDistance > 0.01f)
                {
                    // Project position onto circle of radius PendulumLength
                    FVector2D NewDirection = CurrentPosition / NewDistance;
                    CurrentPosition = NewDirection * PendulumLength;
                    
                    // Remove radial velocity component, keep only tangential
                    FVector2D RadialVelocity = NewDirection * FVector2D::DotProduct(Velocity, NewDirection);
                    Velocity -= RadialVelocity;
                }
                else
                {
                    CurrentPosition = FVector2D(0, PendulumLength);
                }
            }
            else
            {
                // If at center, set to resting position (hanging down)
                CurrentPosition = FVector2D(0, PendulumLength);
                Velocity = FVector2D::ZeroVector;
            }

            // Apply the offset to the held object icon
            HeldObjectIcon->SetRenderTranslation(CurrentPosition);
        }

        PreviousMousePosition = CurrentMousePosition;
    }
}