// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CrosshairWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "GAS/LOLAbilitySystemStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC) {
		OwnerASC->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetCrosshairTag()).AddUObject(this, &UCrosshairWidget::CrosshairTagUpdated);
		OwnerASC->GenericGameplayEventCallbacks.Add(ULOLAbilitySystemStatics::GetTargetUpdatedTag()).AddUObject(this, &UCrosshairWidget::TargetUpdated);
	}CachedPlayerController = GetOwningPlayer();
	CrosshairCanvasPanelSlot = Cast<UCanvasPanelSlot>(Slot);
	if (!CrosshairCanvasPanelSlot) {
		UE_LOG(LogTemp, Error, TEXT("Crosshair Widget Need a Canvas Panel!"));
	}
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (CrosshairImage->GetVisibility() == ESlateVisibility::Visible) {
		UpdateCrossHairPosition();
	}
}

void UCrosshairWidget::CrosshairTagUpdated(const FGameplayTag, int32 NewCount)
{
	CrosshairImage->SetVisibility(NewCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UCrosshairWidget::UpdateCrossHairPosition()
{
	if (!CachedPlayerController || !CrosshairCanvasPanelSlot) return;
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	int32 SizeX, SizeY;
	CachedPlayerController->GetViewportSize(SizeX, SizeY);
	if (!AimTarget) {
		FVector2D ViewportSize = FVector2D((float)SizeX, (float)SizeY);
		CrosshairCanvasPanelSlot->SetPosition(ViewportSize / 2.f / ViewportScale);
		return;
	}
	FVector2D TargetScreenPosition;
	CachedPlayerController->ProjectWorldLocationToScreen(AimTarget->GetActorLocation(), TargetScreenPosition);
	if (TargetScreenPosition.X > 0 && TargetScreenPosition.X < SizeX && TargetScreenPosition.Y>0 && TargetScreenPosition.Y < SizeY) {
		CrosshairCanvasPanelSlot->SetPosition(TargetScreenPosition / ViewportScale);
	}
}

void UCrosshairWidget::TargetUpdated(const FGameplayEventData* EventData)
{
	AimTarget = EventData->Target;
	CrosshairImage->SetColorAndOpacity(AimTarget ? HasTargetColor : NoTargetColor);
}
