ScriptName PO3_ALMonitorSoul extends ReferenceAlias

import PO3_SKSEFunctions

;-- Variables ---------------------------------------
Actor soul

;-- Events ---------------------------------------
Event OnLoad()

	soul = self.GetReference() as Actor

	soul.SetGhost()			;prevent hits

	soul.StopCombatAlarm()	;stop combat	

	PreventActorDetection(soul) ;prevents other NPCs/followers from aggroing
	PreventActorDetecting(soul) ;prevents this actor from aggroing

	soul.ResetInventory()
	
	soul.EvaluatePackage()	;evaluate package
	
endEvent

Event OnCellDetach()

	soul = self.GetReference() as Actor
	
	soul.SetAV("Variable09",0)

endEvent


Event OnDetachedFromCell()

	soul = self.GetReference() as Actor
	
	soul.SetAV("Variable09",0)

endEvent