// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../FunctionLibrary/Types.h"
#include "TDSCharacter.generated.h"

class UTDSStaminaComponent;
class UTDSInventoryComponent;
class ATDS_WeaponDefault;
class ATDSWeaponPickup;

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

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld();

	UFUNCTION(BlueprintCallable)
	ATDS_WeaponDefault* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetMovementDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsSprinting() const;

	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);

	UFUNCTION()
	void WeaponReloadStart(UAnimMontage* AnimMontage);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* AnimMontage);

	UFUNCTION()
	void WeaponReloadEnd(bool bIsSuccess, int32 AmmoToSubtract);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess);

	UFUNCTION()
	void WeaponFire(UAnimMontage* AnimMontage);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponFire_BP(UAnimMontage* AnimMontage);

	UFUNCTION()
	void StartSwitchWeapon(ATDSWeaponPickup* WeaponPickup);

	UFUNCTION()
	void EndSwitchWeapon();

protected:
	float AxisX = 0.0f;
	float AxisY = 0.0f;

	int32 CurrentIndexWeapon = 0;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraHeightMin = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraHeightMax = 1200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "10", ClampMax = "100"))
	float CameraStep = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSStaminaComponent* StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTDSInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UDecalComponent* CurrentCursor = nullptr;
	ATDS_WeaponDefault* CurrentWeapon = nullptr;

	virtual void BeginPlay() override;

private:
	void InputAxisX(float Value);
	void InputAxisY(float Value);

	void OnStartWalking();
	void OnStartAiming();
	void OnStartRunning();

	bool WantsToSprint = false;
	void OnStartSprinting();
	void OnStopSprinting();

	void InputAttackPressed();
	void InputAttackReleased();
	
	bool DoSlideUp = true;
	float CurrentCameraDistance = 0;

	FTimerHandle CameraTimerHandle;

	void CameraScroll();

	UFUNCTION()
	void CameraSlide(float Value);

	UFUNCTION()
	void MovementTick(float DeltaSeconds);
	
	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState(EMovementState InMovementState);

	UFUNCTION(BlueprintCallable)
	void InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon);

	void OnStaminaEmpty();
	void OnStaminaChanged(float Stamina);

	void TryReloadWeapon();

	void TrySwitchNextWeapon();
	void TrySwitchPreviousWeapon();
};