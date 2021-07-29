// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemyController::AEnemyController(FObjectInitializer const& object_initializer /*= FObjectInitializer::Get()*/)
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> obj(TEXT("BehaviorTree'/Game/AI/btree.btree'"));
	if (obj.Succeeded())
	{
		btree = obj.Object;
	}
	behavior_tree_component = object_initializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	Blackboard = object_initializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	if(RunBehaviorTree(btree))
		behavior_tree_component->StartTree(*btree);
}

void AEnemyController::OnPossess(APawn* const pawn)
{
	Super::OnPossess(pawn);
}

class UBlackboardComponent* AEnemyController::get_blackboard() const
{
	return Blackboard;
}
