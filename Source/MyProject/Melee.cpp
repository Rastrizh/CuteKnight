// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"

void AMelee::OnMeleeHit(AActor* SelfActor, AActor* OtherActor)
{
	if (AEnemy *enemy = Cast<AEnemy>(OtherActor))
	{
		isOverlappingEnemy = true;
		Destroy(true);
		UE_LOG(LogTemp, Warning, TEXT("hit enemy!"));
	}
}

// Sets default values
AMelee::AMelee()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	box->SetBoxExtent({ 8.0f, 8.0f , 8.0f });
	box->bHiddenInGame = false;
	box->SetCollisionProfileName(TEXT("OverlapAll"));

	OnActorBeginOverlap.AddDynamic(this, &AMelee::OnMeleeHit);
	//OnActorHit.AddDynamic(this, &AMelee::OnMeleeHit);

	SetRootComponent(box);
}

// Called when the game starts or when spawned
void AMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_life_time += DeltaTime;

	if (m_life_time >= 0.1f)
		Destroy(true);
}

