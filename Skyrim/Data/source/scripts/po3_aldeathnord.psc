scriptName PO3_ALDeathNord extends activemagiceffect

import po3_SKSEFunctions
import po3_Events_AME

;-- Properties --------------------------------------
ReferenceAlias Property PlayerAlias Auto
GlobalVariable Property AL_SovngardeUniqueOnly Auto

;-- Variables ---------------------------------------
Actor victim

;-- Event -------------------------------------------
Event OnEffectStart(Actor akTarget, Actor akCaster)

	victim = akTarget
	
endEvent

Event OnDeath(Actor akKiller)

	if victim
		if IsSoulTrapped(victim)
			return
		endIf			
		
		bool onlyUniques = AL_SovngardeUniqueOnly.GetValue() as bool
		actorbase base = victim.GetActorBase()
		if !base || onlyUniques && !base.IsUnique()
			return 
		endif
		
		if (PlayerAlias as po3_alsoulmanager).AddSoul(base, 0)
			RegisterForActorResurrected(self)
		endif
		
	endif
	
endEvent

Event OnActorResurrected(Actor akTarget, bool abResetInventory)

	if akTarget == victim
	
		actorbase base = victim.GetActorBase()	
		(PlayerAlias as po3_alsoulmanager).RemoveSoul(base, 0)	
		
		UnregisterForActorResurrected(self)	
		
	endif

endEvent