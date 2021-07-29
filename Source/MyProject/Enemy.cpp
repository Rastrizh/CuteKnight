// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "StateComponent.h"
#include "MyProjectCharacter.h"
#include "Melee.h"
#include "Perception/PawnSensingComponent.h"
#include "EnemyController.h"

AEnemy::AEnemy(const FObjectInitializer& ObjectInitalizer)
	:
	Super(ObjectInitalizer)
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	PlayerLocation = FVector(0.0f, 0.0f, 0.0f);

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(14.0f);
	GetCapsuleComponent()->SetCapsuleRadius(11.0f);
	GetCapsuleComponent()->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("OverlapAll"));
	
	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	Senses = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	Senses->HearingThreshold = 0.0f;
	Senses->LOSHearingThreshold = 0.0f;
	Senses->SightRadius = 300.0f;
	Senses->SetPeripheralVisionAngle(90.0f);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	GetSprite()->SetRelativeRotation(GetCapsuleComponent()->GetRelativeRotation());
	bReplicates = true;

	CharacterState = CreateDefaultSubobject<UStateComponent>(TEXT("CharacterState"));

	OnActorBeginOverlap.AddDynamic(this, &AEnemy::OnOverlap);
	Senses->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePawn);
}

void AEnemy::OnOverlap(AActor* SelfActor, AActor* OtherActor)
{
	if (AMyProjectCharacter *player = Cast<AMyProjectCharacter>(OtherActor))
	{
		CharacterState->ChangeState(TEXT("Dead"));
		UE_LOG(LogTemp, Warning, TEXT("Enemy has been hited by the player!"));
	}
	else if (AMelee *melee = Cast<AMelee>(OtherActor))
	{
		SetActorEnableCollision(false);
		CharacterState->ChangeState(TEXT("Dead"));
		UE_LOG(LogTemp, Warning, TEXT("Enemy has been hited by the players' attack!"));
	}
}

void AEnemy::OnSeePawn(APawn* pawn)
{
	if (auto player = Cast<AMyProjectCharacter>(pawn))
	{
		CharacterState->ChangeState(TEXT("Walk"));
		PlayerLocation = player->GetActorLocation();
		Cast<AAIController>(Controller)->MoveToActor(pawn);
	}
}

void AEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
