// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	virtual void OnRep_ReplicatedMovement() override;
	void Elim();
	UFUNCTION(NetMulticast,Reliable)
	void MulticastElim();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor,float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();
	void PollInit();

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;

	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
		void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere,Category=Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	UPROPERTY(EditAnywhere, Category = "Player Stats")
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
		float Health = MaxHealth;

	UFUNCTION()
		void OnRep_Health();

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	UPROPERTY(VisibleAnywhere)
		UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissolveTrack;


	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
	
	UPROPERTY(VisibleAnywhere,Category=Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;

	void RotateInPlace(float DeltaTime);

public:

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const{
		return AO_Yaw;
	}
	FORCEINLINE float GetAO_Pitch() const {
		return AO_Pitch;
	}
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {
		return TurningInPlace;
	}
	FVector GetHitTarget()const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const {
		return FollowCamera;
	}
	FORCEINLINE bool ShouldRotateRootBone()const {
		return bRotateRootBone;
	}
	FORCEINLINE bool IsElimmed()const {
		return bElimmed;
	}
	FORCEINLINE float GetHealth()const {
		return Health;
	}
	FORCEINLINE float GetMaxHealth()const {
		return MaxHealth;
	}
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const {
		return Combat;
	}
	FORCEINLINE bool GetDisableGameplay() const {
		return bDisableGameplay;
	}
};