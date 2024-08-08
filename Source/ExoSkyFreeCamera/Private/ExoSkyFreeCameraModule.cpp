#include "Editor.h"
#include "LevelEditor.h"
#include "LevelEditorViewport.h"
#include "Containers/Ticker.h"
#include "MouseDeltaTracker.h"

#include "CameraFunctions.h"
#include "ExoSkyFreeCameraModule.h"

#define LOCTEXT_NAMESPACE "ExoSkyFreeCameraModule"

void FExoSkyFreeCameraModule::StartupModule()
{
	FreeCameraTicker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FExoSkyFreeCameraModule::Tick), 0.0f);
}

void FExoSkyFreeCameraModule::ShutdownModule()
{
	FTSTicker::GetCoreTicker().RemoveTicker(FreeCameraTicker);
}

bool FExoSkyFreeCameraModule::Tick(float DeltaTime)
{
	if(GCurrentLevelEditingViewportClient)
	{
		// Just in case there is a teleport to an object
		GCurrentLevelEditingViewportClient->bLockFlightCamera = false;

		// Setup Camera Transforms
		CameraTransform = UExoSkyFreeCameraFunctions::GetEditorCameraTransform();
		FVector CameraLocation = CameraTransform.GetLocation();
		FQuat CameraRotation = CameraTransform.GetRotation();

		float CameraSpeed = GCurrentLevelEditingViewportClient->GetCameraSpeed() * GCurrentLevelEditingViewportClient->GetCameraController()->GetConfig().MovementAccelerationRate;

		// Setup Relative Direction Vectors
		FVector ForwardVector = CameraTransform.GetRotation().GetForwardVector();
		FVector UpVector = -CameraTransform.GetRotation().GetUpVector();
		FVector RightVector = CameraTransform.GetRotation().GetRightVector();

		FViewport* Viewport = GCurrentLevelEditingViewportClient->Viewport;
		
		FVector CurrentMousePosition = FVector(Viewport->GetMouseX(), Viewport->GetMouseY(), 0.0);
		FVector DragDelta = CurrentMousePosition - LastMousePosition;

		bool bTransformedThisFrame = false;

		// Handle 6DOF Camera Movement
		if(Viewport->KeyState(EKeys::MiddleMouseButton) || Viewport->KeyState(EKeys::LeftMouseButton))
		{
			GCurrentLevelEditingViewportClient->bLockFlightCamera = true;

			FVector MovementVector = FVector(0.0,0.0,0.0);

			MovementVector += RightVector * DragDelta.X;
			MovementVector += UpVector * DragDelta.Y;

			// Clamp Magnitude to 1
			MovementVector.Normalize();
			MovementVector *= CameraSpeed;

			CameraLocation += MovementVector * DeltaTime;
			bTransformedThisFrame = true;
		}

		// Handle 6DOF Camera Rotation
		if(Viewport->KeyState(EKeys::RightMouseButton))
		{
			GCurrentLevelEditingViewportClient->bLockFlightCamera = true;

			float RollAmount = 0.0;
			float PitchAmount = 0.0; 
			float YawAmount = 0.0;	

			// Add Roll Input on Q/E
			RollAmount += Viewport->KeyState(EKeys::Q) ? 1.0 : 0.0;
			RollAmount += Viewport->KeyState(EKeys::E) ? -1.0 : 0.0;

			// Add Pitch/Yaw on Mouse Delta
			PitchAmount += DragDelta.Y;
			YawAmount += DragDelta.X;

			// Construct Rotation Quaternions Per-Axis
			FQuat RollRotation(FVector(1.0, 0.0, 0.0),  RollAmount * RollSensitivity * DeltaTime);
			FQuat PitchRotation(FVector(0.0, 1.0, 0.0), PitchAmount * PitchSensitivity * DeltaTime);
			FQuat YawRotation(FVector(0.0, 0.0, 01.0),  YawAmount * YawSensitivity * DeltaTime);

			// Rotate the Camera Locally by Multiplying the Original Camera uaternion by the Per-Axis Camera Quaternions
			CameraRotation *= RollRotation;
			CameraRotation *= PitchRotation;
			CameraRotation *= YawRotation;

			float UpAmount = 0.0;

			UpAmount += Viewport->KeyState(EKeys::LeftShift) ? 1.0 : 0.0;
			UpAmount += Viewport->KeyState(EKeys::SpaceBar) ? -1.0 : 0.0;
			UpAmount *= CameraSpeed;

			CameraLocation += UpVector * UpAmount * DeltaTime;
			bTransformedThisFrame = true;
		}

		if(bTransformedThisFrame)
		{
			CameraTransform.SetRotation(CameraRotation);
			CameraTransform.SetLocation(CameraLocation);
			RefreshViewportCameraTransform();
		}
		LastMousePosition = CurrentMousePosition;
	}
	else
	{
		LastMousePosition = FVector(0.0,0.0,0.0);
	}
	return true;
}

void FExoSkyFreeCameraModule::RefreshViewportCameraTransform()
{
	UExoSkyFreeCameraFunctions::SetEditorCameraTransform(CameraTransform);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExoSkyFreeCameraModule, ExoSkyFreeCamera)