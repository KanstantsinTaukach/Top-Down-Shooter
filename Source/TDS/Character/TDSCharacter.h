// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../FunctionLibrary/Types.h"
#include "TDSCharacter.generated.h"

class UTDSStaminaComponent;
class UTextRenderComponent;

UCLASS(Blueprintable)
class ATDSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATDSCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

public:
	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetMovementDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSprinting() const;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;	

private:
	void InputAxisX(float Value);
	void InputAxisY(float Value);

	void OnStartWalking();
	void OnStartAiming();
	void OnStartRunning();

	bool WantsToSprint = false;

	void OnStartSprinting();
	void OnStopSprinting();
	
	UFUNCTION()
	void MovementTick(float DeltaTime);

	UFUNCTION()
	void CameraSlide(float Value);	

	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState(EMovementState InMovementState);	

	bool DoSlideUp = true;
	float CurrentCameraDistance = 0;

	FTimerHandle CameraTimerHandle;

	void CameraScroll();

	void OnStaminaEmpty();
	void OnStaminaChanged(float Stamina);

protected:
	float AxisX = 0.0f;
	float AxisY = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Run_State;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintAngleThreshold = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	bool DoSmoothScrolling = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "DoSmoothScrolling"))
	float TimerStep = 0.002f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (EditCondition = "DoSmoothScrolling"))
	float SmoothCameraStep = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraHeightMin = 700.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraHeightMax = 1200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "10", ClampMax = "100"))
	float CameraStep = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSStaminaComponent* StaminaComponent;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTextRenderComponent* StaminaTextComponent;

	virtual void BeginPlay() override;
};