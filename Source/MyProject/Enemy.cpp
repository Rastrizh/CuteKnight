// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "StateComponent.h"

AEnemy::AEnemy()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(14.0f);
	GetCapsuleComponent()->SetCapsuleRadius(11.0f);
	
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

	CharacterState = CreateDefaultSubobject<UStateComponent>(TEXT("CharacterState"));
}

void AEnemy::Update(float delta_time)
{
	if (CharacterState->GetName() == "Idle")
	{
		CharacterState->SetFlipbook(IdleAnimation);
	}
	else if (CharacterState->GetName() == "Dead")
	{
		CharacterState->SetFlipbook(DeadAnimation);
	}
	else if (CharacterState->GetName() == "Walk")
	{
		CharacterState->SetFlipbook(WalkAnimation);
	}
}

void AEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CharacterState->Update(this, DeltaSeconds);
	GetSprite()->SetFlipbook(CharacterState->GetFlipbook());
}
