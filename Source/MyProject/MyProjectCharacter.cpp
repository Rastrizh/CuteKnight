// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyProjectCharacter.h"
#include "Melee.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "StateComponent.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(14.0f);
	GetCapsuleComponent()->SetCapsuleRadius(11.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 750.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 150.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraLagSpeed = 5.0f;
	CameraBoom->CameraRotationLagSpeed = 5.0f;
	CameraBoom->CameraLagMaxDistance = 100.0f;

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 750.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->Mass = 30.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 5.0f;
	GetCharacterMovement()->AirControl = 20.0f;
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 300.0f;
	GetCharacterMovement()->GroundFriction = 5.0f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxFlySpeed = 400.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
	
	IdleAnimation  = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/Idle.Idle'"), NULL, LOAD_None, NULL);
	SpawnAnimation = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/Spawn.Spawn'"), NULL, LOAD_None, NULL);
	DeadAnimation  = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/Dead.Dead'"), NULL, LOAD_None, NULL);
	WalkAnimation  = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/Walk.Walk'"), NULL, LOAD_None, NULL);
	DuckAnimation  = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/Duck.Duck'"), NULL, LOAD_None, NULL);
	JumpAnimation  = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/Jump.Jump'"), NULL, LOAD_None, NULL);
	StabAnimation  = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/Stab.Stab'"), NULL, LOAD_None, NULL);
	UpStabAnimation= LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/UpStab.UpStab'"), NULL, LOAD_None, NULL);
	HurtAnimation  = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/CuteNight/Animations/Hurt.Hurt'"), NULL, LOAD_None, NULL);

	CharacterState = CreateDefaultSubobject<UStateComponent>(TEXT("CharacterState"));
	CharacterState->SetFlipbook(IdleAnimation);

	states.Add(static_cast<State*>(new IdleState(IdleAnimation)));
	states.Add(static_cast<State*>(new SpawnState(SpawnAnimation)));
	states.Add(static_cast<State*>(new DeadState(DeadAnimation)));
	states.Add(static_cast<State*>(new WalkState(WalkAnimation)));
	states.Add(static_cast<State*>(new DuckState(DuckAnimation)));
	states.Add(static_cast<State*>(new JumpState(JumpAnimation)));
	states.Add(static_cast<State*>(new StabState(StabAnimation)));
	states.Add(static_cast<State*>(new UpStabState(UpStabAnimation)));
	states.Add(static_cast<State*>(new HurtState(HurtAnimation)));

	current_state = states[AnimState::Idle];
	current_state->prev_state = current_state;
}

void AMyProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCharacter(DeltaSeconds);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AMyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMyProjectCharacter::Attack);

	PlayerInputComponent->BindAction<FChangeStateDelegate>("Jump", IE_Pressed, this, &AMyProjectCharacter::ChangeState, states[AnimState::Jump]);
	PlayerInputComponent->BindAction<FChangeStateDelegate>("Attack", IE_Pressed, this, &AMyProjectCharacter::ChangeState, states[AnimState::Stab]);
	PlayerInputComponent->BindAction<FChangeStateDelegate>("Duck", IE_Pressed, this, &AMyProjectCharacter::ChangeState, states[AnimState::Duck]);
	PlayerInputComponent->BindAction<FChangeStateDelegate>("Duck", IE_Released, this, &AMyProjectCharacter::ChangeState, states[AnimState::Idle]);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyProjectCharacter::TouchStopped);

	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectCharacter::MoveRight);
}

void AMyProjectCharacter::UpdateCharacter(float DeltaSeconds)
{
	// Update animation to match the motion

	current_state->Update(*this, DeltaSeconds);

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

void AMyProjectCharacter::MoveRight(float Value)
{
	if (Value != 0.0f && !GetCharacterMovement()->IsFalling() && !isAttacking)
	{
		ChangeState(states[AnimState::Walk]);
	}
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AMyProjectCharacter::ChangeState(State* state)
{
	state->prev_state = current_state;
	current_state = state;
	if (!GetSprite()->IsLooping())
	{
		GetSprite()->SetLooping(true);
		GetSprite()->Play();
	}
}

void AMyProjectCharacter::SpawnHitBox()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	FTransform MeleeTransform;
	MeleeTransform.SetLocation(GetActorForwardVector() * 25 + GetActorLocation());
	MeleeTransform.SetRotation(GetActorRotation().Quaternion());
	MeleeTransform.SetScale3D(FVector(1.0f));

	if (!AttackingBox)
		AttackingBox = GetWorld()->SpawnActor<AMelee>(AMelee::StaticClass(), MeleeTransform, SpawnParams);
}

void AMyProjectCharacter::Attack()
{
	isAttacking = true;
}

void AMyProjectCharacter::StopAttack()
{
	isAttacking = false;
	AttackingBox = nullptr;
}


void StabState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{
	elapsed += DeltaSeconds;

	APlayerController* PlayerController = character.GetWorld()->GetFirstPlayerController();
	character.DisableInput(PlayerController);

	if (GetElapsed() >= GetDuration() / 2)
		character.SpawnHitBox();

	if (GetElapsed() >= GetDuration())
	{
		character.EnableInput(PlayerController);
		character.StopAttack();
		auto PlayerVelosity = character.GetVelocity();
		if (PlayerVelosity.X == 0.0f)
		{
			character.ChangeState(character.states[AnimState::Idle]);
		}
		else
		{
			character.ChangeState(character.states[AnimState::Walk]);
		}
		elapsed = 0;
		return;
	}
	character.GetSprite()->SetFlipbook(m_animation);
}

void UpStabState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{

}

void HurtState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{

}

void JumpState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{
	elapsed += DeltaSeconds;
	if (GetElapsed() >= GetDuration() && !character.GetCharacterMovement()->IsFalling())
	{
		character.ChangeState(character.states[AnimState::Idle]);
		elapsed = 0;
		return;
	}
	character.GetSprite()->SetFlipbook(character.JumpAnimation);
}

void DuckState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{
	elapsed += DeltaSeconds;
	if (GetElapsed() >= GetDuration())
	{
		character.GetSprite()->SetLooping(false);
		elapsed = 0;
		return;
	}
	character.GetSprite()->SetFlipbook(character.DuckAnimation);
}

void WalkState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{
	auto PlayerVelosity = character.GetVelocity();
	if (PlayerVelosity.X != 0.0f && character.GetCharacterMovement()->IsFalling())
	{
		character.ChangeState(character.states[AnimState::Jump]);
		return;
	}
	else if (PlayerVelosity.X == 0.0f)
	{
		character.ChangeState(character.states[AnimState::Idle]);
		return;
	}
	character.GetSprite()->SetFlipbook(character.WalkAnimation);
}

void DeadState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{

}

void SpawnState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{
	character.GetSprite()->SetFlipbook(character.SpawnAnimation);
}

void IdleState::Update(AMyProjectCharacter& character, float DeltaSeconds)
{
	character.GetSprite()->SetFlipbook(character.IdleAnimation);
}

State::State(UPaperFlipbook* anim) : m_animation(anim)
{
}

float State::GetDuration() const
{
	return m_animation->GetTotalDuration() - m_animation->GetTotalDuration() / m_animation->GetFramesPerSecond();
}

bool State::IsAnimEnds()
{
	return true;
}

void AMyProjectCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	Jump();
}

void AMyProjectCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}
