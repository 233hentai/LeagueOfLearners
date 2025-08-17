// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemTreeWidget.h"
#include "Widgets/SplineWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UItemTreeWidget::DrawFromNode(const ITreeNodeInterface* NodeInterface)
{
	if (!NodeInterface) return;
	if (CurrentCenterItem == NodeInterface->GetItemObject()) return;
	ClearTree();
	CurrentCenterItem = NodeInterface->GetItemObject();
	float NextLeafPostionX = 0.f;
	UCanvasPanelSlot* CenterWidgetPanelSlot = nullptr;
	UUserWidget* CenterWidget = CreateWidgetForNode(NodeInterface, CenterWidgetPanelSlot);
	TArray<UCanvasPanelSlot*> LowerStreamSlots;
	TArray<UCanvasPanelSlot*> UpperStreamSlots;

	DrawStream(false, NodeInterface, CenterWidget, CenterWidgetPanelSlot, 0, NextLeafPostionX, LowerStreamSlots);
	float MaxLowerStreamX = NextLeafPostionX - NodeSize.X - NodeGap.X;
	float LowerMoveAmount = 0.f - MaxLowerStreamX / 2;
	for (UCanvasPanelSlot* StreamSlot : LowerStreamSlots) {
		StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{ LowerMoveAmount, 0.f });
	}
	NextLeafPostionX = 0.f;

	DrawStream(true, NodeInterface, CenterWidget, CenterWidgetPanelSlot, 0, NextLeafPostionX, UpperStreamSlots);
	float MaxUpperStreamX = NextLeafPostionX - NodeSize.X - NodeGap.X;
	float UpperMoveAmount = 0.f - MaxUpperStreamX / 2;
	for (UCanvasPanelSlot* StreamSlot : UpperStreamSlots) {
		StreamSlot->SetPosition(StreamSlot->GetPosition() + FVector2D{ UpperMoveAmount, 0.f });
	}

	CenterWidgetPanelSlot->SetPosition(FVector2D::Zero());
}

void UItemTreeWidget::ClearTree()
{
	RootPanel->ClearChildren();
}

UUserWidget* UItemTreeWidget::CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot*& OutSlot)
{
	if(!Node) return nullptr;
	UUserWidget* NodeWidget = Node->GetWidget();
	OutSlot = RootPanel->AddChildToCanvas(NodeWidget);
	if (OutSlot) {
		OutSlot->SetSize(NodeSize);
		OutSlot->SetAnchors(FAnchors(0.5f));
		OutSlot->SetAlignment(FVector2D(0.5f));
		OutSlot->SetZOrder(1);
	}
	return NodeWidget;
}

void UItemTreeWidget::CreateConnection(const UUserWidget* From, const UUserWidget* To)
{
	if (!From || !To) return;
	USplineWidget* Connection = CreateWidget<USplineWidget>(GetOwningPlayer());
	UCanvasPanelSlot* ConnectionPanelSlot = RootPanel->AddChildToCanvas(Connection);
	if (ConnectionPanelSlot) {
		ConnectionPanelSlot->SetAnchors(FAnchors{ 0.f });
		ConnectionPanelSlot->SetAlignment(FVector2D{ 0.f });
		ConnectionPanelSlot->SetPosition(FVector2D::Zero());
		ConnectionPanelSlot->SetZOrder(0);
	}
	Connection->SetupSpline(From, To, SourcePortLocalPos, DestinationPortLocalPos, SourcePortDirection, DestinationPortDirection);
	Connection->SetSplineStyle(ConnectionColor, ConnectionThickness);
}

void UItemTreeWidget::DrawStream(bool bUpperStream, const ITreeNodeInterface* StartingNodeInterface, UUserWidget* StartingNodeWidget, UCanvasPanelSlot* StartingNodeSlot, int StartingNodeDepth, float& NextLeafPositionX, TArray<UCanvasPanelSlot*>& OutStreamSlot)
{
	TArray<const ITreeNodeInterface*> NextTreeNodeInterfaces = bUpperStream ? StartingNodeInterface->GetInputs() : StartingNodeInterface->GetOutputs();
	float StartingNodePositionY = (NodeSize.Y + NodeGap.Y) * StartingNodeDepth * (bUpperStream ? -1 : 1);
	if (NextTreeNodeInterfaces.Num() == 0) {
		StartingNodeSlot->SetPosition(FVector2D{ NextLeafPositionX,StartingNodePositionY });
		NextLeafPositionX += NodeSize.X + NodeGap.X;
		return;
	}
	float NextNodePosXSum = 0;
	for (const ITreeNodeInterface* NextTreeNodeInterface : NextTreeNodeInterfaces) {
		UCanvasPanelSlot* NextWidgetSlot;
		UUserWidget* NextWidget = CreateWidgetForNode(NextTreeNodeInterface, NextWidgetSlot);
		OutStreamSlot.Add(NextWidgetSlot);
		if (bUpperStream) {
			CreateConnection(NextWidget, StartingNodeWidget);
		}
		else {
			CreateConnection(StartingNodeWidget, NextWidget);
		}
		DrawStream(bUpperStream, NextTreeNodeInterface, NextWidget, NextWidgetSlot, StartingNodeDepth + 1, NextLeafPositionX, OutStreamSlot);
		NextNodePosXSum += NextWidgetSlot->GetPosition().X;
	}
	float StartingNodePositionX = NextNodePosXSum / NextTreeNodeInterfaces.Num();
	StartingNodeSlot->SetPosition(FVector2D{ StartingNodePositionX,StartingNodePositionY });
}

