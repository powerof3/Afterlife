ScriptName PO3_Afterlife Hidden

;Get all souls
Actorbase[] Function GetStoredSouls(int aiType) global native

;Store soul
Bool Function StoreSoul(Actorbase akActorbase, int aiType) global native

;Clear soul
Bool Function ClearSoul(Actorbase akActorbase, int aiType) global native

;Clears all souls
Function ClearAllSouls(int aiType) global native

;Clears non-unique souls
Int Function ClearAllGenericSouls(int aiType) global native

;Gets past souls that have died before the mod was installed (only valid for Sovngarde)
Int Function GetPastSouls(int aiType, bool abOnlyUnique) global native