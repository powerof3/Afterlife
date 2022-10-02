#include "Patches.h"

namespace Patches
{
	void PatchDialogue()
	{
		using Flags = RE::TOPIC_INFO_DATA::TOPIC_INFO_FLAGS;

		if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
			const auto afterlifeFaction = dataHandler->LookupForm<RE::TESFaction>(0x800, "Afterlife.esp");
			if (!afterlifeFaction) {
				logger::info("couldn't find afterlife faction");
				return;
			}

			//quests containing soul/generic dialogue, block everything else
			constexpr std::array<RE::FormID, 5> whitelistedQuest{ 0x00046EF1, 0x00046EF2, 0x02016072, 0x00013EB3, 0x0006F453 };

			//blacklist soul cairn dialogue that makes explicit reference to cause of death
			constexpr std::array<RE::FormID, 16> blacklistedTopics{
				0x02016049, 0x020183AE, 0x020183AF, 0x0201604A, 0x0201604B, 0x02016053, 0x02016057, 0x02016058, 0x02016059, 0x0201605A, 0x0201605B, 0x02016060, 0x02016061, 0x02016063, 0x02016051, 0x02016069
			};

			constexpr auto add_blocking_condition = [&](RE::TESTopicInfo* a_topicInfo) {
				auto newNode = new RE::TESConditionItem;

				newNode->next = nullptr;
				newNode->data.comparisonValue.f = 0.0f;
				newNode->data.functionData.function = RE::FUNCTION_DATA::FunctionID::kGetInFaction;
				newNode->data.functionData.params[0] = afterlifeFaction;

				if (a_topicInfo->objConditions.head == nullptr) {
					a_topicInfo->objConditions.head = newNode;
				} else {
					newNode->next = a_topicInfo->objConditions.head;
					a_topicInfo->objConditions.head = newNode;
				}
			};

			logger::info("Patching dialogues.");
			std::uint32_t topicCount = 0;

			for (const auto& topic : dataHandler->GetFormArray<RE::TESTopic>()) {
				if (topic && topic->topicInfos && topic->numTopicInfos > 0) {
					if (const auto quest = topic->ownerQuest; quest && std::ranges::find(whitelistedQuest, quest->GetFormID()) != whitelistedQuest.end()) {
						if (quest->GetFormID() == 0x02016072) {
							std::span<RE::TESTopicInfo*> span(topic->topicInfos, topic->numTopicInfos);
							for (const auto& topicInfo : span) {
								if (topicInfo && std::ranges::find(blacklistedTopics, topicInfo->GetFormID()) != blacklistedTopics.end()) {
									add_blocking_condition(topicInfo);
									topicCount++;
								}
							}
						}
					} else {
						std::span<RE::TESTopicInfo*> span(topic->topicInfos, topic->numTopicInfos);
						for (const auto& topicInfo : span) {
							if (topicInfo) {
								add_blocking_condition(topicInfo);
								topicCount++;
							}
						}
					}
				}
			}

			logger::info("Patched {} dialogues.", topicCount);
		}
	}

    void Install()
	{
		PatchDialogue();
	}
}
