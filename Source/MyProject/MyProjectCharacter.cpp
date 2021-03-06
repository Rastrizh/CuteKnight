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
#include "Enemy.h"


DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter(const FObjectInitializer& ObjectInitializer)
	:
	Super(ObjectInitializer)
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
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 300.0f;
	GetCharacterMovement()->GroundFriction = 5.0f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxFlySpeed = 300.0f;

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
	
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation0;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation1;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation2;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation3;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation4;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation5;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation6;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation7;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation8;

		FConstructorStatics()
			:
			Animation0(TEXT("PaperFlipbook'/Game/CuteNight/Animations/Idle.Idle'")),
			Animation1(TEXT("PaperFlipbook'/Game/CuteNight/Animations/Spawn.Spawn'")),
			Animation2(TEXT("PaperFlipbook'/Game/CuteNight/Animations/Dead.Dead'")),
			Animation3(TEXT("PaperFlipbook'/Game/CuteNight/Animations/Walk.Walk'")),
			Animation4(TEXT("PaperFlipbook'/Game/CuteNight/Animations/Duck.Duck'")),
			Animation5(TEXT("PaperFlipbook'/Game/CuteNight/Animations/Jump.Jump'")),
			Animation6(TEXT("PaperFlipbook'/Game/CuteNight/Animations/Stab.Stab'")),
			Animation7(TEXT("PaperFlipbook'/Game/CuteNight/Animations/UpStab.UpStab'")),
			Animation8(TEXT("PaperFlipbook'/Game/CuteNight/Animations/Hurt.Hurt'"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	IdleAnimation  = ConstructorStatics.Animation0.Object;
	SpawnAnimation = ConstructorStatics.Animation1.Object;
	DeadAnimation  = ConstructorStatics.Animation2.Object;
	WalkAnimation  = ConstructorStatics.Animation3.Object;
	DuckAnimation  = ConstructorStatics.Animation4.Object;
	JumpAnimation  = ConstructorStatics.Animation5.Object;
	StabAnimation  = ConstructorStatics.Animation6.Object;
	UpStabAnimation= ConstructorStatics.Animation7.Object;
	HurtAnimation  = ConstructorStatics.Animation8.Object;

	GetSprite()->SetFlipbook(IdleAnimation);

	CharacterState = CreateDefaultSubobject<UStateComponent>(TEXT("CharacterState"));
	CharacterState->SetFlipbook(IdleAnimation);
	CharacterState->ChangeState(FString(TEXT("Idle")));

	OnActorBeginOverlap.AddDynamic(this, &AMyProjectCharacter::OnHit);

	AttackingBox = nullptr;
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

	PlayerInputComponent->BindAction<FStateDelegate>("Jump", IE_Pressed, CharacterState, &UStateComponent::ChangeState, FString(TEXT("Jump")));
	PlayerInputComponent->BindAction<FStateDelegate>("Attack", IE_Pressed, CharacterState, &UStateComponent::ChangeState, FString(TEXT("Stab")));
	PlayerInputComponent->BindAction<FStateDelegate>("Duck", IE_Pressed, CharacterState, &UStateComponent::ChangeState, FString(TEXT("Duck")));
	PlayerInputComponent->BindAction<FStateDelegate>("Duck", IE_Released, CharacterState, &UStateComponent::ChangeState, FString(TEXT("Idle")));

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyProjectCharacter::TouchStopped);

	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectCharacter::MoveRight);
}

void AMyProjectCharacter::OnHit(AActor* SelfActor, AActor* OtherActor)
{
	if (AEnemy* enemy = Cast<AEnemy>(OtherActor))
	{
		CharacterState->ChangeState(TEXT("Hurt"));
		UE_LOG(LogTemp, Warning, TEXT("You has been hited by the enemy!"));
	}
}

void AMyProjectCharacter::UpdateCharacter(float DeltaSeconds)
{
	// Update animation to match the motion
	Update(DeltaSeconds);
	GetSprite()->SetFlipbook(CharacterState->GetFlipbook());

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
		CharacterState->ChangeState(FString(TEXT("Walk")));
	}
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AMyProjectCharacter::Update(float delta_time)
{
	if (CharacterState->GetName() == "Idle")
	{
		CharacterState->SetFlipbook(IdleAnimation);
	}
	else if (CharacterState->GetName() == "Spawn")
	{
		CharacterState->SetFlipbook(SpawnAnimation);
	}
	else if (CharacterState->GetName() == "Dead")
	{
		CharacterState->SetFlipbook(DeadAnimation);
	}
	else if (CharacterState->GetName() == "Walk")
	{
		auto PlayerVelosity = GetVelocity();
		if (PlayerVelosity.X != 0.0f && GetCharacterMovement()->IsFalling())
		{
			CharacterState->ChangeState(FString(TEXT("Jump")));
			return;
		}
		else if (PlayerVelosity.X == 0.0f)
		{
			CharacterState->ChangeState(FString(TEXT("Idle")));
			return;
		}
		CharacterState->SetFlipbook(WalkAnimation);
	}
	else if (CharacterState->GetName() == "Duck")
	{
		CharacterState->SetElapsed(CharacterState->GetElapsed() + delta_time);
		if (CharacterState->IsAnimationEnds())
		{
			GetSprite()->SetLooping(false);
			CharacterState->SetElapsed(0);
			return;
		}
		CharacterState->SetFlipbook(DuckAnimation);
	}
	else if (CharacterState->GetName() == "Jump")
	{
		GetCharacterMovement()->AirControl = 20.0f;
		CharacterState->SetElapsed(CharacterState->GetElapsed() + delta_time);
		if (CharacterState->IsAnimationEnds())
		{
			CharacterState->ChangeState(FString(TEXT("Idle")));
			return;
		}
		CharacterState->SetFlipbook(JumpAnimation);
	}
	else if (CharacterState->GetName() == "Stab")
	{
		CharacterState->SetElapsed(CharacterState->GetElapsed() + delta_time);

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		DisableInput(PlayerController);
		if (GetCharacterMovement()->IsFalling())
		{
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), -1.0f);
			GetCharacterMovement()->AirControl = 0.0f;
		}
		SpawnHitBox();

		if (CharacterState->IsAnimationEnds())
		{
			EnableInput(PlayerController);
			StopAttack();
			CharacterState->ChangeState(FString(TEXT("Idle")));
			return;
		}
		CharacterState->SetFlipbook(StabAnimation);
	}
	else if (CharacterState->GetName() == "UpStab")
	{
		CharacterState->SetFlipbook(UpStabAnimation);
	}
	else if (CharacterState->GetName() == "Hurt")
	{
		CharacterState->SetElapsed(CharacterState->GetElapsed() + delta_time);
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		DisableInput(PlayerController);
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), -1.0f);
		GetCharacterMovement()->AirControl = 0.0f;
		if (CharacterState->IsAnimationEnds())
		{
			EnableInput(PlayerController);
			CharacterState->ChangeState(FString(TEXT("Idle")));
			return;
		}
		CharacterState->SetFlipbook(HurtAnimation);
	}
}

void AMyProjectCharacter::SpawnHitBox()
{
	if (AttackingBox)
		if (AttackingBox->IsOverlappingEnemy())
			return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	FTransform MeleeTransform;
	MeleeTransform.SetLocation(GetActorForwardVector() * 25 + GetActorLocation());
	MeleeTransform.SetRotation(GetActorRotation().Quaternion());
	MeleeTransform.SetScale3D(FVector(1.0f));

	GetWorld()->SpawnActor<AMelee>(AMelee::StaticClass(), MeleeTransform, SpawnParams);
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
