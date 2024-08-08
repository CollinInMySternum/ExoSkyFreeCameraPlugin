#pragma once

#include "UObject/Object.h"
#include "CoreMinimal.h"

#include "CameraFunctions.generated.h"

UCLASS()
class UExoSkyFreeCameraFunctions : public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, Category = "EXOSKY Free Editor Camera")
    static bool SetEditorCameraTransform(FTransform& NewTransform);

    UFUNCTION(BlueprintPure, Category = "EXOSKY Free Editor Camera")
    static FTransform GetEditorCameraTransform();
};