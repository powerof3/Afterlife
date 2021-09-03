#include "functions.h"
#include "serialization.h"

static void PatchDialogue()
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

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
		PatchDialogue();
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= fmt::format(FMT_STRING("{}.log"), Version::PROJECT);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] [%l] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "Afterlife";
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("loaded plugin");

	SKSE::Init(a_skse);

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(Afterlife::RegisterFuncs);

	const auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID(Serialization::kAfterlife);
	serialization->SetSaveCallback(Serialization::SaveCallback);
	serialization->SetLoadCallback(Serialization::LoadCallback);
	serialization->SetRevertCallback(Serialization::RevertCallback);

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(OnInit);

	return true;
}
