ScriptName PO3_ALSoulManager extends ReferenceAlias

import po3_sksefunctions

;-- Properties --------------------------------------
Actor Property PlayerRef Auto

Globalvariable[] Property limitGlobal Auto
Globalvariable[] Property countGlobal Auto
Globalvariable[] Property uniqueGlobal Auto

GlobalVariable Property AL_SovngardePastSoulsTracker Auto

Faction Property MQ304LostSoulFaction Auto
Faction Property MQSovngardeSparringFaction Auto

Location Property DLC1SoulCairnLocation Auto
Location Property SovngardeLocation Auto

Quest[] Property aliasesQuest Auto
Quest Property MQ305 Auto

Objectreference[] Property soulCairn_Markers Auto
Objectreference[] Property sovngarde_Markers Auto


;-- Variables ---------------------------------------
Bool soulCairn_Init = false
Bool sovngarde_Init = false

;-- Functions ---------------------------------------
Function ResetActor(actor akActor)

	akActor.SetGhost()
	
	akActor.StopCombatAlarm()
	
	PreventActorDetection(akActor)
	
	akActor.ResetInventory()
	
	akActor.EvaluatePackage()
	
endFunction

Function CreateSouls(Int aiType, objectreference[] xMarkers)

	actorbase[] bases = po3_afterlife.GetStoredSouls(aiType)
	Int idx = bases.length
	if idx > 0
		int max = 29
		if aiType == 0 && !MQ305.IsCompleted()
			max = 20
		endif
		while idx
			idx -= 1
			Actorbase base = bases[idx]
			if base
				Actor newActor = xMarkers[GenerateRandomInt(0, max)].PlaceAtMe(base, 1, false, false) as actor
				if newActor
					ReferenceAlias nthAlias = aliasesQuest[aiType].GetNthAlias(idx) as ReferenceAlias
					if nthAlias
						nthAlias.ForceRefTo(newActor)
						ResetActor(newActor)
						if aiType == 0
							newActor.AddToFaction(MQ304LostSoulFaction)
							if max == 29
								newActor.AddToFaction(MQSovngardeSparringFaction)
							endIf			
						endIf
					endIf
				endif
			endif
		endWhile
	endif
	
endFunction

Bool function AddSoul(actorbase akActorBase, Int aiType)

	int killLimit = limitGlobal[aiType].GetValue() as Int
	int killCount = countGlobal[aiType].GetValue() as Int
	
	if KillCount <= KillLimit && po3_afterlife.StoreSoul(akActorBase, aiType)
		countGlobal[aiType].Mod(1.0)
		return true
	endIf
	
	return false
	
endFunction

function RemoveSoul(actorbase akActorBase, Int aiType)

	if po3_afterlife.ClearSoul(akActorBase, aiType)
		countGlobal[aiType].Mod(-1.0)
	endIf
	
endFunction

function ClearSouls(int aiType)

	Int idx = po3_afterlife.GetStoredSouls(aiType).length
	while idx
		idx -= 1
		ReferenceAlias nthAlias = aliasesQuest[aiType].GetNthAlias(idx) as ReferenceAlias
		if nthAlias
			Actor newActor  = nthAlias.GetReference() as Actor
			if newActor
				newActor.Disable()
				newActor.Delete()
				nthAlias.Clear()
			endif
		endif
	endwhile
	
endFunction

;-- Events ---------------------------------------
Event OnInit()

	countGlobal[0].Mod(po3_afterlife.GetPastSouls(0, uniqueGlobal[0].GetValue() as bool))
	AL_SovngardePastSoulsTracker.SetValue(1.0)

endEvent

Event OnPlayerLoadGame()

	if AL_SovngardePastSoulsTracker.GetValue() == 0.0
		countGlobal[0].Mod(po3_afterlife.GetPastSouls(0, uniqueGlobal[0].GetValue() as bool))
		AL_SovngardePastSoulsTracker.SetValue(1.0)
	endif

endEvent

Event OnLocationChange(location akOldLoc, location akNewLoc)

	if akNewLoc
		if akNewLoc == SovngardeLocation && !sovngarde_Init
			sovngarde_Init = true
			CreateSouls(0, sovngarde_Markers)
			
		elseIf akNewLoc == DLC1SoulCairnLocation && !soulCairn_Init
			soulCairn_Init = true					
			CreateSouls(1, soulCairn_Markers)
			
		endIf
	endIf
	
	if akOldLoc
		if akOldLoc == SovngardeLocation && !akNewLoc.IsChild(SovngardeLocation) && sovngarde_Init
			sovngarde_Init = false		
			ClearSouls(0)
			
		elseIf akOldLoc == DLC1SoulCairnLocation && soulCairn_Init
			soulCairn_Init = false
			ClearSouls(1)
		
		endIf
	endIf
	
endEvent