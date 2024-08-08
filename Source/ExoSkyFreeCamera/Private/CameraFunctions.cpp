#include "Editor.h"
#include "LevelEditorViewport.h"
#include "LevelEditor.h"
#include "CameraController.h"

#include "CameraFunctions.h"

bool UExoSkyFreeCameraFunctions::SetEditorCameraTransform(FTransform& NewTransform)
{
    if(GCurrentLevelEditingViewportClient)
    {
        FViewportCameraTransform& CameraTransform = GCurrentLevelEditingViewportClient->GetViewTransform();
        CameraTransform.SetLocation(NewTransform.GetLocation());
        CameraTransform.SetRotation(NewTransform.GetRotation().Rotator());

        UCameraComponent* CameraComponent = GCurrentLevelEditingViewportClient->GetCameraComponentForView();
        if(IsValid(CameraComponent))
        {
            AActor* CameraActor = CameraComponent->GetOwner();
            if(IsValid(CameraActor))
            {
                CameraActor->SetActorLocationAndRotation(NewTransform.GetLocation(), NewTransform.GetRotation(), false, nullptr, ETeleportType::TeleportPhysics);

                FEditorCameraController* EditorCamera = GCurrentLevelEditingViewportClient->GetCameraController();
                FCameraControllerConfig& CameraConfig = EditorCamera->AccessConfig();
                CameraConfig.bLockedPitch = false;
                return true;
            }
        }
    }
    return false;
}

FTransform UExoSkyFreeCameraFunctions::GetEditorCameraTransform()
{
    if(GCurrentLevelEditingViewportClient)
    {
        FViewportCameraTransform& CameraTransform = GCurrentLevelEditingViewportClient->GetViewTransform();
        FTransform NewTransform = FTransform();
        NewTransform.SetLocation(CameraTransform.GetLocation());
        NewTransform.SetRotation(FQuat(CameraTransform.GetRotation()));
        return NewTransform;
    }
    return FTransform();
}
