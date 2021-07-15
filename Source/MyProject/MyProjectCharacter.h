// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "MyProjectCharacter.generated.h"

class UTextRenderComponent;
class UPaperFlipbook;
class UStateComponent;

UENUM()
enum AnimState
{
	Idle,
	Spawn,
	Dead,
	Walk,
	Duck,
	Jump,
	Stab,
	UpStab,
	Hurt
};

class State
{
public:
	State() = default;
	State(UPaperFlipbook* anim);
	virtual ~State() {}

	virtual void Update(AMyProjectCharacter&, float DeltaSeconds) = 0;

	bool operator==(const State& rhs) const { return m_animation == rhs.GetAnimation(); }

	UPaperFlipbook* GetAnimation() const { return m_animation; }
	float GetDuration() const;
	float GetElapsed() const { return elapsed; }

	void SetElapsed(const float e) { elapsed = e; }
	void AddElapsed(const float e) { elapsed += e; }

	bool IsAnimEnds();

	State* prev_state;
protected:
	UPaperFlipbook* m_animation;
	float elapsed;
};

class IdleState : public State
{
public:
	IdleState() = default;
	IdleState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~IdleState() override {}

	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};
class SpawnState : public State
{
public:
	SpawnState() = default;
	SpawnState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~SpawnState() override {}
	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};
class DeadState : public State
{
public:
	DeadState() = default;
	DeadState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~DeadState() override {}
	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};
class WalkState : public State
{
public:
	WalkState() = default;
	WalkState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~WalkState() override {}
	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};
class DuckState : public State
{
public:
	DuckState() = default;
	DuckState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~DuckState() override {}
	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};
class JumpState : public State
{
public:
	JumpState() = default;
	JumpState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~JumpState() override {}
	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};
class StabState : public State
{
public:
	StabState() = default;
	StabState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~StabState() override {}
	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};
class UpStabState : public State
{
public:
	UpStabState() = default;
	UpStabState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~UpStabState() override {}
	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};
class HurtState : public State
{
public:
	HurtState() = default;
	HurtState(UPaperFlipbook* anim) : State(anim) {}
	virtual ~HurtState() override {}
	virtual void Update(AMyProjectCharacter& character, float DeltaSeconds) override;
};

/**
 * This class is the default character for MyProject, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config=Game)
class AMyProjectCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	virtual void Tick(float DeltaSeconds) override;

	bool isAttacking = false;

public:
	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* SpawnAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* DeadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* WalkAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* DuckAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* StabAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* UpStabAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* HurtAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	class AMelee* AttackingBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = States)
	class UStateComponent* CharacterState;
	
	TArray<FString> state_names = { "Idle", "Spawn", "Dead", "Walk", "Duck", "Jump", "Stab", "UpStab", "Heart" };

	void ChangeState(FString state_name);
	DECLARE_DELEGATE_OneParam(FStateDelegate, FString);

	void UpdateState(float DeltaSeconds);

	void SpawnHitBox();
	void Attack();
	void StopAttack();

	/** Called for side to side input */
	void MoveRight(float Value);

	void UpdateCharacter(float DeltaSeconds);

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	AMyProjectCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
