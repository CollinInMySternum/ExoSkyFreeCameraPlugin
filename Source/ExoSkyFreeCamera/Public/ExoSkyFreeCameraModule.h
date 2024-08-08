// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FExoSkyFreeCameraModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	bool Tick(float DeltaTime);
	void RefreshViewportCameraTransform();

	FTransform CameraTransform;

	FTSTicker::FDelegateHandle FreeCameraTicker;

	float RollSensitivity  = 1.0f;
	float PitchSensitivity = 0.5f;
	float YawSensitivity   = 0.5f;

	FVector LastMousePosition;
};