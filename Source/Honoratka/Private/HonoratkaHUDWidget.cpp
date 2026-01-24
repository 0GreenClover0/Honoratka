#include "HonoratkaHUDWidget.h"
#include "TimerWidget.h"

void UHonoratkaHUDWidget::SetTimer(ATimer* Timer)
{
    if (TimerWidget)
    {
        TimerWidget->SetTimer(Timer);
    }
}
