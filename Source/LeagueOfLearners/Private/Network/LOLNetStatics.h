// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LOLNetStatics.generated.h"

/**
 * 
 */
UCLASS()
class ULOLNetStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static uint8 GetPlayerCountPerTeam();
};
