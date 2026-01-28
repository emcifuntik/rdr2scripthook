// Auto-generated TypeScript declarations for natives module
// Do not edit manually - regenerate with tools/codegen/generate_natives.py

declare module "natives" {
    /** Vector3 type returned by some natives */
    interface Vector3 {
        x: number;
        y: number;
        z: number;
    }

    /** Native string handle returned by varString and similar natives */
    interface NativeString {
        /** Raw pointer to game memory - use when passing to other natives */
        readonly __nativePtr__: number;
        /** String value for display */
        readonly value: string;
        toString(): string;
    }

    // AICOVERPOINT
    export function taskEnterCover(ped: number): void;
    export function taskExitCover(ped: number): void;
    /** args: f_0 = Volume Handle f_1 = integer (?) (only the number 1 is ever used here, or is not used at all) f_2 = integer (-1 to 32 in R* Scripts) */
    export function addCoverBlockingArea(args: any): void;
    export function addScriptedCoverPoint(data: any): number;
    export function areLoadCoverAnimsLoaded(ped: number): boolean;
    export function doesCoverPointExist(handle: number): boolean;
    /** 1 = In cover while crouched 2 = In cover while standing 3 = Not in cover */
    export function getCoverPointStateFromPed(ped: number): number;
    /** Makes ped flinch (if in cover) like they have been shot at */
    export function requestFlinchCoverAnim(ped: number): void;
    /** Stops running cover anims and releases them _STOP_RENDERING_* - _STOP_SCRIPTED* */
    export function stopRunningCoverAnims(ped: number): void;
    export function taskAiSeekCoverToCoverPoint(args: any): void;

    // AITRANSPORT
    /** flagId: see SET_TRANSPORT_CONFIG_FLAG */
    export function getTransportConfigFlag(transportEntity: number, flagId: number, p2: boolean): boolean;
    export function isPedEnteringTransport(ped: number, transportEntity: number, p2: boolean): boolean;
    export function isPedExitingTransport(ped: number, transportEntity: number): boolean;
    export function setPedOffTransportSeat(ped: number, flags: number): void;
    /** seat: see CREATE_PED_INSIDE_VEHICLE */
    export function setPedOnTransportSeat(ped: number, transportEntity: number, seat: number, flags: number): void;
    export function setTransportAccessibleSeatFlags(transportEntity: number, flags: number): void;
    /** flagId: enum eTransportConfigFlags { 	TCF_NotConsideredForEntryByLocalPlayer, 	TCF_0xB78D6624, 	TCF_0xA9700425, 	TCF_0x8D7E4641, 	TCF_0xF24BAA1F, 	TCF_0x63B77935, 	TCF_NotConsideredForEntryByAllPla... */
    export function setTransportConfigFlag(transportEntity: number, flagId: number, value: boolean): void;
    /** Request a ped to enter/join a transport seat. args is a script struct<9> (72 bytes); each field is 8-byte (alignas(8)).  struct TaskEnterTransportArgs { 	alignas(8) Any     p0;        // unused/res... */
    export function taskEnterTransport(args: any): void;
    /** Request a ped to exit a transport. args is a script struct<7> (56 bytes); each field is 8-byte (alignas(8)).  struct TaskExitTransportArgs { 	alignas(8) Any     p0;       // unused/reserved in obse... */
    export function taskExitTransport(args: any): void;
    export function clearAllSeatPreferenceSlots(ped: number): void;
    /** Resets the value set by _SET_TRANSPORT_EXIT_BLEND_RATIO to 0.0f */
    export function clearTransportExitBlendRatio(ped: number): void;
    /** seatIndex: see CREATE_PED_INSIDE_VEHICLE */
    export function getPedInTransportSeat(transportEntity: number, seatIndex: number): number;
    /** See _SET_TRANSPORT_USAGE_FLAGS */
    export function getTransportUsageFlags(transportEntity: number, flags: number): any;
    /** Checks if ped is placed on target transportEntity */
    export function isPedOnTransportEntity(ped: number, transportEntity: number): boolean;
    export function isPedOnTransportSeat(ped: number, p1: boolean): boolean;
    /** Called together with IS_VEHICLE_SEAT_FREE */
    export function isTransportSeatFree(transportEntity: number, seatIndex: number): boolean;
    export function isTransportSeatOccupied(transportEntity: number, seatIndex: number): boolean;
    export function setAiCanUseTransport(transportEntity: number, state: boolean): void;
    export function setPedUseTransportSeatPreference(ped: number, transportEntity: number, preferenceSlot: number, p3: number, seatIndex: number): void;
    export function setTransportExclusiveDriver(transportEntity: number, ped: number, seatIndex: number): void;
    /** Exit/dismount speed/blend multiplier for the transport user ped. >0 enables override, 0 = off; only works while ped is a transport user. R* Script usage: rcm_doctors_opinion1 - immediately after TA... */
    export function setTransportExitBlendRatio(ped: number, ratio: number): void;
    export function setTransportPrioritySeat(transportEntity: number, seatIndex: number): void;
    /** enum eTransportUsageFlags { 	TUF_INVALID = 0, 	TUF_ALLOW_DRIVER_ME = (1 << 0), 	TUF_ALLOW_DRIVER_GANG = (1 << 1), 	TUF_ALLOW_DRIVER_CREW = (1 << 2), 	TUF_ALLOW_DRIVER_FRIENDS = (1 << 3), 	TUF_ALLOW... */
    export function setTransportUsageFlags(transportEntity: number, flags: number): void;

    // ANIMSCENE
    export function abortAnimScene(animScene: number, p1: boolean): void;
    export function attachAnimSceneToEntity(animScene: number, entity: number, p2: number): void;
    export function attachAnimSceneToEntityPreservingLocation(animScene: number, entity: number, p2: number): void;
    export function blockAnimSceneFadingNextFrame(p0: boolean, p1: boolean): void;
    export function checkOwnershipOfAnimScene(animScene: number): boolean;
    export function couldAnimSceneEntityReachExitNextFrame(animScene: number, entityName: string, p2: any, p3: any): boolean;
    export function detachAnimScene(animScene: number): void;
    export function detachAnimScenePreservingLocation(animScene: number): void;
    export function doesAnimSceneExist(animScene: number): boolean;
    export function fadeAnimSceneAudioIn(animScene: number, p1: number): void;
    export function fadeAnimSceneAudioOut(animScene: number, p1: number): void;
    export function getAnimSceneBool(animScene: number, name: string): boolean;
    export function getAnimSceneCurrentActiveCameraCount(animScene: number): number;
    export function getAnimSceneEntityLocationData(animScene: number, entityName: string, matrix: Vector3, p3: boolean, playbackListName: string, p5: number): boolean;
    export function getAnimSceneFloat(animScene: number, name: string): number;
    export function getAnimSceneInt(animScene: number, name: string): number;
    export function getAnimSceneOrigin(animScene: number, position: Vector3, rotation: Vector3, order: number): void;
    export function getAnimScenePhase(animScene: number): number;
    export function hasAnimSceneExited(animScene: number, p1: boolean): boolean;
    export function hasEntityExitedAnimScene(animScene: number, entityName: string): boolean;
    export function isAnimSceneExitingThisFrame(animScene: number): boolean;
    export function isAnimSceneFinished(animScene: number, p1: boolean): boolean;
    export function isAnimSceneInSection(animScene: number, sectionName: string, p2: boolean): boolean;
    export function isAnimSceneLoaded(animScene: number, p1: boolean, p2: boolean): boolean;
    export function isAnimSceneMetadataLoaded(animScene: number, p1: boolean): boolean;
    export function isAnimSceneRunning(animScene: number, p1: boolean): boolean;
    export function isEntityExitingAnimSceneThisFrame(animScene: number, entityName: string): boolean;
    export function isEntityPlayingAnimScene(entity: number, animScene: number): boolean;
    export function loadAnimScene(animScene: number): void;
    export function removeAnimSceneEntity(animScene: number, entityName: string, entity: number): void;
    export function requestAnimScenePlayList(animScene: number, playlistName: string): boolean;
    export function resetAnimScene(animScene: number, playbackListName: string): void;
    export function resumeAnimSceneFromLastCheckpoint(animScene: number): void;
    export function setAnimSceneBool(animScene: number, name: string, value: boolean, p3: boolean): void;
    export function setAnimSceneEntity(animScene: number, entityName: string, entity: number, flags: number): void;
    export function setAnimSceneFloat(animScene: number, name: string, value: number, p3: boolean, p4: boolean): void;
    export function setAnimSceneInt(animScene: number, name: string, value: number, p3: boolean): void;
    export function setAnimSceneOrigin(animScene: number, posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, order: number): void;
    export function setAnimScenePaused(animScene: number, toggle: boolean): void;
    export function setAnimScenePlaybackList(animScene: number, playbackListName: string): void;
    export function setAnimScenePlayList(animScene: number, playlistName: string, p2: boolean): void;
    export function setAnimSceneRate(animScene: number, rate: number): void;
    export function startAnimScene(animScene: number): void;
    export function takeOwnershipOfAnimScene(animScene: number): void;
    export function triggerAnimSceneSkip(animScene: number): void;
    export function wasAnimSceneSkipped(animScene: number): boolean;
    export function clearAnimSceneWasSkipped(animScene: number): void;
    export function clearBreakoutArchetype(ped: number): void;
    /** flags: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eAnimSceneFlag */
    export function createAnimScene(animDict: string, flags: number, playbackListName: string, p3: boolean, p4: boolean): number;
    /** Returns mgmHandle */
    export function createMgmSystem(mgmFilename: string): number;
    export function deleteAnimScene(animScene: number): void;
    export function deleteMgmSystem(mgmHandle: number): void;
    export function doesAnimSceneOwnershipOfEntityExist(animScene: number, entityName: string): boolean;
    export function doesAnimScenePlayListExist(animScene: number, playbackListName: string): boolean;
    export function doesEntityWithIdExistInAnimScene(animScene: number, entityId: string): boolean;
    export function getAnimSceneDict(animScene: number): number;
    export function getAnimSceneDuration(animScene: number): number;
    export function getAnimSceneObject(animScene: number, name: string, isNetwork: boolean): number;
    export function getAnimScenePed(animScene: number, name: string, isNetwork: boolean): number;
    export function getAnimScenePlaybackListPhaseAudioLoadStress(animScene: number, phaseName: string): number;
    export function getAnimSceneRate(animScene: number): number;
    export function getAnimSceneTime(animScene: number): number;
    export function getAnimSceneVehicle(animScene: number, name: string, isNetwork: boolean): number;
    /** _HAS_L* (?) */
    export function hasEntityEnteredAnimScene(animScene: number, entityName: string): boolean;
    export function isAnimSceneAborted(animScene: number): boolean;
    export function isAnimSceneLoading(animScene: number, p1: boolean): boolean;
    export function isAnimSceneMetadataAssetInRangeLoading(animScene: number, p1: boolean): boolean;
    export function isAnimScenePaused(animScene: number): boolean;
    export function isAnimScenePlaybackListPhaseActive(animScene: number, phaseName: string): boolean;
    export function isAnimScenePlaybackListPhaseLoaded(animScene: number, phaseName: string): boolean;
    export function isAnimScenePlaybackListPhaseLoading(animScene: number, phaseName: string): boolean;
    export function isAnimSceneSkippable(animScene: number): boolean;
    /** MGM stands for MiniGameMoments. */
    export function isMgmSystemLoaded(mgmFilename: string): boolean;
    /** Used to request MiniGameMoments Assets.  mgmFilename's: Poker PokerArthur PokerArthurCamp PokerJohn PokerJohnCamp */
    export function loadMgmAssets(mgmFilename: string): boolean;
    /** Pauses all script threads except the one that called it. */
    export function pauseScriptThreads(toggle: boolean): void;
    export function releaseAnimScenePlayList(animScene: number, playlistName: string): boolean;
    export function requestPhotoModeDefreeze(): void;
    export function requestPhotoModeFreeze(): void;
    export function setBreakoutArchetype(ped: number, archetype: string): void;
    export function setMgmEvent(mgmEventHandle: number, p1: string, seatId: any, p3: number, p4: number): void;

    // ATTRIBUTE
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function addAttributePoints(ped: number, attributeIndex: number, p2: number): void;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function disableAttributeOverpower(ped: number, attributeIndex: number): void;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK  Old name: _SET_ATTRIBUTE_OVERPOWER_VALUE */
    export function enableAttributeOverpower(ped: number, attributeIndex: number, value: number, makeSound: boolean): void;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getAttributeBaseRank(ped: number, attributeIndex: number): number;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getAttributeBonusRank(ped: number, coreIndex: number): number;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getAttributePoints(ped: number, attributeIndex: number): number;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getAttributeRank(ped: number, attributeIndex: number): number;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getDefaultAttributePointsNeededForRank(modelHash: number, attributeIndex: number, rank: number): number;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getDefaultAttributeRank(modelHash: number, attributeIndex: number): number;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getDefaultMaxAttributeRank(modelHash: number, attributeIndex: number): number;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getMaxAttributePoints(ped: number, attributeIndex: number): number;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function getMaxAttributeRank(ped: number, attributeIndex: number): number;
    /** attributeIndex: enum ePedAttribute { 	PA_HEALTH, 	PA_STAMINA, 	PA_SPECIALABILITY, 	PA_COURAGE, 	PA_AGILITY, 	PA_SPEED, 	PA_ACCELERATION, 	PA_BONDING, 	SA_HUNGER, 	SA_FATIGUED, 	SA_INEBRIATED, 	SA_P... */
    export function setAttributeBaseRank(ped: number, attributeIndex: number, newValue: number): void;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function setAttributeBonusRank(ped: number, attributeIndex: number, newValue: number): void;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function setAttributePoints(ped: number, attributeIndex: number, p2: number): void;
    export function stopItemPreview(): void;
    /** coreIndex: see _SET_ATTRIBUTE_CORE_VALUE  Previously incorrectly named ENABLE_ATTRIBUTE_OVERPOWER */
    export function enableAttributeCoreOverpower(ped: number, coreIndex: number, value: number, makeSound: boolean): void;
    export function getAttributeCoreOverpowerSecondsLeft(ped: number, coreIndex: number): number;
    /** Gets the ped's core value on a scale of 0 to 100. coreIndex: see _SET_ATTRIBUTE_CORE_VALUE */
    export function getAttributeCoreValue(ped: number, coreIndex: number): number;
    export function getAttributeOverpowerSecondsLeft(ped: number, attributeIndex: number): number;
    export function isAttributeCoreOverpowered(ped: number, coreIndex: number): boolean;
    /** attributeIndex: see SET_ATTRIBUTE_BASE_RANK */
    export function isAttributeOverpowered(ped: number, attributeIndex: number): boolean;
    /** coreIndex: enum eAttributeCore { 	ATTRIBUTE_CORE_HEALTH, 	ATTRIBUTE_CORE_STAMINA, 	ATTRIBUTE_CORE_DEADEYE }; */
    export function setAttributeCoreValue(ped: number, coreIndex: number, value: number): void;
    /** Displays status effects on core icons (includes warnings).  enum eUiRpgStatusEffect { 	STATUS_NONE, 	STATUS_COLD, 	STATUS_HOT, 	STATUS_OVERFED, 	STATUS_DIRTY, 	STATUS_SNAKE_VENOM, 	STATUS_ARROW_WOU... */
    export function setStatusEffectCoreIcon(statusEffectType: number): void;
    /** Starts core periodic icon. statusEffectType: see 0xA4D3A1C008F250DF */
    export function setStatusEffectPeriodicIcon(statusEffectType: number): void;
    /** Params: p1 is related to satchel_category */
    export function startItemPreview(p0: any, p1: number): void;
    /** Stops periodic icon. statusEffectType: see 0xA4D3A1C008F250DF */
    export function stopStatusEffectPeriodicIcon(statusEffectType: number): void;

    // AUDIO
    export function addEntityToAudioMixGroup(entity: number, groupName: string, p2: number): void;
    export function addPedToConversation(convoRoot: string, ped: number, characterName: string): void;
    /** Old name: AUDIO_IS_SCRIPTED_MUSIC_PLAYING */
    export function audioIsMusicPlaying(): boolean;
    export function audioTriggerExplosion(name: string, x: number, y: number, z: number): void;
    export function cancelMusicEvent(eventName: string): boolean;
    export function clearAmbientZoneListState(ambientZone: string, p1: boolean): void;
    export function clearAmbientZoneState(zoneName: string, p1: boolean): void;
    export function clearConversationHistory(): void;
    export function createNewScriptedConversation(convoRoot: string): boolean;
    export function disablePedPainAudio(ped: number, toggle: boolean): void;
    /** Checks if the ped can play the speech or has the speech file, last parameter is usually false. */
    export function doesContextExistForThisPed(ped: number, speechName: string, unk: boolean): boolean;
    export function forcePedPanicWalla(): void;
    /** Old name: _FORCE_VEHICLE_ENGINE_AUDIO */
    export function forceUseAudioGameObject(vehicle: number, audioName: string): void;
    export function getCurrentScriptedConversationLine(p0: string): number;
    export function getMusicPlaytime(): number;
    export function getSoundId(): number;
    export function isAmbientSpeechDisabled(ped: number): boolean;
    export function isAmbientSpeechPlaying(ped: number): boolean;
    export function isAnimalVocalizationPlaying(pedHandle: number): boolean;
    export function isAnySpeechPlaying(ped: number): boolean;
    export function isAudioSceneActive(scene: string): boolean;
    /** Checks whether the horn of a vehicle is currently played. */
    export function isHornActive(vehicle: number): boolean;
    export function isPedInCurrentConversation(p0: string, ped: number, p2: any): boolean;
    export function isScriptedConversationLoaded(convoRoot: string): boolean;
    export function isScriptedConversationPlaying(p0: string): boolean;
    export function isScriptedSpeechPlaying(p0: any): boolean;
    export function isStreamPlaying(streamId: number): boolean;
    export function loadStream(streamName: string, soundSet: string): boolean;
    export function pauseScriptedConversation(p0: string, p1: boolean, p2: boolean, p3: boolean, p4: boolean): void;
    /** Play a speech from a position. params struct for ScriptedSpeechParams see: PLAY_PED_AMBIENT_SPEECH_NATIVE */
    export function playAmbientSpeechFromPositionNative(x: number, y: number, z: number, params: any): boolean;
    export function playAnimalVocalization(ped: number, vocalizationName: string, p2: boolean): void;
    export function playEndCreditsMusic(play: boolean): void;
    /** Valid pain IDs: 0..12 */
    export function playPain(ped: number, painId: number, p2: number, p3: boolean, isNetwork: boolean): void;
    /** struct ScriptedSpeechParams { 	const char* speechName; 	const char* voiceName; 	alignas(8) int variation; 	alignas(8) Hash speechParamHash; 	alignas(8) Ped listenerPed; 	alignas(8) BOOL syncOverNet... */
    export function playPedAmbientSpeechNative(speaker: number, params: any): boolean;
    export function playSound(audioName: string, audioRef: string, p2: boolean, p3: any, p4: boolean, p5: any): void;
    export function playSoundFromEntity(audioName: string, entity: number, audioRef: string, isNetwork: boolean, p4: any, p5: any): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/audio/frontend_soundsets */
    export function playSoundFrontend(audioName: string, audioRef: string, p2: boolean, p3: any): void;
    export function playStreamFromPed(ped: number, streamId: number): void;
    export function playStreamFromPosition(x: number, y: number, z: number, streamId: number): void;
    export function playStreamFrontend(streamId: number): void;
    export function preloadScriptConversation(convoRoot: string, p1: boolean, p2: boolean, clone: boolean): void;
    export function prepareMusicEvent(eventName: string): boolean;
    export function prepareSound(soundName: string, soundsetName: string, soundId: number): boolean;
    /** https://github.com/femga/rdr3_discoveries/tree/master/audio/soundsets */
    export function prepareSoundset(soundsetName: string, p1: boolean): boolean;
    export function prepareSoundWithEntity(soundName: string, entity: number, soundsetName: string, soundId: number): boolean;
    /** nullsub, doesn't do anything */
    export function registerScriptWithAudio(p0: boolean): void;
    export function releaseNamedScriptAudioBank(audioBank: string): void;
    export function releaseScriptAudioBank(): void;
    export function releaseSoundId(soundId: number): void;
    export function removeEntityFromAudioMixGroup(entity: number, p1: number): void;
    export function removePortalSettingsOverride(p0: string): void;
    export function requestScriptAudioBank(audioBank: string): boolean;
    export function restartScriptedConversation(p0: string): void;
    export function setAmbientVoiceName(ped: number, name: string): void;
    export function setAmbientZoneListState(ambientZone: string, p1: boolean, p2: boolean): void;
    export function setAmbientZoneListStatePersistent(ambientZone: string, p1: boolean, p2: boolean): void;
    export function setAmbientZoneState(zoneName: string, isEnabled: boolean, p2: boolean): void;
    export function setAmbientZoneStatePersistent(ambientZone: string, p1: boolean, p2: boolean): void;
    /** Not implemented. */
    export function setAnimalMood(animal: number, mood: number): void;
    /** Audio flags can be found here: https://pastebin.com/40qPV6EJ https://github.com/femga/rdr3_discoveries/tree/master/audio/audio_flags */
    export function setAudioFlag(flagName: string, toggle: boolean): void;
    export function setAudioOnlineTransitionStage(p0: string): void;
    export function setAudioSceneVariable(scene: string, variable: string, value: number): void;
    export function setAudioVehiclePriority(vehicle: number, p1: any): void;
    /** nullsub, doesn't do anything */
    export function setGpsActive(active: boolean): void;
    export function setHornEnabled(vehicle: number, toggle: boolean): void;
    export function setIsScriptedSpeechDisabled(ped: number, disabled: boolean): any;
    export function setPedInteriorWallaDensity(p0: number, p1: number): void;
    export function setPedIsDrunk(ped: number, toggle: boolean): void;
    /** https://en.m.wikipedia.org/wiki/Walla */
    export function setPedWallaDensity(p0: number, p1: number): void;
    export function setPortalSettingsOverride(p0: string, p1: string): void;
    export function setStaticEmitterEnabled(emitterName: string, toggle: boolean): void;
    export function skipToNextScriptedConversationLine(p0: string): void;
    export function startAudioScene(scene: string): boolean;
    export function startPreloadedConversation(convoRoot: string): void;
    export function startScriptConversation(convoRoot: string, p1: boolean, p2: boolean, clone: boolean): void;
    export function stopAudioScene(scene: string): void;
    export function stopAudioScenes(): void;
    export function stopCurrentPlayingAmbientSpeech(ped: number, p1: any): void;
    export function stopCurrentPlayingSpeech(ped: number, p1: any): void;
    export function stopPedSpeaking(ped: number, shaking: boolean): void;
    export function stopScriptedConversation(p0: string, p1: boolean, p2: boolean): number;
    export function stopStream(streamId: number): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/audio/music_events/music_events.lua */
    export function triggerMusicEvent(eventName: string): boolean;
    /** nullsub, doesn't do anything */
    export function unregisterScriptWithAudio(): void;
    export function useFootstepScriptSweeteners(ped: number, p1: boolean, hash: number): void;
    export function blockSpeechContext(context: string, block: boolean): void;
    export function clearConversationHistoryForScriptedConversation(convoRoot: string): void;
    /** Create a scripted speech to control speech. If handle is less than 0, it's invalid. params struct for ScriptedSpeechParams see: PLAY_PED_AMBIENT_SPEECH_NATIVE Returns scriptedSpeech handle. */
    export function createNewScriptedPedAmbientSpeech(speaker: number, params: any): number;
    export function getEntityAudioMixGroup(entity: number): number;
    /** Gets the hash for the last played speech line. */
    export function getLastPlayedSpeechForPed(ped: number): number;
    /** Creates stream and returns streamId handle to be used with PLAY_STREAM_* natives https://github.com/femga/rdr3_discoveries/tree/master/audio/create_stream */
    export function getLoadedStreamIdFromCreation(streamName: string, soundSet: string): number;
    export function getPedSongIndexHost(ped: number): any;
    export function hasSoundAudioNameFinished(audioName: string, soundsetName: string): boolean;
    export function hasSoundIdFinished(soundId: number): boolean;
    export function isAnyConversationPlaying(p0: boolean): boolean;
    export function isPedInAnyConversation(ped: number, p1: boolean): boolean;
    /** item: FUSE, value returned from 0x2E1CDC1FF3B8473E soundSet: HUD_SHOP_SOUNDSET, COMPANIONS_ROBBERIES_SOUNDSET */
    export function isScriptedAudioCustom(item: number, soundSet: number): boolean;
    export function isScriptedConversationCreated(convoRoot: string): boolean;
    export function isScriptedConversionOngoing(p0: string): boolean;
    export function playAnimalVocalizationPheromoneVialResponse(ped: number, p1: number, p2: boolean): void;
    /** Params: p5 seems to be always 0 */
    export function playSoundFromEntityWithSet(soundId: number, soundName: string, entity: number, soundsetName: string, p4: boolean, p5: any): void;
    /** item: value returned from 0x2E1CDC1FF3B8473E soundSet: HUD_SHOP_SOUNDSET, COMPANIONS_ROBBERIES_SOUNDSET */
    export function playSoundFromItem(item: number, soundSet: number, p2: any): void;
    export function playSoundFromPosition(audioName: string, x: number, y: number, z: number, audioRef: string, isNetwork: boolean, p6: any, p7: boolean, p8: any): void;
    /** Starts Audio Loop _PLAY_SOUND_FROM_ENTITY* - _PLAY_SOUND_FRONTEND* */
    export function playSoundFromPositionWithId(soundId: number, soundName: string, x: number, y: number, z: number, soundsetName: string, p6: boolean, p7: number, p8: boolean): void;
    /** Play/advance a scripted speech created via _CREATE_NEW_SCRIPTED_PED_AMBIENT_SPEECH and return a status code. Return values:   0 = not ready/invalid/failed   1 = started/playing   2 = finished/consu... */
    export function playSoundFromScriptedPedAmbientSpeech(scriptedSpeech: number): number;
    export function playSoundFrontendWithSoundId(soundId: number, name: string, soundSet: string, p3: boolean): void;
    export function releaseShardSounds(soundName: string, soundsetName: string): void;
    export function releaseSoundset(soundsetName: string): void;
    export function setAmbientZonePosition(ambientZone: string, x: number, y: number, z: number, heading: number): void;
    export function setAudioSceneset(audioName: string, sceneset: string): boolean;
    /** p1: Entity.Relationship p2: Player, Enemy, Teammate, Neutral */
    export function setSoundRelationshipOnPed(ped: number, p1: string, p2: string): void;
    export function setVariableOnSoundWithId(soundId: number, variableName: string, variableValue: number): void;
    export function setVariableOnSoundWithName(variableName: string, variableValue: number, audioName: string, audioRef: string): void;
    /** Hashes: VOFX_PLAYER_MALE01, VOFX_PLAYER_MALE02, VOFX_PLAYER_MALE03, VOFX_PLAYER_FEMALE01, VOFX_PLAYER_FEMALE02, VOFX_PLAYER_FEMALE03 */
    export function setVofxPedVoice(ped: number, voice: number): void;
    /** whistleConfig: Ped.WhistlePitch (0.0 - 1.0), Ped.WhistleClarity (0.0 - 1.0), Ped.WhistleShape (0.0 - 10.0) */
    export function setWhistleConfigForPed(ped: number, whistleConfig: string, value: number): void;
    export function startAudioSceneset(audioName: string, sceneset: string): boolean;
    export function stopAllScriptedAudioSounds(): void;
    export function stopAllScriptedConversions(p0: boolean, p1: boolean, p2: boolean): void;
    export function stopAudioSceneset(sceneset: string): void;
    export function stopSoundWithId(soundId: number): void;
    export function stopSoundWithName(audioName: string, audioRef: string): void;
    export function triggerMusicEventWithHash(eventName: number): any;
    /** _UNLOAD_[A-C]* - USE_* */
    export function unloadSpeechContext(speechContext: string): void;
    /** Only used in R* SP Scripts */
    export function updateSoundPosition(soundId: number, x: number, y: number, z: number): void;

    // BOUNTY
    export function bountyGetBountyOnPlayer(gamerHandle: any, bountyData: any): boolean;
    export function bountyGetCooldownCollection(p0: any): boolean;
    export function bountyGetLegendaryTarget(p0: any, p1: any): boolean;
    export function bountyGetWantedPosterSlot(p0: number, p1: number, p2: any): boolean;
    export function bountyRequestBeginLegendaryMission(outRpcGuid: any, p1: number, p2: number): boolean;
    export function bountyRequestBeginLegendaryMissionForPosse(outRpcGuid: any, p1: number, p2: number): boolean;
    export function bountyCancelLegendaryMission(): void;
    export function bountyCancelWantedPoster(): void;
    export function bountyClearBeingBountyHunter(): void;
    export function bountyClearBeingTarget(): void;
    export function bountyIsRequestPending(rpcGuid: any): boolean;
    export function bountyRequestBecomeTargetOfCharacterBountyHunt(outRpcGuid: any): boolean;
    export function bountyRequestBeginWantedPoster(outRpcGuid: any, p1: number): boolean;
    export function bountyRequestBribeJailGuard(outRpcGuid: any, p1: number): boolean;
    export function bountyRequestClaimCharacterBounty(outRpcGuid: any, p1: number, p2: any): boolean;
    export function bountyRequestCompleteLegendaryMission(outRpcGuid: any, p1: any): boolean;
    export function bountyRequestCompleteSplitWantedPoster(outRpcGuid: any, p1: any): boolean;
    export function bountyRequestCompleteWantedPoster(outRpcGuid: any, p1: any): boolean;
    export function bountyRequestEscapedCharacterBountyHunt(outRpcGuid: any): boolean;
    export function bountyRequestPayOffBounty(outRpcGuid: any): boolean;
    export function bountyRequestPayOffBountyEx(outRpcGuid: any, p1: number, costType: number): boolean;
    export function bountyRequestPosseLeaderClaimCharacterBounty(outRpcGuid: any, p1: number, p2: any): boolean;
    export function bountyRequestPosseLeaderEscapedCharacterBountyHunt(outRpcGuid: any): boolean;
    export function bountyRequestPosseMemberClaimCharacterBountyShare(outRpcGuid: any, p1: any): boolean;
    export function bountyRequestPosseMemberEscapedCharacterBountyHunt(outRpcGuid: any): boolean;
    /** crimeType: see _REPORT_CRIME */
    export function bountyRequestSelfReportCrime(outRpcGuid: any, crimeType: number, p2: boolean): boolean;
    export function bountyRequestSelfReportKilledByBountyHunter(outRpcGuid: any): boolean;
    export function bountyRequestServedFullJailSentence(outRpcGuid: any): boolean;

    // BRAIN
    export function disableScriptBrainSet(brainSet: number): void;
    export function enableScriptBrainSet(brainSet: number): void;
    /** Called before starting a new thread_monitor script thread in startup_mp/startup_tlg Alternative name _REGISTER_SCRIPT_BRAIN  Old name: _PREPARE_SCRIPT_BRAIN */
    export function reactivateAllObjectBrainsThatAreWaitingTillOutOfRange(): void;
    export function reactivateNamedObjectBrainsWaitingTillOutOfRange(scriptName: string): void;
    /** Registers a script for any object with a specific model hash. */
    export function registerObjectScriptBrain(scriptName: string, modelHash: number, p2: number, activationRange: number, p4: number, p5: number): void;
    export function getScriptBrainEntity(): number;
    export function removeScriptBrainEntity(entity: number): void;
    /** Returns threadId */
    export function startPreloadedScriptBrain(entity: number, scriptName: string, scriptStackSize: number, p3: boolean): number;
    /** Returns threadId */
    export function startScriptBrain(entity: number, scriptName: string, p2: number, p3: any, p4: number, p5: boolean): number;

    // BUILTIN
    /** Rounds a float value up to the next whole number */
    export function ceil(value: number): number;
    export function cos(value: number): number;
    /** Rounds a float value down to the next whole number */
    export function floor(value: number): number;
    /** Old name: _LOG10 */
    export function log10(value: number): number;
    export function pow(base: number, exponent: number): number;
    export function round(value: number): number;
    export function settimera(value: number): void;
    export function settimerb(value: number): void;
    /** THREAD_PRIO_HIGHEST = 0 THREAD_PRIO_NORMAL = 1 THREAD_PRIO_LOWEST = 2 THREAD_PRIO_MANUAL_UPDATE = 100 */
    export function setThisThreadPriority(priority: number): void;
    export function shiftLeft(value: number, bitShift: number): number;
    export function shiftRight(value: number, bitShift: number): number;
    export function sin(value: number): number;
    export function sqrt(value: number): number;
    /** Counts up. Every 1000 is 1 real-time second. Use SETTIMERA(int value) to set the timer (e.g.: SETTIMERA(0)). */
    export function timera(): number;
    export function timerb(): number;
    /** Gets the current frame time. */
    export function timestep(): number;
    export function toFloat(value: number): number;
    /** Calculates distance between vectors. The value returned will be in meters. */
    export function vdist(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number): number;
    /** Calculates distance between vectors but does not perform Sqrt operations. (Its way faster) The value returned will be in RAGE units. */
    export function vdist2(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number): number;
    /** Calculates the magnitude of a vector. */
    export function vmag(x: number, y: number, z: number): number;
    /** Calculates the magnitude of a vector but does not perform Sqrt operations. (Its way faster) */
    export function vmag2(x: number, y: number, z: number): number;
    export function wait(ms: number): void;

    // CAM
    /** p7 (length) determines the length of the spline, affects camera path and duration of transition between previous node and this one  p8 big values ~100 will slow down the camera movement before reac... */
    export function addCamSplineNode(camera: number, x: number, y: number, z: number, xRot: number, yRot: number, zRot: number, length: number, p8: number, p9: number): void;
    export function allowMotionBlurDecay(cam: number, p1: boolean): void;
    /** Last param determines if its relative to the Entity */
    export function attachCamToEntity(cam: number, entity: number, xOffset: number, yOffset: number, zOffset: number, isRelative: boolean): void;
    /** boneIndex: https://github.com/femga/rdr3_discoveries/tree/master/boneNames */
    export function attachCamToPedBone(cam: number, ped: number, boneIndex: number, x: number, y: number, z: number, heading: boolean): void;
    /** Only used in R* Script fm_mission_controller */
    export function cinematicLocationOverrideTargetEntityThisUpdate(name: string, entity: number): void;
    export function cinematicLocationStopScriptedShotEvent(p0: any, p1: any, p2: any): void;
    export function cinematicLocationTriggerScriptedShotEvent(dictionary: string, shotName: string, cameraName: string, p3: any): void;
    export function createCam(camName: string, p1: boolean): number;
    export function createCamera(camHash: number, p1: boolean): number;
    export function createCameraWithParams(camHash: number, posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, fov: number, p8: boolean, p9: any): number;
    export function createCamWithParams(camName: string, posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, fov: number, p8: boolean, p9: number): number;
    /** BOOL param indicates whether the cam should be destroyed if it belongs to the calling script. */
    export function destroyAllCams(p0: boolean): void;
    /** BOOL param indicates whether the cam should be destroyed if it belongs to the calling script. */
    export function destroyCam(cam: number, p1: boolean): void;
    export function detachCam(cam: number): void;
    export function disableCamCollisionForObject(entity: number): void;
    /** Old name: _DISABLE_VEHICLE_FIRST_PERSON_CAM_THIS_FRAME */
    export function disableCinematicBonnetCameraThisUpdate(): void;
    /** nullsub, doesn't do anything */
    export function disableFirstPersonFlashEffectThisUpdate(): void;
    /** Old name: _DISABLE_FIRST_PERSON_CAM_THIS_FRAME */
    export function disableOnFootFirstPersonViewThisUpdate(): void;
    /** Returns whether or not the passed camera handle exists. */
    export function doesCamExist(cam: number): boolean;
    /** Fades the screen in.  duration: The time the fade should take, in milliseconds. */
    export function doScreenFadeIn(duration: number): void;
    /** Fades the screen out.  duration: The time the fade should take, in milliseconds. */
    export function doScreenFadeOut(duration: number): void;
    export function forceCinematicRenderingThisUpdate(p0: boolean): void;
    export function getCamCoord(cam: number): Vector3;
    export function getCamFov(cam: number): number;
    /** rotationOrder: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eEulerRotationOrder */
    export function getCamRot(cam: number, rotationOrder: number): Vector3;
    /** Can use this with SET_CAM_SPLINE_PHASE to set the float it this native returns.  (returns 1.0f when no nodes has been added, reached end of non existing spline) */
    export function getCamSplinePhase(cam: number): number;
    export function getFinalRenderedCamCoord(): Vector3;
    export function getFinalRenderedCamFov(): number;
    export function getFinalRenderedCamRot(rotationOrder: number): Vector3;
    export function getFirstPersonAimCamZoomFactor(): number;
    export function getGameplayCamCoord(): Vector3;
    export function getGameplayCamFov(): number;
    export function getGameplayCamRelativeHeading(): number;
    export function getGameplayCamRelativePitch(): number;
    export function getGameplayCamRot(rotationOrder: number): Vector3;
    /** More info: see HAS_LETTER_BOX */
    export function getLetterBoxRatio(): number;
    export function getRenderingCam(): number;
    /** More info: https://en.wikipedia.org/wiki/Letterboxing_(filming) */
    export function hasLetterBox(): boolean;
    /** Old name: _INVALIDATE_VEHICLE_IDLE_CAM */
    export function invalidateCinematicVehicleIdleMode(): void;
    export function isAimCamActive(): boolean;
    /** Returns whether or not the passed camera handle is active. */
    export function isCamActive(cam: number): boolean;
    export function isCamInterpolating(cam: number): boolean;
    export function isCamRendering(cam: number): boolean;
    export function isCamShaking(cam: number): boolean;
    export function isCinematicCamRendering(): boolean;
    export function isDeathFailCameraRunning(): boolean;
    export function isFirstPersonAimCamActive(): boolean;
    export function isFirstPersonCameraActive(p0: any, p1: any, p2: any): boolean;
    export function isFollowVehicleCamActive(): boolean;
    export function isGameplayCamLookingBehind(): boolean;
    export function isGameplayCamRendering(): boolean;
    export function isGameplayCamShaking(): boolean;
    export function isGameplayHintActive(): boolean;
    export function isInterpolatingFromScriptCams(): boolean;
    export function isInterpolatingToScriptCams(): boolean;
    export function isScreenFadedIn(): boolean;
    export function isScreenFadedOut(): boolean;
    export function isScreenFadingIn(): boolean;
    export function isScreenFadingOut(): boolean;
    export function isSphereVisible(x: number, y: number, z: number, radius: number): boolean;
    export function playCamAnim(cam: number, animName: string, animDictionary: string, x: number, y: number, z: number, xRot: number, yRot: number, zRot: number, animFlags: number, rotOrder: number): boolean;
    export function pointCamAtCoord(cam: number, x: number, y: number, z: number): void;
    export function pointCamAtEntity(cam: number, entity: number, p2: number, p3: number, p4: number, p5: boolean): void;
    /** ease - smooth transition between the camera's positions easeTime - Time in milliseconds for the transition to happen  If you have created a script (rendering) camera, and want to go back to the  ch... */
    export function renderScriptCams(render: boolean, ease: boolean, easeTime: number, p3: boolean, p4: boolean, renderingFlags: number): void;
    /** Set camera as active/inactive. */
    export function setCamActive(cam: number, active: boolean): void;
    export function setCamActiveWithInterp(camTo: number, camFrom: number, duration: number, easeLocation: number, easeRotation: number): void;
    /** Allows you to aim and shoot at the direction the camera is facing. */
    export function setCamAffectsAiming(cam: number, toggle: boolean): void;
    export function setCamControlsMiniMapHeading(cam: number, p1: boolean): void;
    /** Sets the position of the cam. */
    export function setCamCoord(cam: number, posX: number, posY: number, posZ: number): void;
    export function setCamFarClip(cam: number, farClip: number): void;
    /** Sets the field of view of the cam.  Min: 1.0f Max: 130.0f */
    export function setCamFov(cam: number, fieldOfView: number): void;
    export function setCamMotionBlurStrength(cam: number, strength: number): void;
    export function setCamNearClip(cam: number, nearClip: number): void;
    export function setCamParams(cam: number, posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, fieldOfView: number, p8: any, graphType1: number, graphType2: number, rotationOrder: number, p12: any, p13: any): void;
    /** Sets the rotation of the cam. */
    export function setCamRot(cam: number, rotX: number, rotY: number, rotZ: number, rotationOrder: number): void;
    export function setCamSplineDuration(cam: number, timeDuration: number): void;
    export function setCamSplinePhase(cam: number, p1: number): void;
    export function setCamSplineSmoothingStyle(cam: number, smoothingStyle: number): void;
    export function setCinematicButtonActive(p0: boolean): void;
    export function setCinematicModeActive(p0: boolean): void;
    export function setFirstPersonAimCamRelativeHeadingLimitsThisUpdate(p0: number, p1: number): void;
    /** Old name: _SET_FIRST_PERSON_CAM_PITCH_RANGE */
    export function setFirstPersonAimCamRelativePitchLimitsThisUpdate(p0: number, p1: number): void;
    /** Forces gameplay cam to specified ped as if you were the ped or spectating it */
    export function setGameplayCamFollowPedThisUpdate(ped: number): void;
    /** Old name: _DISABLE_CAM_COLLISION_FOR_ENTITY */
    export function setGameplayCamIgnoreEntityCollisionThisUpdate(entity: number): void;
    export function setGameplayCamMaxMotionBlurStrengthThisUpdate(p0: number): void;
    /** Sets the camera position relative to heading in float from -360 to +360.  Heading is always 0 in aiming camera. */
    export function setGameplayCamRelativeHeading(heading: number, p1: number): void;
    /** Sets the camera pitch.  Parameters: x = pitches the camera on the x axis. Value2 = always seems to be hex 0x3F800000 (1.000000 float). */
    export function setGameplayCamRelativePitch(x: number, Value2: number): void;
    /** Sets the amplitude for the gameplay (i.e. 3rd or 1st) camera to shake. */
    export function setGameplayCamShakeAmplitude(amplitude: number): void;
    /** Hash used in finale1.ysc: 1726668277 */
    export function setGameplayCoordHint(x: number, y: number, z: number, duration: number, blendOutDuration: number, blendInDuration: number, p6: number): void;
    /** p6 & p7 - possibly length or time */
    export function setGameplayEntityHint(entity: number, xOffset: number, yOffset: number, zOffset: number, p4: boolean, p5: number, p6: number, p7: number, p8: any): void;
    export function setGameplayHintBaseOrbitPitchOffset(p0: number): void;
    /** Old name: _SET_GAMEPLAY_HINT_ANIM_OFFSETX */
    export function setGameplayHintCameraRelativeSideOffset(p0: number): void;
    /** Old name: _SET_GAMEPLAY_HINT_ANIM_OFFSETY */
    export function setGameplayHintCameraRelativeVerticalOffset(p0: number): void;
    export function setGameplayHintFollowDistanceScalar(p0: number): void;
    export function setGameplayHintFov(FOV: number): void;
    export function setGameplayObjectHint(p0: any, p1: number, p2: number, p3: number, p4: boolean, p5: any, p6: any, p7: any): void;
    export function setGameplayPedHint(p0: number, x1: number, y1: number, z1: number, p4: boolean, p5: any, p6: any, p7: any): void;
    export function setGameplayVehicleHint(p0: any, p1: number, p2: number, p3: number, p4: boolean, p5: any, p6: any, p7: any): void;
    /** Forces gameplay cam to specified vehicle as if you were in it */
    export function setInVehicleCamStateThisUpdate(vehicle: number, p1: number): void;
    export function setScriptedCameraIsFirstPersonThisFrame(p0: boolean): void;
    /** Old name: _ANIMATE_GAMEPLAY_CAM_ZOOM */
    export function setThirdPersonCamOrbitDistanceLimitsThisUpdate(p0: number, distance: number): void;
    /** minimum: Degrees between -180f and 180f. maximum: Degrees between -180f and 180f.  Clamps the gameplay camera's current yaw.  Eg. _CLAMP_GAMEPLAY_CAM_YAW(0.0f, 0.0f) will set the horizontal angle d... */
    export function setThirdPersonCamRelativeHeadingLimitsThisUpdate(minimum: number, maximum: number): void;
    /** minimum: Degrees between -90f and 90f. maximum: Degrees between -90f and 90f.  Clamps the gameplay camera's current pitch.  Eg. _CLAMP_GAMEPLAY_CAM_PITCH(0.0f, 0.0f) will set the vertical angle dir... */
    export function setThirdPersonCamRelativePitchLimitsThisUpdate(minimum: number, maximum: number): void;
    export function setWidescreenBorders(p0: boolean, p1: number): void;
    export function shakeCam(cam: number, type: string, amplitude: number): void;
    export function shakeGameplayCam(shakeName: string, intensity: number): void;
    export function stopCamPointing(cam: number): void;
    export function stopCamShaking(cam: number, p1: boolean): void;
    export function stopCodeGameplayHint(p0: boolean): void;
    export function stopGameplayCamShaking(p0: boolean): void;
    export function stopGameplayHint(p0: boolean): void;
    /** This native makes the gameplay camera zoom into first person/third person with a special effect. blendBackSmoothingType: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eBlendBackS... */
    export function stopRenderingScriptCamsUsingCatchUp(render: boolean, distance: number, blendBackSmoothingType: number, p3: boolean, p4: boolean, p5: boolean): void;
    export function camCreate(cameraDictionary: string): void;
    export function camCreate2(cameraDictionary: string): void;
    export function camDestroy(cameraDictionary: string): void;
    export function cinematicLocationSetLocationAndRotation(name: string, x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number): void;
    export function cinematicLocationTriggerScriptedShotEvent2(dictionary: string, shotName: string, duration: number): void;
    /** Creates Kill Cam for specified Ped Handle */
    export function createKillCam(ped: number): void;
    export function disableCinematicModeThisFrame(): void;
    /** Does the same as 0x9C473089A934C930 (DISABLE_ON_FOOT_FIRST_PERSON_VIEW_THIS_UPDATE) */
    export function disableOnFootFirstPersonViewThisUpdate2(): void;
    /** Used for DUELING_MANAGE_DEATH_CAMERA - Initializing death camera Params: targetPed = death cam focuses on it */
    export function forceCinematicDeathCamOnPed(targetPed: number): void;
    /** Returns true if first person camera is active in saloon1.ysc */
    export function forceFirstPersonCamThisFrame(): boolean;
    export function forceLetterBoxThisUpdate(): void;
    /** Forces camera position to furthest 3rd person */
    export function forceThirdPersonCamFarThisFrame(): void;
    /** Forces camera position to second furthest 3rd person */
    export function forceThirdPersonCamThisFrame(): void;
    /** Forces camera position to closest 3rd person */
    export function forceThirdPersonCloseThisFrame(): void;
    export function freezeGameplayCamThisFrame(): void;
    export function getPhotoModeDof(): number;
    export function getPhotoModeFocalLength(): number;
    export function getPhotoModeFocusDistance(): number;
    /** Only used in R* Script camera_photomode */
    export function isAnimSceneCamActive(): boolean;
    export function isCameraAvailable(cameraDictionary: string): boolean;
    export function isCamDataDictLoaded(cameraDictionary: string): boolean;
    export function isCamPhotofxRunning(): boolean;
    export function isCinematicCamLocationLoaded(sLocationDictName: string): boolean;
    /** Checks data related to Cinematic Cam Locations, if the check fails, the location is being loaded using 0x1B3C2D961F5FC0E1. */
    export function isCinematicCamLocationLoaded2(locationDictName: string): boolean;
    export function isInCinematicMode(): boolean;
    /** Returns true if player is in first person */
    export function isInFullFirstPersonMode(): boolean;
    export function loadCameraDataDict(cameraDictionary: string): void;
    export function loadCinematicCamLocation(locationDictName: string): void;
    export function pauseCameraFocus(cam: number, pause: boolean): void;
    /** Used to enable headshot kill replay when you headshot set ped. Params: p1 seems to be 0 or 1 in R* Scripts */
    export function reactivatePedHeadshotExecuteSlowcam(ped: number, p1: number): void;
    /** Creates Cinematic Black Bars (at top and bottom) Disable instantly: false/false, Enable instantly: true/true */
    export function requestLetterBoxNow(p0: boolean, p1: boolean): void;
    export function requestLetterBoxOvertime(p0: number, p1: number, p2: boolean, p3: number, p4: boolean, p5: boolean): void;
    export function setCamDofParams(cam: number, args: any): void;
    export function setCamFocusDistance(cam: number, distance: number): void;
    export function setGameplayCamInitialHeading(camInitialHeading: number): void;
    export function setGameplayCamInitialPitch(camInitialPitch: number): void;
    /** Used in Script Function SHOP_CAMERA_SUPPORT_START_NEW_ORBIT */
    export function setGameplayCamInitialZoom(camInitialZoom: number): void;
    /** Sets the third person gameplay camera zoom level and blends in. Must be called every frame to interpolate. Offset and distance permanently affects subtle zoom in weapon wheel and possibly in menus ... */
    export function setGameplayCamParamsThisUpdate(speed: number, respectHorizontalOffset: boolean, horizontalOffset: number, respectDistance: boolean, distance: number): void;
    /** Used for DUELING_MANAGE_DEATH_CAMERA - Initializing death camera _SET_P* - _SET_S* */
    export function setStartCinematicDeathCam(p0: boolean): void;
    /** [SHOP_CAMERA_SUPPORT_START_NEW_ORBIT] p0: struct<32> 256 */
    export function startCameraOrbit(p0: any): void;
    /** script_rel: DRUNK_SHAKE, REINFORCED_LASSO_STRUGGLE_SHAKE, CORRECTOR_SHAKE, MINIGAME_BOUNTY_SHAKE, HAND_SHAKE, MINIGAME_TRAIN_SHAKE script_mp_rel: DRUNK_SHAKE, REINFORCED_LASSO_STRUGGLE_SHAKE _STOP_... */
    export function stopGameplayCamShakingWithName(shakeName: string, p1: boolean): void;
    export function triggerMissionFailedCam(): void;
    export function unloadCameraDataDict(cameraDictionary: string): void;
    export function unloadCinematicCameraLocation(dictionaryName: string): void;

    // CLOCK
    export function addToClockTime(hours: number, minutes: number, seconds: number): void;
    export function advanceClockTimeTo(hour: number, minute: number, second: number): void;
    export function getClockDayOfMonth(): number;
    /** Gets the current day of the week.  0: Sunday 1: Monday 2: Tuesday 3: Wednesday 4: Thursday 5: Friday 6: Saturday */
    export function getClockDayOfWeek(): number;
    /** Gets the current ingame hour, expressed without zeros. (09:34 will be represented as 9) */
    export function getClockHours(): number;
    /** Gets the current ingame clock minute. */
    export function getClockMinutes(): number;
    export function getClockMonth(): number;
    /** Gets the current ingame clock second. Note that ingame clock seconds change really fast since a day in RDR is only 48 minutes in real life. */
    export function getClockSeconds(): number;
    export function getClockYear(): number;
    export function getMillisecondsPerGameMinute(): number;
    export function getPosixTime(year: number, month: number, day: number, hour: number, minute: number, second: number): void;
    export function pauseClock(toggle: boolean, unused: any): void;
    export function setClockDate(day: number, month: number, year: number): void;
    /** SET_CLOCK_TIME(12, 34, 56); */
    export function setClockTime(hour: number, minute: number, second: number): void;
    export function addTimeToDateTime(inDateTime: any, timeToAdd: any, outDateTime: any): void;
    /** Same as GET_POSIX_TIME except that it takes a single pointer to a struct. */
    export function getPosixTimeStruct(outTime: any): void;
    /** Base year is 1898. */
    export function getSecondsSinceBaseYear(): number;
    export function pauseClockThisFrame(toggle: boolean): void;
    export function setMillisecondsPerGameMinute(ms: number): void;

    // COLLECTION
    export function collectableCategorySetHasReceivedList(p0: any, p1: any, p2: any): void;
    /** collectableCategory: ANTIQUE_BOTTLES, BIRD_EGGS, ARROWHEADS, FAMILY_HEIRLOOMS, WILD_FLOWERS, COINS, LOST_JEWELRY_RINGS, LOST_JEWELRY_BRACELETS, LOST_JEWELRY_EARRINGS, LOST_JEWELRY_NECKLACES, TAROT_... */
    export function collectableGetCategoryItemSetBuyAward(collectableCategory: number, p1: number): number;
    export function collectableCategoryGetNumCollectables(collectableCategory: number, collectableSubcategory: number): number;
    export function collectableCategoryGetNumFound(collectableCategory: number, collectableSubcategory: number): number;
    export function collectableCategoryGetNumTurnedIn(collectableCategory: number, collectableSubcategory: number): number;
    export function collectableCategoryGetToastTextureDictionary(collectableCategory: number, collectableSubcategory: number): number;
    export function collectableCategoryGetToastTextureName(collectableCategory: number, collectableSubcategory: number): number;
    /** Used in Script Function NET_COLLECTABLES_HANDLE_ITEM_ADDED Returns collectableCategory Hash */
    export function collectableGetCategory(collectableItem: number): number;
    export function collectableGetCollectableItemHash(index: number, collectableCategory: number, collectableSubcategory: number): number;
    export function collectableGetIpl(collectableItem: number): number;
    export function collectableGetNumFound(collectableItem: number): number;
    export function collectableGetNumTurnedIn(collectableItem: number): number;
    export function collectableGetPlacementLocation(collectableItem: number): Vector3;
    export function collectableGetSubcategory(collectableItem: number): number;
    export function collectableIncrementNumFound(collectableItem: number, amount: number): void;
    export function collectableIncrementNumTurnedIn(collectableItem: number, amount: number): void;
    /** Returns discoveredItemHash _COLLECTABLE_C* - _COLLECTABLE_G* */
    export function collectableSetItemHashDiscovered(collectableItem: number): number;

    // COMPANION
    export function activateCompanionAnalysis(groupId: number): void;
    /** Used for Script Function NET_FETCH_CLIENT_ACTIVATE_COMAPNION_ANALYSIS: Hiding! Ped */
    export function addCompanionFlag(ped: number, p1: number): void;
    export function deactivateCompanionAnalysis(groupId: number): void;
    /** enum _0x18F77396 */
    export function getCompanionActivity(groupId: number): number;
    export function removeCompanionFlag(ped: number, p1: number): void;
    /** enum _0x18F77396 */
    export function setCompanionActivity(groupId: number, activity: number): void;

    // COMPAPP

    // COMPENDIUM
    export function compendiumAnimalGetSampleInventoryItem(compendiumEntry: number): any;
    export function compendiumAnimalHasSample(compendiumEntry: number): boolean;
    export function compendiumAnimalHasStamp(compendiumEntry: number): boolean;
    export function compendiumAnimalObservedByStatName(animalType: number, disableCompendiumToast: boolean): void;
    export function compendiumAnimalSetDiscovered(compendiumEntry: number): void;
    export function compendiumFishCaught(ped: number, category: number): void;
    export function compendiumFishGetLureSuitabilityByStatItem(animalType: number, baitType: number): number;
    export function compendiumGangAmbushSurvived(p0: any): void;
    export function compendiumGangBountyCaptured(p0: any): void;
    export function compendiumGangCampFound(p0: any, p1: any): void;
    export function compendiumGangEncountered(p0: any): void;
    export function compendiumGangHideoutFound(p0: any, p1: any): void;
    export function compendiumGangMemberKilled(p0: any): void;
    export function compendiumGetEntryByIndexInSubcategory(category: number, subcategory: number, count: number): number;
    export function compendiumGetEntryByPedIndex(category: number, ped: number): number;
    export function compendiumGetEntryByStatItem(category: number, animalType: number): number;
    export function compendiumGetMapDiscoverableFromStatItem(animalStatItem: number, x: number, y: number, z: number): number;
    export function compendiumGetNumEntriesInSubcategory(category: number, subcategory: number): number;
    export function compendiumGetShortDescriptionFromPed(ped: number): number;
    export function compendiumGetStudyAwardId(ped: number): any;
    export function compendiumGetSubcategoryPedIsIn(category: number, ped: number): number;
    export function compendiumGetSubcategorySampleToastDescComplete(category: number, subcategory: number): NativeString;
    export function compendiumGetSubcategorySampleToastDescProgress(category: number, subcategory: number): NativeString;
    export function compendiumGetSubcategorySampleToastTitle(category: number, subcategory: number): NativeString;
    export function compendiumGetSubcategoryToastAppId(category: number, subcategory: number): any;
    /** herbType: https://alloc8or.re/rdr3/doc/enums/eHerbType.txt Vector3: Player Location */
    export function compendiumHerbPicked(herbType: number, x: number, y: number, z: number): void;
    export function compendiumHorseBonding(ped: number, bondingLevel: number): void;
    /** Only gets called if bSetObserved is true and animalType is matching */
    export function compendiumHorseObserved(ped: number, disableCompendiumToast: boolean): void;
    /** NET_PLAYER_HORSE_PROCESS_EVENT_HORSE_BREAKING */
    export function compendiumHorseWildBroken(ped: number): void;
    export function compendiumWasAnimalObserved(ped: number): boolean;
    export function compendiumGetNumOfEntriesInCategory(category: number): number;
    export function compendiumGetSubcategoryHashFromAnimalType(category: number, animalType: number): number;

    // CRASHLOG

    // CREW
    export function networkClanGetLocalMembershipsCount(): number;
    export function networkClanGetMembershipDesc(memberDesc: any, p1: number): boolean;
    export function networkClanIsEmblemReady(p0: any, p1: any): boolean;
    export function networkClanPlayerGetDesc(clanDesc: any, bufferSize: number, gamerHandle: any): boolean;
    export function networkClanPlayerIsActive(gamerHandle: any): boolean;
    export function networkClanReleaseEmblem(p0: any): void;
    export function networkClanRequestEmblem(p0: any): boolean;
    export function networkClanServiceIsValid(): boolean;
    export function networkFindGamersInCrew(crewId: number): boolean;
    export function networkGetPrimaryClanDataClear(): any;
    export function networkGetPrimaryClanDataNew(p0: any, p1: any): boolean;
    export function networkGetPrimaryClanDataPending(): boolean;
    export function networkGetPrimaryClanDataStart(p0: any, p1: any): boolean;
    export function networkGetPrimaryClanDataSuccess(): boolean;
    export function networkAcceptClanInvite(crewInviteIndex: number): boolean;
    export function networkClanInvitePlayer(p0: any): boolean;
    export function networkClanSetActive(p0: any): any;

    // DATABINDING
    export function databindingIsEntryValid(entryId: number): boolean;
    export function databindingReadInt(p0: any): number;
    export function databindingWriteStringFromParent(p0: any, p1: string, p2: string): void;
    export function databindingAddDataBool(p0: any, p1: string, p2: boolean): any;
    export function databindingAddDataBoolByHash(p0: any, p1: number, p2: boolean): any;
    export function databindingAddDataBoolFromPath(p0: string, p1: string, p2: boolean): any;
    /** Returns entryId Hash */
    export function databindingAddDataContainer(entryId: number, p1: string): number;
    export function databindingAddDataContainerByHash(p0: any, p1: number): any;
    export function databindingAddDataContainerFromPath(p0: string, p1: string): number;
    export function databindingAddDataContainerFromPathByHash(p0: string, p1: number): any;
    export function databindingAddDataFloat(p0: any, p1: string, p2: number): any;
    export function databindingAddDataGangId(p0: any, p1: string, gangId: any): any;
    export function databindingAddDataHash(p0: any, p1: string, p2: number): any;
    export function databindingAddDataHashByHash(p0: any, p1: number, p2: number): any;
    export function databindingAddDataInt(p0: any, p1: string, p2: number): any;
    export function databindingAddDataIntByHash(p0: any, p1: number, p2: number): any;
    export function databindingAddDataPosseId(p0: any, p1: string, posseId: any): any;
    export function databindingAddDataString(p0: any, p1: string, p2: string): any;
    export function databindingAddDataStringByHash(p0: any, p1: number, p2: string): any;
    export function databindingAddDataStringFromPath(p0: string, p1: string, p2: string): any;
    export function databindingAddHashArray(p0: any, p1: string): any;
    export function databindingAddStringArray(p0: any, p1: string): any;
    export function databindingAddUiItemList(p0: any, p1: string): any;
    export function databindingAddUiItemListByHash(p0: any, p1: number): any;
    export function databindingAddUiItemListFromPath(p0: string, p1: string): any;
    export function databindingClearBindingArray(entryId: number): void;
    export function databindingGetArrayCount(entryId: number): any;
    export function databindingGetDataContainerFromChildIndex(entryId: number, p1: number): any;
    export function databindingGetDataContainerFromPath(p0: string): any;
    export function databindingGetItemContextByIndex(p0: any, index: number): any;
    export function databindingInsertUiItemToListFromContextHashAlias(p0: any, index: number, p2: number, p3: any): void;
    export function databindingInsertUiItemToListFromContextStringAlias(p0: any, index: number, p2: string, p3: any): void;
    export function databindingInsertUiItemToListFromPathStringAlias(p0: any, p1: any, p2: string, p3: any): void;
    export function databindingReadDataBool(p0: any): any;
    export function databindingReadDataBoolFromParent(p0: any, p1: string): any;
    export function databindingReadDataBoolFromParentByHash(p0: any, p1: number): any;
    export function databindingReadDataHashStringFromParent(p0: any, p1: string): any;
    export function databindingReadDataHashStringFromParentByHash(p0: any, p1: number): any;
    export function databindingReadDataIntFromParent(p0: any, p1: string): any;
    export function databindingReadDataIntFromParentByHash(p0: any, p1: number): any;
    export function databindingReadDataString(p0: any): any;
    export function databindingReadDataStringFromParent(p0: any, p1: string): any;
    export function databindingReadFloat(entryId: number): number;
    export function databindingReadHash(entryId: number): number;
    export function databindingRemoveBindingArrayItemByDataContextId(p0: any, entryId: number): void;
    export function databindingRemoveDataEntry(entryId: number): void;
    /** Remove a UI item from its list by index. Video: https://imgur.com/a/LDAUVkh */
    export function databindingRemoveUiItemFromListByIndex(entryId: number, index: number): void;
    export function databindingSetTemplatedUiItemHashAlias(p0: any, p1: number, p2: number): void;
    export function databindingSetTemplatedUiItemListSize(p0: any, p1: number): void;
    export function databindingWriteDataBool(p0: any, p1: boolean): void;
    export function databindingWriteDataBoolFromParent(p0: any, p1: string, p2: boolean): void;
    export function databindingWriteDataFloat(p0: any, p1: number): void;
    export function databindingWriteDataGangId(p0: any, p1: string, gangId: any): void;
    export function databindingWriteDataHashString(p0: any, p1: number): void;
    export function databindingWriteDataHashStringFromParent(p0: any, p1: string, p2: number): void;
    export function databindingWriteDataHashStringFromParentByHash(p0: any, p1: number, p2: any): void;
    export function databindingWriteDataInt(p0: any, p1: number): void;
    export function databindingWriteDataIntFromParent(p0: any, p1: string, p2: number): void;
    export function databindingWriteDataIntFromParentByHash(p0: any, p1: number, p2: any): void;
    export function databindingWriteDataPosseId(p0: any, p1: string, posseId: any): void;
    export function databindingWriteDataScriptVariables(p0: number, p1: number, ...args: any[]): void;
    export function databindingWriteDataString(p0: any, p1: string): void;
    export function databindingWriteStringFromHash(p0: any, p1: number, p2: string): void;
    export function virtualCollectionExists(p0: any): any;
    export function virtualCollectionItemAdd(p0: any, index: number, p2: number, p3: any): void;
    export function virtualCollectionReset(p0: any): void;
    export function virtualCollectionSetInterestIndex(p0: any, interestIndex: number): void;
    export function virtualCollectionSetSize(p0: any, size: number): void;

    // DATAFILE
    export function dataarrayGetBool(arrayData: any, arrayIndex: number): boolean;
    export function dataarrayGetCount(arrayData: any): number;
    export function dataarrayGetDict(arrayData: any, arrayIndex: number): any;
    export function dataarrayGetFloat(arrayData: any, arrayIndex: number): number;
    export function dataarrayGetInt(arrayData: any, arrayIndex: number): number;
    export function dataarrayGetString(arrayData: any, arrayIndex: number): NativeString;
    /** Types: 1 = Boolean 2 = Integer 3 = Float 4 = String 5 = Vector3 6 = Object 7 = Array */
    export function dataarrayGetType(arrayData: any, arrayIndex: number): number;
    export function dataarrayGetVector(arrayData: any, arrayIndex: number): Vector3;
    export function datadictGetArray(objectData: any, key: string): any;
    export function datadictGetBool(objectData: any, key: string): boolean;
    export function datadictGetDict(objectData: any, key: string): any;
    export function datadictGetFloat(objectData: any, key: string): number;
    export function datadictGetInt(objectData: any, key: string): number;
    export function datadictGetString(objectData: any, key: string): NativeString;
    /** Types: 1 = Boolean 2 = Integer 3 = Float 4 = String 5 = Vector3 6 = Object 7 = Array */
    export function datadictGetType(objectData: any, key: string): number;
    export function datadictGetVector(objectData: any, key: string): Vector3;
    export function datadictIsArrayValid(fileDict: any): boolean;
    export function datadictIsDictValid(fileDict: any): boolean;
    export function datadictSetInt(objectData: any, key: string, value: number): void;
    export function datafileCreate(index: number): void;
    export function datafileDelete(index: number): void;
    export function datafileDeleteRequestedFile(p0: any): boolean;
    export function datafileGetFileDict(index: number): any;
    export function datafileHasLoadedFileData(p0: any): boolean;
    export function datafileHasValidFileData(p0: any): boolean;
    export function datafileSelectActiveFile(p0: any, p1: any): boolean;
    /** Reloops value returned by UGC_QUERY_GET_CONTENT_NUM */
    export function datafileUgcSelectData(ugcRequestId: any, index: number, p2: any): any;
    /** Adds the given request ID to the watch list. */
    export function datafileWatchRequestId(id: number): void;
    export function parseddataIsFileLoaded(fileHandle: number): boolean;
    export function parseddataIsFileValid(fileHandle: number): boolean;
    /** Old name: _DATAFILE_GET_HASH */
    export function parseddataRqFilloutHash(p0: number, p1: any): boolean;
    /** Old name: _DATAFILE_GET_DATA_NODE_INDEX */
    export function parseddataRqFilloutNode(p0: number, p1: any): boolean;
    export function parseddataRqFilloutString127(p0: string, p1: any): boolean;
    export function ugc2SetPlayerData(p0: any, p1: any, p2: any, p3: any): any;
    export function parseddataGetBool(p0: boolean, p1: any, p2: number): boolean;
    /** Returns false when there are no entries. */
    export function parseddataGetEntries(p0: any): boolean;
    /** Opens file. */
    export function parseddataGetFile(p0: any): void;
    export function parseddataGetFloat(p0: any, p1: any, p2: number): boolean;
    export function parseddataGetInt(p0: any, p1: any, p2: number): boolean;
    export function parseddataGetNumChildren(p0: any, p1: any): any;
    export function parseddataGetSection(p0: number, p1: any, section: number): boolean;
    /** LOAD_PARSEDDATA_FILE_FAILSAFE_HASH Returns parseddata script fileHandle */
    export function parseddataLoadFileHash(p0: number): number;
    export function parseddataRegisterQuery(p0: any, p1: any, p2: any): any;
    /** Old name: _DATAFILE_GET_BOOL */
    export function parseddataRqFilloutBool(p0: boolean, p1: any): boolean;
    /** Old name: _DATAFILE_GET_FLOAT */
    export function parseddataRqFilloutFloat(p0: number, p1: any): boolean;
    /** Old name: _DATAFILE_GET_INT */
    export function parseddataRqFilloutInt(p0: number, p1: any): boolean;
    /** Old name: _DATAFILE_GET_STRING */
    export function parseddataRqFilloutString63(p0: string, p1: any): boolean;
    /** Old name: _DATAFILE_GET_VECTOR */
    export function parseddataRqFilloutVector(p0: Vector3, p1: any): boolean;
    export function parseddataRqGetNumNodes(p0: any): any;
    export function parseddataUnloadFile(fileHandle: number): void;

    // DEBUG
    /** Return example: 1207.69_dev_pc, 1436.28_dev_live_tu  Old name: _GET_GAME_BUILD_STRING */
    export function getGameVersionName(): NativeString;

    // DECORATOR
    /** Returns whether or not the specified property is set for the entity. */
    export function decorExistOn(entity: number, propertyName: string): boolean;
    export function decorGetBool(entity: number, propertyName: string): boolean;
    export function decorGetFloat(entity: number, propertyName: string): number;
    export function decorGetInt(entity: number, propertyName: string): number;
    /** type: see DECOR_REGISTER */
    export function decorIsRegisteredAsType(propertyName: string, type: number): boolean;
    /** type: enum eDecorType { 	DECOR_TYPE_UNKNOWN, 	DECOR_TYPE_FLOAT, 	DECOR_TYPE_BOOL, 	DECOR_TYPE_INT, 	DECOR_TYPE_STRING, 	DECOR_TYPE_TIME, 	DECOR_TYPE_PLAYER_INDEX }; */
    export function decorRegister(propertyName: string, type: number): void;
    export function decorRemove(entity: number, propertyName: string): boolean;
    export function decorRemoveAll(entity: number): boolean;
    /** This function sets metadata of type bool to specified entity. */
    export function decorSetBool(entity: number, propertyName: string, value: boolean): boolean;
    export function decorSetFloat(entity: number, propertyName: string, value: number): boolean;
    /** Sets property to int. */
    export function decorSetInt(entity: number, propertyName: string, value: number): boolean;
    export function decorSetString(entity: number, propertyName: string, value: string): boolean;
    export function decorGetPlayerIndex(entity: number, propertyName: string): number;
    /** type: see DECOR_REGISTER */
    export function decorRegisterNetworked(propertyName: string, type: number, isNetworked: boolean): void;
    export function decorSetPlayerIndex(entity: number, propertyName: string, value: number): boolean;

    // DLC
    export function getIsLoadingScreenActive(): boolean;
    export function isDlcPresent(dlcHash: number): boolean;
    export function getSpecialEditionCashCampBonusEnabled(): boolean;
    export function getSpecialEditionCoreStatsBonusEnabled(): boolean;

    // ENTITY
    export function applyForceToEntity(entity: number, forceFlags: number, x: number, y: number, z: number, offX: number, offY: number, offZ: number, boneIndex: number, isDirectionRel: boolean, ignoreUpVec: boolean, isForceRel: boolean, p12: boolean, p13: boolean): void;
    /** p6/relative - makes the xyz force not relative to world coords, but to something else p7/highForce - setting false will make the force really low */
    export function applyForceToEntityCenterOfMass(entity: number, forceType: number, x: number, y: number, z: number, component: number, isDirectionRel: boolean, isForceRel: boolean, p8: boolean): void;
    /** Attaches entity1 to bone (boneIndex) of entity2.  boneIndex - this is different to boneID, use GET_PED_BONE_INDEX to get the index from the ID. use the index for attaching to specific bones. entity... */
    export function attachEntityToEntity(entity1: number, entity2: number, boneIndex: number, xPos: number, yPos: number, zPos: number, xRot: number, yRot: number, zRot: number, p9: boolean, useSoftPinning: boolean, collision: boolean, isPed: boolean, vertexIndex: number, fixedRot: boolean, p15: boolean, p16: boolean): void;
    export function attachEntityToEntityPhysically(entity1: number, entity2: number, p2: number, boneIndex: number, offsetX: number, offsetY: number, offsetZ: number, p7: number, p8: number, p9: number, p10: number, p11: number, p12: number, p13: number, p14: boolean, p15: boolean, p16: boolean, p17: boolean, p18: number, p19: boolean, p20: number, p21: number): void;
    export function clearEntityLastDamageEntity(entity: number): void;
    export function createForcedObject(x: number, y: number, z: number, p3: any, modelHash: number, p5: boolean): void;
    export function createModelHide(x: number, y: number, z: number, radius: number, model: number, p5: boolean): void;
    export function createModelHideExcludingScriptObjects(x: number, y: number, z: number, radius: number, model: number, p5: boolean): void;
    /** Only works with objects! */
    export function createModelSwap(x: number, y: number, z: number, radius: number, originalModel: number, newModel: number, p6: boolean): void;
    /** Deletes the specified entity, then sets the handle pointed to by the pointer to NULL. */
    export function deleteEntity(entity: number): void;
    export function detachEntity(entity: number, p1: boolean, collision: boolean): void;
    export function doesEntityBelongToThisScript(entity: number, p1: boolean): boolean;
    /** Checks if the Entity exists */
    export function doesEntityExist(entity: number): boolean;
    export function doesEntityHaveDrawable(entity: number): boolean;
    export function doesEntityHavePhysics(entity: number): boolean;
    export function findAnimEventPhase(animDictionary: string, animName: string, p2: string, p3: any, p4: any): boolean;
    export function forceEntityAiAndAnimationUpdate(entity: number, p1: boolean): void;
    export function freezeEntityPosition(entity: number, toggle: boolean): void;
    export function getAnimDuration(animDict: string, animName: string): number;
    /** enum eCarriableState { 	CARRIABLE_STATE_NONE, 	CARRIABLE_STATE_TRANSITIONING_TO_HOGTIED, 	CARRIABLE_STATE_CARRIABLE_INTRO, 	CARRIABLE_STATE_CARRIABLE, 	CARRIABLE_STATE_BEING_PICKED_UP_FROM_GROUND, ... */
    export function getCarriableEntityState(entity: number): number;
    export function getEntityAlpha(entity: number): number;
    export function getEntityAttachedTo(entity: number): number;
    export function getEntityBoneIndexByName(entity: number, boneName: string): number;
    export function getEntityCollisionDisabled(entity: number): boolean;
    /** Gets the current coordinates for a specified entity. `entity` = The entity to get the coordinates from. `alive` = Unused by the game, potentially used by debug builds in order to assert whether or ... */
    export function getEntityCoords(entity: number, alive: boolean, realCoords: boolean): Vector3;
    /** Gets the entity's forward vector in XY(Z) eulers. */
    export function getEntityForwardVector(entity: number): Vector3;
    /** Gets the X-component of the entity's forward vector. */
    export function getEntityForwardX(entity: number): number;
    /** Gets the Y-component of the entity's forward vector. */
    export function getEntityForwardY(entity: number): number;
    /** Returns the heading of the entity in degrees. Also know as the "Yaw" of an entity. */
    export function getEntityHeading(entity: number): number;
    export function getEntityHealth(entity: number): number;
    export function getEntityHeight(entity: number, X: number, Y: number, Z: number, atTop: boolean, inWorldCoords: boolean): number;
    export function getEntityHeightAboveGround(entity: number): number;
    /** Returns the LOD distance of an entity. */
    export function getEntityLodDist(entity: number): number;
    export function getEntityMatrix(entity: number, rightVector: Vector3, forwardVector: Vector3, upVector: Vector3, position: Vector3): void;
    export function getEntityMaxHealth(entity: number, p1: boolean): number;
    /** Returns the model hash from the entity */
    export function getEntityModel(entity: number): number;
    export function getEntityPitch(entity: number): number;
    export function getEntityPopulationType(entity: number): number;
    /** Displays the current ROLL axis of the entity [-180.0000/180.0000+] (Sideways Roll) such as a vehicle tipped on its side */
    export function getEntityRoll(entity: number): number;
    export function getEntityRotation(entity: number, rotationOrder: number): Vector3;
    /** Result is in meters per second (m/s) */
    export function getEntitySpeed(entity: number): number;
    export function getEntitySpeedVector(entity: number, relative: boolean): Vector3;
    /** Get how much of the entity is submerged.  1.0f is whole entity. */
    export function getEntitySubmergedLevel(entity: number): number;
    /** Returns entityType: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eEntityType */
    export function getEntityType(entity: number): number;
    export function getEntityUprightValue(entity: number): number;
    export function getEntityVelocity(entity: number, p1: number): Vector3;
    export function getIsAnimal(entity: number): boolean;
    export function getMatchingEntities(volume: number, itemSet: number, entityType: number, p3: any, p4: number, p5: string): number;
    export function getNearestParticipantToEntity(entity: number): number;
    export function getNearestPlayerToEntity(entity: number, playerPedToIgnore: number, flags: number): number;
    export function getNearestPlayerToEntityOnTeam(entity: number, team: number, playerPedToIgnore: number, flags: number): number;
    /** Simply returns whatever is passed to it (Regardless of whether the handle is valid or not). */
    export function getObjectIndexFromEntityIndex(entity: number): number;
    export function getOffsetFromEntityGivenWorldCoords(entity: number, posX: number, posY: number, posZ: number): Vector3;
    /** Offset values are relative to the entity.  x = left/right y = forward/backward z = up/down */
    export function getOffsetFromEntityInWorldCoords(entity: number, offsetX: number, offsetY: number, offsetZ: number): Vector3;
    /** Simply returns whatever is passed to it (Regardless of whether the handle is valid or not). */
    export function getPedIndexFromEntityIndex(entity: number): number;
    /** Simply returns whatever is passed to it (Regardless of whether the handle is valid or not). */
    export function getVehicleIndexFromEntityIndex(entity: number): number;
    /** Returns the coordinates of an entity-bone. https://github.com/femga/rdr3_discoveries/tree/master/boneNames */
    export function getWorldPositionOfEntityBone(entity: number, boneIndex: number): Vector3;
    export function hasAnimEventFired(entity: number, actionHash: number): boolean;
    export function hasCollisionLoadedAroundEntity(entity: number): boolean;
    /** Old name: _HAS_COLLISION_LOADED_AT_COORDS */
    export function hasCollisionLoadedAroundPosition(xPos: number, yPos: number, zPos: number): boolean;
    export function hasEntityAnimFinished(entity: number, animDict: string, animName: string, p3: number): boolean;
    export function hasEntityBeenDamagedByAnyObject(entity: number): boolean;
    export function hasEntityBeenDamagedByAnyPed(entity: number): boolean;
    export function hasEntityBeenDamagedByAnyVehicle(entity: number): boolean;
    export function hasEntityBeenDamagedByEntity(entity1: number, entity2: number, p2: boolean, p3: boolean): boolean;
    export function hasEntityClearLosToCoord(entity: number, x: number, y: number, z: number, flags: number): boolean;
    export function hasEntityClearLosToEntity(entity1: number, entity2: number, traceType: number): boolean;
    /** Has the entity1 got a clear line of sight to the other entity2 from the direction entity1 is facing. */
    export function hasEntityClearLosToEntityInFront(entity1: number, entity2: number, traceType: number): boolean;
    export function hasEntityCollidedWithAnything(entity: number): boolean;
    export function isAnEntity(handle: number): boolean;
    export function isEntityAnObject(entity: number): boolean;
    export function isEntityAttached(entity: number): boolean;
    export function isEntityAttachedToAnyObject(entity: number): boolean;
    export function isEntityAttachedToAnyPed(entity: number): boolean;
    export function isEntityAttachedToAnyVehicle(entity: number): boolean;
    export function isEntityAttachedToEntity(from: number, to: number): boolean;
    /** Checks if entity is within x/y/zSize distance of x/y/z.   Last three are unknown ints, almost always p7 = 0, p8 = 1, p9 = 0 */
    export function isEntityAtCoord(entity: number, xPos: number, yPos: number, zPos: number, xSize: number, ySize: number, zSize: number, p7: boolean, p8: boolean, p9: number): boolean;
    /** Checks if entity1 is within the box defined by x/y/zSize of entity2.  Last three parameters are almost always p5 = 0, p6 = 1, p7 = 0 */
    export function isEntityAtEntity(entity1: number, entity2: number, xSize: number, ySize: number, zSize: number, p5: boolean, p6: boolean, p7: number): boolean;
    export function isEntityAMissionEntity(entity: number): boolean;
    export function isEntityAPed(entity: number): boolean;
    export function isEntityAVehicle(entity: number): boolean;
    export function isEntityDead(entity: number): boolean;
    export function isEntityInAir(entity: number, p1: any): boolean;
    /** Creates a spherical cone at origin that extends to surface with the angle specified. Then returns true if the entity is inside the spherical cone  Angle is measured in degrees. */
    export function isEntityInAngledArea(entity: number, originX: number, originY: number, originZ: number, edgeX: number, edgeY: number, edgeZ: number, angle: number, p8: boolean, p9: boolean, p10: any): boolean;
    export function isEntityInArea(entity: number, x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, p7: boolean, p8: boolean, p9: any): boolean;
    export function isEntityInVolume(entity: number, volume: number, p2: boolean, p3: number): boolean;
    export function isEntityInWater(entity: number): boolean;
    export function isEntityOccluded(entity: number): boolean;
    export function isEntityOnScreen(entity: number): boolean;
    export function isEntityPlayingAnim(entity: number, animDict: string, animName: string, animType: number): boolean;
    export function isEntityStatic(entity: number): boolean;
    export function isEntityTouchingEntity(entity: number, targetEntity: number): boolean;
    export function isEntityTouchingModel(entity: number, modelHash: number): boolean;
    export function isEntityUpright(entity: number, angle: number): boolean;
    export function isEntityUpsidedown(entity: number): boolean;
    export function isEntityVisible(entity: number): boolean;
    export function isEntityVisibleToScript(entity: number): boolean;
    export function isEntityWaitingForWorldCollision(entity: number): boolean;
    export function isMapEntityPinned(p0: any): boolean;
    export function pinClosestMapEntity(modelHash: number, x: number, y: number, z: number, flags: number): any;
    export function placeEntityOnGroundProperly(entity: number, p1: boolean): boolean;
    /** https://github.com/femga/rdr3_discoveries/tree/master/animations */
    export function playEntityAnim(entity: number, animName: string, animDict: string, p3: number, loop: boolean, stayInAnim: boolean, p6: boolean, delta: number, bitset: any): boolean;
    export function removeForcedObject(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    export function removeModelHide(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any): void;
    export function removeModelSwap(x: number, y: number, z: number, radius: number, originalModel: number, newModel: number, p6: boolean): void;
    export function resetEntityAlpha(entity: number): void;
    /** Sets the loot table an entity will carry. Returns true if loot table has been successfully set. Returns false if entity is not a ped or object. https://github.com/femga/rdr3_discoveries/blob/master... */
    export function scriptOverrideEntityLootTablePermanent(entity: number, lootTable: number): boolean;
    export function setCanAutoVaultOnEntity(entity: number, toggle: boolean): void;
    export function setCanClimbOnEntity(entity: number, toggle: boolean): void;
    /** skin - everything alpha except skin Set entity alpha level. Ranging from 0 to 255 but changes occur after every 20 percent (after every 51). */
    export function setEntityAlpha(entity: number, alphaLevel: number, skin: boolean): void;
    export function setEntityAlwaysPrerender(entity: number, toggle: boolean): void;
    /** Makes the specified entity (ped, vehicle or object) persistent. Persistent entities will not automatically be removed by the engine. */
    export function setEntityAsMissionEntity(entity: number, scriptHostObject: boolean, grabFromOtherScript: boolean): void;
    /** Marks the specified entity (ped, vehicle or object) as no longer needed. Entities marked as no longer needed, will be deleted as the engine sees fit. */
    export function setEntityAsNoLongerNeeded(entity: number): void;
    export function setEntityCanBeDamaged(entity: number, toggle: boolean): void;
    export function setEntityCanBeDamagedByRelationshipGroup(entity: number, bCanBeDamaged: boolean, relGroup: number): void;
    /** Sets whether the entity can be targeted without being in line-of-sight. */
    export function setEntityCanBeTargetedWithoutLos(entity: number, toggle: boolean): void;
    export function setEntityCollision(entity: number, toggle: boolean, keepPhysics: boolean): void;
    export function setEntityCompletelyDisableCollision(entity: number, toggle: boolean, keepPhysics: boolean): void;
    export function setEntityCoords(entity: number, xPos: number, yPos: number, zPos: number, xAxis: boolean, yAxis: boolean, zAxis: boolean, clearArea: boolean): void;
    /** Axis - Invert Axis Flags */
    export function setEntityCoordsNoOffset(entity: number, xPos: number, yPos: number, zPos: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): void;
    export function setEntityDynamic(entity: number, toggle: boolean): void;
    export function setEntityHasGravity(entity: number, toggle: boolean): void;
    export function setEntityHeading(entity: number, heading: number): void;
    /** Sets the entity's health. healthAmount sets the health value to that, and sets the maximum health core value. Setting healthAmount to 0 will kill the entity. entityKilledBy parameter can also be 0 */
    export function setEntityHealth(entity: number, healthAmount: number, entityKilledBy: number): void;
    /** Sets a ped or an object totally invincible. It doesn't take any kind of damage. Peds will not ragdoll on explosions. */
    export function setEntityInvincible(entity: number, toggle: boolean): void;
    export function setEntityIsTargetPriority(entity: number, p1: boolean, p2: number): void;
    export function setEntityLoadCollisionFlag(entity: number, toggle: boolean): void;
    /** LOD distance can be 0 to 0xFFFF (higher values will result in 0xFFFF) as it is actually stored as a 16-bit value (aka uint16_t). */
    export function setEntityLodDist(entity: number, value: number): void;
    export function setEntityMaxHealth(entity: number, value: number): void;
    export function setEntityMotionBlur(entity: number, toggle: boolean): void;
    /** Old name: _SET_ENTITY_DECALS_DISABLED */
    export function setEntityNoweapondecals(entity: number, toggle: boolean): void;
    export function setEntityNoCollisionEntity(entity1: number, entity2: number, thisFrameOnly: boolean): void;
    export function setEntityOnlyDamagedByPlayer(entity: number, toggle: boolean): void;
    export function setEntityOnlyDamagedByRelationshipGroup(entity: number, p1: boolean, relationshipGroup: number): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/AI/ENTITY_PROOFS BOOL p2: handles an additional special proofs flag, so it simply indicates whether it should be enabled or disabled, not sure ... */
    export function setEntityProofs(entity: number, proofsBitset: number, specialFlag: boolean): void;
    export function setEntityQuaternion(entity: number, x: number, y: number, z: number, w: number): void;
    export function setEntityRenderScorched(entity: number, toggle: boolean): void;
    export function setEntityRequiresMoreExpensiveRiverCheck(entity: number, toggle: boolean): void;
    export function setEntityRotation(entity: number, pitch: number, roll: number, yaw: number, rotationOrder: number, p5: boolean): void;
    /** Old name: _SET_ENTITY_CLEANUP_BY_ENGINE */
    export function setEntityShouldFreezeWaitingOnCollision(entity: number, toggle: boolean): void;
    /** Note that the third parameter(denoted as z) is "up and down" with positive numbers encouraging upwards movement. */
    export function setEntityVelocity(entity: number, x: number, y: number, z: number): void;
    export function setEntityVisible(entity: number, toggle: boolean): void;
    /** This is an alias of SET_ENTITY_AS_NO_LONGER_NEEDED. */
    export function setObjectAsNoLongerNeeded(object: number): void;
    /** This is an alias of SET_ENTITY_AS_NO_LONGER_NEEDED. */
    export function setPedAsNoLongerNeeded(ped: number): void;
    /** This is an alias of SET_ENTITY_AS_NO_LONGER_NEEDED. */
    export function setVehicleAsNoLongerNeeded(vehicle: number): void;
    /** Doesn't actually return anything. */
    export function stopEntityAnim(entity: number, animation: string, animGroup: string, p3: number): boolean;
    export function wouldEntityBeOccluded(entityModelHash: number, x: number, y: number, z: number, p4: boolean): boolean;
    export function addEntityTrackingTrails(entity: number): void;
    export function attachEntityToCoordsPhysically(entity: number, p1: number, x: number, y: number, z: number, offsetX: number, offsetY: number, offsetZ: number, timer: number, p9: boolean, p10: number, p11: number, p12: number, p13: number, p14: number, p15: number): void;
    /** Alters entity's health by 'amount'. Can be negative (to drain health). In the scripts entity2 and weaponHash are unused (zero). */
    export function changeEntityHealth(entity: number, amount: number, entity2: number, weaponHash: number): boolean;
    export function createFootpathTrail(p0: any, waypointRecord: string, bUseSnowOffset: boolean, p3: number, p4: number, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, bInit: boolean): any;
    export function deleteCarriable(entity: number): void;
    /** Must be called from a background script, otherwise it will do nothing. */
    export function deleteEntity2(entity: number): void;
    /** Returns true if calling script owns specified entity */
    export function doesThreadOwnThisEntity(entity: number): boolean;
    /** Returns the ped which is currently looting another ped. Returns null if no one is looting the ped. */
    export function findEntityLootingPed(entity: number): number;
    /** Enable/disable automatic ambient passenger population on a train wagon (carriage). 	- toggle=true: wagon is kept populated; removed/deleted passengers are replaced quickly. 	- toggle=false: stop au... */
    export function forceTrainWagonPopulation(trainWagon: number, toggle: boolean): void;
    /** Returns a hash of an entity's name. (Alternative Name: _GET_ENTITY_PROMPT_NAME_HASH) */
    export function getCarriableFromEntity(entity: number): number;
    export function getEntitiesInVolume(volume: number, itemSet: number, entityType: number): number;
    export function getEntitiesNearPoint(x: number, y: number, z: number, radius: number, itemSet: number, p5: number): number;
    /** Returns a normalized value between 0.0f and 1.0f. You can get the actual anim time by multiplying this by GET_ANIM_DURATION */
    export function getEntityAnimCurrentTime(entity: number, animDict: string, animName: string): number;
    /** Params: p1 = 0 in R* Scripts (GET_DOOR_ENTITY_FROM_ID) https://github.com/femga/rdr3_discoveries/blob/master/doorHashes/doorhashes.lua */
    export function getEntityByDoorhash(doorHash: number, p1: number): number;
    export function getEntityCanBeDamaged(entity: number): boolean;
    /** flagId: see _SET_ENTITY_CARRYING_FLAG */
    export function getEntityCarryingFlag(entity: number, flagId: number): boolean;
    /** Returns zero if the entity is not a carriable */
    export function getEntityCarryConfig(entity: number): number;
    /** Gets the entity's forward vector in YX(Z) eulers. Similar to GET_ENTITY_FORWARD_VECTOR */
    export function getEntityForwardVectorYx(entity: number): Vector3;
    /** Returns (CUR_HEALTH / MAX_HEALTH) */
    export function getEntityHealthFloat(entity: number): number;
    /** Note: this native was removed in 1232 but added back in 1311 */
    export function getEntityProofs(entity: number): number;
    export function getEntityScript(entity: number, argStruct: any): number;
    export function getEntityThreatTier(entity: number): number;
    export function getEntityWorldPositionOfDimensions(entity: number, minimum: Vector3, maximum: Vector3): void;
    export function getIsBird(entity: number): boolean;
    export function getIsCarriablePelt(entity: number): boolean;
    export function getIsPredator(entity: number): boolean;
    /** Valid indices: 0 - 3 Index 1 always returns a `hogtied` config, doesn't matter the entity. It's for humans only and the ped must be resurrected first if it's dead. */
    export function getOptimalCarryConfig(entity: number, index: number): number;
    /** Returns the ped's animal type hash: https://alloc8or.re/rdr3/doc/enums/eAnimalType.txt Combine this with GET_STRING_FROM_HASH_KEY to display localized entity names */
    export function getPedAnimalType(ped: number): number;
    export function getPinnedMapEntity(p0: any): number;
    /** Returns false if entity is not a ped or object. */
    export function getScriptOverrideEntityLootTablePermanent(entity: number, lootTable: number): boolean;
    export function isCarriableModel(model: number): boolean;
    /** Getter for FREEZE_ENTITY_POSITION */
    export function isEntityFrozen(entity: number): boolean;
    export function isEntityFullyLooted(entity: number): boolean;
    export function isEntityOnTrainTrack(entity: number): boolean;
    export function isEntityOwnedByPersistenceSystem(entity: number): boolean;
    /** Params: p1 (probably animType) = 1, 0 */
    export function isEntityPlayingAnyAnim(entity: number, p1: number): boolean;
    export function isEntityUnderwater(entity: number, p1: boolean): boolean;
    export function isTrackedEntityVisible(entity: number): boolean;
    export function pauseEntityTracking(entity: number, pause: boolean): void;
    export function requestEntityLootList(mount: number, argStruct: any, visiblelootslotrequestType: number, flag: number, p4: number, p5: boolean): boolean;
    /** Alternative Name: _GET_ENTITY_FROM_MAP_OBJECT; You can get existing objects and manipulate them using this native. */
    export function searchBuildingPoolForEntityWithThisModel(modelHash: number): number;
    /** https://gfycat.com/amazingmiserlyamericanquarterhorse */
    export function setEntityAnimCurrentTime(entity: number, animDict: string, animName: string, time: number): void;
    export function setEntityAnimSpeed(entity: number, animDict: string, animName: string, speedMultiplier: number): void;
    /** Changes type and quality of skins type hashes: https://pastebin.com/C1WvQjCy */
    export function setEntityCarcassType(entity: number, type: number): void;
    /** flagId: https://github.com/femga/rdr3_discoveries/tree/master/AI/CARRYING_FLAGS https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/CCarryingFlags__Flags  enum eCarryingFlag { 	CARRYIN... */
    export function setEntityCarryingFlag(entity: number, flagId: number, value: boolean): void;
    export function setEntityCoordsAndHeading(entity: number, xPos: number, yPos: number, zPos: number, heading: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): void;
    export function setEntityCoordsAndHeadingNoOffset(entity: number, xPos: number, yPos: number, zPos: number, heading: number, p5: boolean, p6: boolean): void;
    export function setEntityCustomPickupRadius(entity: number, radius: number): void;
    export function setEntityFadeIn(entity: number): void;
    export function setEntityFullyLooted(entity: number, looted: boolean): void;
    export function setEntityLightsEnabled(entity: number, enabled: boolean): void;
    /** tier: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eEntityThreatTier */
    export function setEntityThreatTier(entity: number, tier: number, p2: boolean): void;
    /** Sets a material fill level (e.g., stew, mug, chips, jugs). Params: p1: 0 or 2; name: depth/degree of fill label name; fillState: 0.0-1.0 (some up to 3.0) */
    export function setMaterialFillLevelForEntity(entity: number, expressionType: number, dofName: string, fillState: number): void;
    export function unpinMapEntity(entity: number): void;

    // EVENT
    /** eventType: https://alloc8or.re/rdr3/doc/enums/eEventType.txt https://github.com/femga/rdr3_discoveries/blob/master/AI/EVENTS */
    export function addShockingEventAtPosition(eventType: number, x: number, y: number, z: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: number, p10: number): number;
    /** eventType: https://alloc8or.re/rdr3/doc/enums/eEventType.txt */
    export function addShockingEventForEntity(eventType: number, entity: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: boolean, p9: boolean, p10: number, p11: number): number;
    /** eventType: https://alloc8or.re/rdr3/doc/enums/eEventType.txt */
    export function isShockingEventInSphere(eventType: number, x: number, y: number, z: number, radius: number): boolean;
    export function removeAllShockingEvents(p0: boolean): void;
    /** eventType: https://alloc8or.re/rdr3/doc/enums/eEventType.txt */
    export function removeAllShockingEventsOfType(eventType: number, p1: boolean): void;
    export function removeShockingEvent(event: number): boolean;
    export function removeShockingEventSpawnBlockingAreas(): void;
    export function setDecisionMaker(ped: number, name: number): void;
    export function setDecisionMakerToDefault(ped: number): void;
    export function suppressShockingEventsNextFrame(): void;
    /** Models used in the scripts: P_REGISTER05X, P_REGISTER06X, P_REGISTER03X, PLAYER_ZERO, PLAYER_THREE, A_C_HORSE_MORGAN_FLAXENCHESTNUT */
    export function addModelToEventMonitor(model: number, p1: boolean, p2: boolean): void;
    export function createShockingEvent(args: any): number;
    export function eventFlushAllEventTrackers(ped: number): void;
    /** Returns eventType */
    export function eventGetRecentEvent(entity: number, p1: number, p2: number): number;
    export function eventGetSourceEntityFromEvent(entity: number, eventType: number, p2: number, p3: number): number;
    export function eventGetTargetEntityFromEvent(entity: number, eventType: number, p2: number, p3: number): number;
    export function eventGetTimeSinceEvent(entity: number, eventType: number, p2: number, p3: number): number;
    export function isEventTrackerActive(eventName: string, shockingEvent: number): boolean;
    export function removeAllShockingEventsInArea(x: number, y: number, z: number, radius: number, p4: boolean): void;
    /** eventType: https://alloc8or.re/rdr3/doc/enums/eEventType.txt */
    export function removeAllShockingEventsOfTypeInArea(eventType: number, x: number, y: number, z: number, radius: number, p5: boolean): void;
    export function setEventTrackerForPed(ped: number, eventName: string, p2: number): void;

    // FIRE
    /** https://github.com/femga/rdr3_discoveries/tree/master/graphics/explosions  explosionType: enum eExplosionTag { 	EXP_TAG_DONTCARE = -1, 	EXP_TAG_GRENADE, 	EXP_TAG_STICKYBOMB, 	EXP_TAG_MOLOTOV, 	EXP_... */
    export function addExplosion(x: number, y: number, z: number, explosionType: number, damageScale: number, isAudible: boolean, isInvisible: boolean, cameraShake: number): void;
    /** Changes explosionFx (Visual Effect) for specified explosionType explosionType: see ADD_EXPLOSION explosionFx: https://github.com/femga/rdr3_discoveries/blob/master/graphics/explosions/explosion_vfx... */
    export function addExplosionWithUserVfx(x: number, y: number, z: number, explosionType: number, explosionFx: number, damageScale: number, isAudible: boolean, isInvisible: boolean, cameraShake: number): void;
    /** explosionType: see ADD_EXPLOSION */
    export function addOwnedExplosion(ped: number, x: number, y: number, z: number, explosionType: number, damageScale: number, isAudible: boolean, isInvisible: boolean, cameraShake: number): void;
    export function getClosestFirePos(outPosition: Vector3, x: number, y: number, z: number): boolean;
    export function getNumberOfFiresInRange(x: number, y: number, z: number, radius: number): number;
    /** explosionType: see ADD_EXPLOSION */
    export function getOwnerOfExplosionInAngledArea(explosionType: number, x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, radius: number): number;
    export function isEntityOnFire(entity: number): boolean;
    /** explosionType: see ADD_EXPLOSION */
    export function isExplosionActiveInArea(explosionType: number, x1: number, y1: number, z1: number, x2: number, y2: number, z2: number): boolean;
    /** explosionType: see ADD_EXPLOSION */
    export function isExplosionInAngledArea(explosionType: number, x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, angle: number): boolean;
    /** explosionType: see ADD_EXPLOSION */
    export function isExplosionInArea(explosionType: number, x1: number, y1: number, z1: number, x2: number, y2: number, z2: number): boolean;
    /** explosionType: see ADD_EXPLOSION */
    export function isExplosionInSphere(explosionType: number, x: number, y: number, z: number, radius: number): boolean;
    export function removeScriptFire(fireHandle: number): void;
    /** fireFlags: 2 = zone/env fire, 8 = scorched carcass. */
    export function startEntityFire(entity: number, intensity: number, boneIndex: number, fireFlags: number): void;
    /** Starts a fire:  xyz: Location of fire maxChildren: The max amount of times a fire can spread to other objects. Must be 25 or less, or the function will do nothing. isGasFire: Whether or not the fir... */
    export function startScriptFire(x: number, y: number, z: number, p3: number, p4: number, p5: boolean, soundsetName: string, p7: number, p8: number): number;
    export function stopEntityFire(p0: any, p1: any): void;
    export function stopFireInRange(x: number, y: number, z: number, radius: number): void;
    /** Adds an explosion with entity as damage causer. explosionType: see ADD_EXPLOSION _A* - _ADD_D* */
    export function addExplosionWithDamageCauser(entity: number, p1: number, x: number, y: number, z: number, explosionType: number, damageScale: number, isAudible: boolean, isInvisible: boolean, cameraShake: number): void;
    /** Adds an explosion with vfx and entity as damage causer. explosionFx: see ADD_EXPLOSION_WITH_USER_VFX _A* - _ADD_D* */
    export function addExplosionWithUserVfxAndDamageCauser(entity: number, p1: boolean, x: number, y: number, z: number, explosionType: number, explosionFx: number, damageScale: number, isAudible: boolean, isInvisible: boolean, cameraShake: number): void;
    export function getClosestFirePosInVolume(outPosition: Vector3, posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number): boolean;
    /** Returns true if entity is being damaged by fire, once damage caused to entity by fire (like burned appearance) has cleared over time, the native returns false. */
    export function isEntityBeingDamagedByFire(entity: number): boolean;
    export function isEntityConsumedByFire(entity: number): boolean;
    /** explosionType: see ADD_EXPLOSION */
    export function isExplosionInVolume(explosionType: number, volume: number): boolean;
    /** Tested with fire & dynamite. Only returns true using value p1 = 1 and when the ped is affected by fire. */
    export function isPedShockingEventActive(ped: number, p1: number): boolean;
    export function stopFireInBox(posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number): void;

    // FLOCK
    /** index: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eAnimalTuningBools https://github.com/femga/rdr3_discoveries/tree/master/AI/ANIMAL_TUNING_BOOL_PARAMS */
    export function getAnimalTuningBoolParam(animal: number, index: number): boolean;
    /** index: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eAnimalTuningFloats https://github.com/femga/rdr3_discoveries/tree/master/AI/ANIMAL_TUNING_FLOAT_PARAMS */
    export function getAnimalTuningFloatParam(animal: number, index: number): number;
    export function getSpeciesTuningFloatParam(p0: number, p1: number, p2: number): number;
    export function resetAnimalTuningBoolParam(animal: number, index: number): void;
    export function resetAnimalTuningFloatParam(animal: number, index: number): void;
    export function setAnimalTuningBoolParam(animal: number, index: number, value: boolean): void;
    export function setAnimalTuningFloatParam(animal: number, index: number, value: number): void;
    export function setSpeciesTuningBoolParam(p0: number, p1: number, p2: number, p3: boolean): void;
    export function setSpeciesTuningFloatParam(p0: number, p1: number, p2: number, p3: number): void;
    export function addPedToFlock(p0: any, ped: number): void;
    export function clearHerd(herdHandle: number): void;
    export function createHerd(): number;
    export function deleteHerd(herdHandle: number): void;
    /** Ped (horse) will run away from players and mounting will trigger them to buck until disabled. Used for: REL_DOMESTICATED_ANIMAL */
    export function getAnimalIsWild(ped: number): boolean;
    /** enum eAnimalRarityLevel { 	ARL_COMMON, 	ARL_RARE, 	ARL_LEGENDARY, 	ARL_NUMRARITYLEVELS }; */
    export function getAnimalRarity(ped: number): number;
    export function isHerdValid(herdHandle: number): boolean;
    /** _IS_E* - _IS_M* */
    export function isPedInHerd(herdHandle: number, ped: number): boolean;
    export function removeHerdPed(herdHandle: number, ped: number): void;
    export function setAnimalIsWild(ped: number, toggle: boolean): void;
    /** rarityLevel: see _GET_ANIMAL_RARITY */
    export function setAnimalRarity(ped: number, rarityLevel: number): void;

    // GANG
    export function networkGetGangId(player: number): any;
    export function networkGetGangLeader(gangId: any): number;
    export function networkGetNumGangMembers(gangId: any): number;
    export function networkIsGangActive(gangId: any): boolean;
    export function networkIsGangIdValid(gangId: any): boolean;
    export function networkIsGangInSession(gangId: any): boolean;
    export function networkIsGangLeader(player: number): boolean;
    export function networkIsInSameGang(player1: number, player2: number): boolean;
    export function networkGetGangLeaderHandle(gangId: any, gamerHandle: any): boolean;
    export function networkGetGangMembers(gangId: any, memberHandles: any): number;
    export function networkGetGangPrivacy(): number;
    export function networkGetGangSize(gangId: any): number;
    export function networkIsGangMember(gangId: any, player: number): boolean;
    export function networkIsGangOpen(gangId: any): boolean;
    export function networkIsInMyGang(player: number): boolean;
    /** banTimeSeconds is 120 in R* Scripts */
    export function networkKickGangMember(player: number, banTimeSeconds: number): void;
    export function networkLeaveGang(disband: boolean): void;
    /** Returns true if join succeeded, false if failed. */
    export function networkRequestGangJoin(gangId: any): boolean;
    export function networkSetGangPrivacy(privacyType: number): boolean;
    export function networkSetGangSize(size: number): boolean;
    /** openStatus = true -> sets privacyType = 2 (PUBLIC_ADVERTISED) openStatus = false -> sets privacyType = 1 (INVITE_ONLY)  campSize: NET_CAMP_SIZE_SMALLEST = 4, NET_CAMP_SIZE_LARGEST = 7 */
    export function networkStartGang(openStatus: boolean, campSize: number): void;

    // GOOGLE_ANALYTICS
    export function googleAnalyticsEndEvent(): boolean;
    export function googleAnalyticsPopPage(pageName: string): void;
    export function googleAnalyticsPushPage(pageName: string): void;
    export function googleAnalyticsStartEvent(eventCategory: string, eventAction: string, eventLabel: string, eventValue: number): boolean;

    // GRAPHICS
    export function addDecal(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any, p15: any, p16: any, p17: any, p18: any, p19: any, p20: any, p21: any): number;
    export function addPetrolTrailDecalInfo(x: number, y: number, z: number, p3: number): void;
    /** Returns veg modifier handle */
    export function addVegModifierSphere(x: number, y: number, z: number, radius: number, modType: number, flags: number, p6: number): number;
    export function allowPickupLightSync(pickupObject: number, allow: boolean): void;
    export function animpostfxHasEventTriggeredByStackhash(effectNameHash: number, eventType: number, bPeekOnly: boolean, bIsRegistered: boolean): boolean;
    export function animpostfxIsPreloadingByStackhash(effectNameHash: number): boolean;
    export function animpostfxIsRunning(effectName: string): boolean;
    /** https://github.com/femga/rdr3_discoveries/blob/master/graphics/animpostfx */
    export function animpostfxPlay(effectName: string): void;
    export function animpostfxStop(effectName: string): void;
    export function animpostfxStopAll(): void;
    export function attachTvAudioToEntity(entity: number): void;
    /** Called together with FREE_MEMORY_FOR_LOW_QUALITY_PHOTO */
    export function beginCreateLowQualityCopyOfPhoto(p0: number): boolean;
    export function beginTakeHighQualityPhoto(): boolean;
    export function blockPickupPlacementLight(pickup: number, toggle: boolean): void;
    export function cascadeShadowsClearShadowSampleType(): void;
    /** When this is set to ON, shadows only draw as you get nearer.  When OFF, they draw from a further distance. */
    export function cascadeShadowsEnableEntityTracker(toggle: boolean): void;
    export function cascadeShadowsSetCascadeBounds(p0: any, p1: boolean, p2: number, p3: number, p4: number, p5: number, p6: boolean, p7: number): void;
    /** Possible values: "CSM_ST_POINT" "CSM_ST_LINEAR" "CSM_ST_BOX3x3" "CSM_ST_BOX4x4" "CSM_ST_DITHER2_LINEAR" "CSM_ST_CUBIC" "CSM_ST_POISSON16" "CSM_ST_SOFT8" "CSM_ST_SOFT16" "CSM_ST_SOFT32" "CSM_ST_DITH... */
    export function cascadeShadowsSetShadowSampleType(type: string): void;
    export function clearTimecycleModifier(): void;
    export function createCheckpointWithNamehash(typeHash: number, posX1: number, posY1: number, posZ1: number, posX2: number, posY2: number, posZ2: number, radius: number, red: number, green: number, blue: number, alpha: number, reserved: number): number;
    /** Creates a tracked point, useful for checking the visibility of a 3D point on screen. */
    export function createTrackedPoint(): number;
    export function deleteCheckpoint(checkpoint: number): void;
    export function destroyTrackedPoint(point: number): void;
    export function disableEntitymask(): void;
    export function disableHdtexThisFrame(): void;
    export function doesParticleFxLoopedExist(ptfxHandle: number): boolean;
    export function drawLightWithRange(posX: number, posY: number, posZ: number, colorR: number, colorG: number, colorB: number, range: number, intensity: number): void;
    /** nullsub, doesn't do anything (GTA5 leftover, there is no phone in RDR3) */
    export function drawLowQualityPhotoToPhone(p0: boolean, photoRotation: number): void;
    /** Draws a rectangle on the screen.  -x: The relative X point of the center of the rectangle. (0.0-1.0, 0.0 is the left edge of the screen, 1.0 is the right edge of the screen)  -y: The relative Y poi... */
    export function drawRect(x: number, y: number, width: number, height: number, red: number, green: number, blue: number, alpha: number, p8: boolean, p9: boolean): void;
    /** Draws a 2D sprite on the screen.  Parameters: textureDict - Name of texture dictionary to load texture from  textureName - Name of texture to load from texture dictionary  screenX/Y - Screen offset... */
    export function drawSprite(textureDict: string, textureName: string, screenX: number, screenY: number, width: number, height: number, heading: number, red: number, green: number, blue: number, alpha: number, p11: boolean): void;
    export function drawTvChannel(xPos: number, yPos: number, xScale: number, yScale: number, rotation: number, red: number, green: number, blue: number, alpha: number): void;
    export function enableEntitymask(): void;
    /** Old name: _ENABLE_EXTRA_TIMECYCLE_MODIFIER_STRENGTH */
    export function enableMoonCycleOverride(strength: number): void;
    /** nullsub, doesn't do anything */
    export function enableMovieSubtitles(toggle: boolean): void;
    export function endPetrolTrailDecals(): void;
    export function freeMemoryForHighQualityPhoto(): void;
    export function freeMemoryForLowQualityPhoto(): void;
    export function freeMemoryForMissionCreatorPhoto(): void;
    export function getScreenCoordFromWorldCoord(worldX: number, worldY: number, worldZ: number, screenX: number, screenY: number): boolean;
    /** Hardcoded to always set x to 1280 and y to 720. */
    export function getScreenResolution(x: number, y: number): void;
    /** Old name: _GET_STATUS_OF_DRAW_LOW_QUALITY_PHOTO */
    export function getStatusOfCreateLowQualityCopyOfPhoto(p0: any): number;
    /** contentId: returned by NETWORK::_UGC_QUERY_GET_CREATOR_PHOTO(uVar0, 0, sParam3) */
    export function getStatusOfLoadMissionCreatorPhoto(contentId: string): number;
    /** 0 = succeeded 1 = getting status 2 = failed */
    export function getStatusOfSaveHighQualityPhoto(): number;
    /** 0 = succeeded 1 = getting status 2 = failed */
    export function getStatusOfSortedListOperation(): number;
    export function getStatusOfTakeHighQualityPhoto(): number;
    export function getTimecycleModifierIndex(): number;
    export function getTimecycleTransitionModifierIndex(): number;
    export function getTogglePausedRenderphasesStatus(): boolean;
    export function getTvChannel(): number;
    export function isDecalAlive(decal: number): boolean;
    export function isPhotoFrame(): boolean;
    export function isTrackedPointVisible(point: number): boolean;
    /** Old name: _IS_TV_PLAYLIST_ITEM_PLAYING */
    export function isTvshowCurrentlyPlaying(videoCliphash: number): boolean;
    export function loadMissionCreatorPhoto(p0: any, p1: any, p2: any, p3: any): boolean;
    export function pedshotIsAvailable(): boolean;
    export function queueOperationToCreateSortedListOfPhotos(): boolean;
    export function removeDecal(decal: number): void;
    export function removeDecalsFromObject(obj: number): void;
    /** Removes all decals in range from a position, it includes the bullet holes, blood pools, petrol... */
    export function removeDecalsInRange(x: number, y: number, z: number, range: number): void;
    export function removeGrassCullSphere(handle: number): void;
    export function removeParticleFx(ptfxHandle: number, p1: boolean): void;
    export function removeParticleFxFromEntity(entity: number): void;
    export function removeParticleFxInRange(X: number, Y: number, Z: number, radius: number): void;
    export function removeVegModifierSphere(vegModifierHandle: number, p1: number): void;
    /** Sets an unknown value related to timecycles. */
    export function resetAdaptation(unk: number): void;
    /** Resets the effect of SET_PARTICLE_FX_OVERRIDE */
    export function resetParticleFxOverride(name: string): void;
    export function resetPausedRenderphases(): void;
    export function saveHighQualityPhoto(unused: number): boolean;
    /** Does not affect weapons, particles, fire/explosions, flashlights or the sun. When set to true, all emissive textures (including ped components that have light effects), street lights, building ligh... */
    export function setArtificialLightsState(state: boolean): void;
    /** Sets the checkpoint color. */
    export function setCheckpointRgba(checkpoint: number, red: number, green: number, blue: number, alpha: number): void;
    /** Sets the checkpoint icon color. */
    export function setCheckpointRgba2(checkpoint: number, red: number, green: number, blue: number, alpha: number): void;
    export function setDisablePetrolDecalsIgnitingThisFrame(): void;
    /** Returns handle to be used with REMOVE_GRASS_CULL_SPHERE */
    export function setGrassCullSphere(x: number, y: number, z: number, p3: number, p4: number): number;
    /** Old name: _SET_HIDOF_ENV_BLUR_PARAMS */
    export function setHidofOverride(p0: boolean, p1: boolean, p2: number, p3: number, p4: number, p5: number): void;
    /** Related to Campfires. p1: AMB_BONFIRE_MP, AMB_CAMPFIRE_LRG_MP */
    export function setParticleFxAmbientColour(entity: number, p1: string, r: number, g: number, b: number): void;
    export function setParticleFxBulletImpactLodrangeScale(p0: number): void;
    export function setParticleFxBulletImpactScale(scale: number): void;
    export function setParticleFxFootLodrangeScale(p0: number): void;
    export function setParticleFxLoopedAlpha(ptfxHandle: number, alpha: number): void;
    export function setParticleFxLoopedColour(ptfxHandle: number, r: number, g: number, b: number, p4: boolean): void;
    export function setParticleFxLoopedEvolution(ptfxHandle: number, propertyName: string, amount: number, noNetwork: boolean): void;
    export function setParticleFxLoopedFarClipDist(ptfxHandle: number, range: number): void;
    export function setParticleFxLoopedOffsets(ptfxHandle: number, x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number): void;
    export function setParticleFxLoopedScale(ptfxHandle: number, scale: number): void;
    export function setParticleFxNonLoopedAlpha(alpha: number): void;
    export function setParticleFxNonLoopedColour(r: number, g: number, b: number): void;
    export function setParticleFxOverride(oldAsset: string, newAsset: string): void;
    /** https://imgur.com/a/I2swSDJ  Old name: _SET_PICKUP_OBJECT_GLOW_ENABLED */
    export function setPickupLight(object: number, toggle: boolean): void;
    /** Sets a flag defining whether or not script draw commands should continue being drawn behind the pause menu. This is usually used for draw commands that are used with a world render target. */
    export function setScriptGfxDrawBehindPausemenu(toggle: boolean): void;
    /** Sets the draw order for script draw commands. */
    export function setScriptGfxDrawOrder(drawOrder: number): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/graphics/timecycles */
    export function setTimecycleModifier(modifierName: string): void;
    export function setTimecycleModifierStrength(strength: number): void;
    export function setTrackedPointInfo(point: number, x: number, y: number, z: number, radius: number): void;
    export function setTransitionOutOfTimecycleModifier(strength: number): void;
    export function setTransitionTimecycleModifier(modifierName: string, transitionBlend: number): void;
    /** Probably changes tvs from being a 3d audio to being "global" audio */
    export function setTvAudioFrontend(toggle: boolean): void;
    export function setTvChannel(channel: number): void;
    export function setTvChannelPlaylist(tvChannel: number, playlistName: string, restart: boolean): void;
    export function setTvVolume(volume: number): void;
    export function startNetworkedParticleFxLoopedOnEntity(effectName: string, entity: number, xOffset: number, yOffset: number, zOffset: number, xRot: number, yRot: number, zRot: number, scale: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): number;
    export function startNetworkedParticleFxLoopedOnEntityBone(effectName: string, entity: number, xOffset: number, yOffset: number, zOffset: number, xRot: number, yRot: number, zRot: number, boneIndex: number, scale: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): number;
    export function startNetworkedParticleFxNonLoopedAtCoord(effectName: string, xPos: number, yPos: number, zPos: number, xRot: number, yRot: number, zRot: number, scale: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): boolean;
    export function startNetworkedParticleFxNonLoopedOnEntity(effectName: string, entity: number, offsetX: number, offsetY: number, offsetZ: number, rotX: number, rotY: number, rotZ: number, scale: number, axisX: boolean, axisY: boolean, axisZ: boolean): boolean;
    /** https://github.com/femga/rdr3_discoveries/blob/master/graphics/ptfx/ptfx_assets_looped.lua */
    export function startParticleFxLoopedAtCoord(effectName: string, x: number, y: number, z: number, xRot: number, yRot: number, zRot: number, scale: number, xAxis: boolean, yAxis: boolean, zAxis: boolean, p11: boolean): number;
    export function startParticleFxLoopedOnEntity(effectName: string, entity: number, xOffset: number, yOffset: number, zOffset: number, xRot: number, yRot: number, zRot: number, scale: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): number;
    export function startParticleFxLoopedOnEntityBone(effectName: string, entity: number, xOffset: number, yOffset: number, zOffset: number, xRot: number, yRot: number, zRot: number, boneIndex: number, scale: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): number;
    export function startParticleFxLoopedOnPedBone(effectName: string, ped: number, xOffset: number, yOffset: number, zOffset: number, xRot: number, yRot: number, zRot: number, boneIndex: number, scale: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): number;
    /** https://github.com/femga/rdr3_discoveries/blob/master/graphics/ptfx/ptfx_assets_non_looped.lua */
    export function startParticleFxNonLoopedAtCoord(effectName: string, xPos: number, yPos: number, zPos: number, xRot: number, yRot: number, zRot: number, scale: number, xAxis: boolean, yAxis: boolean, zAxis: boolean): boolean;
    export function startParticleFxNonLoopedOnEntity(effectName: string, entity: number, offsetX: number, offsetY: number, offsetZ: number, rotX: number, rotY: number, rotZ: number, scale: number, axisX: boolean, axisY: boolean, axisZ: boolean): boolean;
    export function startParticleFxNonLoopedOnPedBone(effectName: string, ped: number, offsetX: number, offsetY: number, offsetZ: number, rotX: number, rotY: number, rotZ: number, boneIndex: number, scale: number, axisX: boolean, axisY: boolean, axisZ: boolean): boolean;
    export function startPetrolTrailDecals(p0: any, p1: any): void;
    export function stopParticleFxLooped(ptfxHandle: number, p1: boolean): void;
    export function togglePausedRenderphases(toggle: boolean): void;
    export function updateLightsOnEntity(entity: number): void;
    /** fxName: see data_0/data/effects/ptfx/fxlists/ */
    export function useParticleFxAsset(fxName: string): void;
    /** https://i.imgur.com/ULQU9US.jpg More rounded and small puddle */
    export function addBloodPool(x: number, y: number, z: number, unused: boolean): void;
    /** Creates blood pools for the given ped in some interval for a few seconds. */
    export function addBloodPoolsForPed(ped: number): void;
    export function addBloodPoolsForPedWithParams(ped: number, p1: number, size: number, p3: number): void;
    /** https://i.imgur.com/rPITUCV.jpg More customizable and more like quadrants */
    export function addBloodPool2(x: number, y: number, z: number, p3: number, size: number, p5: number, permanent: boolean, p7: number, p8: boolean): void;
    export function addBloodTrailPoint(x: number, y: number, z: number): void;
    export function addBloodTrailSplat(x: number, y: number, z: number): void;
    export function addEntityToEntityMask(entity: number, mask: number): void;
    export function addEntityToEntityMaskWithIntensity(entity: number, mask: number, intensity: number): void;
    /** Adds Vegetation Blocking Zone, Added Snow Flattening veg mod Zone Returns veg modifier handle */
    export function addVegModifierZone(volume: number, p1: number, flags: number, p3: number): number;
    export function animpostfxClearEffect(effectName: string): void;
    /** Known effects: MP_Trans_SceneToPhoto MP_Trans_WinLose SpectateFilter MP_CharacterCreatorPhoto MP_Trans_PhotoToScene InterrogationHit */
    export function animpostfxGetStackhash(effectName: string): number;
    export function animpostfxHasLoaded(effectName: string): boolean;
    export function animpostfxIsStackhashPlaying(effectNameHash: number): boolean;
    export function animpostfxIsTagPlaying(effectName: string): boolean;
    export function animpostfxPlayTag(effectNameHash: number): void;
    export function animpostfxPlayTimed(effectName: string, duration: number): void;
    export function animpostfxPreloadPostfx(effectName: string): void;
    export function animpostfxPreloadPostfxByStackhash(effectNameHash: number): void;
    export function animpostfxSetPostfxColor(effectName: string, p1: number, red: number, green: number, blue: number, alpha: number): void;
    /** Health Core Effect Filter Potency: p1 = 1 Stamina Core Effect Filter Potency: p1 = 2 Multiple Core Effect Filter Potency: p1 = 3 */
    export function animpostfxSetPotency(effectName: string, p1: number, potency: number): void;
    /** must be called after ANIMPOSTFX_PLAY, strength 0.0f - 1.0f */
    export function animpostfxSetStrength(effectName: string, strength: number): void;
    export function animpostfxSetToUnload(effectName: string): void;
    export function animpostfxStopStackhashPostfx(effectNameHash: number): void;
    export function animpostfxStopTag(effectName: string): void;
    export function blockPickupObjectLight(pickupObject: number, toggle: boolean): void;
    /** p1: 0.3f in R* Scripts */
    export function bloodTrailForWaypoint(waypointRecording: string, p1: number): void;
    export function changePhotoModeContrast(value: number): void;
    export function changePhotoModeExposure(value: number): void;
    export function createSwatchTextureDict(slots: number): boolean;
    export function destroySwatchTextureDict(): void;
    /** Only used in guama1 R* Script Disables lod/distant lights when BOOL is set to true */
    export function disableFarArtificialLights(disable: boolean): void;
    export function disableStaticVegModifier(p0: number): void;
    export function doesCheckpointHaveFx(checkpoint: number): boolean;
    /** https://github.com/femga/rdr3_discoveries/blob/master/graphics/markers/marker_types.lua */
    export function drawMarker(type: number, posX: number, posY: number, posZ: number, dirX: number, dirY: number, dirZ: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number, red: number, green: number, blue: number, alpha: number, bobUpAndDown: boolean, faceCamera: boolean, p19: number, rotate: boolean, textureDict: string, textureName: string, drawOnEnts: boolean): void;
    export function enableStaticVegModifier(p0: number): void;
    /** Example: local hash = GetHashKey("CLOTHING_ITEM_M_EYES_001_TINT_001") _GENERATE_SWATCH_TEXTURE(0, hash, 0, true) metapedType: see 0xEC9A1261BF0CE510 */
    export function generateSwatchTexture(slotId: number, componentHash: number, metapedType: number, p3: boolean): void;
    /** Example: https://pastebin.com/tTgpER9A */
    export function generateSwatchTextureDirectly(slot: number, p1: any): void;
    export function getCurrentNumberOfLocalPhotos(): number;
    export function getEntityMaskLayers(entity: number, layer0: number, layer1: number, layer2: number, layer3: number): boolean;
    /** Always returns 200. */
    export function getMaxNumberOfLocalPhotos(): number;
    /** _GET_C* - _GET_E* */
    export function getModifiedVisibilityDistance(): number;
    export function getPhotoModeContrast(): number;
    export function getPhotoModeExposure(): number;
    /** Returns proxyInteriorIndex */
    export function getProxyInteriorIndex(interiorId: number): number;
    export function isProxyInteriorIndexArtificialLightsEnabled(proxyInteriorIndex: number): boolean;
    export function isStaticVegModifierEnabled(p0: number): boolean;
    export function isTextureInDict(txdHash: number, dict: number): boolean;
    export function isTrackedPointValid(point: number): boolean;
    /** Returns iNumPixels, iPixelsVisible */
    export function numPixelsVisibleAtTrackedPoint(iTrackedPoint: number): number;
    export function pedshotFinishCleanupData(): void;
    export function pedshotGeneratePersonaPhoto(texture: string, ped: number, playerSlot: number): boolean;
    export function pedshotInitCleanupData(): void;
    export function pedshotPreviousPersonaPhotoDataCleanup(): void;
    export function pedshotSetPersonaPhotoType(personaPhotoLocalCacheType: number): void;
    export function removeEntityFromEntityMask(entity: number): void;
    /** Used for script function RPG_GLOBAL_STATS__PRIVATE__DEACTIVATE_STAT_FLAG - Inspiration Aura unequip */
    export function resetEntityAura(): void;
    export function setCloudHeight(height: number): void;
    export function setCloudLayer(x: number, y: number, p2: number): void;
    export function setCloudNoise(x: number, y: number, z: number): void;
    /** Only used in finale2, smuggler2, winter4 _SET_CLOUD_A* - _SET_CLOUD_H* */
    export function setCloudPosition(x: number, y: number, z: number): void;
    export function setDistrictPhotoTakenStat(p0: string): void;
    /** Used for script function RPG_GLOBAL_STATS__PRIVATE__ACTIVATE_STAT_FLAG - Quite and Inspiration Aura equip Params: 0f, 2f, 2f */
    export function setEntityAura(p0: number, p1: number, p2: number): void;
    export function setEntityMaskLayers(entity: number, layer0: number, layer1: number, layer2: number, layer3: number): void;
    /** Only used in guama1 R* SP Script while spawning the ship _SET_ENTITY_QUATERNION_* - SET_ENTITY_RENDER_* */
    export function setEntityRenderGuarmaShip(vehicle: number, toggle: boolean): void;
    /** https://gfycat.com/meagerfaireyra */
    export function setLightsColorForEntity(entity: number, red: number, green: number, blue: number): void;
    export function setLightsIntensityForEntity(entity: number, intensity: number): void;
    /** type must be less than or equal to 20 */
    export function setLightsTypeForEntity(entity: number, type: number): void;
    /** _SET_PARTICLE_FX_LOOPED_FA* - _SET_PARTICLE_FX_LOOPED_OF* */
    export function setParticleFxLoopedUpdateDistantSmoke(ptfxHandle: number, scalar: number): void;
    export function setParticleFxNonLoopedEmitterScale(p0: number, p1: number, p2: number): void;
    /** Enables/disables a kind of 'shiny' effect on metals. */
    export function setPearlescentFxEnabled(object: number, toggle: boolean): void;
    export function setPhotoInPhotomodeStat(p0: boolean): void;
    export function setPhotoModeExposureLocked(locked: boolean): void;
    export function setPhotoOverlayEffectStat(p0: number): void;
    export function setPhotoSelfStat(p0: boolean): void;
    export function setPhotoStudioStat(p0: number): void;
    export function setPlayerAppearInPhoto(player: number): void;
    export function setPosseIdForPhoto(posseId: any): void;
    /** state: false disables artificial interior light sources for specific proxyInteriorIndex */
    export function setProxyInteriorIndexArtificialLightsState(proxyInteriorIndex: number, state: boolean): void;
    export function setRegionPhotoTakenStat(p0: string): void;
    export function setSniperGlintsEnabled(enabled: boolean): void;
    /** enum class eSnowCoverageType { 	Primary, 	Secondary, 	Xmas, 	XmasSecondary // since b1232 }; */
    export function setSnowCoverageType(type: number): void;
    export function setStatePhotoTakenStat(p0: string): void;
    export function startParticleFxNonLoopedOnPedBone2(effectName: string, ped: number, offsetX: number, offsetY: number, offsetZ: number, rotX: number, rotY: number, rotZ: number, boneIndex: number, scale: number, axisX: boolean, axisY: boolean, axisZ: boolean): boolean;
    /** Resets the exposure to the value when exposure lock was enabled */
    export function updatePhotoModeExposure(): void;

    // HUD
    /** Old name: _ALLOW_PAUSE_MENU_WHEN_DEAD_THIS_FRAME */
    export function allowPauseWhenNotInStateOfPlayThisFrame(): void;
    export function busyspinnerIsOn(): boolean;
    /** Removes the loading prompt at the bottom right of the screen. */
    export function busyspinnerOff(): void;
    export function clearAllHelpMessages(): void;
    export function createFakeMpGamerTag(ped: number, username: string, pointedClanTag: boolean, isRockstarClan: boolean, clanTag: string, clanFlag: number): number;
    export function disableFrontendThisFrame(): void;
    /** If Hud should be displayed */
    export function displayHud(toggle: boolean): void;
    /** Checks if the passed gxt name exists in the game files. */
    export function doesTextLabelExist(label: string): boolean;
    /** Note: you must use VAR_STRING. Byte code very similar to TEXT_COMMAND_DISPLAY_TEXT in V Old name: _GET_TEXT_SUBSTRING */
    export function getCharacterFromAudioConversationFilename(text: string, position: number, length: number): NativeString;
    /** Gets a string literal from a label name.  Old name: _GET_LABEL_TEXT */
    export function getFilenameForAudioConversation(labelName: string): NativeString;
    export function getHudScreenPositionFromWorldPosition(worldX: number, worldY: number, worldZ: number, screenX: number, screenY: number): number;
    /** Returns the length of the string passed (much like strlen). */
    export function getLengthOfLiteralString(string: string): number;
    export function getLengthOfLiteralStringInBytes(string: string): number;
    export function getNamedRendertargetRenderId(name: string): number;
    /** Returns the label text given the hash.  Old name: _GET_LABEL_TEXT_BY_HASH */
    export function getStringFromHashKey(labelHash: number): NativeString;
    export function hideHudAndRadarThisFrame(): void;
    export function hideLoadingOnFadeThisFrame(): void;
    export function isHudHidden(): boolean;
    export function isMpGamerTagActive(gamerTagId: number): boolean;
    export function isNamedRendertargetLinked(modelHash: number): boolean;
    export function isNamedRendertargetRegistered(name: string): boolean;
    /** Returns true when either Pause Menu, a Frontend Menu, Online Policies menu or Social Club menu is active. */
    export function isPauseMenuActive(): boolean;
    export function isRadarHidden(): boolean;
    export function isRadarHiddenByScript(): boolean;
    export function isRadarPreferenceSwitchedOn(): boolean;
    export function isSubtitlePreferenceSwitchedOn(): boolean;
    export function linkNamedRendertarget(modelHash: number): void;
    export function registerNamedRendertarget(name: string, p1: boolean): boolean;
    export function releaseNamedRendertarget(name: string): boolean;
    export function removeMpGamerTag(gamerTagId: number): void;
    export function setFrontendActive(active: boolean): void;
    export function setMissionName(p0: boolean, name: string): void;
    export function setMissionNameForUgcMission(p0: boolean, name: string): void;
    export function setMpGamerTagBigText(gamerTagId: number, string: string): void;
    export function setMpGamerTagName(gamerTagId: number, string: string): void;
    export function setTextRenderId(renderId: number): void;
    export function textBlockIsLoaded(textBlock: string): boolean;
    export function textBlockRequest(textBlock: string): void;
    export function uiGetSceneUiobject(p0: any): any;
    export function uiMovieviewSetRenderTarget(p0: any, p1: any): void;
    export function uiPromptIsControlActionActive(controlAction: number): boolean;
    export function uiRequestScene(p0: any, p1: any): any;
    export function busyspinnerSetText(text: string): void;
    export function createMpGamerTag(player: number, username: string, pointedClanTag: boolean, isRockstarClan: boolean, clanTag: string, clanFlag: number): number;
    export function createMpGamerTagOnEntity(entity: number, text: string): number;
    /** Old name: _DISPLAY_HUD_COMPONENT */
    export function disableHudContext(component: number): void;
    /** Disables reduced time scale while menus such as weapon wheel and satchel are open. */
    export function disableReducedMenuTimeScale(): void;
    /** nullsub, this native does nothing since build 1436, use _BG_DISPLAY_TEXT (0x16794E044C9EFB58) instead. */
    export function displayText(text: string, xPos: number, yPos: number): void;
    export function doesTextBlockExist(textDatabase: string): boolean;
    /** https://github.com/femga/rdr3_discoveries/tree/master/graphics/HUD/hud_presets Old name: _HIDE_HUD_COMPONENT */
    export function enableHudContext(component: number): void;
    export function enableHudContextThisFrame(component: number): void;
    /** Enables reduced time scale while menus such as weapon wheel and satchel are open. */
    export function enableReducedMenuTimeScale(): void;
    /** colorNameHash: https://alloc8or.re/rdr3/doc/enums/eColor.txt */
    export function getColorFromName(colorNameHash: number, red: number, green: number, blue: number, alpha: number): void;
    /** Returns the current state value for a HUD component slot ID. Enum: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eHudVisibilitySlotType  Notes: - Component-specific semantics: th... */
    export function getHudVisibilitySlotState(hudSlot: number): number;
    /** _GET_FILENAME_* - _GET_FRAME* */
    export function getLabelText2(label: string): NativeString;
    /** Similar to 0x9D7E12EC6A1EE4E5(GET_TEXT_SUBSTRING) but starts at the beginning of the string _GET_FILE* - _GET_FRAME* */
    export function getTextSubstring2(text: string, length: number): NativeString;
    /** _GET_BOUNTY* - _GET_CHARACTER* */
    export function getTextSubstring3(text: string, begin: number, length: number): NativeString;
    /** Returns closest horse entity handle (about 3 meters; facing, directly riding, etc). Maybe when horse hud interaction prompts are allowed to show (?)  Params: p0 is usually true, if its false the na... */
    export function hudCheckClosestHorse(p0: boolean): number;
    /** Returns the hash of the currently highlighted item in the weapon wheel. Only works while the wheel is open.  Use in conjunction with IS_CONTROL_JUST_RELEASED(0, 'INPUT_OPEN_WHEEL_MENU') to detect i... */
    export function hudGetInventoryWheelCurrentlyHighlighted(): number;
    export function hudHideThisFrame(): void;
    export function isMpGamerTagActiveOnEntity(gamerTagId: number, entity: number): boolean;
    export function journalCanWriteEntry(p0: any): any;
    export function journalClearAllProgress(): void;
    export function journalGetEntryAtIndex(p0: any): any;
    export function journalGetEntryCount(): any;
    export function journalGetEntryInfo(p0: any, p1: any): any;
    export function journalGetGrimeAtIndex(p0: any): any;
    export function journalGetTextureWithLayout(p0: any, p1: any, p2: any): any;
    export function journalMarkRead(p0: any): void;
    export function journalWriteEntry(p0: any): void;
    export function mpGamerTagDisableReviveTopIcon(gamerTagId: number): void;
    export function mpGamerTagEnableReviveTopIcon(gamerTagId: number): void;
    /** string1 is the only string used in the scripts, the others are null (0) */
    export function setCurrentUgcMissionDescription(active: boolean, string1: string, string2: string, string3: string, string4: string): void;
    export function setMpGamerTagColour(gamerTagId: number, colour: number): void;
    export function setMpGamerTagNamePosse(gamerTagId: number, text: string): void;
    /** Found icons: SPEAKER, THROPY */
    export function setMpGamerTagSecondaryIcon(gamerTagId: number, icon: number): void;
    /** Found icons: https://pastebin.com/xx6rEgiG */
    export function setMpGamerTagTopIcon(gamerTagId: number, icon: number): void;
    /** Found types: GENERIC_PLAYER, DEADDROP, HOTPROPERTY, MINIGAMES */
    export function setMpGamerTagType(gamerTagId: number, type: number): void;
    /** nullsub, doesn't do anything */
    export function setMpGamerTagUnkAllowLocalized(gamerTagId: number, allow: boolean): void;
    /** visibility: enum eUIGamertagVisibility { 	UIGAMERTAGVISIBILITY_NONE, 	UIGAMERTAGVISIBILITY_ICON, 	UIGAMERTAGVISIBILITY_SIMPLE, 	UIGAMERTAGVISIBILITY_COMPLEX }; */
    export function setMpGamerTagVisibility(gamerTagId: number, visibility: number): void;
    /** This native does nothing since build 1436, use _BG_SET_TEXT_COLOR (0x16FA5CE47F184F1E) instead. */
    export function setTextColor(r: number, g: number, b: number, a: number): void;
    export function showHorseCores(state: boolean): void;
    export function showPlayerCores(state: boolean): void;
    export function textBlockDelete(textBlock: string): void;
    export function textBlockIsStreamed(textBlock: string): boolean;
    export function uiPromptAddGroupLink(p0: any, prompt: number, p2: any): void;
    export function uiPromptAddGroupReturnLink(p0: any, prompt: number): void;
    /** id is the return value from 0xD9459157EB22C895. */
    export function uiPromptClearHorizontalOrientation(id: number): void;
    export function uiPromptClearPromptPriorityPreference(): void;
    export function uiPromptContextSetPoint(prompt: number, x: number, y: number, z: number): void;
    export function uiPromptContextSetRadius(prompt: number, radius: number): void;
    /** Attaches a Volume */
    export function uiPromptContextSetVolume(prompt: number, volume: number): void;
    export function uiPromptCreate(inputHash: number, labelName: string, p2: any, p3: any, p4: any, p5: number): number;
    export function uiPromptDelete(prompt: number): void;
    export function uiPromptDisablePromptsThisFrame(): void;
    export function uiPromptDisablePromptTypeThisFrame(p0: number): void;
    export function uiPromptDoesAmbientGroupExist(hash: number): boolean;
    /** https://github.com/femga/rdr3_discoveries/tree/master/graphics/HUD/prompts/prompt_types */
    export function uiPromptEnablePromptTypeThisFrame(p0: number): void;
    export function uiPromptFilterClear(): void;
    export function uiPromptGetGroupActivePage(hash: number): number;
    export function uiPromptGetGroupIdForScenarioPoint(p0: any, p1: number): number;
    export function uiPromptGetGroupIdForTargetEntity(entity: number): number;
    export function uiPromptGetMashModeProgress(prompt: number): number;
    export function uiPromptGetProgress(prompt: number): number;
    export function uiPromptGetUrgentPulsingEnabled(prompt: number): boolean;
    export function uiPromptHasHoldAutoFillMode(prompt: number): boolean;
    export function uiPromptHasHoldMode(prompt: number): boolean;
    export function uiPromptHasHoldModeCompleted(prompt: number): boolean;
    export function uiPromptHasManualMashMode(prompt: number): boolean;
    export function uiPromptHasMashMode(prompt: number): boolean;
    export function uiPromptHasMashModeCompleted(prompt: number): boolean;
    export function uiPromptHasMashModeFailed(prompt: number): boolean;
    export function uiPromptHasMashModeJustPressed(prompt: number): boolean;
    export function uiPromptHasPressedTimedModeCompleted(prompt: number): boolean;
    export function uiPromptHasPressedTimedModeFailed(prompt: number): boolean;
    /** Params: p1 is 0 */
    export function uiPromptHasStandardModeCompleted(prompt: number, p1: number): boolean;
    export function uiPromptIsActive(prompt: number): boolean;
    export function uiPromptIsEnabled(prompt: number): boolean;
    export function uiPromptIsHoldModeRunning(prompt: number): boolean;
    export function uiPromptIsJustPressed(prompt: number): boolean;
    export function uiPromptIsJustReleased(prompt: number): boolean;
    export function uiPromptIsPressed(prompt: number): boolean;
    export function uiPromptIsReleased(prompt: number): boolean;
    export function uiPromptIsValid(prompt: number): boolean;
    export function uiPromptRegisterBegin(): number;
    export function uiPromptRegisterEnd(prompt: number): void;
    export function uiPromptRemoveGroup(prompt: number, p1: any): void;
    export function uiPromptRestartModes(prompt: number): void;
    /** Note: you must use VAR_STRING for p1 if string is not part of text database tabAmount: specifies number of tabs in prompt group tabDefaultIndex: specifies starting index p3 if is set > 3 you can no... */
    export function uiPromptSetActiveGroupThisFrame(hash: number, name: string, tabAmount: number, tabDefaultIndex: number, p4: number, prompt: number): any;
    export function uiPromptSetAllowedAction(prompt: number, action: number): void;
    export function uiPromptSetAmbientGroupThisFrame(entity: number, p1: number, p2: number, p3: number, p4: number, name: string, p6: number): any;
    /** attribute: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eUIPromptAttribute */
    export function uiPromptSetAttribute(prompt: number, attribute: number, enabled: boolean): void;
    export function uiPromptSetBeatMode(prompt: number, toggle: boolean): void;
    export function uiPromptSetBeatModeGrayedOut(prompt: number, p1: any): void;
    export function uiPromptSetControlAction(prompt: number, action: number): any;
    export function uiPromptSetEnabled(prompt: number, toggle: boolean): void;
    /** tabIndex: specifies tab of prompt */
    export function uiPromptSetGroup(prompt: number, groupId: number, tabIndex: number): void;
    export function uiPromptSetHoldAutoFillMode(prompt: number, autoFillTimeMs: number, holdTimeMs: number): void;
    export function uiPromptSetHoldAutoFillWithDecayMode(prompt: number, autoFillTimeMs: number, holdTimeMs: number): void;
    export function uiPromptSetHoldIndefinitelyMode(prompt: number): void;
    /** Params: p2 is 304000 in R* SP Script coachrobberies */
    export function uiPromptSetHoldMode(prompt: number, holdTimeMs: number): void;
    export function uiPromptSetManualResolved(prompt: number, p1: any): void;
    export function uiPromptSetMashAutoFillMode(prompt: number, autoFillTimeMs: number, mashes: number): void;
    export function uiPromptSetMashIndefinitelyMode(prompt: number): void;
    export function uiPromptSetMashManualCanFailMode(prompt: number, p1: number, p2: number, p3: number, p4: any): void;
    export function uiPromptSetMashManualMode(prompt: number, p1: number, p2: number, p3: number, p4: any): void;
    /** standard (prompt not held) rate: 0.035f fast (prompt held) rate: 0.015f punitive (been hit) rate: 0.14f */
    export function uiPromptSetMashManualModeDecaySpeed(prompt: number, speed: number): void;
    /** standard (prompt not held) rate: (1f / 128f) fast (prompt held) rate: (1f / 64f) punitive (been hit) rate: (1f / 128f) */
    export function uiPromptSetMashManualModeIncreasePerPress(prompt: number, rate: number): void;
    export function uiPromptSetMashManualModePressedGrowthSpeed(prompt: number, speed: number): void;
    export function uiPromptSetMashMode(prompt: number, mashes: number): void;
    /** Sets the mode for the given prompt to mash mode. decreaseSpeed: 0.0f will result in the prompt not showing the mash progress at all. 0.01f - ?.0f. At speeds around 7.0f to 8.0f the prompt basically... */
    export function uiPromptSetMashWithResistanceCanFailMode(prompt: number, mashes: number, decreaseSpeed: number, startProgress: number): void;
    export function uiPromptSetMashWithResistanceMode(prompt: number, mashes: number, p2: number, p3: number): void;
    export function uiPromptSetOrderingAsInputType(prompt: number, p1: any): void;
    export function uiPromptSetPressedTimedMode(prompt: number, depletionTimeMs: number): void;
    /** priority: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/ePromptPriority */
    export function uiPromptSetPriority(prompt: number, priority: number): void;
    export function uiPromptSetPromptPriorityPreference(ped: number): void;
    /** This returns an id that can be used with 0x6095358C4142932A. */
    export function uiPromptSetRegisterHorizontalOrientation(): number;
    /** Used for controllers */
    export function uiPromptSetRotateMode(prompt: number, p1: number, counterclockwise: boolean): void;
    export function uiPromptSetSpinnerPosition(prompt: number, p1: any): void;
    export function uiPromptSetSpinnerSpeed(prompt: number, p1: any): void;
    /** holdType: SHORT_TIMED_EVENT_MP, SHORT_TIMED_EVENT, MEDIUM_TIMED_EVENT, LONG_TIMED_EVENT, RUSTLING_CALM_TIMING, PLAYER_FOCUS_TIMING, PLAYER_REACTION_TIMING */
    export function uiPromptSetStandardizedHoldMode(prompt: number, holdType: number): void;
    export function uiPromptSetStandardMode(prompt: number, releaseMode: boolean): void;
    export function uiPromptSetTag(prompt: number, p1: any): void;
    export function uiPromptSetTargetMode(prompt: number, p1: number, p2: number, p3: any): void;
    export function uiPromptSetTargetModeProgress(prompt: number, progress: number): void;
    export function uiPromptSetTargetModeTarget(prompt: number, p1: number, p2: number): void;
    export function uiPromptSetText(prompt: number, text: string): void;
    /** TM_ANY = 0, TM_ON_FOOT, TM_IN_VEHICLE */
    export function uiPromptSetTransportMode(prompt: number, mode: number): void;
    /** Params: type = mostly 0, 6 (net_mission_intro_story_gvo), 7 (fm_mission_controller), 14 (net_ugc_end_flow_transition_online), 15 (net_main_[tlg_]offline) */
    export function uiPromptSetType(prompt: number, type: number): void;
    export function uiPromptSetUrgentPulsingEnabled(prompt: number, toggle: boolean): void;
    export function uiPromptSetVisible(prompt: number, toggle: boolean): void;
    export function uiPromptWasBeatModePressedInTimeWindow(prompt: number): boolean;

    // IK
    export function inverseKinematicsRequestLookAt(ped: number, args: any): void;
    /** Seems to disable IK on ped */
    export function inverseKinematicsSetDisabledForPed(ped: number, p1: number, p2: boolean): void;

    // INTERACTION
    /** Changes the mouse cursor's sprite.  spriteId's: https://github.com/femga/rdr3_discoveries/tree/master/graphics/HUD/cursor_sprites#readme  Old name: _SET_MOUSE_CURSOR_SPRITE */
    export function setMouseCursorStyle(spriteId: number): void;
    /** Shows the cursor on screen for one frame.  Old name: _SET_MOUSE_CURSOR_ACTIVE_THIS_FRAME */
    export function setMouseCursorThisFrame(): void;
    /** Returns true if player is moving mouse while cursor is active _PI* - _PO* */
    export function pointerIsBeingMoved(): boolean;
    /** Returns true if player is holding LMB while cursor is active _PI* - _PO* */
    export function pointerIsLeftButtonHeld(): boolean;
    /** Returns true if player releases LMB if cursor is active _PI* - _PO* */
    export function pointerIsLeftButtonJustReleased(): boolean;
    /** Allows camera to be moved if middle mouse button is held while in first person Must be called every frame _SET* */
    export function setAllowFirstPersonMouseCameraMovement(): void;

    // INTERIOR
    /** https://github.com/femga/rdr3_discoveries/tree/master/interiors/interior_sets */
    export function activateInteriorEntitySet(interior: number, entitySetName: string, p2: number): void;
    export function clearRoomForEntity(entity: number): void;
    export function clearRoomForGameViewport(): void;
    export function deactivateInteriorEntitySet(interior: number, entitySetName: string, p2: boolean): void;
    export function disableInterior(interior: number, toggle: boolean): void;
    export function forceRoomForEntity(entity: number, interior: number, roomHashKey: number): void;
    export function forceRoomForGameViewport(interiorID: number, roomHashKey: number): void;
    export function getInteriorAtCoords(x: number, y: number, z: number): number;
    export function getInteriorAtCoordsWithType(x: number, y: number, z: number, interiorType: string): number;
    /** Hashed version of GET_INTERIOR_AT_COORDS_WITH_TYPE */
    export function getInteriorAtCoordsWithTypehash(x: number, y: number, z: number, typeHash: number): number;
    export function getInteriorFromCollision(x: number, y: number, z: number): number;
    /** Returns the handle of the interior that the entity is in. Returns 0 if outside. */
    export function getInteriorFromEntity(entity: number): number;
    export function getInteriorFromPrimaryView(): number;
    export function getInteriorLocationAndNamehash(interior: number, position: Vector3, nameHash: number): void;
    /** Seems to do the exact same as INTERIOR::GET_ROOM_KEY_FROM_ENTITY */
    export function getKeyForEntityInRoom(entity: number): number;
    /** Gets the room hash key from the room that the specified entity is in. Each room in every interior has a unique key. Returns 0 if the entity is outside. */
    export function getRoomKeyFromEntity(entity: number): number;
    /** Returns true if the collision at the specified coords is marked as being outside (false if there's an interior) */
    export function isCollisionMarkedOutside(x: number, y: number, z: number): boolean;
    export function isInteriorEntitySetActive(interior: number, entitySetName: string): boolean;
    export function isInteriorReady(interior: number): boolean;
    export function isInteriorScene(): boolean;
    export function isValidInterior(interior: number): boolean;
    export function pinInteriorInMemory(interior: number): void;
    export function retainEntityInInterior(entity: number, interior: number): void;
    /** Actually returns void in IDA but the script header defines a BOOL return type */
    export function setInteriorInUse(interior: number): boolean;
    /** Does something similar to INTERIOR::DISABLE_INTERIOR.  You don't fall through the floor but everything is invisible inside and looks the same as when INTERIOR::DISABLE_INTERIOR is used. Peds behave... */
    export function unpinInterior(interior: number): void;
    export function getInteriorMinimapHash(interior: number): number;
    export function getInteriorPosition(interior: number): Vector3;
    export function isInteriorEntitySetValid(interior: number, entitySetName: string): boolean;

    // INVENTORY
    export function inventoryCopyMpInventoryToMissionInventory(p0: boolean, p1: boolean, bCopySatchelItems: boolean, bCopyEmotes: boolean, bCopyHorse: boolean, p5: boolean): void;
    export function inventoryDisableMissionInventoryPickups(): void;
    export function inventoryGetChildrenInSlotCount(inventoryId: number, guid: any, slotId: number): number;
    export function inventoryGetGuidFromItemid(inventoryId: number, guid: any, p2: number, slotId: number, outGuid: any): boolean;
    export function inventoryGetInventoryItem(inventoryId: number, inData: any, outData: any, p3: boolean): boolean;
    /** p1: WARDROBE, KIT_CAMP, CHARACTER, KIT_MOONSHINER_PROPERTY Returns slot hash */
    export function getDefaultItemSlotInfo(item: number, p1: number): number;
    export function getItemRoleMaxLevelCount(inventoryId: number, eRoleMaxLevel: number): number;
    export function getItemSlotMaxCount(provision: number, slotId: number): number;
    /** inventoryItemSlotHash: https://pastebin.com/P6fyr3vr */
    export function inventoryAddItemWithGuid(inventoryId: number, guid1: any, guid2: any, item: number, inventoryItemSlot: number, p5: number, addReason: number): boolean;
    /** Applies weapon stats to the 'CatalogItemInspection' stats entry id. p0: value returned by 0x9D21B185ABC2DBC5 _INVENTORY_GET* */
    export function inventoryApplyWeaponStatsToEntry(databindingEntryId: number, p1: boolean, ped: number): void;
    /** inventoryId: see _INVENTORY_GET_PED_INVENTORY_ID */
    export function inventoryAreLocalChangesAllowed(inventoryId: number): boolean;
    export function inventoryCompareGuids(guid1: any, guid2: any): boolean;
    export function inventoryCopyItemToInventory(inventoryId: number, inventoryIdCloned: number, p2: any, p3: any): void;
    export function inventoryCopyItemToMissionInventory(guid: any, p1: boolean): void;
    /** data: return value of DATABINDING::_DATABINDING_ADD_DATA_CONTAINER(..., 'CatalogItemInspection'); name: effects p2, p3: false  Returns databindingEntryId of 'CatalogItemInspection' container to be ... */
    export function inventoryCreateCatalogItemInspectionEffectsEntry(data: number, name: string, p2: boolean, p3: boolean): number;
    /** data: return value of DATABINDING::_DATABINDING_ADD_DATA_CONTAINER(..., 'CatalogItemInspection'); name: stats, compareStats p2: 0 p3: -1  Returns databindingEntryId of 'CatalogItemInspection' conta... */
    export function inventoryCreateCatalogItemInspectionStatsEntry(data: number, name: string, p2: number, p3: number): number;
    /** filterName (collections): "ALL", "ALL SATCHEL", "ALL HORSES", "ALL COACHES", "ALL MOUNTS", "ALL CLOTHING", "ALL WEAPONS", "ALL SATCHEL EXCLUDING CLOTHING", "ALL EXCLUDING CLOTHING" slotId: -1591664... */
    export function inventoryCreateItemCollection(inventoryId: number, filterName: string, slotId: number, size: number): number;
    /** Returns collectionId */
    export function inventoryCreateItemCollection2(collectionSize: number): number;
    export function inventoryCreateItemCollectionWithFilter(inventoryId: number, filter: any, numInCollection: number): number;
    /** p1: 32 Returns collectionId */
    export function inventoryCreateSortedCollection(inventoryId: number, p1: number, size: number): number;
    /** Example: (1, WEAPON_REVOLVER_CATTLEMAN, 0) - disables cattleman revolver on weapon wheel */
    export function inventoryDisableItem(inventoryId: number, item: number, gtxReason: number): void;
    /** Params: p1 = 0 */
    export function inventoryDisableWeapons(inventoryId: number, p1: any): void;
    export function inventoryDoesItemOwnEquipment(inventoryId: number, guid: any, item: number): boolean;
    export function inventoryEnableItem(inventoryId: number, item: number): void;
    export function inventoryEnableWeapons(inventoryId: number): void;
    export function inventoryEquipItemWithGuid(inventoryId: number, guid: any, bEquipped: boolean): boolean;
    export function inventoryFitsSlotId(item: number, slotId: number): boolean;
    export function inventoryGetChildrenCount(inventoryId: number, parentGuid: any): number;
    export function inventoryGetFullInventoryItemData(inventoryId: number, guid: any, p2: any, p3: number, p4: number): boolean;
    /** Returns a unique inventory ID for this ped. For the local player ped, it is an eInventories value. For other peds, it is the inventory address casted to unsigned int.  enum eInventories { 	INVENTOR... */
    export function inventoryGetInventoryIdFromPed(ped: number): number;
    export function inventoryGetInventoryItemChild(inventoryId: number, parentGuid: any, childIndex: any, outInventoryItem: any): boolean;
    /** Writes up to maxResults compatible SLOTID_* hashes for item into outSlotIds (int32 array; unused entries are 0). Commonly used to gather valid equip slots for component/decoration items. Returns tr... */
    export function inventoryGetInventoryItemCompatibleSlots(item: number, outSlotIds: number, maxResults: number): boolean;
    export function inventoryGetInventoryItemCountWithGuid(inventoryId: number, guid: any, p2: boolean): number;
    export function inventoryGetInventoryItemCountWithItemid(inventoryId: number, eInventoryItem: number, p2: boolean): number;
    export function inventoryGetInventoryItemDescriptionHash(item: number): number;
    export function inventoryGetInventoryItemEquippedInSlot(inventoryId: number, guid: any, slotId: number, p3: number, p4: any): number;
    export function inventoryGetInventoryItemEquippedInSlotByRef(inventoryId: number, guid: any, slotId: number, outGuid: any): boolean;
    export function inventoryGetInventoryItemFitSlot(p0: number, p1: any, p2: number): boolean;
    export function inventoryGetInventoryItemHidden(inventoryId: number, guid: any): boolean;
    export function inventoryGetInventoryItemInspectionInfo(item: number, info: any): boolean;
    export function inventoryGetInventoryItemInUse(inventoryId: number, guid: any): boolean;
    export function inventoryGetInventoryItemIsAnimalPelt(item: number): boolean;
    /** Outputs the last creation date of the item for the selected inventory. Returns true if successful, false otherwise. */
    export function inventoryGetInventoryItemLastCreation(inventoryId: number, item: number, year: number, month: number, day: number, hour: number, minute: number, second: number): boolean;
    /** soundType: see 0x2BAE4880DCDD560B Returns item Hash to be used with _IS_SCRIPTED_AUDIO_CUSTOM and _PLAY_SOUND_FROM_ITEM (p0) */
    export function inventoryGetInventoryItemSound(item: number, soundType: number): number;
    /** Returns CopyID */
    export function inventoryGetInventoryItemWeaponCopyId(inventoryId: number, guid: any): number;
    /** soundType: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/CItemInfoSoundsInterface__sSoundsInfo__eSoundType */
    export function inventoryGetIsInventoryItemSoundValid(item: number, soundType: number): boolean;
    export function inventoryGetItemExpiryTime(itemGUID: any): number;
    /** collectionId is < outCollectionSize */
    export function inventoryGetItemFromCollectionIndex(collectionId: number, itemIndex: number, itemData: any): boolean;
    export function inventoryIsGuidValid(guid: any): boolean;
    /** Alternative Name: _INVENTORY_IS_ITEM_DISABLED */
    export function inventoryIsInventoryItemEquipped(inventoryId: number, item: number, p2: boolean): boolean;
    /** flag: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/ItemDatabaseItemFlags 2097152 (is item read?), 8388608 (is item sent/received/mailable?), 16777216 (is item consumable?) */
    export function inventoryIsInventoryItemFlagEnabled(item: number, flag: number): boolean;
    export function inventoryIsItemExpired(itemGUID: any): boolean;
    export function inventoryIsPlayerInventoryMirroringTransactions(): boolean;
    export function inventoryIsUsingBackupInventory(): boolean;
    /** guid1: old parent GUID guid2: new parent GUID guid3: new item GUID (out param) */
    export function inventoryMoveInventoryItem(inventoryId: number, guid1: any, guid2: any, slotId: number, quantity: number, outGuid: any): boolean;
    /** Max num of collections is 5, so release your unused ones. */
    export function inventoryReleaseItemCollection(collectionId: number): boolean;
    /** removeReason: REMOVE_REASON_DEFAULT (eRemoveItemReason) Example: INVENTORY::_0x5D6182F3BCE1333B(1, joaat("REMOVE_REASON_DEFAULT")); -> clears weapon wheel Only used in R* SP Scripts */
    export function inventoryRemoveInventoryItems(inventoryId: number, removeReason: number): boolean;
    export function inventoryRemoveInventoryItemWithGuid(inventoryId: number, guid: any, quantity: number, removeReason: number): boolean;
    export function inventoryRemoveInventoryItemWithItemid(inventoryId: number, item: number, quantity: number, removeReason: number): boolean;
    /** Used with CClothingItem */
    export function inventorySetInventoryItemHidden(inventoryId: number, guid: any, hidden: boolean): void;
    /** Used with CSatchelItem, R* Script usage: fisihing_core */
    export function inventorySetInventoryItemHidden2(inventoryId: number, guid: any, hidden: boolean): void;
    export function inventorySetInventoryItemInspectionEnabled(inventoryId: number, p1: any, enabled: boolean): boolean;
    /** Only works on CClothingItem */
    export function inventorySetInventoryItemInUse(inventoryId: number, guid: any, inUse: boolean): void;
    /** OWE_INVALID = -1, OWE_GOOD_IN_HOT OWE_GOOD_IN_NONE OWE_GOOD_IN_COLD OWE_GOOD_IN_ALL */
    export function inventorySetInventoryItemWeatherEffectiveness(inventoryId: number, guid: any, weatherEffectiveness: number): void;
    export function inventorySwapInventoryItem(inventoryId: number, guid1: any, guid2: any): boolean;
    /** Getter: _INVENTORY_GET_FULL_INVENTORY_ITEM_DATA */
    export function inventoryUpdateInventoryItem(inventoryId: number, guid1: any, guid2: any, p3: number): boolean;
    /** Only used in R* SP Scripts */
    export function inventoryUseBackupInventory(p0: boolean): void;
    export function inventoryUseMissionInventory(enable: boolean, mirrorTransactions: boolean): void;
    /** eInventoryItem: CLOTHING_FANCY_SUIT, CLOTHING_GUNSLINGER_OUTFIT, etc. Only used in R* SP Scripts */
    export function inventoryUseSatchelItem(inventoryId: number, eInventoryItem: number, p2: boolean): void;
    export function setCarriableCarryActionPromptOverride(data: any): void;
    export function setItemPromptInfoRequest(p0: any): void;
    /** This native has no functionality. */
    export function setUseMissionInventory(toggle: boolean): void;

    // ITEMDATABASE
    export function itemdatabaseFilloutItemByName(key: number, outData: any): boolean;
    export function itemdatabaseFilloutItemEffectIdInfo(key: number, outData: any): boolean;
    /** Outputs item infos. struct ItemInfo { 	Hash category; 	Hash itemType; 	Hash unk2; 	Hash model; 	Hash award; }; */
    export function itemdatabaseFilloutItemInfo(key: number, outData: any): boolean;
    export function itemdatabaseIsBuyableAwardValid(award: number): boolean;
    export function itemdatabaseCanEquipItemOnCategory(key: number, category: number, slotId: number): boolean;
    /** Returns collectionId to be used with 0x8750F69A720C2E41 (p0) and 0xCBB7B6EDFA933ADE (p0) struct ItemCollectionFilter { 	Hash slotId; 	Hash slotId2; 	Hash tag; 	Hash ciCategory; 	Hash cost; 	Hash un... */
    export function itemdatabaseCreateItemCollection(data: any, size: number, comparisonType: number): number;
    /** Params: tag = TAG_ITEM_PROPERTY (tagType(?)) */
    export function itemdatabaseDoesBundleHaveTag(bundle: number, tag: number, tagType: number): boolean;
    export function itemdatabaseDoesItemHaveTag(item: number, tag: number, tagType: number): boolean;
    export function itemdatabaseFilloutAcquireCost(key: number, costtype: number, outData: any): boolean;
    export function itemdatabaseFilloutAwardAcquireCost(award: number, costtype: number, index: number, outData: any): boolean;
    export function itemdatabaseFilloutAwardItemInfo(award: number, index: number, outData: number): boolean;
    export function itemdatabaseFilloutBundle(bundle: number, costtype: number, index: number, outData: any): boolean;
    export function itemdatabaseFilloutBundleUiData(bundle: number, outData: any): boolean;
    export function itemdatabaseFilloutBuyAwardAcquireCosts(award: number, outData: any, outUnk: number, p3: number): boolean;
    export function itemdatabaseFilloutBuyAwardUiData(award: number, outData: any): boolean;
    export function itemdatabaseFilloutItem(key: number, costtype: number, index: number, outData: any): boolean;
    export function itemdatabaseFilloutItemEffectIds(key: number, outData: any): boolean;
    export function itemdatabaseFilloutModifier(key: number, index: number, outData: any): boolean;
    export function itemdatabaseFilloutPriceModifierByKey(key: number, outData: any): boolean;
    export function itemdatabaseFilloutSatchelData(key: number, outSatchelItemSize: number): boolean;
    /** Params: sellType = SELL_SHOP_DEFAULT */
    export function itemdatabaseFilloutSellPrice(key: number, sellType: number, outData: any): boolean;
    export function itemdatabaseFilloutTagData(key: number, outData: any, outIndex: number, p3: number): boolean;
    export function itemdatabaseFilloutUiData(key: number, outData: any): boolean;
    export function itemdatabaseGetAcquireCost(key: number, index: number, outData: any): boolean;
    /** Returns the number of <Item>s <acquirecosts> has from the key in catalog_sp.ymt */
    export function itemdatabaseGetAcquireCostsCount(key: number): number;
    export function itemdatabaseGetAcquireCostsCountFromCostType(key: number, costtype: number): number;
    export function itemdatabaseGetAwardAcquireCost(award: number, index: number, outData: any): boolean;
    export function itemdatabaseGetAwardAcquireCostCount(key: number): number;
    export function itemdatabaseGetAwardAcquireCostCountFromCostType(award: number, costtype: number): number;
    export function itemdatabaseGetAwardCostModifiers(award: number, outData: any): boolean;
    export function itemdatabaseGetAwardInfo(award: number, outData: any): boolean;
    /** Returns iAwardItemCount */
    export function itemdatabaseGetAwardItemCount(award: number): number;
    export function itemdatabaseGetBundleAcquireCostModifiers(bundle: number, outData: any): boolean;
    export function itemdatabaseGetBundleId(bundle: number): number;
    export function itemdatabaseGetBundleItemCount(bundleId: number, data: any): number;
    /** Outputs bundle item info. struct BundleItemInfo { 	Hash item; 	Hash slotId; 	int unk2; 	int unk3; }; */
    export function itemdatabaseGetBundleItemInfo(bundleId: number, data: any, index: number, outBundle: number): boolean;
    /** Returns (collection?) size/index (?) _ITEMDATABASE_GET_(A)* - _ITEMDATABASE_GET_(B)* */
    export function itemdatabaseGetCollectionSize(collectionId: number): number;
    /** Params: p2 can be a component item hash */
    export function itemdatabaseGetComponentItem(collectionId: number, index: number, outKey: number): boolean;
    export function itemdatabaseGetFitsSlotCount(category: number): number;
    export function itemdatabaseGetFitsSlotInfo(category: number, index: number, outSlotId: number): boolean;
    export function itemdatabaseGetHasSlotCount(category: number): number;
    export function itemdatabaseGetHasSlotInfo(category: number, index: number, outSlotId: number): boolean;
    export function itemdatabaseGetItemPathset(key: number, defaultPathset: number): number;
    export function itemdatabaseGetItemPriceModifiers(key: number, outData: any): boolean;
    export function itemdatabaseGetItemTagType(item: number, tag: number): number;
    /** Returns an alternative cost hash to COST_SHOP_DEFAULT */
    export function itemdatabaseGetModifiedPrice(key: number, index: number): number;
    export function itemdatabaseGetNumberOfModifiedPrices(key: number): number;
    export function itemdatabaseGetNumberOfModifiers(key: number): number;
    /** _ITEMDATABASE_GET_* - _ITEMDATABASE_IS_* */
    export function itemdatabaseGetPriorityAccessAward(award: number): boolean;
    export function itemdatabaseGetShopInventoriesItemsCount(shopType: number): number;
    export function itemdatabaseGetShopInventoriesItemInfo(shopType: number, index: number, outData: any): boolean;
    /** Same Native Function as 0x17721003A66C72BF */
    export function itemdatabaseGetShopInventoriesItemInfoByKey(shopType: number, key: number, outData: any): boolean;
    export function itemdatabaseGetShopInventoriesRequirementGroupInfo(shopType: number, key: number, index: number, outData: any): boolean;
    export function itemdatabaseGetShopInventoriesRequirementInfo(shopType: number, key: number, groupIndex: number, index: number, outData: any): boolean;
    export function itemdatabaseGetShopLayoutInfo(layout: number, outData: any): boolean;
    export function itemdatabaseGetShopLayoutMenuInfoById(layout: number, menu: number, outData: any): boolean;
    export function itemdatabaseGetShopLayoutMenuInfoByIndex(layout: number, menu: number, index: number, outData: any): boolean;
    export function itemdatabaseGetShopLayoutMenuPageKey(layout: number, menu: number, index: number, outPageKey: number): boolean;
    /** Outputs the layout page info at the selected index. struct LayoutPageInfo { 	Hash pageKey; 	Hash unk1; 	BOOL unk2; 	int numItems; }; */
    export function itemdatabaseGetShopLayoutPageInfoByIndex(layout: number, index: number, outData: any): boolean;
    export function itemdatabaseGetShopLayoutPageInfoByKey(layout: number, pageKey: number, outData: any): boolean;
    export function itemdatabaseGetShopLayoutPageItemKey(layout: number, pageKey: number, index: number, outItemKey: number, outMenuId: number, outLayout: number): boolean;
    export function itemdatabaseGetShopLayoutRootMenuInfo(layout: number, index: number, outData: any): boolean;
    /** Params: mode is 0 */
    export function itemdatabaseIsBundleValid(bundle: number, mode: number): boolean;
    export function itemdatabaseIsIntrinsicItem(key: number): boolean;
    /** Params: mode is 0 */
    export function itemdatabaseIsKeyValid(key: number, mode: number): boolean;
    export function itemdatabaseIsOverpoweredItem(key: number): boolean;
    export function itemdatabaseIsShopKeyValid(shopType: number): boolean;
    export function itemdatabaseIsShopLayoutKeyValid(layout: number): boolean;
    /** Returns docData.iNumTotalLabelTypes */
    export function itemdatabaseLocalizationGetNumLabelTypes(p0: any): number;
    /** Returns iNumValuesForType */
    export function itemdatabaseLocalizationGetNumValues(p0: any, p1: any): number;
    /** Returns LabelType */
    export function itemdatabaseLocalizationGetType(p0: any, p1: any): any;
    export function itemdatabaseLocalizationGetValue(p0: any, label: number, p2: any): number;
    export function itemdatabaseReleaseItemCollection(collectionId: number): boolean;

    // ITEMSET
    export function addToItemset(entity: number, itemset: number): boolean;
    export function cleanItemset(itemset: number): void;
    export function createItemset(p0: boolean): number;
    export function destroyItemset(itemset: number): void;
    export function getIndexedItemInItemset(index: number, itemset: number): number;
    export function getIndexedScenarioPointIndexInItemset(index: number, itemset: number): any;
    export function getItemsetSize(itemset: number): number;
    export function isInItemset(entity: number, itemset: number): boolean;
    export function isItemsetValid(itemset: number): boolean;
    export function removeFromItemset(entity: number, itemset: number): void;
    export function clearItemset(itemset: number): void;

    // LAW
    export function addBounty(player: number, itemValueAmount: number): void;
    export function areWitnessesActive(player: number): boolean;
    export function clearBounty(player: number): void;
    export function clearPlayerPastCrimes(player: number): void;
    export function clearWantedScore(player: number): void;
    export function getBounty(player: number): number;
    /** Reads one entry from the player's registered-crimes list (oldest -> newest) at the given index. 	- Commonly iterated with index 0..23. 	- Returns true if the entry exists/was copied into outData. 	... */
    export function getPlayerRegisteredCrime(player: number, index: number, outData: any): boolean;
    export function getWantedScore(player: number): number;
    export function isLawIncidentActive(player: number): boolean;
    /** Returns amount of suppressed crimes to be used later in the function MPINTRO_CRIME_MONITOR_MAINTAIN */
    export function numCrimesSuppressed(player: number, crimeType: number): number;
    export function resetWantedForNewIncident(player: number): void;
    export function setBounty(player: number, amount: number): void;
    export function setDisableDisturbanceCrimes(player: number, p1: boolean): void;
    /** Default range is 1.0f */
    export function setLawSenseRangeModifier(player: number, range: number): void;
    export function setPlayerArrestedInRegion(player: number, lawRegionHash: number): void;
    export function setPlayerTurnedInBountyInRegion(player: number, lawRegionHash: number): void;
    export function setPostponeDisturbanceCrimesDuringCombat(player: number, p1: boolean): void;
    export function setWantedScore(player: number, intensity: number): void;
    /** crimeType: see _REPORT_CRIME */
    export function suppressCrimeThisFrame(player: number, crimeType: number, p2: number, p3: number, p4: number): void;
    export function addWitnessResponse(player: number, crimeType: number, pedGroup: number): void;
    /** Only used in rcm_homerob00 R* Script */
    export function areAnyLawPedsInvestigating(): boolean;
    export function areInvestigatorsActive(player: number, areInvestigatorsActive: boolean, p2: any): boolean;
    /** Only used in trainrobbery_ambient R* Script */
    export function areLawPedsEnabledForTrain(): boolean;
    export function areWitnessesPending(player: number): boolean;
    export function createGuardZone(name: string): void;
    /** Returns true when investigation creation was successful */
    export function createGuardZoneForEntity(guardZoneName: string, entity: number, x: number, y: number, z: number): boolean;
    /** dispatchResponseHash: see common/data/dispatchresponses/.. */
    export function createLawDispatchResponseForCoords(x: number, y: number, z: number, dispatchResponseHash: number): any;
    export function disableGuardZone(name: string): void;
    export function enableDispatchLaw(toggle: boolean): void;
    export function enableDispatchLaw2(toggle: boolean): void;
    export function forceLawOnLocalPlayerImmediately(): void;
    /** p0 is always BOUNTYHUNTERSGLOBALCOOLDOWN in R* scripts */
    export function getBountyHunterGlobalCooldown(p0: number): number;
    /** Returns bounty (increment) value */
    export function getCrimeBountyAmountByType(crimeType: number): number;
    export function getCrimeDispatchTypeForPlayer(player: number): number;
    /** See _REPORT_CRIME */
    export function getHudPlayerCrimeType(player: number): number;
    /** Returns the amount of time (probably in game minutes) since last seen by the law / left the wanted radius */
    export function getTimeSinceLastSeenByLaw(player: number): number;
    export function isGuardPedInvestigating(ped: number): boolean;
    export function lawWitnessResponseTask(pedGroup1: number, ped: number, pedGroup2: number, x: number, y: number, z: number, crimeType: number): boolean;
    /** p0 is always BOUNTYHUNTERSGLOBALCOOLDOWN in R* scripts */
    export function pauseBountyHunterCooldown(p0: number, p1: boolean, p2: any): void;
    export function removeGuardZone(name: string): void;
    /** crimeType: enum eCrimeType : Hash { 	CRIME_ACCOMPLICE = 0xAF074F6D, 	CRIME_ARSON = 0x68134DC7, 	CRIME_ASSAULT = 0x0BADC882, 	CRIME_ASSAULT_ANIMAL = 0x18DA55EE, 	CRIME_ASSAULT_CORPSE = 0x4E5F23F2, 	... */
    export function reportCrime(player: number, crimeType: number, bounty: number, entity: number, isKnownSuspect: boolean): void;
    export function reportPlayerLawDispatchResponseOverride(player: number, dispatchResponseHash: number): void;
    export function setAllowDisabledLawResponses(toggle: boolean): void;
    /** p0 is always BOUNTYHUNTERSGLOBALCOOLDOWN in R* scripts */
    export function setBountyHunterGlobalCooldown(p0: number, p1: number): void;
    /** Force clears local player's wanted level */
    export function setBountyHunterPursuitCleared(): void;
    export function setCantLoseLawThisResponse(enabled: boolean): void;
    /** Note: This native is only used in multiplayer scripts dispatchResponseHash: see update1/common/data/dispatchresponses/.. */
    export function setCustomLawDispatchResponse(dispatchResponseHash: number): void;
    export function setDispatchMultiplierOverride(multiplier: number): void;
    export function setGuardZonePosition(name: string, x: number, y: number, z: number): void;
    export function setGuardZonePosition2(name: string, x: number, y: number, z: number): void;
    export function setGuardZoneVolumeRegistrationEnd(name: string, volume: number): void;
    export function setGuardZoneVolumeRegistrationStart(name: string, volume: number): void;
    export function setGuardZoneVolumeRestricted(name: string, volume: number): void;
    export function setGuardZoneVolumeThreat(name: string, volume: number): void;
    export function setGuardZoneVolumeWarning(name: string, volume: number): void;
    export function setLawDisabled(toggle: boolean): void;
    export function setLawRbsVolume(player: number, p1: number): void;
    /** enum eLawRegion : Hash { 	LAW_DISPATCH_REGION_NONE = 0, 	LAW_REGION_AGUASDULCES = 0x2F573EBE, 	LAW_REGION_ANNESBURG = 0x68CAFD50, 	LAW_REGION_ARMADILLO = 0xF0B90756, 	LAW_REGION_BAYOU_NWA = 0x80966... */
    export function setLawRegion(player: number, lawRegionHash: number, stateHash: number): void;
    /** behaviour: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/CLawBehavior__Flags */
    export function setPedLawBehaviour(ped: number, behaviour: number): void;

    // LOCALIZATION
    /** 0 = american (en-US) 1 = french (fr-FR) 2 = german (de-DE) 3 = italian (it-IT) 4 = spanish (es-ES) 5 = brazilian (pt-BR) 6 = polish (pl-PL) 7 = russian (ru-RU) 8 = korean (ko-KR) 9 = chinesetrad (z... */
    export function getCurrentLanguage(): number;
    /** 0 = DATE_FORMAT_DMY 1 = DATE_FORMAT_MDY 2 = DATE_FORMAT_YMD  Old name: _LOCALIZATION_GET_SYSTEM_DATE_FORMAT */
    export function localizationGetSystemDateType(): number;
    /** Same return values as GET_CURRENT_LANGUAGE */
    export function localizationGetSystemLanguage(): number;
    /** Returns true if the current language is american, french, german, italian, spanish, brazilian or mexican. _DOES_* */
    export function doesCurrentLanguageSupportCondensedStyle(): boolean;

    // MAP
    export function addPointToGpsMultiRoute(x: number, y: number, z: number, p3: boolean): void;
    export function allowSonarBlips(toggle: boolean): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/useful_info_from_rpfs/textures/blips https://github.com/femga/rdr3_discoveries/tree/master/useful_info_from_rpfs/textures/blips_mp */
    export function blipAddForCoords(blipHash: number, x: number, y: number, z: number): number;
    export function blipAddForEntity(blipHash: number, entity: number): number;
    export function blipAddForPickupPlacement(blipHash: number, pickup: number): number;
    export function blipAddForRadius(blipHash: number, x: number, y: number, z: number, radius: number): number;
    /** https://alloc8or.re/rdr3/doc/enums/eBlipModifier.txt https://github.com/femga/rdr3_discoveries/tree/master/useful_info_from_rpfs/blip_modifiers  Old name: _BLIP_SET_MODIFIER */
    export function blipAddModifier(blip: number, modifierHash: number): boolean;
    /** If modifierHash is 0, ALL modifiers will be removed. */
    export function blipRemoveModifier(blip: number, modifierHash: number): boolean;
    export function clearGpsCustomRoute(): void;
    /** Clears the GPS flags. */
    export function clearGpsFlags(): void;
    /** Does the same as SET_GPS_MULTI_ROUTE_RENDER(false); */
    export function clearGpsMultiRoute(): void;
    export function clearGpsPlayerWaypoint(): void;
    /** If Minimap / Radar should be displayed. */
    export function displayRadar(toggle: boolean): void;
    export function doesBlipExist(blip: number): boolean;
    /** Doesn't actually return anything. */
    export function forceSonarBlipsThisFrame(): any;
    export function getBlipCoords(blip: number): Vector3;
    /** Returns the Blip handle of given Entity. */
    export function getBlipFromEntity(entity: number): number;
    export function getMainPlayerBlipId(): number;
    export function isBlipOnMinimap(blip: number): boolean;
    export function isWaypointActive(): boolean;
    /** Locks the minimap to the specified angle in integer degrees.  angle: The angle in whole degrees. If less than 0 or greater than 360, unlocks the angle. */
    export function lockMinimapAngle(angle: number): void;
    export function removeBlip(blip: number): void;
    export function resetMinimapFow(hash: number): void;
    export function setBlipCoords(blip: number, posX: number, posY: number, posZ: number): void;
    export function setBlipFlashes(blip: number, p1: number, p2: number): boolean;
    export function setBlipFlashTimer(blip: number, blipType: number, blipHash: number): void;
    export function setBlipNameFromTextFile(blip: number, textLabel: string): void;
    export function setBlipNameToPlayerName(blip: number, player: number): void;
    export function setBlipRotation(blip: number, rotation: number): void;
    export function setBlipScale(blip: number, scale: number): void;
    export function setBlipSprite(blip: number, hash: number, p2: boolean): void;
    export function setGpsCustomRouteRender(p0: boolean, p1: number, p2: number): void;
    /** https://alloc8or.re/rdr3/doc/enums/rage__eGpsFlags.txt */
    export function setGpsFlags(p0: number, p1: number): void;
    export function setGpsMultiRouteRender(toggle: boolean): void;
    /** Up to eight coordinates may be revealed per frame */
    export function setMinimapFowRevealCoordinate(x: number, y: number, z: number, p3: number): void;
    export function setMinimapFowRevealVolume(volume: number, p1: number): void;
    /** Reveals the entire minimap (FOW = Fog of War) */
    export function setMinimapHideFow(toggle: boolean): void;
    export function setRadarAsExteriorThisFrame(): void;
    export function setRadarZoom(zoomLevel: number): void;
    export function setWaypointOff(): void;
    export function startGpsMultiRoute(colorNameHash: number, onFoot: boolean, inVehicle: boolean): void;
    export function triggerSonarBlip(typeHash: number, x: number, y: number, z: number): void;
    export function unlockMinimapAngle(): void;
    /** Not sure what exactly this does, but it calls rage::fwuiBlip::ClearScriptIdentity() internally */
    export function abandonBlip(blip: number): void;
    /** list of minimap props: https://github.com/femga/rdr3_discoveries/tree/master/graphics/minimap/minimapObjects */
    export function addPropToMinimap(minimapProp: number, x: number, y: number, rotation: number, p4: number): void;
    export function blipAddForArea(blipHash: number, x: number, y: number, z: number, scaleX: number, scaleY: number, scaleZ: number, p7: number): number;
    export function blipAddForStyle(styleHash: number): number;
    export function blipAddForVolume(blipHash: number, volume: number): number;
    export function blipAddStyle(blip: number, styleHash: number): boolean;
    /** https://github.com/femga/rdr3_discoveries/tree/master/useful_info_from_rpfs/blip_styles Removes any existing modifiers and sets the style. */
    export function blipSetStyle(blip: number, styleHash: number): boolean;
    /** Clears blip data, must be called before REMOVE_BLIP. Blips seem to be handled via databinding internally, this function should then allow you to clear blip container and therefore free up memory. */
    export function clearBlip(blip: number): boolean;
    /** Removes the blip icon from the entity lockon prompt */
    export function clearBlipIconFromLockonEntityPrompt(entity: number, blip: number): void;
    /** Clears the previously set coordinates for the pause map view, removing any specified focal point and radius that were set using `_SET_PAUSEMAP_COORDS_WITH_RADIUS` (0xE0884C184728C75B). This functio... */
    export function clearPausemapCoords(): void;
    export function doesEntityHaveBlip(entity: number): boolean;
    export function findClosestGpsPosition(x: number, y: number, z: number, outPosition: Vector3): boolean;
    /** Note: Z coordinate will always be zero */
    export function getWaypointCoords(): Vector3;
    /** Unlike `GET_WAYPOINT_COORDS` (0x29B30D07C3F7873B), which returns a single value, this native returns the x and y coordinates of the waypoint separately as float pointers. Image: https://i.imgur.com... */
    export function getWaypointPosition(x: number, y: number): boolean;
    export function hideActivePointsOfInterest(): void;
    export function isBlipAttachedToAnyEntity(blip: number): boolean;
    /** Returns true if the entity lockon prompt contains an blip icon. */
    export function isDisplayBlipIconOnLockonEntityPrompt(entity: number, blip: number): boolean;
    /** Checks if the GPS route to the waypoint is navigable along a road. If a route exists but there is no valid road path, this function returns false. */
    export function isPathForGpsOnRoad(): boolean;
    export function mapDisableRegionBlip(regionHash: number): void;
    export function mapDiscoverySetEnabled(discoveryHash: number): void;
    export function mapDiscoverRegion(discoveryHash: number): void;
    /** regionHash: https://github.com/femga/rdr3_discoveries/tree/master/graphics/minimap/wanted_regions */
    export function mapEnableRegionBlip(regionHash: number, styleHash: number): void;
    export function mapIsDiscoveryActive(discoveryHash: number): boolean;
    export function mapIsRegionHighlightedWithStyle(regionHash: number, styleHash: number): boolean;
    export function removePropFromMinimap(minimapProp: number): void;
    export function revealMinimapFow(hash: number): void;
    /** Removes blip from any entity and makes it static on the map, try it on GET_MAIN_PLAYER_BLIP_ID for a demonstration. */
    export function setBlipFrozen(blip: number): void;
    export function setBlipName(blip: number, name: string): void;
    /** Removes the icon from the lockon prompt. Never executed in R* Scripts due to hardcoded 0. */
    export function setDisplayBlipIconForEntityPromptRemoved(entity: number, p1: number): void;
    /** Activates a blip icon prompt for a specific entity, allowing it to be displayed without requiring a lock-on. This function enables the blip to appear associated with the given entity, making it vis... */
    export function setDisplayBlipIconForEntityPromptWithoutLockon(entity: number): void;
    /** Sets the blip icon to lockon entity prompt. */
    export function setDisplayBlipIconForEntityPromptWithLockon(entity: number, blipIcon: number): void;
    /** Adds entity blip icon to the entity lockon prompt, if invalid param it will remove the icon if it had any. */
    export function setDisplayBlipIconToLockonEntityPrompt(entity: number, blip: number): void;
    /** Used for GUARMA MODE; Enabled: toggle = false, 0; Disabled: toggle = true, 0 Hash p1 seems to be unused, always 0 */
    export function setFowUpdatePlayerOverride(toggle: boolean, p1: number): void;
    export function setMinimapFowOverrideRevealScale(scale: number, p1: number): void;
    export function setMinimapFowShouldUpdate(toggle: boolean, p1: number): void;
    /** hash can be the hash of "guarma" or "world". */
    export function setMinimapZone(zone: number): void;
    export function setPausemapCoordsWithRadius(x: number, y: number, z: number, radius: number): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/graphics/minimap/radar/radar_configs.lua configHash: -1943724816, 347777538, -117986897, -789269373, -547506804, -1986542417, 2080113112 p1: us... */
    export function setRadarConfigType(configHash: number, p1: number): void;
    export function showActivePointsOfInterest(): void;
    export function startGpsCustomRouteFromWaypointRecordingRoute(waypointRecording: string, point: number, numPoints: number, colorNameHash: number, p4: boolean, p5: boolean): void;
    export function triggerSonarBlipOnEntity(typeHash: number, entity: number): void;

    // MINIGAME
    /** Hardcoded to return zero/false. */
    export function dominoesBuyIn(p0: any): any;
    /** Hardcoded to return zero/false. */
    export function dominoesPlaceDomino(p0: any, p1: any): any;
    /** Hardcoded to return zero/false. */
    export function dominoesRequestValidPlacements(p0: any): any;
    export function minigameGetNextEvent(p0: any, p1: any): any;
    export function minigameGetNextEventType(): any;
    export function minigameIsConnectedToServer(p0: any): any;
    export function minigameIsRequestPending(p0: any): any;
    export function minigameIsSeatOccupied(p0: any): boolean;
    export function minigameLeaveTable(p0: any): any;
    export function minigamePopNextEvent(): void;
    export function minigameRequestSeatAtTable(data: any): boolean;
    export function pokerBuyIn(p0: any, p1: any): any;
    export function pokerCall(p0: any, p1: any): any;
    export function pokerCheck(p0: any, p1: any): any;
    export function pokerFold(p0: any): any;
    export function pokerGetGameSettingsForId(p0: any): any;
    export function pokerRaise(p0: any, p1: any): any;
    export function pokerReveal(p0: any): any;

    // MISC
    export function absf(value: number): number;
    export function absi(value: number): number;
    export function acos(p0: number): number;
    /** Appears to remove stealth kill action from memory (?) */
    export function actionManagerEnableAction(hash: number, enable: boolean): void;
    export function actionManagerIsActionEnabled(hash: number): boolean;
    /** nullsub, doesn't do anything */
    export function activityFeedActionStartWithCommandLine(p0: string, p1: string): void;
    /** nullsub, doesn't do anything */
    export function activityFeedActionStartWithCommandLineAdd(p0: string): void;
    /** nullsub, doesn't do anything */
    export function activityFeedAddSubstringToCaption(p0: string): void;
    /** nullsub, doesn't do anything */
    export function activityFeedCreate(p0: string, p1: string): void;
    /** nullsub, doesn't do anything */
    export function activityFeedPost(): void;
    export function addPopMultiplierArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, pedDensity: number, trafficDensity: number, p8: boolean, p9: boolean): number;
    /** Params: p3 is 0 in R* Script utopia2 */
    export function addTacticalNavMeshPoint(x: number, y: number, z: number, p3: number): void;
    export function areStringsEqual(string1: string, string2: string): boolean;
    export function asin(p0: number): number;
    export function atan(p0: number): number;
    export function atan2(p0: number, p1: number): number;
    /** dispatchService: see ENABLE_DISPATCH_SERVICE */
    export function blockDispatchServiceResourceCreation(dispatchService: number, toggle: boolean): void;
    /** Old name: _CANCEL_ONSCREEN_KEYBOARD */
    export function cancelOnscreenKeyboard(): void;
    export function clearAngledAreaOfVehicles(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any): void;
    /** Possible flag names: ALL_BASE = 0, PROJECTILES = 1, BROADCAST = 524288, AMBIENT_POPULATION = 1048576 */
    export function clearArea(x: number, y: number, z: number, radius: number, flag: number): void;
    export function clearBit(address: number, offset: number): void;
    export function clearOverrideWeather(): void;
    export function clearTacticalNavMeshPoints(): void;
    export function clearWeatherTypePersist(): void;
    export function compareStrings(str1: string, str2: string, matchCase: boolean, maxLength: number): number;
    /** Old name: _COPY_MEMORY */
    export function copyScriptStruct(dst: any, src: any, size: number): void;
    /** dispatchService: see ENABLE_DISPATCH_SERVICE */
    export function createIncident(dispatchService: number, x: number, y: number, z: number, numUnits: number, radius: number, outIncidentID: number, p7: any, p8: any): boolean;
    /** Delete an incident with a given id. */
    export function deleteIncident(incidentId: number): void;
    /** Disables composite eat prompt. */
    export function disableLootingCompositeLootableThisFrame(compositeId: number, p1: boolean): void;
    /** enum eOnscreenKeyboardTextType { 	KTEXTTYPE_INVALID = -1, 	KTEXTTYPE_DEFAULT, 	KTEXTTYPE_EMAIL, 	KTEXTTYPE_PASSWORD, 	KTEXTTYPE_NUMERIC, 	KTEXTTYPE_ALPHABET, 	KTEXTTYPE_GAMERTAG, 	KTEXTTYPE_FILENAM... */
    export function displayOnscreenKeyboard(textType: number, windowTitle: string, p2: string, defaultText: string, defaultConcat1: string, defaultConcat2: string, defaultConcat3: string, maxInputLength: number): void;
    export function doesPopMultiplierAreaExist(id: number): boolean;
    /** enum DispatchType { 	DT_Invalid, 	DT_PoliceAutomobile, 	DT_PoliceHelicopter, 	DT_FireDepartment, 	DT_SwatAutomobile, 	DT_AmbulanceDepartment, 	DT_PoliceRiders, 	DT_PoliceVehicleRequest, 	DT_PoliceR... */
    export function enableDispatchService(dispatchService: number, toggle: boolean): void;
    export function fireSingleBullet(args: any): void;
    /** creates single lightning+thunder at random position */
    export function forceLightningFlash(): void;
    export function gameFrameworkManagerInit(transitionMode: number): boolean;
    export function getAngleBetween2dVectors(x1: number, y1: number, x2: number, y2: number): number;
    /** Returns value of the '-benchmarkIterations' command line option.  Old name: _GET_BENCHMARK_ITERATIONS_FROM_COMMAND_LINE */
    export function getBenchmarkIterations(): number;
    /** Returns value of the '-benchmarkPass' command line option.  Old name: _GET_BENCHMARK_PASS_FROM_COMMAND_LINE */
    export function getBenchmarkPass(): number;
    export function getBitsInRange(var: number, rangeStart: number, rangeEnd: number): number;
    export function getClosestPointOnLine(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: boolean): Vector3;
    export function getCoordsOfProjectileTypeWithinDistance(ped: number, weaponHash: number, distance: number, outCoords: Vector3, p4: boolean, mustBeOwnedByThisPed: boolean): boolean;
    /** Params: percentWeather2: 0f - 0.75f in R* Scripts  Old name: _GET_WEATHER_TYPE_TRANSITION */
    export function getCurrWeatherState(weatherType1: number, weatherType2: number, percentWeather2: number): void;
    /** If useZ is false, only the 2D plane (X-Y) will be considered for calculating the distance.  Consider using this faster native instead: BUILTIN::VDIST - DVIST always takes in consideration the 3D co... */
    export function getDistanceBetweenCoords(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, useZ: boolean): number;
    export function getFrameCount(): number;
    /** Also known as "delta time" */
    export function getFrameTime(): number;
    export function getGameTimer(): number;
    export function getGroundZAndNormalFor3dCoord(x: number, y: number, z: number, groundZ: number, normal: Vector3): boolean;
    export function getGroundZFor3dCoord(x: number, y: number, z: number, groundZ: number, p4: boolean): boolean;
    /** Computes a hash for the given string. It is hashed using Jenkins' One-at-a-Time hash algorithm (https://en.wikipedia.org/wiki/Jenkins_hash_function) Note: this implementation is case-insensitive. */
    export function getHashKey(string: string): number;
    /** dx = x1 - x2 dy = y1 - y2 */
    export function getHeadingFromVector2d(dx: number, dy: number): number;
    export function getLinePlaneIntersection(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: number, p10: number, p11: number, p12: number): boolean;
    export function getMissionFlag(): boolean;
    export function getModelDimensions(modelHash: number, minimum: Vector3, maximum: Vector3): void;
    export function getNumberOfFreeStacksOfThisSize(stackSize: number): number;
    export function getNumberOfMicrosecondsSinceLastCall(): number;
    /** Returns NULL unless UPDATE_ONSCREEN_KEYBOARD() returns 1 in the same tick. */
    export function getOnscreenKeyboardResult(): NativeString;
    export function getProjectileOfProjectileTypeWithinDistance(ped: number, weaponHash: number, distance: number, outCoords: Vector3, outProjectile: number, p5: boolean, mustBeOwnedByThisPed: boolean): boolean;
    export function getRainLevel(): number;
    export function getRandomEventFlag(): boolean;
    export function getRandomFloatInRange(startRange: number, endRange: number): number;
    export function getRandomIntInRange(startRange: number, endRange: number): number;
    /** Returns GET_GAME_TIMER() / 1000 Only used in rcm_pearson1.ysc */
    export function getRealWorldTime(): number;
    export function getScriptTimeWithinFrameInMicroseconds(): number;
    export function getSnowLevel(): number;
    export function getSystemTime(): number;
    /** Old name: _GET_BENCHMARK_TIME */
    export function getSystemTimeStep(): number;
    export function getWindDirection(): Vector3;
    export function getWindSpeed(): number;
    /** p3 - possibly radius? */
    export function hasBulletImpactedInArea(x: number, y: number, z: number, p3: number, p4: boolean, p5: boolean): boolean;
    export function hasBulletImpactedInBox(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: boolean, p7: boolean): boolean;
    export function ignoreNextRestart(toggle: boolean): void;
    export function informCodeOfContentIdOfCurrentUgcMission(p0: string): void;
    export function isBitSet(address: number, offset: number): boolean;
    export function isBulletInAngledArea(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: boolean): boolean;
    export function isBulletInArea(p0: number, p1: number, p2: number, p3: number, p4: boolean): boolean;
    export function isBulletInBox(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: boolean): boolean;
    /** Hardcoded to return false. Checks for XBOXONE Game Build. */
    export function isDurangoVersion(): boolean;
    export function isGameSessionStateMachineIdle(): boolean;
    export function isIncidentValid(incidentId: number): boolean;
    /** magdemo = magazine demo, i. e. for magazines such as IGN, pre play phases to prepare articles etc. - example 2012 builds for V Hardcoded to return false. */
    export function isMagDemo1Active(): boolean;
    export function isMinigameInProgress(): boolean;
    /** Hardcoded to return false. Checks for PS4 Game Build. */
    export function isOrbisVersion(): boolean;
    /** Hardcoded to return true. */
    export function isPcVersion(): boolean;
    export function isPositionOccupied(x: number, y: number, z: number, range: number, p4: boolean, p5: boolean, p6: boolean, p7: boolean, p8: boolean, p9: any, p10: boolean): boolean;
    /** Determines whether there is a projectile within the specified coordinates. The coordinates form a rectangle.  ownedByPlayer = only projectiles fired by the player will be detected. */
    export function isProjectileInArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, ownedByPlayer: boolean): boolean;
    export function isProjectileTypeInAngledArea(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: any, p8: boolean): boolean;
    /** Determines whether there is a projectile of a specific type within the specified coordinates. The coordinates form a rectangle. */
    export function isProjectileTypeInArea(xMin: number, yMin: number, zMin: number, xMax: number, yMax: number, zMax: number, weaponType: number, isPlayer: boolean): boolean;
    export function isProjectileTypeWithinDistance(p0: number, p1: number, p2: number, p3: any, p4: number, p5: boolean): boolean;
    /** Hardcoded to return false. */
    export function isStadiaVersion(): boolean;
    export function isStringNull(string: string): boolean;
    export function isStringNullOrEmpty(string: string): boolean;
    /** Returns true if the entire string consists only of space characters. */
    export function isStringNullOrEmptyOrSpaces(string: string): boolean;
    export function networkSetScriptIsSafeForNetworkGame(): void;
    export function nextOnscreenKeyboardResultWillDisplayUsingTheseFonts(fontBitField: number): void;
    export function overrideSaveHouse(override: boolean, x: number, y: number, z: number, heading: number, isAutosave: boolean, returnCoords: Vector3, returnHeading: number): boolean;
    export function pauseDeathArrestRestart(toggle: boolean): void;
    /** spawns a few distant/out-of-sight peds, vehicles, animals etc each time it is called */
    export function populateNow(): void;
    /** p3 is usually the same value of radius p8 determines whether the ILO prompt is a lock on prompt with RMB */
    export function registerInteractionLockonPrompt(entity: number, text: string, radius: number, p3: number, flag: number, p5: number, p6: number, prompt: number, p8: boolean, p9: number): boolean;
    export function removeDispatchSpawnBlockingArea(p0: any): void;
    export function removePopMultiplierArea(id: number, p1: boolean): void;
    export function resetDispatchIdealSpawnDistance(): void;
    /** Begins with RESET_*. Next character in the name is either D or E.  Old name: _RESET_BENCHMARK_RECORDING */
    export function resetEndUserBenchmark(): void;
    export function resetScriptTimeWithinFrame(): void;
    export function resetWantedResponseNumPedsToSpawn(): void;
    /** Saves the benchmark recording to %USERPROFILE%\Documents\Rockstar Games\Red Dead Redemption 2\Benchmarks and submits some metrics.  Old name: _SAVE_BENCHMARK_RECORDING */
    export function saveEndUserBenchmark(): void;
    export function scriptRaceGetPlayerSplitTime(p0: any, p1: any, p2: any): boolean;
    export function scriptRaceInit(numCheckpoints: number, numLaps: number, numPlayers: number, p3: any): void;
    export function scriptRacePlayerHitCheckpoint(part: number, checkpoint: number, lap: number, time: number): void;
    export function scriptRaceShutdown(): void;
    export function setBit(address: number, offset: number): void;
    export function setBitsInRange(var: number, rangeStart: number, rangeEnd: number, p3: number): void;
    /** Cheats are GTA IV cheats:  0 = unknown 1 = unknown (same as 0) 2 = Max Health and Armor 3 = Raise Wanted Level 4 = Lower Wanted Level 5 = unknown (does nothing) 6 = Change Weather 7 = Spawn Annihil... */
    export function setCheatActive(cheatId: number): void;
    export function setCreditsActive(toggle: boolean): void;
    /** Params: BOOL p3 is always true  Old name: _SET_WEATHER_TYPE_TRANSITION */
    export function setCurrWeatherState(weatherType1: number, weatherType2: number, percentWeather2: number, enabled: boolean): void;
    export function setDispatchIdealSpawnDistance(fIdealSpawnDistance: number): void;
    /** Sets whether the game should fade in after the player dies or is arrested. */
    export function setFadeInAfterDeathArrest(toggle: boolean): void;
    export function setFadeInAfterLoad(toggle: boolean): void;
    /** Make sure to call this from the correct thread if you're using multiple threads because all other threads except the one which is calling SET_GAME_PAUSED will be paused. */
    export function setGamePaused(toggle: boolean): void;
    /** If true, the player can't save the game. */
    export function setMissionFlag(toggle: boolean): void;
    export function setPedDecomposed(ped: number, toggle: boolean): void;
    /** Old name: _SET_RAIN_LEVEL */
    export function setRain(intensity: number): void;
    /** If the parameter is true, sets the random event flag to true, if the parameter is false, the function does nothing at all. Does nothing if the mission flag is set. */
    export function setRandomEventFlag(toggle: boolean): void;
    export function setRandomSeed(seed: number): void;
    export function setRandomWeatherType(p0: boolean, p1: boolean): void;
    export function setSuperJumpThisFrame(player: number): void;
    export function setThisScriptCanBePaused(toggle: boolean): void;
    export function setThisScriptCanRemoveBlipsCreatedByAnyScript(toggle: boolean): void;
    /** Maximum value is 1.0f At a value of 0.0f the game will still run at a minimum time scale. */
    export function setTimeScale(timeScale: number): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/weather/weather_types.lua */
    export function setWeatherType(weatherType: number, p1: boolean, p2: boolean, transition: boolean, transitionTime: number, p5: boolean): void;
    export function setWindDirection(direction: number): void;
    export function setWindSpeed(speed: number): void;
    export function shootSingleBulletBetweenCoords(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, damage: number, p7: boolean, weaponHash: number, ownerPed: number, isAudible: boolean, isInvisible: boolean, speed: number, p13: boolean): void;
    /** Returns whether the game's measurement system is set to metric. */
    export function shouldUseMetricMeasurements(): boolean;
    /** Begins with START_*. Next character in the name is either D or E.  Old name: _START_BENCHMARK_RECORDING */
    export function startEndUserBenchmark(): void;
    /** nullsub, doesn't do anything */
    export function stopCurrentLoadingProgressTimer(): void;
    /** Begins with STOP_*. Next character in the name is either D or E.  Old name: _STOP_BENCHMARK_RECORDING */
    export function stopEndUserBenchmark(): void;
    /** Returns false if it's a null or empty string or if the string is too long. outInteger will be set to -999 in that case. */
    export function stringToInt(string: string, outInteger: number): boolean;
    export function tan(p0: number): number;
    /** Hardcoded to return false.  Old name: _UI_IS_SINGLEPLAYER_PAUSE_MENU_ACTIVE */
    export function uiStartedEndUserBenchmark(): boolean;
    export function unregisterInteractionLockonPrompt(entity: number): boolean;
    /** Returns the current status of the onscreen keyboard, and updates the output.  Status Codes:  0 - User still editing 1 - User has finished editing 2 - User has canceled editing 3 - Keyboard isn't ac... */
    export function updateOnscreenKeyboard(): number;
    /** Note: The first bit in 'flags' must not be set. It is also required to pass at least one extra argument (this must be a text label string or hash). When passing a hash, flags should be 0. */
    export function varString(flags: number, ...args: any[]): NativeString;
    /** Only used in smuggler2 script Also see weather.xml (OceanWaveMaxAmplitude) */
    export function waterOverrideSetOceanwavemaxamplitude(maxAmplitude: number): void;
    /** Only used in smuggler2 script Also see weather.xml (ShoreWaveAmplitude) */
    export function waterOverrideSetShorewaveamplitude(amplitude: number): void;
    export function addDispatchSpawnBlockingArea(volume: number): any;
    /** Only used in script function PROCESS_ZONE_CREATION Returns Pop multiplier volume ID */
    export function addPopMultiplierVolume(volume: number, pedDensity: number, vehicleDensity: number, p3: boolean, p4: boolean): number;
    export function clearAllBitFlags(bitFlags: any): void;
    export function clearBitFlag(bitFlags: any, flag: number): void;
    export function clearVolumeArea(volume: number, flag: number): void;
    export function clearWeatherTypePersistOvertime(milliseconds: number): void;
    export function clearWeatherVariation(weatherType: string, p1: boolean): void;
    export function countBitFlags(bitFlags: any): number;
    /** aiMemoryType: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/_CREATE_AI_MEMORY */
    export function createAiMemory(args: any, aiMemoryType: number): void;
    /** Returns a formatted string (0x%x) */
    export function createColorString(rgb: number): NativeString;
    /** dispatchService: see ENABLE_DISPATCH_SERVICE  The entities must be added to itemSet. */
    export function createIncidentWithEntities(dispatchService: number, x: number, y: number, z: number, itemSet: number, radius: number, outIncidentID: number): boolean;
    /** Disables composite pick prompt. */
    export function disableLootingCompositePickableThisFrame(compositeId: number, p1: boolean): void;
    export function doesItemHaveValidBase(item: number): boolean;
    export function doesPopMultiplierAreaExistForVolume(volume: number): boolean;
    export function doesStringExistInString(string1: string, string2: string): boolean;
    /** p3 is always -1.0f in the scripts */
    export function forceLightningFlashAtCoords(x: number, y: number, z: number, p3: number): void;
    export function gameFrameworkManagerGetMode(): number;
    export function gameFrameworkManagerShutdown(): void;
    export function getAiPedDoesHaveEventMemory(args: any, p1: number): boolean;
    /** https://easings.net/  enum class eEasingCurveType { 	TYPE_LINEAR, 	TYPE_QUADRATIC_IN, 	TYPE_QUADRATIC_OUT, 	TYPE_QUADRATIC_INOUT, 	TYPE_CUBIC_IN, 	TYPE_CUBIC_OUT, 	TYPE_CUBIC_INOUT, 	TYPE_QUARTIC_I... */
    export function getEasingCurveValue(t: number, b: number, d: number, easingCurveType: number): number;
    export function getEntityFromItem(item: number): number;
    /** Returns the weather type that has been set by a script */
    export function getForcedWeather(weather: number, p1: number): void;
    /** Returns rage::fwTimer::sm_nonScaledClippedTime */
    export function getGameTimerNonScaledClipped(): number;
    /** Raycasts downward from coords to find terrain/water.  - flags: collision mask (R* scripts commonly use 17, 129, or 3423). - outGroundZ: receives the hit Z. - outMaterialHash: receives the surface/m... */
    export function getGroundZAndMaterialFor3dCoord(x: number, y: number, z: number, flags: number, outGroundZ: number, outMaterialHash: number, outFlags: number): boolean;
    /** 0 = invalid 1 = CEntity 2 = rage::volBase 3 = rage::volSphere 4 = rage::volBox 5 = rage::volAggregate 6 = rage::volCylinder 7 = CScriptedCoverPoint 8 = rage::ptfxScriptInfo 9 = CPed 10 = CVehicle 1... */
    export function getItemType(handle: number): number;
    /** Event names in the scripts: MGBegin, MGEnd, ReadyForCut */
    export function getLootingEventHasFired(ped: number, eventName: string): boolean;
    export function getMaxNumInstructions(): number;
    export function getNextWeatherTypeHashName(): number;
    export function getNumberOfInstructions(): number;
    export function getObjectFromIndexedItem(item: number): number;
    export function getPedFromIndexedItem(item: number): number;
    export function getPrevWeatherTypeHashName(): number;
    export function getRandomWeatherType(): number;
    export function getRandomWeatherTypeIndex(): number;
    /** Only 0 and 1 are valid for p0, higher values causes the native to return 2. */
    export function getStatusOfSavegameOperation(p0: number): number;
    export function getStringFromBool(value: boolean): NativeString;
    export function getStringFromFloat(value: number, digits: number): NativeString;
    /** Returns a string in the following format: <<%.4f,%.4f,%.4f>> */
    export function getStringFromVector(x: number, y: number, z: number): NativeString;
    export function getTemperatureAtCoords(x: number, y: number, z: number): number;
    export function getVehicleFromIndexedItem(item: number): number;
    export function getVolumeFromIndexedItem(item: number): number;
    /** Note: the buffer should be exactly 32 bytes long */
    export function intToString(value: number, format: string, buffer: string): void;
    export function isAnyBitFlagSet(bitFlags: any): boolean;
    export function isBaseACoverPoint(handle: number): boolean;
    export function isBaseAPersistentCharacter(handle: number): boolean;
    export function isBitFlagSet(bitFlags: any, flag: number): boolean;
    export function isGlobalBlockValid(index: number): boolean;
    export function isMissionCreatorActive(): boolean;
    export function isPedDecomposed(ped: number): boolean;
    export function isPlayerOwningStandaloneSp(): boolean;
    export function lootTablesGetInfo(ped: number, p1: boolean, p2: boolean, lootTableKey: number, p4: any, p5: any): void;
    /** p0 must be < 2 */
    export function queueSavegameOperation(p0: number): boolean;
    /** Reads the passed value as floating point value and returns it. Example: _READ_INT_AS_FLOAT(0x3F800000) returns 1.0f because 0x3F800000 is the hexadecimal representation of 1.0f. */
    export function readIntAsFloat(value: number): number;
    export function removePopMultiplierAreaForVolume(volume: number, p1: number): void;
    export function resetDispatchMaxSpawnDistance(): void;
    export function resetDispatchMinSpawnDistance(): void;
    /** Used in CAIConditionAmbientAIMemoryReactionsEnabled */
    export function setAiMemoryReactionsEnabled(enabled: boolean): void;
    /** Similar to SET_BIT but specifically designed for large (>32 flags) bit flag sets. The flags are stored in an int array where each int has the ability to hold 32 flags. Flags 0-31 would be stored in... */
    export function setBitFlag(bitFlags: any, flag: number): void;
    export function setDispatchMaxSpawnDistance(maxSpawnDistance: number): void;
    export function setDispatchMinSpawnDistance(minSpawnDistance: number): void;
    /** Note: this native was added in build 1232.56 */
    export function setGameLogicPaused(): void;
    export function setGlobalBlockIsLoaded(index: number, toggle: boolean): void;
    export function setIncidentUnk(incidentId: number): void;
    export function setLootPeltSatchelItem(ped: number, item: any): void;
    export function setOverrideWeather(weatherType: number): void;
    export function setSnowLevel(level: number): void;
    export function setWeatherType2(weatherType: number, p1: number, p2: number, p3: number, p4: boolean): void;
    export function setWeatherTypeFrozen(toggle: boolean): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/weather/weather_variations.lua */
    export function setWeatherVariation(weatherType: string, variation: string): void;
    export function shouldUse24HourClock(): boolean;
    /** Same as SHOULD_USE_METRIC_MEASUREMENTS */
    export function shouldUseMetricMeasurements2(): boolean;
    export function shouldUseMetricTemperature(): boolean;
    export function shouldUseMetricWeight(): boolean;
    /** Counts the number of segments in a string separated by specified delimiters, ignoring consecutive delimiters. Example usage: int count = MISC::_STRING_SPLIT_AND_COUNT_SEGMENTS("qadr_ui-qadr_ui;qadr... */
    export function stringSplitAndCountSegments(inputString: string, delimiters: string): number;

    // MISSIONDATA
    export function missiondataGetCatagory(missionId: number): number;
    export function missiondataGetHighScore(missionId: number): number;
    export function missiondataGetRating(missionId: number): number;
    export function missiondataGetReplayState(p0: any): number;
    export function missiondataGetTextureName(missionId: number): number;
    export function missiondataGetTextureTxd(missionId: number): number;
    export function missiondataIsRequiredStoryMission(missionId: number): boolean;
    export function missiondataIsValid(p0: any): boolean;
    export function missiondataSetHighScore(missionId: number, score: number): void;
    export function missiondataSetRatingScores(missionId: number, bronzeScore: number, silverScore: number, goldScore: number): void;
    export function missiondataSetReplayLockedForCategory(category: number, locked: boolean): void;
    /** see: missions.meta */
    export function missiondataWasCompleted(missionId: number): boolean;
    export function missiondataIsReplayCategoryLocked(category: number): boolean;
    /** MISSION_RATING_INCOMPLETE = 0, MISSION_RATING_SKIPPED, MISSION_RATING_COMPLETE, MISSION_RATING_BRONZE, MISSION_RATING_SILVER, MISSION_RATING_GOLD, */
    export function missiondataSetMissionRating(missionId: number, rating: number): void;
    /** replayState: MISSIONDATA_GET_REPLAY_STATE */
    export function missiondataSetReplayStateLocked(missionId: number, replayState: number): void;
    export function missiondataTimecycleBoxDelete(): void;
    export function missiondataTimecycleBoxExists(): boolean;
    export function missiondataTimecycleBoxSetModifier(timecycleName: string): void;

    // MONEY
    export function moneyDecrementCashBalance(amount: number): boolean;
    export function moneyGetCashBalance(): number;
    export function moneyIncrementCashBalance(amount: number, addReason: number): boolean;
    export function networkGetCashBalance(): number;
    export function networkGetStringCashBalance(): NativeString;
    export function networkIsMoneyBalanceNotLessThan(cashBalance: number, goldBarBalance: number): boolean;

    // NETSHOPPING
    export function cashinventoryInitSessionStatus(p0: number, p1: number): boolean;
    export function cashinventoryIsConnectionFaulted(): boolean;
    export function cashinventoryInitSessionIsFaulted(): boolean;
    export function cashinventoryIsSessionReady(): boolean;
    export function cashinventoryTransactionAddAward(id: number, hash: number, p2: any, p3: any): boolean;
    export function cashinventoryTransactionCheckout(id: number): boolean;
    export function cashinventoryTransactionCheckoutStatus(id: number, status: number): boolean;
    export function cashinventoryTransactionDelete(id: number): boolean;
    export function cashinventoryTransactionFireAndForgetItem(actionHash: number, id: number, item: any, p3: number): boolean;
    export function cashinventoryTransactionGetAction(id: number): number;
    export function cashinventoryTransactionGetBasketIsValid(id: number): boolean;
    export function cashinventoryTransactionGetItemInfo(id: number, index: number, itemInfo: any): boolean;
    export function cashinventoryTransactionGetNumOfItems(id: number): number;
    export function cashinventoryTransactionResponseGetItemInfo(id: number, index: number, itemInfo: any): boolean;
    export function cashinventoryTransactionStart(id: number, type: number, actionHash: number): boolean;
    export function cashinventoryTransactionValidateItem(p0: number, p1: any): number;

    // NETWORK
    export function activateDamageTrackerOnNetworkId(netID: number, toggle: boolean): void;
    export function canRegisterMissionEntities(ped_amt: number, vehicle_amt: number, object_amt: number, pickup_amt: number): boolean;
    export function canRegisterMissionObjects(amount: number): boolean;
    export function canRegisterMissionPeds(amount: number): boolean;
    export function canRegisterMissionPickups(amount: number): boolean;
    export function canRegisterMissionVehicles(amount: number): boolean;
    /** Old name: _CLEAR_LAUNCH_PARAMS */
    export function clearServiceEventArguments(): void;
    export function cloudDidRequestSucceed(id: number): boolean;
    export function cloudHasRequestCompleted(id: number): boolean;
    /** Takes the specified time and writes it to the structure specified in the second argument.  struct date_time {     int year;     int PADDING1;     int month;     int PADDING2;     int day;     int P... */
    export function convertPosixTime(posixTime: number, timeStructure: any): void;
    export function getCloudTimeAsInt(): number;
    export function getLaunchParamValue(paramName: string): NativeString;
    /** Always returns 60 */
    export function getMaxNumNetworkObjects(): number;
    /** Always returns 110 */
    export function getMaxNumNetworkPeds(): number;
    /** Always returns 80 */
    export function getMaxNumNetworkPickups(): number;
    /** Always returns 40 */
    export function getMaxNumNetworkVehicles(): number;
    export function getNetworkTime(): number;
    export function getNetworkTimeAccurate(): number;
    export function getNumCreatedMissionObjects(p0: boolean): number;
    export function getNumCreatedMissionPeds(p0: boolean): number;
    export function getNumCreatedMissionVehicles(p0: boolean): number;
    /** p0 appears to be for MP */
    export function getNumReservedMissionObjects(p0: boolean): number;
    /** p0 appears to be for MP */
    export function getNumReservedMissionPeds(p0: boolean): number;
    /** p0 appears to be for MP */
    export function getNumReservedMissionVehicles(p0: boolean): number;
    /** Used in Script Function NET_ACE_CLIENT_VERIFY_ENTITY_RESERVATIONS Coords: Slot world position  Old name: _GET_RESERVATIONS_FOR_SLOT_WORLD_POSITION */
    export function getReservedMissionEntitiesInArea(x: number, y: number, z: number, p3: boolean, peds: number, vehicles: number, objects: number, pickups: number): void;
    /** 0 = succeeded 1 = pending 2 = failed */
    export function getStatusOfTextureDownload(textureDownloadId: number): number;
    /** Subtracts the second argument from the first. */
    export function getTimeDifference(timeA: number, timeB: number): number;
    /** Adds the first argument to the second. */
    export function getTimeOffset(timeA: number, timeB: number): number;
    export function getUniqueIntForPlayer(player: number): number;
    export function hasNetworkTimeStarted(): boolean;
    export function isDamageTrackerActiveOnNetworkId(netID: number): boolean;
    /** Old name: _IS_ENTITY_GHOSTED_TO_LOCAL_PLAYER */
    export function isEntityAGhost(entity: number): boolean;
    export function isNetworkIdOwnedByParticipant(netId: number): boolean;
    /** Note: this native was added in build 1311.16 */
    export function isObjectReassignmentInProgress(): boolean;
    export function isSphereVisibleToAnotherMachine(p0: any, p1: any, p2: any, p3: any, p4: any): boolean;
    export function isSphereVisibleToPlayer(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any): boolean;
    /** Subtracts the second argument from the first, then returns whether the result is negative. */
    export function isTimeLessThan(timeA: number, timeB: number): boolean;
    /** Subtracts the first argument from the second, then returns whether the result is negative. */
    export function isTimeMoreThan(timeA: number, timeB: number): boolean;
    export function keepNetworkIdInFastInstance(netId: number, p1: boolean, p2: number): void;
    export function networkAcceptRsInvite(p0: number): boolean;
    export function networkAccessTunableBool(tunableContext: number, tunableName: number): boolean;
    export function networkAccessTunableInt(tunableContext: number, tunableName: number, value: number): boolean;
    export function networkActionPlatformInvite(): boolean;
    export function networkActivityResetToIdle(): void;
    export function networkActivitySetCurrent(netPlaylistActivity: number): void;
    export function networkAddFriend(gamerHandle: any, message: string): boolean;
    export function networkAllowAllEntityFadingForInstances(toggle: boolean): void;
    export function networkAllowEntityFadingForInstances(entity: number, toggle: boolean): void;
    /** Old name: _NETWORK_ALLOW_LOCAL_ENTITY_ATTACHMENT */
    export function networkAllowRemoteAttachmentModification(entity: number, toggle: boolean): void;
    export function networkAreHandlesTheSame(gamerHandle1: any, gamerHandle2: any): boolean;
    /** Old name: _NETWORK_IS_PLAYER_EQUAL_TO_INDEX */
    export function networkArePlayersInSameTutorialSession(player: number, index: number): boolean;
    export function networkAutoSessionCanSplitSession(p0: number): boolean;
    export function networkAutoSessionFinishInstance(): void;
    export function networkAutoSessionIsAllowedToMerge(): boolean;
    export function networkAutoSessionIsObjectCreationPaused(): boolean;
    export function networkAutoSessionSplitSession(playersToTake: number, maxInstancePlayers: number, sessionFlags: number, bucketId: number): boolean;
    export function networkAwardHasReachedMaxclaim(p0: any): boolean;
    export function networkCanAccessMultiplayer(loadingState: number): boolean;
    export function networkCanRefreshFriendPage(): boolean;
    export function networkCanSessionEnd(): boolean;
    export function networkCanViewGamerUserContent(gamerHandle: any): boolean;
    export function networkCheckAccessAndAlertIfFail(): boolean;
    export function networkCheckCommunicationPrivileges(p0: number): boolean;
    export function networkCheckUserContentPrivileges(p0: number): boolean;
    export function networkClearClockTimeOverride(): void;
    export function networkClearFoundGamers(): void;
    export function networkClearGetGamerStatus(): void;
    export function networkClearPlatformInvite(): void;
    export function networkConcealPlayer(player: number, toggle: boolean): void;
    export function networkDidFindGamersSucceed(): boolean;
    export function networkDidGetGamerStatusSucceed(): boolean;
    export function networkDisableLeaveRemotePedBehind(toggle: boolean): void;
    export function networkDisableProximityMigration(netID: number): void;
    export function networkDisableRealtimeMultiplayer(): void;
    /** Hardcoded to return -1. */
    export function networkDisplaynamesFromHandlesStart(p0: any, p1: any): number;
    export function networkDoesNetworkIdExist(netID: number): boolean;
    export function networkDoesTunableExist(tunableContext: number, tunableName: number): boolean;
    /** nullsub, doesn't do anything */
    export function networkDumpNetIfConfig(): void;
    export function networkEndTutorialSession(): void;
    export function networkGetAssistedDamageOfEntity(player: number, entity: number, p2: number): boolean;
    /** Old name: _NETWORK_GET_AVERAGE_LATENCY_FOR_PLAYER */
    export function networkGetAverageLatency(player: number): number;
    /** Old name: _NETWORK_GET_AVERAGE_PACKET_LOSS_FOR_PLAYER */
    export function networkGetAveragePacketLoss(player: number): number;
    /** Same as NETWORK_GET_AVERAGE_LATENCY (0xD414BE129BB81B32)  Old name: _NETWORK_GET_AVERAGE_LATENCY_FOR_PLAYER_2 */
    export function networkGetAveragePing(player: number): number;
    export function networkGetDestroyerOfNetworkId(netId: number, weaponHash: number): number;
    /** Hardcoded to return zero. */
    export function networkGetDisplaynamesFromHandles(p0: any, p1: any, p2: any): number;
    export function networkGetEntityFromNetworkId(netId: number): number;
    export function networkGetEntityIsNetworked(entity: number): boolean;
    export function networkGetEntityKillerOfPlayer(player: number, weaponHash: number): number;
    /** Always returns a null string. */
    export function networkGetGamertagFromHandle(gamerHandle: any): NativeString;
    export function networkGetGameMode(): number;
    export function networkGetGlobalClock(hour: number, minute: number, second: number): boolean;
    export function networkGetGlobalMultiplayerClock(hours: number, minutes: number, seconds: number): void;
    /** Old name: _NETWORK_GET_OLDEST_RESEND_COUNT_FOR_PLAYER */
    export function networkGetHighestReliableResendCount(player: number): number;
    export function networkGetHostOfScript(scriptName: string, p1: number, p2: number): number;
    export function networkGetHostOfThisScript(): number;
    export function networkGetHostOfThread(threadId: number): number;
    export function networkGetInstanceIdOfThisScript(): number;
    export function networkGetLocalHandle(gamerHandle: any): void;
    /** Seems to always return 0, but it's used in quite a few loops.  for (num3 = 0; num3 < NETWORK::0xCCD8C02D(); num3++)     {         if (NETWORK::NETWORK_IS_PARTICIPANT_ACTIVE(PLAYER::0x98F3B274(num3)... */
    export function networkGetMaxNumParticipants(): number;
    export function networkGetNetworkIdFromEntity(entity: number): number;
    export function networkGetNetworkIdFromRopeId(ropeId: number): number;
    /** nullsub, doesn't do anything */
    export function networkGetNetStatisticsInfo(): void;
    /** Hardcoded to return zero.  ==== PS4 specific info ====  Returns some sort of unavailable reason: -1 = REASON_INVALID  0 = REASON_OTHER  1 = REASON_SYSTEM_UPDATE  2 = REASON_GAME_UPDATE  3 = REASON_... */
    export function networkGetNpUnavailableReason(): number;
    /** Returns the amount of players connected in the current session. Only works when connected to a session/server. */
    export function networkGetNumConnectedPlayers(): number;
    export function networkGetNumParticipants(): number;
    export function networkGetNumScriptParticipants(scriptName: string, instanceId: number, position: number): number;
    /** Old name: _NETWORK_GET_NUM_UNACKED_FOR_PLAYER */
    export function networkGetNumUnackedReliables(player: number): number;
    export function networkGetParticipantIndex(index: number): number;
    export function networkGetPlayerFromGamerHandle(gamerHandle: any): number;
    export function networkGetPlayerIndex(player: number): number;
    /** Returns the Player associated to a given Ped when in an online session. */
    export function networkGetPlayerIndexFromPed(ped: number): number;
    /** Hardcoded to return zero. */
    export function networkGetPromotionDlgSeenCount(): number;
    export function networkGetRandomIntRanged(rangeStart: number, rangeEnd: number): number;
    export function networkGetRecentGamerNames(p0: number, p1: number, outData: any, dataSize: number): boolean;
    export function networkGetRopeIdFromNetworkId(netId: number): number;
    export function networkGetScriptStatus(): number;
    export function networkGetThisScriptIsNetworkScript(): boolean;
    export function networkGetTimeoutTime(): number;
    export function networkGetTotalNumFriends(): number;
    export function networkGetTotalNumPlayers(): number;
    export function networkGetTunableCloudCrc(): number;
    /** Old name: _NETWORK_GET_UNRELIABLE_RESEND_COUNT_FOR_PLAYER */
    export function networkGetUnreliableResendCount(player: number): number;
    export function networkHandleFromFriend(friendIndex: number, gamerHandle: any): void;
    export function networkHandleFromPlayer(player: number, gamerHandle: any): void;
    export function networkHashFromPlayerHandle(player: number): number;
    export function networkHasControlOfEntity(entity: number): boolean;
    export function networkHasControlOfNetworkId(netId: number): boolean;
    export function networkHasControlOfPickup(pickup: number): boolean;
    export function networkHasControlOfPickupPlacement(p0: any): boolean;
    export function networkHasEntityBeenRegisteredWithThisThread(entity: number): boolean;
    export function networkHasPendingInviteFailure(): boolean;
    export function networkHasReceivedHostBroadcastData(): boolean;
    export function networkHasRosPrivilege(index: number): boolean;
    export function networkHasSocialClubAccount(): boolean;
    /** Returns whether the signed-in user has valid Rockstar Online Services (ROS) credentials. */
    export function networkHasValidRosCredentials(): boolean;
    export function networkHaveOnlinePrivileges(): boolean;
    export function networkHaveRosBannedPriv(): boolean;
    export function networkIsAimCamActive(player: number): boolean;
    export function networkIsClockTimeOverridden(): boolean;
    export function networkIsCloudAvailable(): boolean;
    /** Old name: _NETWORK_IS_CONNECTION_ENDPOINT_RELAY_SERVER */
    export function networkIsConnectedViaRelay(player: number): boolean;
    /** Hardcoded to return false. */
    export function networkIsCustomUpsellEnabled(): boolean;
    export function networkIsFeatureSupported(featureId: number): boolean;
    export function networkIsFindingGamers(): boolean;
    export function networkIsFriend(gamerHandle: any): boolean;
    export function networkIsGamerInMySession(gamerHandle: any): boolean;
    export function networkIsGameInProgress(): boolean;
    export function networkIsHandleValid(gamerHandle: any): boolean;
    /** If you are host, returns true else returns false. */
    export function networkIsHost(): boolean;
    export function networkIsHostOfThisScript(): boolean;
    export function networkIsInMpCutscene(): boolean;
    /** Hardcoded to return false. */
    export function networkIsInPlatformParty(): boolean;
    /** Hardcoded to return false. */
    export function networkIsInPlatformPartyChat(): boolean;
    export function networkIsInSession(): boolean;
    export function networkIsInSpectatorMode(): boolean;
    export function networkIsInTutorialSession(): boolean;
    export function networkIsParticipantActive(p0: number): boolean;
    /** Hardcoded to return false. */
    export function networkIsPendingFriend(gamerHandle: any): boolean;
    export function networkIsPlatformInvitePending(): boolean;
    export function networkIsPlayerActive(player: number): boolean;
    export function networkIsPlayerAParticipant(player: number): boolean;
    export function networkIsPlayerAParticipantOnScript(p0: number, p1: any, p2: any): boolean;
    export function networkIsPlayerConcealed(player: number): boolean;
    export function networkIsPlayerConnected(player: number): boolean;
    /** Returns true if the passed value is less than 32. */
    export function networkIsPlayerIndexValid(player: number): boolean;
    /** Note: scripts seem to indicate that this was renamed to NETWORK_IS_PLAYER_IN_MP_FAST_INSTANCE */
    export function networkIsPlayerInMpCutscene(player: number): boolean;
    /** Hardcoded to return false. */
    export function networkIsPromotionEnabled(): boolean;
    export function networkIsResettingPopulation(): boolean;
    export function networkIsScriptActive(scriptName: string, p1: number, p2: boolean, p3: number): boolean;
    export function networkIsScriptActiveByHash(scriptHash: number, p1: number, p2: boolean, p3: number): boolean;
    export function networkIsSessionActive(): boolean;
    export function networkIsSessionStarted(): boolean;
    export function networkIsSignedOnline(): boolean;
    export function networkIsTunableCloudRequestPending(): boolean;
    export function networkIsTutorialSessionChangePending(): boolean;
    export function networkPreventScriptHostMigration(): void;
    export function networkRefreshCurrentFriendPage(): boolean;
    export function networkRegisterEntityAsNetworked(entity: number): void;
    export function networkRegisterHostBroadcastVariables(p0: any, p1: any, p2: any): void;
    export function networkRegisterPlayerBroadcastVariables(p0: any, p1: any, p2: any): void;
    /** Note: this native was added in build 1311.23, but was only used after build 1436.25 */
    export function networkRequestCloudTunables(): void;
    export function networkRequestControlOfEntity(entity: number): boolean;
    export function networkRequestControlOfNetworkId(netId: number): boolean;
    export function networkRequestControlOfPickupPlacement(p0: any): boolean;
    export function networkRequestJoin(p0: any): number;
    export function networkRequestRecentGamerNames(p0: number, playerCount: number): boolean;
    /** flags: enum eSessionRequestOptionFlags { 	SESSION_REQUEST_OPTION_FLAG_INCLUDE_GANG_MEMBERS = (1 << 1), 	SESSION_REQUEST_OPTION_FLAG_LEADER_KEEPS_GANG = (1 << 7), };  seamlessType: enum eSeamlessTyp... */
    export function networkRequestSessionSeamless(flags: number, seamlessType: number, sessionRequestId: any): boolean;
    export function networkResetPopulation(p0: boolean, p1: number): boolean;
    export function networkResurrectLocalPlayer(x: number, y: number, z: number, heading: number, p4: number, p5: boolean, p6: any, p7: boolean): void;
    export function networkSeedRandomNumberGenerator(seed: number): void;
    export function networkSessionGetSessionFlags(): number;
    export function networkSessionGetSessionType(): number;
    export function networkSessionIsAnyRequestInProgress(): boolean;
    /** Checks for session flag 'SF_PRIVATE' */
    export function networkSessionIsPrivate(): boolean;
    export function networkSessionIsRequestInProgress(sessionRequestId: any): boolean;
    export function networkSessionIsRequestPendingTransition(sessionRequestId: any): boolean;
    export function networkSessionIsSessionRequestIdValid(sessionRequestId: any): boolean;
    export function networkSessionIsTransitioning(): boolean;
    export function networkSessionLeaveSession(): boolean;
    export function networkSessionLeftQueueOrRequestedSession(sessionRequestId: any): boolean;
    /** See _NETWORK_SESSION_ADD_SESSION_FLAGS */
    export function networkSessionRemoveSessionFlags(flags: number): boolean;
    /** matchType: enum eMatchType { 	MATCHTYPE_DEPRECATED, 	MATCHTYPE_UGCPLAYLIST, 	MATCHTYPE_UGCMISSION, 	MATCHTYPE_MINIGAME, 	MATCHTYPE_SEAMLESS, 	MATCHTYPE_PRIVATE_DO_NOT_USE }; */
    export function networkSessionRequestSessionCompetitive(flags: number, matchType: number, userHash: number, p3: number, sessionRequestId: any): boolean;
    /** Session flag 'SF_PRIVATE' is set internally p1 represents max amount of players in private session */
    export function networkSessionRequestSessionPrivate(flags: number, numPlayers: number, userHash: number, sessionRequestId: any): boolean;
    /** Equivalent to NETWORK_REQUEST_SESSION_SEAMLESS if userHash == 0. Otherwise it is equivalent to NETWORK_SESSION_REQUEST_SESSION_COMPETITIVE(flags, MATCHTYPE_SEAMLESS, userHash, 0, sessionRequestId);... */
    export function networkSessionRequestSessionSeamless(flags: number, seamlessType: number, userHash: number, sessionRequestId: any): boolean;
    export function networkSetCompletedMpIntroFlowOnCurrentSlot(completed: boolean): boolean;
    /** if set to true other network players can't see it if set to false other network player can see it ========================================= ^^ I attempted this by grabbing an object with GET_ENTITY... */
    export function networkSetEntityOnlyExistsForParticipants(entity: number, toggle: boolean): void;
    export function networkSetEntityRemainsWhenUnnetworked(entity: number, toggle: boolean): void;
    export function networkSetFriendlyFireOption(toggle: boolean): void;
    export function networkSetInMpCutscene(p0: boolean, p1: boolean, p2: number, p3: boolean): void;
    export function networkSetInSpectatorMode(toggle: boolean, playerPed: number): void;
    export function networkSetLocalPlayerInvincibleTime(time: number): void;
    export function networkSetLocalPlayerPendingFastInstanceId(instanceId: number): void;
    export function networkSetLocalPlayerSyncLookAt(toggle: boolean): void;
    export function networkSetMissionFinished(): void;
    export function networkSetMpMissionFlagOnCurrentSlot(enabled: boolean, flagIndex: number): boolean;
    /** Old name: _NETWORK_SET_PASSIVE_MODE_OPTION */
    export function networkSetPlayerIsPassive(toggle: boolean): void;
    export function networkSetRecentGamersEnabled(toggle: boolean): void;
    export function networkSetRichPresence(p0: number, p1: any, p2: number, p3: number): void;
    export function networkSetScriptReadyForEvents(toggle: boolean): void;
    export function networkSetThisScriptIsNetworkScript(maxNumMissionParticipants: number, p1: boolean, instanceId: number): void;
    /** Hardcoded to return false. */
    export function networkShouldShowPromotionDlg(): boolean;
    export function networkShowAccountUpgradeUi(): void;
    /** nullsub, doesn't do anything */
    export function networkShowChatRestrictionMsc(player: number): void;
    export function networkShowProfileUi(gamerHandle: any): void;
    /** nullsub, doesn't do anything */
    export function networkShowPsnUgcRestriction(): void;
    export function networkSpawnConfigSetFlags(flags: number): void;
    export function networkSpawnConfigSetGroundToRootOffset(offset: number): void;
    export function networkSpawnConfigSetTuningFloat(p0: number, p1: number): void;
    export function networkStartSoloTutorialSession(): void;
    /** Always returns -1. Seems to be XB1 specific. */
    export function networkStartUserContentPermissionsCheck(gamerHandle: any): number;
    /** Old name: _NETWORK_SET_VEHICLE_WHEELS_DESTRUCTIBLE */
    export function networkTriggerDamageEventForZeroDamage(entity: number, p1: boolean): void;
    export function networkTryAccessTunableBoolHash(tunableContext: number, tunableName: number, defaultValue: boolean): boolean;
    export function networkTryAccessTunableFloatHash(tunableContext: number, tunableName: number, defaultValue: number): number;
    export function networkTryAccessTunableIntHash(tunableContext: number, tunableName: number, defaultValue: number): number;
    /** gets the entity id of a network id */
    export function netToEnt(netHandle: number): number;
    /** gets the object id of a network id */
    export function netToObj(netHandle: number): number;
    /** gets the ped id of a network id */
    export function netToPed(netHandle: number): number;
    export function netToVeh(netHandle: number): number;
    /** Returns the network ID of the given object. */
    export function objToNet(object: number): number;
    /** Return the local Participant ID */
    export function participantId(): number;
    /** Return the local Participant ID.  This native is exactly the same as 'PARTICIPANT_ID' native. */
    export function participantIdToInt(): number;
    /** Returns the network ID of the given ped. */
    export function pedToNet(ped: number): number;
    export function preventMigrationOfEntitiesInFastInstanceForLocalPlayer(toggle: boolean): void;
    export function preventNetworkIdMigration(netId: number): void;
    export function reserveNetworkClientMissionObjects(amount: number): void;
    export function reserveNetworkClientMissionPeds(amount: number): void;
    export function reserveNetworkMissionObjects(amount: number): void;
    export function reserveNetworkMissionPeds(amount: number): void;
    export function reserveNetworkMissionPickups(amount: number): void;
    export function reserveNetworkMissionVehicles(amount: number): void;
    export function setEntityVisibleInCutscene(entity: number, p1: boolean, p2: boolean, p3: number): void;
    /** Old name: _SET_LOCAL_PLAYER_AS_GHOST */
    export function setLocalPlayerAsGhost(toggle: boolean): void;
    export function setLocalPlayerInvisibleLocally(p0: boolean): void;
    export function setLocalPlayerVisibleInCutscene(local: boolean, remote: boolean, instanceId: number): void;
    export function setNetworkIdAlwaysExistsForPlayer(netId: number, player: number, toggle: boolean): void;
    export function setNetworkIdExistsOnAllMachines(netId: number, toggle: boolean): void;
    export function setNetworkIdStopCloning(networkId: number, bStopCloning: boolean): void;
    export function setNetworkIdVisibleInCutscene(p0: any, p1: any, p2: any, p3: any): void;
    export function setPlayerInvisibleLocally(player: number, toggle: boolean): void;
    export function setPlayerVisibleLocally(player: number, toggle: boolean): void;
    export function textureDownloadGetName(textureDownloadId: number): NativeString;
    export function textureDownloadRelease(textureDownloadId: number): void;
    /** Returns textureDownloadId */
    export function textureDownloadRequest(gamerHandle: any, filePath: string, name: string, p3: boolean): number;
    export function ugcClearQueryResults(ugcRequestId: number): void;
    export function ugcDidDescriptionRequestSucceed(description: number): boolean;
    export function ugcDidRequestSucceed(ugcRequestId: number): boolean;
    export function ugcGetCachedDescription(description: number, length: number): NativeString;
    export function ugcHasDescriptionRequestFinished(description: number): boolean;
    export function ugcHasRequestFinished(ugcRequestId: number): boolean;
    export function ugcIsDescriptionRequestInProgress(description: number): boolean;
    export function ugcIsLanguageSupported(languageId: number): boolean;
    export function ugcIsRequestPending(ugcRequestId: number): boolean;
    export function ugcQueryGetContentHasPlayerRecord(p0: any, index: number): boolean;
    export function ugcQueryGetContentNum(ugcRequestId: number): number;
    export function ugcQueryWasForceCancelled(ugcRequestId: number): boolean;
    export function ugcReleaseAllCachedDescriptions(): void;
    export function ugcReleaseCachedDescription(description: number): boolean;
    export function ugcRequestCachedDescription(description: number): number;
    export function ugcRequestContentDataFromParams(contentTypeName: string, contentId: string, fileId: number, fileVersion: number, languageId: number): number;
    export function ugcSetQueryDataFromOffline(p0: boolean): void;
    export function ugcTextureDownloadRequest(p0: any, p1: any, p2: any, p3: any, p4: any, p5: boolean): number;
    /** Returns the network ID of the given vehicle. */
    export function vehToNet(vehicle: number): number;
    export function animSceneToNet(animScene: number): number;
    export function clearLaunchParam(paramName: string): void;
    export function commerceStoreIsEnabled(): boolean;
    export function commerceStoreIsOpen(): boolean;
    export function getLaunchParamExists(paramName: string): boolean;
    export function getLaunchParamString(): NativeString;
    export function getNumCreatedMissionPickups(p0: boolean): number;
    /** Note: this native was added in build 1311.16 */
    export function getNumPeerNegotiationResponses(): number;
    export function getNumReservedMissionPickups(p0: boolean): number;
    export function getPlayerWaypointIsActive(player: number): boolean;
    /** Only used in R* Script net_stable_manager */
    export function getReservedMissionEntitiesForThread(threadId: number, pedMax: number, vehicleMax: number, unkMax: number, pedMin: number, vehicleMin: number, unkMin: number): void;
    export function getSocialMatchmakingAllowed(): boolean;
    export function localPlayerPedshotTextureDownloadRequest(playerSlot: number, personaPhotoLocalCacheType: number): number;
    /** Returns textureDownloadId */
    export function mugshotTextureDownloadRequest(gamerHandle: any, p1: number, name: string, p3: boolean): number;
    export function networkAddPlayerToRecentGamersList(player: number, p1: number): void;
    export function networkAlert(ctx: number, lh: number, ec: number, h: number): void;
    /** Returns value of fwuiCachedSetting "general.onlineNotificationsInStoryMode" */
    export function networkAreOnlineNotificationsShownInStoryMode(): boolean;
    export function networkArePlayersInSamePlatformParty(gamerHandle1: any, gamerHandle2: any): boolean;
    export function networkAutoSessionIsAutoWarpDisabled(): boolean;
    export function networkAutoSessionIsInstancedSession(): boolean;
    export function networkAutoSessionIsProcessingSessionSplit(): boolean;
    export function networkAutoSessionSetAllowedToMerge(toggle: boolean, p1: any, p2: number): void;
    export function networkAutoSessionSetAllowedToSplit(toggle: boolean): void;
    export function networkAutoSessionSetAutoWarpEnabled(toggle: boolean): void;
    export function networkAutoSessionSplitSessionSuccessful(): boolean;
    /** On PC this returns true if gamerHandle is a valid handle. */
    export function networkCanAddFriend(gamerHandle: any): boolean;
    export function networkCanReceiveInviteFromHandle(gamerHandle: any): boolean;
    export function networkClearClockOverrideOvertime(milliseconds: number): void;
    export function networkClockTimeOverride(hour: number, minute: number, second: number, transitionTime: number, pauseClock: boolean): void;
    export function networkClockTimeOverride2(hour: number, minute: number, second: number, transitionTime: number, pauseClock: boolean, clockwise: boolean): void;
    /** Must be called from a background script, otherwise it will do nothing. */
    export function networkDebugRequestEntityPosition(p0: any): void;
    export function networkDidRecentGamerNamesRequestSucceed(): boolean;
    export function networkGetCurrentFriendPageData(p0: any): boolean;
    /** Example:  char displayName[64]; if (_NETWORK_GET_DISPLAY_NAME_FROM_HANDLE(handle, displayName)) { 	// use displayName } */
    export function networkGetDisplayNameFromHandle(gamerHandle: any, displayName: string): boolean;
    export function networkGetGamertagFromFriend(gamerHandle: any): NativeString;
    export function networkGetGamerSessionFromHandle(data: any, count: number): boolean;
    export function networkGetGamerStatus(gamerHandle: any, p1: number): number;
    export function networkGetGlobalEntityFlags(entity: number): number;
    export function networkGetInstanceIdOfThread(threadId: number): number;
    export function networkGetNumRecentGamers(): number;
    export function networkGetPlatformInviteId(): number;
    export function networkGetPlayerFastInstanceId(player: number): number;
    export function networkGetPlayerOwnerOfNetworkId(netId: number): number;
    export function networkGetRank(): number;
    /** Returns CGameConfig->ConfigOnlineServices->RosTitleName (see gameconfig.xml) */
    export function networkGetRosTitleName(): NativeString;
    export function networkGetSessionHost(): number;
    export function networkGetSizeOfHostBroadcastDataStorage(p0: number): number;
    export function networkGetSizeOfPlayerBroadcastDataStorage(p0: number): number;
    export function networkGetXp(): number;
    export function networkHasCompletedMpIntroFlowOnCurrentSlot(): boolean;
    export function networkHasControlOfAnimScene(animScene: number): boolean;
    export function networkHasCurrentGetGamerStatusStarted(): boolean;
    export function networkIsFriendHandleInSameTitle(gamerHandle: any): boolean;
    export function networkIsFriendHandleOnline(gamerHandle: any): boolean;
    /** Hardcoded to return false. */
    export function networkIsInSessionLobby(): boolean;
    export function networkIsPlayerInSpectatorMode(player: number): boolean;
    /** _NETWORK_IS_T* - _NETWORK_RE* */
    export function networkIsPreviousUploadPending(): boolean;
    export function networkIsRecentGamerNamesRequestInProgress(): boolean;
    export function networkIsThreadActive(threadId: number): boolean;
    export function networkIsTrackedPlayerVisible(player: number, trackedPlayer: number): boolean;
    /** Returns false if pedshot push failed */
    export function networkPersonaPhotoWriteLocal(texture: string, playerSlot: number, p2: number, personaPhotoLocalCacheType: number): boolean;
    /** Returns false if pedshot push failed */
    export function networkPersonaPhotoWriteScProfile(texture: string, personaPhotoType: number, formatIndex: number): boolean;
    export function networkRemoveFriend(gamerHandle: any): boolean;
    export function networkRequestControlOfAnimScene(animScene: number): boolean;
    export function networkResurrectLocalPlayer2(args: any): void;
    export function networkSendSessionInvite(gamerHandle: any, contentId: string, data: any, dataSize: number, p4: number, flags: number): boolean;
    /** enum eSessionFlags { 	SESSION_FLAG_NONE = 0, 	SF_INSTANCE = (1 << 0), 	SF_MATCH = (1 << 1), 	SF_PRIVATE = (1 << 2), 	SF_BLOCK_INVITES = (1 << 3), 	SF_BLOCK_JOIN_VIA_PRESENCE = (1 << 4), 	SF_BLOCK_N... */
    export function networkSessionAddSessionFlags(flags: number): boolean;
    /** Note: this native was added in build 1311.23 */
    export function networkSessionAreSessionIdsEqual(sessionId1: any, sessionId2: any): boolean;
    export function networkSessionCancelRequest(sessionRequestId: any): boolean;
    /** Note: this native was added in build 1311.23 */
    export function networkSessionGetSessionId(sessionId: any): void;
    /** Returns result of session request: 0 - NOT_FOUND 1 - IN_PROGRESS 2 - TIMEOUT 3 - PLAYER_OFFLINE 4 - GANG_MEMBERS_CHANGED 5 - PLAYER_CANCELLED 6 - PLAYER_SET_TOO_LARGE 7 - MATCH_ACCEPTED 8 - OTHER */
    export function networkSessionGetSessionRequestResult(sessionRequestId: any, p1: number): number;
    export function networkSessionIsNsrrSuccess(sessionRequestId: any): boolean;
    export function networkSessionIsRequestInProgressByQueueGroup(queueGroup: number): boolean;
    /** Only used in R* Script net_rolling_playlist */
    export function networkSessionPlaylistGetUpcomingContent(): void;
    /** Only used in R* Script net_rolling_playlist */
    export function networkSessionPlaylistGoToNextContent(): void;
    export function networkSessionRemovePlayerFlags(flags: number): boolean;
    export function networkSessionRequestSessionNominated(flags: number, userHash: number, p2: number, sessionRequestId: any): boolean;
    /** category: enum eOnCallType { 	NETWORK_SESSION_REQUEST_ON_CALL_TYPE_STORY = 2, 	NETWORK_SESSION_REQUEST_ON_CALL_TYPE_MATCH = 3 }; */
    export function networkSessionRequestSessionOnCall(flags: number, category: number, p2: any, userHash: number, sessionRequestId: any): boolean;
    export function networkSessionSetPlayerFlags(flags: number): boolean;
    /** Only used in R* Script startup_clip */
    export function networkSessionShutdown(): void;
    export function networkSessionTransitionToSession(sessionRequestId: any): boolean;
    export function networkSetInStaticSpectatorMode(toggle: boolean, x: number, y: number, z: number): void;
    export function networkSpawnConfigAddExclusionVolume(volume: number): void;
    export function networkSpawnConfigAddPropertyPreference(configProperty: number, include: boolean, weight: number): void;
    export function networkSpawnConfigAddPropertyScripted(configProperty: number, include: boolean): void;
    export function networkSpawnConfigAddSpawnPoint(x: number, y: number, z: number, heading: number): void;
    export function networkSpawnConfigRemoveExclusionVolume(volume: number): void;
    export function networkSpawnConfigSearchInProgress(): boolean;
    export function networkSpawnConfigSetCancelSearch(): void;
    export function networkSpawnConfigSetLevelWaterDepth(waterDepthLevel: number): void;
    export function netToAnimScene(netId: number): number;
    export function netToPropset(netId: number): number;
    export function pedmugshotGetStatus(): number;
    export function pedmugshotRequestSend(): any;
    export function pedmugshotTake(): boolean;
    export function propsetToNet(propSet: number): number;
    export function reportPlayer(player: number, reportType: number, description: string, horseName: string): void;
    /** Only used in R* SP Scripts */
    export function requestPedshotTextureLocalBackupDownload(player: number, personaPhotoLocalCacheType: number): NativeString;
    /** Only used in R* SP Script map_app_event_handler */
    export function requestPedshotTextureLocalDownload(gamerHandle: any, p1: number): NativeString;
    export function requestPedshotTextureMultiplayerDownload(gamerHandle: any, p1: number): NativeString;
    export function setDoorNetworked(doorHash: number): void;
    export function setDoorUnnetworked(p0: any, toggle: boolean): void;
    export function setEntityGhostedToLocalPlayer(entity: number, toggle: boolean): void;
    export function setLaunchParamString(params: string): void;
    export function setLaunchParamValue(paramName: string, value: string): void;
    export function setLocalPlayerDamageMultiplierForPlayer(player: number, damageMultiplier: number): void;
    export function setNetworkRespotTimer(entity: number, timer: number, p2: boolean): void;
    /** _SET_PLAYER_V* - _SET_S* */
    export function setPlayerVisibilityToLocalPlayerDisabled(player: number, disabled: boolean): void;
    export function setSocialMatchmakingAllowed(toggle: boolean): void;
    export function textureDownloadReleaseByName(name: string): void;
    export function textureDownloadTextureNameIsValid(name: string): boolean;
    /** Checks if the user has ROS privilege 14. */
    export function ugcHasPrivilege(): boolean;
    export function ugcIsBookMarked(contentId: string): boolean;
    /** Returns ugcRequestId */
    export function ugcQueryByCategory(categoryType: number, p1: number, maxGet: number, contentTypeName: string, p4: number, p5: boolean): number;
    /** Returns ugcRequestId */
    export function ugcQueryByContentId(contentId: string, latestVersion: boolean, contentTypeName: string): number;
    /** Returns ugcRequestId */
    export function ugcQueryByContentType(p0: number, maxGet: number, contentTypeName: string, p3: number, p4: number, p5: number): number;
    export function ugcQueryGetBookMarked(p0: any, index: number): boolean;
    export function ugcQueryGetCreatorHandle(p0: any, index: number, gamerHandle: any): any;
    /** Returns string for GET_STATUS_OF_LOAD_MISSION_CREATOR_PHOTO */
    export function ugcQueryGetCreatorPhoto(p0: any, p1: number, p2: any): NativeString;
    export function ugcQueryGetDate(p0: any, index: number, p2: any): void;
    export function ugcQueryGetLanguage(p0: any, index: number): number;
    export function ugcQueryGetMissionDescHash(p0: any, index: number): number;
    export function ugcQueryGetName(p0: any, index: number): NativeString;
    export function ugcQueryGetOwnerId(p0: any, index: number): NativeString;
    export function ugcQueryGetPlaylistName(p0: any, index: number): NativeString;
    export function ugcQueryGetPosixPublishedDate(p0: any, p1: any): number;
    export function ugcQueryGetPosixUpdatedDate(p0: any, p1: any): number;
    export function ugcQueryGetPublished(p0: any, p1: any): boolean;
    export function ugcQueryGetRating(p0: any, index: number, p2: number): number;
    export function ugcQueryGetRootContentId(p0: any, index: number): NativeString;
    export function ugcQueryGetVersion(p0: any, index: number, p2: number): number;

    // OBJECT
    export function allowDamageEventsForNonNetworkedObjects(enabled: boolean): void;
    export function attachPortablePickupToPed(pickupObject: number, ped: number): void;
    export function blockPickupFromPlayerCollection(p0: any, p1: any): void;
    export function breakAllObjectFragmentBones(object: number): void;
    export function breakObjectFragmentChild(object: number, p1: any, p2: boolean): void;
    /** Old name: _GET_PICKUP_HASH */
    export function convertOldPickupTypeToNew(pickupHash: number): number;
    /** flags: see CREATE_PICKUP */
    export function createAmbientPickup(pickupHash: number, x: number, y: number, z: number, flags: number, amount: number, customModel: number, createAsScriptObject: boolean, scriptHostObject: boolean, customAmmoType: number, p10: number): number;
    export function createObject(modelHash: number, x: number, y: number, z: number, isNetwork: boolean, bScriptHostObj: boolean, dynamic: boolean, p7: boolean, p8: boolean): number;
    export function createObjectNoOffset(modelHash: number, x: number, y: number, z: number, isNetwork: boolean, bScriptHostObj: boolean, dynamic: boolean, p7: boolean): number;
    export function createObjectSkeleton(object: number): boolean;
    /** https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/Placement%20Flags https://github.com/femga/rdr3_discoveries/blob/master/objects/pickup_list.lua */
    export function createPickup(pickupHash: number, x: number, y: number, z: number, flags: number, p5: number, p6: boolean, modelHash: number, p8: number, p9: number, p10: any): number;
    /** flags: see CREATE_PICKUP */
    export function createPickupRotate(pickupHash: number, posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, flags: number, p8: number, p9: number, p10: boolean, modelHash: number, p12: number, p13: number, p14: any): number;
    export function createPortablePickup(pickupHash: number, x: number, y: number, z: number, placeOnGround: boolean, modelHash: number): number;
    /** Deletes the specified object, then sets the handle pointed to by the pointer to NULL. */
    export function deleteObject(object: number): void;
    export function detachPortablePickupFromPed(pickupObject: number): void;
    export function doesObjectOfTypeExistAtCoords(x: number, y: number, z: number, radius: number, hash: number, p5: boolean): boolean;
    export function doesPickupExist(pickup: number): boolean;
    export function doesPickupObjectExist(pickupObject: number): boolean;
    export function doesPickupOfTypeExistInArea(pickupHash: number, x: number, y: number, z: number, radius: number): boolean;
    export function doesRayfireMapObjectExist(object: number): boolean;
    export function doorSystemGetDoorState(doorHash: number): number;
    export function doorSystemGetOpenRatio(doorHash: number): number;
    export function doorSystemSetAutomaticDistance(doorHash: number, distance: number): void;
    export function doorSystemSetAutomaticRate(doorHash: number, rate: number): void;
    /** Door lock states: enum eDoorState { 	DOORSTATE_INVALID = -1, 	DOORSTATE_UNLOCKED, 	DOORSTATE_LOCKED_UNBREAKABLE, 	DOORSTATE_LOCKED_BREAKABLE, 	DOORSTATE_HOLD_OPEN_POSITIVE, 	DOORSTATE_HOLD_OPEN_NEG... */
    export function doorSystemSetDoorState(doorHash: number, state: number): void;
    /** Sets the ajar angle of a door. Ranges from -1.0 to 1.0, and 0.0 is closed / default. */
    export function doorSystemSetOpenRatio(doorHash: number, ajar: number, forceUpdate: boolean): void;
    export function fixObjectFragment(object: number): void;
    export function forcePickupRegenerate(p0: any): void;
    /** missionScriptObject - if true won't return mission script objects scriptHostObject - if true won't return script host objects networkObject - if true won't return networked objects */
    export function getClosestObjectOfType(x: number, y: number, z: number, radius: number, modelHash: number, missionScriptObject: boolean, scriptHostObject: boolean, networkObject: boolean): number;
    export function getObjectFragmentDamageHealth(p0: any, p1: boolean): number;
    /** Old name: _GET_OBJECT_OFFSET_FROM_COORDS */
    export function getOffsetFromCoordAndHeadingInWorldCoords(xPos: number, yPos: number, zPos: number, heading: number, xOffset: number, yOffset: number, zOffset: number): Vector3;
    export function getPickupCoords(pickup: number): Vector3;
    export function getPickupObject(pickup: number): number;
    export function getRayfireMapObject(x: number, y: number, z: number, radius: number, name: string): number;
    export function getRayfireMapObjectAnimPhase(object: number): number;
    export function getSafePickupCoords(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any): Vector3;
    export function getStateOfRayfireMapObject(object: number): number;
    export function getWeaponTypeFromPickupType(pickupHash: number): number;
    export function hasClosestObjectOfTypeBeenBroken(p0: number, p1: number, p2: number, p3: number, modelHash: number, p5: any): boolean;
    export function hasObjectBeenBroken(p0: any): boolean;
    export function hasPickupBeenCollected(pickup: number): boolean;
    export function isDoorClosed(doorHash: number): boolean;
    export function isDoorRegisteredWithSystem(doorHash: number): boolean;
    export function isObjectAPortablePickup(object: number): boolean;
    export function isObjectVisible(object: number): boolean;
    export function isPointInAngledArea(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: number, p10: boolean, p11: boolean): boolean;
    /** Old name: _MARK_OBJECT_FOR_DELETION */
    export function onlyCleanUpObjectWhenOutOfRange(object: number): void;
    export function placeObjectOnGroundProperly(object: number, p1: boolean): boolean;
    export function preventCollectionOfPortablePickup(object: number, p1: boolean, p2: boolean): void;
    export function removeAllPickupsOfType(pickupHash: number): void;
    export function removeDoorFromSystem(doorHash: number): void;
    export function removePickup(pickup: number): void;
    export function setActivateObjectPhysicsAsSoonAsItIsUnfrozen(object: number, toggle: boolean): void;
    export function setCustomTexturesOnObject(object: number, txdHash: number, p2: any, p3: any): void;
    export function setForceObjectThisFrame(x: number, y: number, z: number, p3: number): void;
    export function setLocalPlayerCanCollectPortablePickups(toggle: boolean): void;
    /** Maximum amount of pickup models that can be disallowed is 10.  Old name: _SET_LOCAL_PLAYER_CAN_USE_PICKUPS_WITH_THIS_MODEL */
    export function setLocalPlayerPermittedToCollectPickupsWithModel(modelHash: number, toggle: boolean): void;
    export function setMaxNumPortablePickupsCarriedByPlayer(modelHash: number, p1: number): void;
    export function setObjectAllowLowLodBuoyancy(object: number, toggle: boolean): void;
    /** Adjust the physics parameters of a prop, or otherwise known as "object". This is useful for simulated gravity.  Other parameters seem to be unknown.  p2: seems to be weight and gravity related. Hig... */
    export function setObjectPhysicsParams(object: number, weight: number, p2: number, p3: number, p4: number, p5: number, gravity: number, p7: number, p8: number, p9: number, p10: number, buoyancy: number): void;
    export function setObjectTakesDamageFromCollidingWithBuildings(object: number, enabled: boolean): void;
    export function setObjectTargettable(object: number, targettable: boolean): void;
    /** Alt name: _SET_OBJECT_TINT  Old name: _SET_OBJECT_TEXTURE_VARIATION */
    export function setObjectTintIndex(object: number, textureVariation: number): void;
    export function setPickupDoNotAutoPlaceOnGround(pickupObject: number): void;
    export function setPickupGenerationRangeMultiplier(multiplier: number): void;
    export function setPickupHiddenWhenUncollectable(p0: any, p1: any): void;
    export function setPickupNotLootable(p0: any, p1: any): void;
    export function setPickupParticleFxHighlight(p0: any, p1: any): void;
    export function setPickupParticleFxSpawn(p0: any, p1: any): void;
    export function setPickupRegenerationTime(pickup: number, duration: number): void;
    export function setPickupUncollectable(p0: any, p1: any): void;
    export function setStateOfRayfireMapObject(object: number, state: number): void;
    export function setTeamPickupObject(object: number, p1: any, p2: boolean): void;
    export function slideObject(object: number, toX: number, toY: number, toZ: number, speedX: number, speedY: number, speedZ: number, collision: boolean): boolean;
    export function suppressPickupRewardType(rewardType: number, suppress: boolean): void;
    export function trackObjectVisibility(object: number): void;
    /** Registers a door, hashes: https://github.com/femga/rdr3_discoveries/tree/master/doorHashes */
    export function addDoorToSystemNew(doorHash: number, p1: boolean, p2: boolean, p3: boolean, threadId: number, p5: number, p6: boolean): void;
    export function damageBoneOnProp(object: number, bone: number): void;
    export function doorSystemChangeScriptOwner(doorHash: number): void;
    export function doorSystemForceShut(doorHash: number, p1: boolean): void;
    export function doorSystemGetAutomaticRate(doorHash: number): number;
    export function doorSystemSetAbleToChangeOpenRatioWhileLocked(doorHash: number, p1: boolean): void;
    /** _ALLOW_* - _ATTACH_* */
    export function doorSystemSetAutomaticState(doorHash: number, disable: boolean): void;
    export function getAmmoTypeFromPickupType(pickupHash: number): number;
    export function getLightIntensityFromObject(object: number): number;
    /** Returns float value to be used with _SET_LIGHT_INTENSITY_FOR_OBJECT */
    export function getObjectLightIntensity(object: number): number;
    export function hidePickupObject(pickupObject: number, toggle: boolean): void;
    export function isDoorRegisteredWithNetwork(doorHash: number): boolean;
    /** Returns true if door is alredy registered with owner */
    export function isDoorRegisteredWithOwner(doorHash: number): boolean;
    export function isPickupTypeValid(pickupHash: number): boolean;
    /** _PRE* or _Q* or _RE* */
    export function makeItemCarriable(object: number): void;
    export function resetObjectVelocity(object: number): void;
    export function setAmbientPickupLifetime(lifetime: number): void;
    /** Sets object as auto-jumpable by horse. */
    export function setAutoJumpableByHorse(object: number, p1: boolean): void;
    export function setLightIntensityForObject(object: number, lightIntensity: number): void;
    export function setLightScatteringDisabledForObject(object: number, disable: boolean): void;
    /** Params: value = 0.0 - 586.67 (?) */
    export function setLightTranslucencyForObject(object: number, value: number): void;
    /** Params: p2 controls whether to make pickups usable/collectable or not in networked games */
    export function setNetworkPickupUsableForPlayer(player: number, pickupHash: number, isUsable: boolean): void;
    /** Sets object as not jumpable by horse. */
    export function setNotJumpableByHorse(object: number, p1: boolean): void;
    export function setObjectBreakScale(object: number, scale: number): void;
    export function setObjectBurnIntensity(object: number, intensity: number): void;
    /** Seems to mostly have effect on wood-made objects https://imgur.com/a/32oQvOn */
    export function setObjectBurnLevel(object: number, burnLevel: number, affectAsh: boolean): void;
    export function setObjectBurnOpacity(object: number, opacity: number): void;
    /** p2 is usually the same as speed parameter */
    export function setObjectBurnSpeed(object: number, speed: number, p2: number): void;
    /** Writes an interaction state/flags byte and sets an expiry. Lower 3 bits = timeout preset: 0=none, 1=5s, 2=30s, 3=60s, 4=120s (5=custom, unused here). Bits 3-5 = category (0-7). Bit 6 (0x40) = extra... */
    export function setObjectInteractionPreset(object: number, presetFlags: number): void;
    export function setObjectKickable(object: number, kickable: boolean): void;
    /** _SET_FORCE* - _SET_LOCAL* */
    export function setObjectPromptName(object: number, name: string): void;
    /** _SET_FORCE* - _SET_LOCAL* */
    export function setObjectPromptNameFromGxtEntry(object: number, name: number): void;
    export function setObjectTargettable2(object: number, targettable: boolean): void;
    /** When p1 and p2 are true you can focus on the object (similar to when you focus a ped) */
    export function setObjectTargettableFocus(object: number, p1: boolean, p2: boolean): void;
    export function setPickupCollectableOnMount(object: number): void;

    // PAD
    /** nullsub, doesn't do anything */
    export function clearControlLightEffect(control: number): void;
    /** Old name: _CLEAR_SUPPRESSED_PAD_RUMBLE */
    export function clearControlShakeSuppressedId(control: number): void;
    export function disableAllControlActions(control: number): void;
    export function disableControlAction(control: number, action: number, disableRelatedActions: boolean): void;
    export function enableControlAction(control: number, action: number, enableRelatedActions: boolean): void;
    /** Returns time in ms since last input. */
    export function getControlHowLongAgo(control: number): number;
    export function getControlNormal(control: number, action: number): number;
    export function getControlUnboundNormal(control: number, action: number): number;
    export function getControlValue(control: number, action: number): number;
    export function getDisabledControlNormal(control: number, action: number): number;
    export function getDisabledControlUnboundNormal(control: number, action: number): number;
    export function haveControlsChanged(control: number): boolean;
    export function isControlEnabled(control: number, action: number): boolean;
    export function isControlJustPressed(control: number, action: number): boolean;
    export function isControlJustReleased(control: number, action: number): boolean;
    export function isControlPressed(control: number, action: number): boolean;
    export function isControlReleased(control: number, action: number): boolean;
    export function isDisabledControlJustPressed(control: number, action: number): boolean;
    export function isDisabledControlJustReleased(control: number, action: number): boolean;
    export function isDisabledControlPressed(control: number, action: number): boolean;
    export function isLookInverted(): boolean;
    /** padIndex is not used  Old name: _IS_USING_KEYBOARD */
    export function isUsingKeyboardAndMouse(control: number): boolean;
    /** nullsub, doesn't do anything  Old name: _SET_CONTROL_GROUP_COLOR */
    export function setControlLightEffectColor(control: number, red: number, green: number, blue: number): void;
    /** nullsub, doesn't do anything */
    export function setControlLightEffectFlashingColor(control: number, red: number, green: number, blue: number): void;
    /** Old name: SET_PAD_SHAKE */
    export function setControlShake(control: number, duration: number, frequency: number): void;
    /** Old name: SET_PAD_SHAKE_SUPPRESSED_ID */
    export function setControlShakeSuppressedId(control: number, uniqueId: number): void;
    /** nullsub, doesn't do anything */
    export function setControlTriggerShake(control: number, leftDuration: number, leftFrequency: number, rightDuration: number, rightFrequency: number): void;
    /** This is for simulating player input. value is a float value from 0 - 1  control: see IS_CONTROL_ENABLED  Old name: _SET_CONTROL_NORMAL */
    export function setControlValueNextFrame(control: number, action: number, value: number): boolean;
    export function setInputExclusive(control: number, action: number): void;
    /** Old name: STOP_PAD_SHAKE */
    export function stopControlShake(control: number): void;
    /** Gets the current control context. See: _SET_CONTROL_CONTEXT */
    export function getCurrentControlContext(control: number): number;
    export function getDisabledControlHowLongAgo(control: number): number;
    export function isControlActionValid(action: number, control: number): boolean;
    /** Sets the current control context. Must be called every frame.  context: https://alloc8or.re/rdr3/doc/misc/input_contexts.txt For more information, see common:/data/control/settings.meta https://git... */
    export function setControlContext(control: number, context: number): void;

    // PATHFIND
    export function addNavmeshBlockingObject(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: boolean, p8: any): number;
    export function addNavmeshRequiredRegion(x: number, y: number, radius: number): void;
    export function areNodesLoadedForArea(x1: number, y1: number, x2: number, y2: number): boolean;
    export function doesNavmeshBlockingObjectExist(object: number): boolean;
    /** Returns CGameWorldHeightMap's minimum Z value at specified point (grid node). */
    export function getApproxFloorForPoint(x: number, y: number): number;
    export function getClosestRoad(x: number, y: number, z: number, p3: number, p4: number, p5: Vector3, p6: Vector3, p7: any, p8: any, p9: number, p10: boolean): boolean;
    export function getClosestVehicleNode(x: number, y: number, z: number, outPosition: Vector3, nodeType: number, p5: number, p6: number): boolean;
    export function getClosestVehicleNodeWithHeading(x: number, y: number, z: number, outPosition: Vector3, outHeading: number, nodeType: number, p6: number, p7: number): boolean;
    export function getGpsBlipRouteFound(): boolean;
    export function getGpsBlipRouteLength(): number;
    export function getNthClosestVehicleNode(x: number, y: number, z: number, nthClosest: number, outPosition: Vector3, unknown1: number, unknown2: number, unknown3: any): boolean;
    export function getNthClosestVehicleNodeFavourDirection(x: number, y: number, z: number, desiredX: number, desiredY: number, desiredZ: number, nthClosest: number, outPosition: Vector3, outHeading: number, nodetype: number, p10: any, p11: any): boolean;
    export function getNthClosestVehicleNodeId(x: number, y: number, z: number, nth: number, nodetype: number, p5: number, p6: number): number;
    /** Returns the nth closest vehicle node with a heading to a coord */
    export function getNthClosestVehicleNodeIdWithHeading(x: number, y: number, z: number, nthClosest: number, returnHeading: number, returnNumLanes: number, nodeFlags: number, zMeasureMult: number, zTolerance: number): number;
    export function getNthClosestVehicleNodeWithHeading(x: number, y: number, z: number, nthClosest: number, outPosition: Vector3, heading: number, unknown1: any, unknown2: number, unknown3: number, unknown4: number): boolean;
    export function getNumNavmeshesExistingInArea(p0: number, p1: number, p2: number, p3: number, p4: number, p5: number): number;
    export function getRandomVehicleNode(x: number, y: number, z: number, radius: number, minLanes: number, avoidDeadEnds: boolean, avoidHighways: boolean, outPosition: Vector3, nodeId: number): boolean;
    export function getSafeCoordForPed(x: number, y: number, z: number, onGround: boolean, outPosition: Vector3, flags: number): boolean;
    export function getVehicleNodeIsSwitchedOff(nodeID: number): boolean;
    export function getVehicleNodePosition(nodeId: number, outPosition: Vector3): void;
    /** Returns whether navmesh for the region is loaded. */
    export function isNavmeshLoadedInArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number): boolean;
    /** Gets a value indicating whether the specified position is on a road. */
    export function isPointOnRoad(x: number, y: number, z: number, vehicle: number): boolean;
    /** Returns true if the id is non zero. */
    export function isVehicleNodeIdValid(vehicleNodeId: number): boolean;
    /** Starts a nav mesh query for a path between coordinates with a given ped and returns a handle to be validated by _NAVMESH_REQUESTED_PATH_QUERY_STATUS and then _NAVMESH_REQUESTED_PATH_WAYPOINTS_FOUND... */
    export function navmeshRequestPath(ped: number, x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, bitFlag: number): number;
    export function removeNavmeshBlockingObject(object: number): void;
    /** Old name: REQUEST_PATHS_PREFER_ACCURATE_BOUNDINGSTRUCT */
    export function requestPathNodesInAreaThisFrame(x1: number, y1: number, x2: number, y2: number): boolean;
    export function resetRoadsInVolume(volume: number, p1: boolean): void;
    export function setAmbientPedRangeMultiplierThisFrame(multiplier: number): void;
    /** nullsub, doesn't do anything */
    export function setIgnoreNoGpsFlag(toggle: boolean): void;
    export function setPedPathsBackToOriginal(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any): void;
    export function setPedPathsInArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, unknown: boolean, p7: any): void;
    export function setRoadsBackToOriginal(xMin: number, yMin: number, zMin: number, xMax: number, yMax: number, zMax: number, p6: any, p7: any): void;
    export function setRoadsBackToOriginalInAngledArea(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any): void;
    export function setRoadsInAngledArea(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any): void;
    export function setRoadsInArea(xMin: number, yMin: number, zMin: number, xMax: number, yMax: number, zMax: number, p6: any, p7: any, p8: any): void;
    export function setRoadsInVolume(volume: number, p1: boolean, p2: boolean, p3: boolean): void;
    export function simulatedRouteGetEta(p0: any): number;
    export function simulatedRouteIsLoaded(p0: any): boolean;
    export function simulatedRouteTravelToPoint(p0: any, p1: number, p2: number): void;
    export function addNavmeshBlockingVolume(volume: number, flags: number): boolean;
    export function doesNavmeshBlockingVolumeExist(volume: number): boolean;
    export function getSpawnDataForRoadNode(nodeId: number, x: number, y: number, z: number, outCoords: Vector3, heading: number): void;
    export function navmeshActivateSwap(name: string): boolean;
    export function navmeshAssignNavmeshToVehicle(vehicle: number, navMeshName: string): boolean;
    /** Called in scripts after finished with requested pathes. Immediately resets all values connected to the path handle except query status, which changes from 1 to 2 before eventually becoming fully in... */
    export function navmeshClearRequestedPath(path: number): boolean;
    export function navmeshDeactivateSwap(name: string): boolean;
    export function navmeshDoesSwapExist(name: string): boolean;
    export function navmeshIsSwapActive(name: string): boolean;
    /** Returns the number of waypoints for a requested path (NAVMESH_REQUEST_PATH) if the query is completed (_NAVMESH_REQUESTED_PATH_QUERY_STATUS). For use with _NAVMESH_REQUESTED_PATH_WAYPOINT_BY_INDEX */
    export function navmeshRequestedPathNumWaypoints(path: number): number;
    /** Returns eNavMeshQueryStatus enum eNavMeshQueryStatus { 	QS_NOT_FOUND, 	QS_COMPLETE, 	QS_PENDING };  It appears that the pending state of 2 is at least also used when cleaning up a request (_NAVMESH... */
    export function navmeshRequestedPathQueryStatus(path: number): number;
    /** Returns true if a path of waypoints was found. Waypoints can be retrieved with _NAVMESH_REQUESTED_PATH_NUM_WAYPOINTS and _NAVMESH_REQUESTED_PATH_WAYPOINT_BY_INDEX */
    export function navmeshRequestedPathWaypointsFound(path: number): boolean;
    /** Returns a bit flag for seemingly terrain within the waypoints in the path. Checked against bit value 2 to match water in the path, seems to always contain at least 1 though regardless of location/ped. */
    export function navmeshRequestedPathWaypointsTerrain(path: number): number;
    /** Returns a vector3 waypoint at the specified index for a path. Use _NAVMESH_REQUESTED_PATH_NUM_WAYPOINTS to get available indexes. */
    export function navmeshRequestedPathWaypointByIndex(path: number, waypointIndex: number): Vector3;
    export function removeNavmeshBlockingVolume(volume: number): void;
    export function simulatedRouteCreate(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, p6: number): any;
    export function simulatedRouteDelete(p0: any): void;
    export function simulatedRouteExists(p0: any): boolean;

    // PED
    /** Same as SET_PED_ARMOUR, but ADDS 'amount' to the armor the Ped already has. */
    export function addArmourToPed(ped: number, amount: number): void;
    export function addCustomFormationLocation(groupId: number, x: number, y: number, z: number, position: number): void;
    export function addFormationLocation(groupId: number, p1: number, p2: number, p3: number): boolean;
    /** The hash of the created relationship group is output in the second parameter. */
    export function addRelationshipGroup(name: string, groupHash: number): boolean;
    /** blockingFlags: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eScenarioBlockingFlags */
    export function addScenarioBlockingArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, p6: boolean, blockingFlags: number): number;
    /** damages a ped with the given amount */
    export function applyDamageToPed(ped: number, damageAmount: number, damageArmour: boolean, boneId: number, pedKiller: number): void;
    export function applyPedBloodSpecific(ped: number, p1: any, p2: number, p3: number, p4: number, p5: number, p6: any, p7: number, p8: any): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/peds_customization/ped_decals.lua */
    export function applyPedDamagePack(ped: number, damagePack: string, damage: number, mult: number): void;
    export function canKnockPedOffVehicle(ped: number): boolean;
    export function canPedBeMounted(ped: number): boolean;
    export function canPedInCombatSeeTarget(ped: number, target: number): boolean;
    export function canPedRagdoll(ped: number): boolean;
    /** Returns: 0 - CTR_CANNOT_TARGET 1 - CTR_CAN_TARGET 2 - CTR_NOT_SURE_YET */
    export function canPedSeeEntity(ped: number, targetEntity: number, p2: boolean, doFoliageCheck: boolean): number;
    export function canPedSeePedCached(ped: number, targetPed: number, p2: boolean): number;
    export function clearFacialIdleAnimOverride(ped: number): void;
    export function clearPedBloodDamage(ped: number): void;
    export function clearPedBloodDamageByZone(ped: number, p1: number): void;
    export function clearPedDamageDecalByZone(ped: number, p1: number, p2: string): void;
    export function clearPedDecorations(ped: number): void;
    export function clearPedEnvDirt(ped: number): void;
    export function clearPedLastDamageBone(ped: number): void;
    export function clearPedNonCreationArea(): void;
    /** It clears the wetness of the selected Ped/Player. Clothes have to be wet to notice the difference. */
    export function clearPedWetness(ped: number): void;
    /** flags: see SET_RAGDOLL_BLOCKING_FLAGS */
    export function clearRagdollBlockingFlags(ped: number, flags: number): void;
    export function clearRelationshipBetweenGroups(relationship: number, group1: number, group2: number): void;
    export function clonePed(ped: number, isNetwork: boolean, bScriptHostPed: boolean, copyHeadBlendFlag: boolean): number;
    /** Copies ped's components and props to targetPed. Can be used to clear anything from a ped by cloning it, including bullet holes. */
    export function clonePedToTarget(ped: number, targetPed: number): void;
    export function computeSatchelItemForPedDamage(p0: any, pedAttached: number, damageCleanliness: number): boolean;
    export function countPedsInCombatWithTarget(ped: number, flag: number): number;
    export function countPedsInCombatWithTargetWithinRadius(ped: number, x: number, y: number, z: number, radius: number, flag: number): number;
    /** Creates a new ped group. Groups can contain up to 8 peds.  The parameter is unused.  Returns a handle to the created group, or 0 if a group couldn't be created. */
    export function createGroup(taskAllocator: number): number;
    export function createPed(modelHash: number, x: number, y: number, z: number, heading: number, isNetwork: boolean, bScriptHostPed: boolean, p7: boolean, p8: boolean): number;
    /** seatIndex: enum eVehicleSeat { 	VS_ANY_PASSENGER = -2, 	VS_DRIVER, 	VS_FRONT_RIGHT, 	VS_BACK_LEFT, 	VS_BACK_RIGHT, 	VS_EXTRA_LEFT_1, 	VS_EXTRA_RIGHT_1, 	VS_EXTRA_LEFT_2, 	VS_EXTRA_RIGHT_2, 	VS_EXTR... */
    export function createPedInsideVehicle(vehicle: number, modelHash: number, seatIndex: number, p3: boolean, p4: boolean, p5: boolean): number;
    export function createPedOnMount(mount: number, modelHash: number, index: number, p3: boolean, p4: boolean, p5: boolean, p6: boolean): number;
    /** Deletes the specified ped, then sets the handle pointed to by the pointer to NULL. */
    export function deletePed(ped: number): void;
    export function detachCarriableEntity(entity: number, p1: boolean, p2: boolean): void;
    export function disablePedInjuredOnGroundBehaviour(ped: number): void;
    export function doesGroupExist(groupId: number): boolean;
    /** Forces the ped to fall back and kills it.  It doesn't really explode the ped's head but it kills the ped */
    export function explodePedHead(ped: number, weaponHash: number): void;
    export function fadeAndDestroyPed(ped: number): void;
    export function findAllAttachedCarriableEntities(ped: number, itemset: number): void;
    /** Old name: _FREEZE_PED_CAMERA_ROTATION */
    export function forceAllHeadingValuesToAlign(ped: number): void;
    export function forcePedAiAndAnimationUpdate(ped: number, p1: boolean, p2: boolean): void;
    /** motionStateHash: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/CPedMotionStates__eMotionState */
    export function forcePedMotionState(ped: number, motionStateHash: number, p2: boolean, p3: number, p4: boolean): boolean;
    export function getAnimInitialOffsetPosition(animDict: string, animName: string, x: number, y: number, z: number, xRot: number, yRot: number, zRot: number, p8: number, p9: number): Vector3;
    export function getAnimInitialOffsetRotation(animDict: string, animName: string, x: number, y: number, z: number, xRot: number, yRot: number, zRot: number, p8: number, p9: number): Vector3;
    /** Outputs carriable info for a ped's carriable slot (see TASK_CARRIABLE slot indices). p3 is always 0 in R* scripts. Returns true if outData was filled.  outData: script struct<4> (32 bytes); each fi... */
    export function getCarriedAttachedInfoForSlot(outData: any, ped: number, carriableSlot: number, p3: number): boolean;
    /** Gets the closest ped in a radius. */
    export function getClosestPed(x: number, y: number, z: number, radius: number, p4: boolean, p5: boolean, outPed: number, p7: boolean, p8: boolean, p9: boolean, pedType: number): boolean;
    export function getCombatFloat(ped: number, combatType: number): number;
    export function getCurrentTargetForPed(ped: number): number;
    export function getDeadPedPickupCoords(ped: number, p1: number, p2: number): Vector3;
    export function getGroupSize(groupId: number, hasLeader: boolean, numberOfFollowers: number): void;
    export function getIsPedRespondingToNegativeInteraction(ped: number, player: number): boolean;
    export function getIsPedRespondingToPositiveInteraction(ped: number, player: number): boolean;
    export function getJackTarget(ped: number): number;
    export function getLootingPickupTargetEntity(ped: number): number;
    export function getMeleeTargetForPed(ped: number): number;
    /** This is a way to get what drawables a ped has equipped Example: you are able to tell if the ped has the drawable PLAYER_ZERO_HAT_017 attached Note: this works with non shop components, direct .ydd ... */
    export function getMetaPedAssetGuids(ped: number, index: number, drawable: number, albedo: number, normal: number, material: number): boolean;
    export function getMetaPedAssetTint(ped: number, index: number, pallete: number, tint0: number, tint1: number, tint2: number): boolean;
    export function getMount(ped: number): number;
    export function getNumMetaPedOutfits(ped: number): number;
    export function getPedsJacker(ped: number): number;
    export function getPedAccuracy(ped: number): number;
    export function getPedAsGroupLeader(groupID: number): number;
    export function getPedAsGroupMember(groupID: number, memberNumber: number): number;
    export function getPedBlackboardScriptBool(ped: number, variableName: string): boolean;
    export function getPedBlackboardScriptFloat(ped: number, variableName: string): number;
    export function getPedBlackboardScriptInt(ped: number, variableName: string): number;
    /** Gets the position of the specified bone of the specified ped.  ped: The ped to get the position of a bone from. boneId: The ID of the bone to get the position from. This is NOT the index. offsetX: ... */
    export function getPedBoneCoords(ped: number, boneId: number, offsetX: number, offsetY: number, offsetZ: number): Vector3;
    /** no bone = -1 */
    export function getPedBoneIndex(ped: number, boneId: number): number;
    /** Returns the hash of the weapon/model/object that killed the ped. */
    export function getPedCauseOfDeath(ped: number): number;
    export function getPedCombatMovement(ped: number): number;
    /** flagId: see SET_PED_CONFIG_FLAG */
    export function getPedConfigFlag(ped: number, flagId: number, p2: boolean): boolean;
    export function getPedCrouchMovement(ped: number): boolean;
    /** Old name: _GET_PED_CURRENT_MOVEMENT_SPEED */
    export function getPedCurrentMoveBlendRatio(ped: number, speedX: number, speedY: number): boolean;
    export function getPedDefensiveAreaPosition(ped: number, p1: boolean): Vector3;
    export function getPedGrappleState(ped: number): number;
    /** Returns the groupId of which the specified ped is a member of. */
    export function getPedGroupIndex(ped: number): number;
    export function getPedIsBeingGrappled(ped: number): boolean;
    export function getPedIsDoingCombatRoll(ped: number): boolean;
    export function getPedIsGrappling(ped: number): boolean;
    export function getPedLastDamageBone(ped: number, outBone: number): boolean;
    /** enum ePedLootStatus { 	PLS_NONE, 	PLS_PRE_LOOT, 	PLS_SAMPLING, 	PLS_SKINNING }; */
    export function getPedLootStatusMp(ped: number): number;
    export function getPedMaxHealth(ped: number): number;
    export function getPedMoney(ped: number): number;
    export function getPedMotionFocusEntity(ped: number): number;
    export function getPedNearbyPeds(ped: number, sizeAndPeds: any, ignoredPedType: number, p3: number): number;
    export function getPedNearbyVehicles(ped: number, sizeAndVehs: any): number;
    export function getPedRelationshipGroupDefaultHash(ped: number): number;
    export function getPedRelationshipGroupHash(ped: number): number;
    export function getPedResetFlag(ped: number, flagId: number): boolean;
    /** Returns the entity that killed the ped  It is best to check if the Ped is dead before asking for its killer. */
    export function getPedSourceOfDeath(ped: number): number;
    /** Returns whether the entity is in stealth mode */
    export function getPedStealthMovement(ped: number): boolean;
    export function getPedTimeOfDeath(ped: number): number;
    export function getPedToPlayerWeaponDamageModifier(ped: number): number;
    export function getPedType(ped: number): number;
    export function getPlayerPedIsFollowing(ped: number): number;
    export function getRelationshipBetweenGroups(group1: number, group2: number): number;
    export function getRelationshipBetweenPeds(ped1: number, ped2: number): number;
    export function getSeatPedIsTryingToEnter(ped: number): number;
    export function getSeatPedIsUsing(ped: number): number;
    export function getTrackedPedPixelcount(ped: number): number;
    export function getVehiclePedIsEntering(ped: number): number;
    /** Gets the vehicle the specified Ped is in.  If the Ped is not in a vehicle and includeLastVehicle is true, the vehicle they were last in is returned. */
    export function getVehiclePedIsIn(ped: number, lastVehicle: boolean): number;
    export function getVehiclePedIsUsing(ped: number): number;
    export function givePedHashScenarioProp(ped: number, object: number, conditionalAnim: string, scenarioType: number, p4: number, p5: boolean): boolean;
    export function hasMotionTypeAssetLoaded(nameHash: number, ped: number): boolean;
    export function initPedDefaultHealth(ped: number): void;
    export function instantlyFillPedPopulation(): void;
    export function isAnimalInteractionPossible(ped: number, animal: number): boolean;
    export function isAnyHostilePedNearPoint(ped: number, x: number, y: number, z: number, radius: number): boolean;
    export function isAnyPedNearPoint(x: number, y: number, z: number, radius: number): boolean;
    export function isAnyPedShootingInArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, p6: boolean, p7: boolean): boolean;
    export function isEventInQueue(ped: number, eventType: number): boolean;
    export function isGroupLocallyControlled(groupId: number): boolean;
    export function isInstantlyFillPedPopulationFinished(): boolean;
    export function isLocationSpawnSafe(ped: number, p1: number): boolean;
    export function isPedAimingFromCover(ped: number): boolean;
    export function isPedAPlayer(ped: number): boolean;
    export function isPedBeingDragged(ped: number): boolean;
    export function isPedBeingHogtied(ped: number): boolean;
    export function isPedBeingJacked(ped: number): boolean;
    export function isPedBeingStealthKilled(ped: number): boolean;
    export function isPedBeingStunned(ped: number, weaponType: number): boolean;
    export function isPedCarryingSomething(ped: number): boolean;
    export function isPedClimbing(ped: number): boolean;
    export function isPedDeadOrDying(ped: number, p1: boolean): boolean;
    export function isPedDefensiveAreaActive(ped: number, p1: boolean): boolean;
    export function isPedDiving(ped: number): boolean;
    export function isPedEnteringAnyTransport(ped: number): boolean;
    /** Presumably returns the Entity that the Ped is currently diving out of the way of. */
    export function isPedEvasiveDiving(ped: number, evadingEntity: number): boolean;
    /** angle is ped's view cone */
    export function isPedFacingPed(ped: number, otherPed: number, angle: number): boolean;
    export function isPedFalling(ped: number): boolean;
    export function isPedFallingOver(ped: number): boolean;
    /** Gets a value indicating whether this ped's health is below its fatally injured threshold. The default threshold is 100. If the handle is invalid, the function returns true. */
    export function isPedFatallyInjured(ped: number): boolean;
    export function isPedFleeing(ped: number): boolean;
    export function isPedFullyOnMount(ped: number, p1: boolean): boolean;
    export function isPedGettingIntoAVehicle(ped: number): boolean;
    export function isPedGoingIntoCover(ped: number): boolean;
    export function isPedGroupMember(ped: number, groupId: number, p2: boolean): boolean;
    export function isPedHangingOnToVehicle(ped: number): boolean;
    export function isPedHeadingTowardsPosition(ped: number, x: number, y: number, z: number, p4: number): boolean;
    export function isPedHeadtrackingEntity(ped: number, entity: number): boolean;
    export function isPedHeadtrackingPed(ped1: number, ped2: number): boolean;
    export function isPedHogtied(ped: number): boolean;
    export function isPedHogtying(ped: number): boolean;
    /** Returns true/false if the ped is/isn't humanoid. */
    export function isPedHuman(ped: number): boolean;
    export function isPedIncapacitated(ped: number): boolean;
    /** Gets a value indicating whether this ped's health is below its injured threshold.  The default threshold is 100. */
    export function isPedInjured(ped: number): boolean;
    export function isPedInAnyBoat(ped: number): boolean;
    export function isPedInAnyHeli(ped: number): boolean;
    export function isPedInAnyPlane(ped: number): boolean;
    export function isPedInAnyTaxi(ped: number): boolean;
    export function isPedInAnyTrain(ped: number): boolean;
    /** Gets a value indicating whether the specified ped is in any vehicle. */
    export function isPedInAnyVehicle(ped: number, atGetIn: boolean): boolean;
    export function isPedInCombat(ped: number, target: number): boolean;
    export function isPedInCover(ped: number, p1: boolean, p2: boolean): boolean;
    export function isPedInCoverFacingLeft(ped: number): boolean;
    export function isPedInFlyingVehicle(ped: number): boolean;
    export function isPedInGroup(ped: number): boolean;
    /** Notes: The function only returns true while the ped is:  A.) Swinging a random melee attack (including pistol-whipping)  B.) Reacting to being hit by a melee attack (including pistol-whipping)  C.)... */
    export function isPedInMeleeCombat(ped: number): boolean;
    export function isPedInModel(ped: number, modelHash: number): boolean;
    /** Gets a value indicating whether the specified ped is in the specified vehicle. */
    export function isPedInVehicle(ped: number, vehicle: number, atGetIn: boolean): boolean;
    export function isPedJacking(ped: number): boolean;
    export function isPedJumping(ped: number): boolean;
    export function isPedLassoed(ped: number): boolean;
    /** Returns true/false if the ped is/isn't male. */
    export function isPedMale(ped: number): boolean;
    export function isPedModel(ped: number, modelHash: number): boolean;
    export function isPedOnFoot(ped: number): boolean;
    export function isPedOnMount(ped: number): boolean;
    export function isPedOnSpecificVehicle(ped: number, vehicle: number): boolean;
    /** Gets a value indicating whether the specified ped is on top of any vehicle.  Return 1 when ped is on vehicle. Return 0 when ped is not on a vehicle. */
    export function isPedOnVehicle(ped: number, p1: boolean): boolean;
    /** Returns true if the ped is currently opening a door (CTaskOpenDoor).  Old name: _IS_PED_OPENING_A_DOOR */
    export function isPedOpeningDoor(ped: number): boolean;
    export function isPedPerformingMeleeAction(ped: number, p1: number, p2: number): boolean;
    export function isPedPlantingBomb(ped: number): boolean;
    export function isPedProne(ped: number): boolean;
    /** If the ped handle passed through the parenthesis is in a ragdoll state this will return true. */
    export function isPedRagdoll(ped: number): boolean;
    export function isPedReadyToRender(ped: number): boolean;
    /** Returns whether the specified ped is reloading. */
    export function isPedReloading(ped: number): boolean;
    /** eventType: https://alloc8or.re/rdr3/doc/enums/eEventType.txt */
    export function isPedRespondingToEvent(ped: number, eventType: number): boolean;
    export function isPedRespondingToThreat(ped: number): boolean;
    export function isPedRunningMobilePhoneTask(ped: number): boolean;
    export function isPedRunningRagdollTask(ped: number): boolean;
    /** Returns whether the specified ped is shooting. */
    export function isPedShooting(ped: number): boolean;
    export function isPedSitting(ped: number): boolean;
    /** Detect if ped is in any vehicle [True/False] */
    export function isPedSittingInAnyVehicle(ped: number): boolean;
    /** Detect if ped is sitting in the specified vehicle [True/False] */
    export function isPedSittingInVehicle(ped: number, vehicle: number): boolean;
    export function isPedStopped(ped: number): boolean;
    export function isPedSwimming(ped: number): boolean;
    export function isPedSwimmingUnderWater(ped: number): boolean;
    export function isPedUsingActionMode(ped: number): boolean;
    export function isPedUsingAnyScenario(ped: number): boolean;
    /** Equivalent to IS_PED_USING_SCENARIO from V but takes a hash instead of a string. */
    export function isPedUsingScenarioHash(ped: number, scenarioHash: number): boolean;
    export function isPedUsingThisScenario(ped: number, scenario: number): boolean;
    export function isPedVaulting(ped: number): boolean;
    /** Returns true if ped is in perception (focused and looking at target ped) Most float params are -1.f in R* Scripts */
    export function isTargetPedInPerceptionArea(ped: number, targetPed: number, p2: number, customDistance: number, p4: number, p5: number): boolean;
    /** Returns whether or not a ped is visible within your FOV, not this check auto's to false after a certain distance. Target needs to be tracked first, won't work otherwise. */
    export function isTrackedPedVisible(ped: number): boolean;
    export function knockOffPedProp(ped: number, p1: boolean, p2: boolean, p3: boolean, p4: boolean): void;
    export function knockPedOffVehicle(ped: number): void;
    export function pedCowerInPlace(ped: number, ped2: number): void;
    export function pedCowerMoveToPoint(ped: number, p1: number, p2: number, p3: number, ped2: number, p5: number): void;
    /** Based on TASK_COMBAT_HATED_TARGETS_AROUND_PED, the parameters are likely similar (PedHandle, and area to attack in). */
    export function registerHatedTargetsAroundPed(ped: number, radius: number): void;
    export function registerTarget(ped: number, targetPed: number, p2: boolean): void;
    export function releasePedVisibilityTracking(ped: number): void;
    export function removeGroup(groupId: number): void;
    /** Ped will no longer get angry when you stay near him. */
    export function removePedDefensiveArea(ped: number, toggle: boolean): void;
    export function removePedFromGroup(ped: number): void;
    export function removeRelationshipGroup(groupHash: number): void;
    export function removeScenarioBlockingArea(p0: any, p1: boolean): void;
    export function removeScenarioBlockingAreas(): void;
    /** Params: p2, p3 usually 0 in R* Scripts */
    export function removeShopItemFromPedByCategory(ped: number, componentCategory: number, p2: number, p3: boolean): void;
    export function removeTagFromMetaPed(ped: number, component: number, p2: number): void;
    export function requestPedUseSmallBboxVisibilityTracking(ped: number, p1: boolean): void;
    export function requestPedVehicleVisibilityTracking(ped: number, p1: boolean): void;
    export function requestPedVisibilityTracking(ped: number): void;
    export function resetAiWeaponDamageModifier(): void;
    export function resetGroupFormationDefaultSpacing(groupId: number): void;
    export function resetHorseAvoidanceLevelToDefault(horse: number): void;
    export function resetPedInVehicleContext(ped: number): void;
    /** Resets the value for the last vehicle driven by the Ped. */
    export function resetPedLastVehicle(ped: number): void;
    export function resetPedRagdollTimer(ped: number): void;
    export function resetPedWeaponMovementClipset(ped: number): void;
    /** This function will simply bring the dead ped back to life.  Before calling this function, you may want to declare the position, where your Resurrected ped to be spawn at because theres a chance the... */
    export function resurrectPed(ped: number): void;
    export function reviveInjuredPed(ped: number): void;
    export function setAiMeleeWeaponDamageModifier(modifier: number): void;
    export function setAiWeaponDamageModifier(value: number): void;
    export function setBlockingOfNonTemporaryEvents(ped: number, toggle: boolean): void;
    export function setBlockingOfNonTemporaryEventsForAmbientPedsThisFrame(p0: boolean): void;
    /** combatType: https://github.com/femga/rdr3_discoveries/tree/master/AI/COMBAT_FLOATS  https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eCombatAttributeFloats */
    export function setCombatFloat(ped: number, combatType: number, newValue: number): void;
    export function setCreateRandomCops(toggle: boolean): void;
    export function setEnableBoundAnkles(ped: number, toggle: boolean): void;
    /** Ped can not pull out a weapon when true */
    export function setEnableHandcuffs(ped: number, p1: boolean, p2: boolean): void;
    export function setFacialIdleAnimOverride(ped: number, animName: string, animDict: string): void;
    export function setFormationPositionsTargetRadius(groupId: number, radius: number): boolean;
    /** eFormationType  0: Default 1: Circle Around Leader 2: Alternative Circle Around Leader 3: Line, with Leader at center */
    export function setGroupFormation(groupId: number, formationType: number): void;
    export function setGroupFormationSpacing(groupId: number, p1: number, p2: number, p3: number): void;
    /** Sets the range at which members will automatically leave the group. */
    export function setGroupSeparationRange(groupId: number, separationRange: number): void;
    /** -1 - HORSE_ASSIST__NO_CHANGE  0 - HORSE_ASSIST__MANUAL  1 - HORSE_ASSIST__SEMIASSIST  2 - HORSE_ASSIST__FULLASSIST */
    export function setHorseAvoidanceLevel(horse: number, avoidanceLevel: number): void;
    export function setIkTarget(ped: number, ikIndex: number, entityLookAt: number, boneLookAt: number, offsetX: number, offsetY: number, offsetZ: number, p7: any, blendInDuration: number, blendOutDuration: number): void;
    /** https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/CLootingFlags__Flags https://github.com/femga/rdr3_discoveries/tree/master/AI/LOOTING_FLAGS  lootFlag: enum eLootFlag { 	LOOT_FLAG_I... */
    export function setLootingFlag(ped: number, lootFlag: number, enabled: boolean): void;
    export function setPausePedWritheBleedout(ped: number, toggle: boolean): void;
    /** accuracy = 0-100, 100 being perfectly accurate */
    export function setPedAccuracy(ped: number, accuracy: number): void;
    /** Turns the desired ped into a cop. If you use this on the player ped, you will become almost invisible to cops dispatched for you. You will also report your own crimes, get a generic cop voice, get ... */
    export function setPedAsCop(ped: number, toggle: boolean): void;
    export function setPedAsGroupLeader(ped: number, groupId: number, p2: boolean): void;
    export function setPedAsGroupMember(ped: number, groupId: number): void;
    export function setPedCanArmIk(ped: number, toggle: boolean): void;
    /** When set on a player ped, its just like when you die in RDO */
    export function setPedCanBeIncapacitated(ped: number, toggle: boolean): void;
    /** state: enum eKnockOffVehicle { 	KNOCKOFFVEHICLE_DEFAULT, 	KNOCKOFFVEHICLE_NEVER, 	KNOCKOFFVEHICLE_EASY, 	KNOCKOFFVEHICLE_HARD }; */
    export function setPedCanBeKnockedOffVehicle(ped: number, state: number): void;
    export function setPedCanBeTargetted(ped: number, toggle: boolean): void;
    export function setPedCanBeTargettedByPlayer(ped: number, player: number, toggle: boolean): void;
    export function setPedCanBeTargettedByTeam(ped: number, team: number, toggle: boolean): void;
    export function setPedCanHeadIk(ped: number, toggle: boolean): void;
    export function setPedCanLegIk(ped: number, toggle: boolean): void;
    export function setPedCanPlayAmbientAnims(ped: number, toggle: boolean): void;
    export function setPedCanPlayAmbientBaseAnims(ped: number, toggle: boolean): void;
    export function setPedCanPlayGestureAnims(ped: number, p1: any, p2: any): void;
    export function setPedCanRagdoll(ped: number, toggle: boolean): void;
    export function setPedCanRagdollFromPlayerImpact(ped: number, toggle: boolean): void;
    /** This only will teleport the ped to the group leader if the group leader teleports (sets coords).  Only works in singleplayer */
    export function setPedCanTeleportToGroupLeader(pedHandle: number, groupId: number, toggle: boolean): void;
    export function setPedCanTorsoIk(ped: number, toggle: boolean): void;
    export function setPedCanTorsoReactIk(ped: number, toggle: boolean): void;
    export function setPedCanTorsoVehicleIk(ped: number, toggle: boolean): void;
    export function setPedCanUseAutoConversationLookat(ped: number, toggle: boolean): void;
    /** Overrides the ped's collision capsule radius for the current tick. Must be called every tick to be effective.  Setting this to 0.001 will allow warping through some objects. */
    export function setPedCapsule(ped: number, value: number): void;
    /** Old name: SET_PED_CLOTH_PACKAGE_INDEX */
    export function setPedClothPinFrames(ped: number, p1: boolean): void;
    /** abilityLevel: enum eCombatAbilityLevel { 	CAL_POOR, 	CAL_AVERAGE, 	CAL_PROFESSIONAL }; */
    export function setPedCombatAbility(ped: number, abilityLevel: number): void;
    /** attributeIndex: https://alloc8or.re/rdr3/doc/enums/eCombatAttribute.txt https://github.com/femga/rdr3_discoveries/tree/master/AI/COMBAT_ATTRIBUTES */
    export function setPedCombatAttributes(ped: number, attributeIndex: number, enabled: boolean): void;
    /** 0 - Stationary (Will just stand in place) 1 - Defensive (Will try to find cover and very likely to blind fire) 2 - Offensive (Will attempt to charge at enemy but take cover as well) 3 - Suicidal Of... */
    export function setPedCombatMovement(ped: number, combatMovement: number): void;
    /** range: enum eCombatRange { 	CR_NEAR, 	CR_MEDIUM, 	CR_FAR, 	CR_VERY_FAR }; */
    export function setPedCombatRange(ped: number, range: number): void;
    /** flagId: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/ePedScriptConfigFlags https://alloc8or.re/rdr3/doc/enums/ePedScriptConfigFlags.txt https://github.com/femga/rdr3_discoveries... */
    export function setPedConfigFlag(ped: number, flagId: number, value: boolean): void;
    export function setPedDefensiveAreaDirection(ped: number, p1: number, p2: number, p3: number, p4: boolean): void;
    export function setPedDefensiveAreaVolume(ped: number, volume: number, p2: boolean, p3: boolean, p4: boolean): void;
    export function setPedDesiredHeading(ped: number, heading: number): void;
    /** Used in various R* MP & SP Scripts */
    export function setPedFiringPattern(ped: number, patternHash: number): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/AI/FLEE_ATTRIBUTES  attributeFlags: enum eFleeAttribute { 	FA_FORCE_EXIT_VEHICLE = (1 << 16), 	FA_DISABLE_MOUNT_USAGE = (1 << 20), 	FA_DISABLE_... */
    export function setPedFleeAttributes(ped: number, attributeFlags: number, enable: boolean): void;
    export function setPedGestureGroup(ped: number, gesture: string, p2: number): void;
    export function setPedGravity(ped: number, toggle: boolean): void;
    export function setPedGroupMemberPassengerIndex(ped: number, index: number): void;
    export function setPedHearingRange(ped: number, value: number): void;
    export function setPedHighlyPerceptive(ped: number, toggle: boolean): void;
    export function setPedIdRange(ped: number, value: number): void;
    export function setPedInjuredOnGroundBehaviour(ped: number, unk: number): void;
    /** Ped: The ped to warp. vehicle: The vehicle to warp the ped into. seatIndex: see CREATE_PED_INSIDE_VEHICLE */
    export function setPedIntoVehicle(ped: number, vehicle: number, seatIndex: number): void;
    export function setPedKeepTask(ped: number, toggle: boolean): void;
    export function setPedLassoHogtieFlag(ped: number, flagId: number, value: boolean): void;
    export function setPedLegIkMode(ped: number, mode: number): void;
    export function setPedLodMultiplier(ped: number, multiplier: number): void;
    /** Sets the maximum health of a ped. */
    export function setPedMaxHealth(ped: number, value: number): void;
    export function setPedMaxMoveBlendRatio(ped: number, value: number): void;
    export function setPedMaxTimeInWater(ped: number, value: number): void;
    export function setPedMaxTimeUnderwater(ped: number, value: number): void;
    export function setPedMinMoveBlendRatio(ped: number, value: number): void;
    export function setPedModelIsSuppressed(model: number, toggle: boolean): void;
    export function setPedMoney(ped: number, amount: number): void;
    export function setPedMoveAnimsBlendOut(ped: number): void;
    /** Min: 0.0f Max: 1.15f */
    export function setPedMoveRateOverride(ped: number, value: number): void;
    /** nullsub, doesn't do anything */
    export function setPedNameDebug(ped: number, name: string): void;
    /** The distance between these points, is the diagonal of a box (remember it's 3D). */
    export function setPedNonCreationArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number): void;
    export function setPedOntoMount(ped: number, mount: number, seatIndex: number, p3: boolean): void;
    export function setPedOwnsAnimal(ped: number, animal: number, p2: boolean): void;
    export function setPedPanicExitScenario(ped: number, x: number, y: number, z: number): boolean;
    export function setPedRagdollForceFall(ped: number): void;
    /** Causes Ped to ragdoll on collision with any object (e.g Running into trashcan). If applied to player you will sometimes trip on the sidewalk. */
    export function setPedRagdollOnCollision(ped: number, toggle: boolean, p2: boolean): void;
    export function setPedRandomComponentVariation(ped: number, p1: number): void;
    export function setPedRelationshipGroupDefaultHash(ped: number, hash: number): void;
    export function setPedRelationshipGroupHash(ped: number, relationshipGroup: number): void;
    /** Needs to be called every frame  flagid:https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/ePedScriptResetFlags https://github.com/femga/rdr3_discoveries/tree/master/AI/CPED_RESET_FLAGS */
    export function setPedResetFlag(ped: number, flagId: number, doReset: boolean): void;
    export function setPedSeeingRange(ped: number, value: number): void;
    /** Params: shootRate = 0 - 1000 */
    export function setPedShootRate(ped: number, shootRate: number): void;
    /** lookIntensity: see SET_PED_SHOULD_PLAY_FLEE_SCENARIO_EXIT */
    export function setPedShouldPlayCombatScenarioExit(ped: number, x: number, y: number, z: number, lookIntensity: number): boolean;
    /** Old name: _SET_PED_SHOULD_PLAY_DIRECTED_SCENARIO_EXIT */
    export function setPedShouldPlayDirectedNormalScenarioExit(ped: number, x: number, y: number, z: number): boolean;
    /** lookIntensity: see SET_PED_SHOULD_PLAY_FLEE_SCENARIO_EXIT */
    export function setPedShouldPlayEmotionalScenarioExit(ped: number, x: number, y: number, z: number, lookIntensity: number, p5: boolean): boolean;
    /** lookIntensity: 0 - REACT_LOOK_NONE 1 - REACT_LOOK_LOW 2 - REACT_LOOK_MEDIUM 3 - REACT_LOOK_HIGH */
    export function setPedShouldPlayFleeScenarioExit(ped: number, x: number, y: number, z: number, lookIntensity: number): boolean;
    export function setPedShouldPlayImmediateScenarioExit(ped: number): void;
    export function setPedShouldPlayNormalScenarioExit(ped: number): void;
    /** lookIntensity: see SET_PED_SHOULD_PLAY_FLEE_SCENARIO_EXIT */
    export function setPedShouldPlayQuickScenarioExit(ped: number, x: number, y: number, z: number, lookIntensity: number, p5: boolean): boolean;
    export function setPedSphereDefensiveArea(ped: number, x: number, y: number, z: number, radius: number, p5: boolean, p6: boolean, p7: boolean): void;
    /** Not implemented. */
    export function setPedStealthMovement(ped: number, toggle: boolean, p2: any, p3: any): void;
    export function setPedSweat(ped: number, sweat: number): void;
    /** TLR_ExitTask = 0, TLR_NeverLoseTarget, TLR_SearchForTarget */
    export function setPedTargetLossResponse(ped: number, responseType: number): void;
    export function setPedToInformRespectedFriends(ped: number, radius: number, maxFriends: number): void;
    /** Old name: _SET_PED_DAMAGE_MODIFIER */
    export function setPedToPlayerWeaponDamageModifier(ped: number, damageModifier: number): void;
    /** nmTaskMessageParameterName: See physicstasks.ymt. Search for DraggedByCart or 0xD00820D7 (Used in R* SP Script marston8) */
    export function setPedToRagdoll(ped: number, timeMin: number, timeMax: number, ragdollType: number, abortIfInjured: boolean, abortIfDead: boolean, nmTaskMessageParameterName: string): boolean;
    export function setPedToRagdollWithFall(ped: number, timeMin: number, timeMax: number, ragdollType: number, falldirX: number, falldirY: number, falldirZ: number, p7: number, p8: number, p9: number, p10: number, p11: number, p12: number, p13: number): boolean;
    export function setPedUsingActionMode(ped: number, bActionModeEnabled: boolean, p2: number, action: string): void;
    export function setPedVisualFieldCenterAngle(ped: number, angle: number): void;
    export function setPedVisualFieldMaxAngle(ped: number, value: number): void;
    export function setPedVisualFieldMinAngle(ped: number, value: number): void;
    export function setPedVisualFieldPeripheralRange(ped: number, range: number): void;
    /** combined with PED::SET_PED_WETNESS_HEIGHT(), this native makes the ped drenched in water up to the height specified in the other function */
    export function setPedWetnessEnabledThisFrame(ped: number): void;
    /** It adds the wetness level to the player clothing/outfit. As if player just got out from water surface. */
    export function setPedWetnessHeight(ped: number, height: number): void;
    export function setPopControlSphereThisFrame(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/AI/RAGDOLL_BLOCKING_FLAGS  flags: enum eRagdollBlockingFlags { 	RBF_BULLET_IMPACT = (1 << 0), 	RBF_VEHICLE_IMPACT = (1 << 1), 	RBF_FIRE = (1 <<... */
    export function setRagdollBlockingFlags(ped: number, flags: number): void;
    export function setRelationshipBetweenGroups(relationship: number, group1: number, group2: number): void;
    export function setScenarioPedDensityMultiplierThisFrame(multiplier: number): void;
    export function spawnpointsCancelSearch(): void;
    export function spawnpointsGetNumSearchResults(): number;
    export function spawnpointsGetSearchResult(randomInt: number, x: number, y: any, z: number): void;
    export function spawnpointsGetSearchResultFlags(p0: any, p1: any): void;
    export function spawnpointsIsSearchActive(): boolean;
    export function spawnpointsIsSearchComplete(): boolean;
    export function spawnpointsIsSearchFailed(): boolean;
    /** Params: p4 = 35.f, duration = 5000 in R* Scripts */
    export function spawnpointsStartSearch(x: number, y: number, z: number, width: number, p4: number, spawnpointsFlag: number, p6: number, duration: number, p8: number): void;
    /** Searching area between coords 1 and 2 */
    export function spawnpointsStartSearchInAngledArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, width: number, spawnpointsFlag: number, p8: number, duration: number, p10: number): void;
    export function specialFunctionDoNotUse(ped: number, p1: boolean): void;
    /** Returns time since the specified ped last shot, in seconds. (fPlayerJustShotTime) */
    export function timeSincePedLastShot(ped: number): number;
    /** If toggle is true, when the ped is using a scenario he will stop it and become scared If toggle is false, the ped will not be scared anymore and continue his scenario  Old name: _SET_PED_SCARED_WHE... */
    export function toggleScenarioPedCowerInPlace(ped: number, toggle: boolean): void;
    /** Despite this function's name, it simply returns whether the specified handle is a Ped. */
    export function wasPedSkeletonUpdated(ped: number): boolean;
    export function addPedStayOutVolume(ped: number, volume: number): boolean;
    export function addPedSubscribeToLegendaryBlips(ped: number): boolean;
    /** flag: see ADD_SCENARIO_BLOCKING_AREA */
    export function addScenarioBlockingVolume(volume: number, p1: boolean, flag: number): any;
    /** Forces transition now, called together with 0xD65FDC686A031C83 */
    export function addScenarioTransition(ped: number): void;
    /** Creates ped overlay in texture override data and returns it's index. This index are used for further overlay editing.  albedoHash: a hash of overlay's albedo texture colorType: a color type(from 0 ... */
    export function addTextureLayer(textureId: number, albedoHash: number, normalHash: number, materialHash: number, blendType: number, texAlpha: number, sheetGridIndex: number): number;
    /** Applies damage pack to a ped bone with offset and rotation. Note: for boneId only PD_Vomit seems to work.  Preview: https://imgur.com/a/qwEGXEu */
    export function applyPedDamagePackToBone(ped: number, boneId: number, xOffset: number, yOffset: number, zOffset: number, xRot: number, yRot: number, zRot: number, damagePack: string): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/clothes/metaped_outfits.lua */
    export function applyPedMetaPedOutfit(requestId: number, ped: number, p2: boolean, p3: boolean): boolean;
    export function applyShopItemToPed(ped: number, componentHash: number, immediately: boolean, isMp: boolean, p4: boolean): void;
    export function applyTextureOnPed(ped: number, componentHash: number, textureId: number): void;
    export function areAllAmbientPedReservationsReady(): boolean;
    export function attachVolumeToEntity(volume: number, entity: number, offsetX: number, offsetY: number, offsetZ: number, rotX: number, rotY: number, rotZ: number, p8: number, p9: boolean): void;
    /** Returns true if `listener` can hear `source`. If `includeNoiseBoost` is true, the source's noise radius is applied (easier to hear). It treats the source as louder—its current noise expands the eff... */
    export function canPedHearTargetPed(source: number, listener: number, includeNoiseBoost: boolean): boolean;
    /** p2 is always 0, p3 is always 0, p4 is always 1 */
    export function canPedUseScenarioPoint(ped: number, scenario: number, p2: any, p3: any, p4: any): boolean;
    /** Alters entity's stamina by 'amount'. Can be negative (to drain stamina). float amount: -1000.0 - 1000.0 */
    export function changePedStamina(ped: number, amount: number): boolean;
    /** Used in Script Functions PLAYER_HORSE_RELEASE_HORSE_TO_AMBIENT_WORLD (p1 = true), HORSE_SETUP_PLAYER_HORSE_ATTRIBUTES (p1 = false) Set to false for player horse in scripts and seems it's only true ... */
    export function clearActiveAnimalOwner(horse: number, clear: boolean): void;
    export function clearPedActionDisableFlag(ped: number, actionDisableFlag: number): void;
    export function clearPedBloodDamageFacial(ped: number, p1: number): void;
    /** Params: p1 = 1 in R* Scripts */
    export function clearPedCombatStyle(ped: number, p1: number): void;
    /** _CLEAR_PED_COMBAT_* */
    export function clearPedCombatStyleMod(ped: number, combatStyleModHash: number): void;
    /** Clears locomotion archetype */
    export function clearPedDesiredLocoForModel(ped: number): void;
    export function clearPedDesiredLocoMotionType(ped: number): void;
    export function clearPedGrappleFlag(ped: number, flag: number): void;
    export function clearPedTargetActionDisableFlag(ped: number, actionDisableFlag: number): void;
    /** Removes every texture layer Old Name: _RESET_PED_TEXTURE_2 */
    export function clearPedTexture(textureId: number): void;
    export function clearPeltFromHorse(horse: number, peltId: number): void;
    /** Computes the loot table for an animal/human carcass given its model and processing quality. Returns the number of loot entries written. Results are written into outLoot starting at index 1. Usage/E... */
    export function computeLootForPedCarcass(outLoot: any, model: number, damageCleanliness: number, skinningQuality: number): number;
    /** Returns estimated max speed (m/s) for the ped move blend ratio. Move blend ratio is in a range of 0.0 - 3.0. _COMPUTE_S* */
    export function computePedMoveBlendRatioForMaxSpeed(ped: number, maxMoveBlendRatio: number): number;
    /** Related to dead animals items/loots Notice: skinningQuality is partially calculated using pedQuality */
    export function computeSatchelItemForPedCarcass(outInventoryItemArray: any, ped: number, damageCleanliness: number, skinningQuality: number): number;
    /** Returns ped move blend ratio corresponding to the specified speed. */
    export function computeSpeedForPedMoveBlendRatio(ped: number, speed: number): number;
    /** Creates a handle to an instance of "CScriptResource_GravityWell", this system forces local ped to target specified position when moving, however player still can interrupt this. Can be useful to "p... */
    export function createGravityWell(xPos: number, yPos: number, zPos: number, heading: number, radius: number, p5: number, p6: number, p7: number, stopAtDestination: boolean): number;
    /** Only used in SP scripts, for example odriscolls1: BOOLS: true, true, true, false, false */
    export function createMetaPed(requestId: number, x: number, y: number, z: number, heading: number, p5: boolean, p6: boolean, p7: boolean, p8: boolean, p9: boolean): number;
    /** Creates prop from metaped asset bundle https://github.com/femga/rdr3_discoveries/blob/master/objects/metaped_asset_bundles_list.lua Creates a pickup-able metaped component. asset doesn't seems to b... */
    export function createMetaPedAsset(asset: number, posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, p7: boolean, p8: boolean, p9: boolean): number;
    /** Creates metaped from ped outfit requestId. See _REQUEST_METAPED_OUTFIT */
    export function createMetaPedOutfitPed(requestId: number, x: number, y: number, z: number, heading: number, p5: boolean, p6: boolean, p7: boolean, p8: boolean): number;
    export function detachVolumeFromEntity(volume: number, entity: number): void;
    /** The time of the shot must've occured <= shotNearTimeMs for this native to return true */
    export function detectPlayerShotNearPed(player: number, ped: number, shotNearTimeMs: number): boolean;
    export function disableAllLookAtRequests(ped: number, p1: number): void;
    export function disableAmbientLookAtRequests(p0: any, p1: any): void;
    export function doesMetaPedOutfitExistForPedModel(outfit: number, model: number): boolean;
    export function doesMetaPedSuboutfitExistForPedModel(outfit: number, suboutfit: number, model: number): boolean;
    /** Note: you have to update your ped's variation after calling (using 0xCC8CA3E88256E58F)  Body Types: MPCREATOR_NEUTRAL MPCREATOR_SKINNY MPCREATOR_SKINNY_MUSCULAR MPCREATOR_HEAVY MPCREATOR_HEAVY_MUSC... */
    export function equipMetaPedOutfit(ped: number, hash: number): void;
    /** Changes Multiplayer ped face and body type components, they can be stacked Params: p3 = 1 Body shape for mp_male from 124 - 128, 110 - 115 for mp_female Face shape for mp_male from 110 - 123, 96 - ... */
    export function equipMetaPedOutfitExtra(ped: number, component: number, p2: any, p3: any): void;
    /** Sets the outfit preset for the ped. The presetId is an index which determines its preset outfit. p2 is always false in the scripts. If p2 is true as player, then certain components like facial hair... */
    export function equipMetaPedOutfitPreset(ped: number, presetId: number, p2: boolean): void;
    export function equipMetaPedSuboutfit(ped: number, suboutfit: number, p2: number): void;
    export function fakeSetPedLocoInjured(ped: number, enabled: boolean): void;
    export function forcePedDeath(ped: number, pedKiller: number, weapon: number): void;
    export function getAccuracyAgainstLocalPlayerModifier(ped: number): number;
    export function getActiveAnimalOwner(animal: number): number;
    /** Returns kneeling, sitting, squating, and sleeping scenario hashes */
    export function getActiveDynamicScenario(ped: number): number;
    /** Returns kneeling, sitting, squating, and sleeping scenario hashes */
    export function getActiveDynamicScenario2(ped: number): number;
    export function getBlockingOfNonTemporaryEvents(ped: number): boolean;
    /** https://github.com/nativewrappers/nativewrappers/blob/main/src/redm/entities/HorsePeltEntries.ts https://pastebin.com/D58XgYBm */
    export function getCarriedPeltSkins(mount: number, outData: any): number;
    export function getCarrierAsHuman(entity: number): number;
    export function getCarrierAsMount(entity: number): number;
    export function getCarrierAsPed(entity: number): number;
    export function getCategoryOfComponentAtIndex(ped: number, componentIndex: number, p2: any): number;
    /** Gets MetaPedExpression at index specified  For index, see: _SET_CHAR_EXPRESSION  Old name: _GET_PED_FACE_FEATURE */
    export function getCharExpression(ped: number, index: number): number;
    export function getDefaultRelationshipGroupHash(modelHash: number): number;
    export function getFirstEntityPedIsCarrying(ped: number): number;
    export function getGroupFormation(groupId: number): number;
    export function getHealthRechargeMultiplier(ped: number): number;
    /** Returns an int based on enum eTamingState  enum eTamingState { 	ATS_INVALID = 0, 	ATS_INACTIVE, 	ATS_TARGET_DETECTED, 	ATS_CALLED_OUT, 	ATS_MOUNTABLE, 	ATS_BEING_PATTED, 	ATS_BREAKING_ACTIVE, 	ATS_... */
    export function getHorseTamingState(horse: number): number;
    export function getIncapacitationTimeRemaining(ped: number): number;
    /** If p2 is false, then this native will return true until the interaction is complete. If true, the native will return true until player pockets robbery item. _GET_IS_PED_[M-R]* */
    export function getIsPedBeingRobbed(ped: number, player: number, trueUntilPlayerPocketsItem: boolean): boolean;
    export function getIsPedCommandHashPresent(ped: number, commandHash: number): boolean;
    /** Returns true if ped is in a dispute another ped (pedInDisputeWith can also be 0) */
    export function getIsPedInDisputeWithPed(ped: number, pedInDisputeWith: number): boolean;
    /** motivationState: see _SET_PED_MOTIVATION */
    export function getIsPedMotivationStateEnabled(ped: number, motivationState: number): boolean;
    export function getLassoedLassoer(ped: number): number;
    /** _IS_PED_S* - _IS_PED_U* This native name may or may not be misleading. */
    export function getLassoerOfPed(ped: number): number;
    export function getLassoTarget(ped: number): number;
    /** Returns last horse the ped was leading */
    export function getLastLedMount(ped: number): number;
    export function getLastMount(ped: number): number;
    export function getLastVehicleDraftHorseWasAttachedTo(horse: number): number;
    /** lootFlag: see SET_LOOTING_FLAG */
    export function getLootingFlag(ped: number, lootFlag: number): boolean;
    export function getMetaPedRace(ped: number): number;
    /** enum eMetaPedType { 	MPT_MALE, 	MPT_FEMALE, 	MPT_TEEN, 	MPT_ANIMAL, 	MPT_NONE }; */
    export function getMetaPedType(ped: number): number;
    export function getNumComponentsInPed(ped: number): number;
    /** Works similar to 0x90403E8107B60E81 (_GET_NUM_COMPONENTS_IN_PED) but is used to get category hashes instead */
    export function getNumComponentCategoriesInPed(ped: number): number;
    export function getNumFreeSlotsInPedPool(): number;
    export function getNumReservedAmbientPedsDesired(): number;
    export function getNumReservedAmbientPedsReady(): number;
    export function getNumReservedHealth(ped: number): any;
    export function getNumReservedStamina(ped: number): number;
    export function getPedsInCombatWithTarget(ped: number, itemset: number, flag: number): number;
    /** AI_ATTITUDE_NEUTRAL = 0, AI_ATTITUDE_FRIENDLY, AI_ATTITUDE_WARY, AI_ATTITUDE_COMBATIVE, AI_ATTITUDE_NEVER_MET */
    export function getPedAttitude(ped: number, player: number): number;
    /** Can be used to get a peds foliage active status: variableName = FoliageActive */
    export function getPedBlackboardBool(ped: number, variableName: string): boolean;
    /** Can be used to get a peds foliage raw height: variableName = FoliageHeight */
    export function getPedBlackboardFloat(ped: number, variableName: string): number;
    export function getPedBlackboardHash(ped: number, variableName: string): number;
    export function getPedBrawlingStyle(ped: number): number;
    export function getPedCanBeIncapacitatedThisFrame(ped: number): boolean;
    export function getPedCombatAttribute(ped: number, attributeIndex: number): boolean;
    /** Returns category hash that each ped component has. Hash examples: MASKS, HATS, HEADS, HORSE_MANES */
    export function getPedComponentCategoryByIndex(ped: number, index: number): number;
    /** Returns true if _GET_PED_DAMAGE_CLEANLINESS was ever lower than 2 */
    export function getPedDamaged(ped: number): boolean;
    /** enum ePedDamageCleanliness { 	PED_DAMAGE_CLEANLINESS_POOR, 	PED_DAMAGE_CLEANLINESS_GOOD, 	PED_DAMAGE_CLEANLINESS_PERFECT }; */
    export function getPedDamageCleanliness(ped: number): number;
    export function getPedDefensiveVolume(ped: number, p1: any): number;
    /** Returns the ped's dirt amount as a scalar in [0.0, 1.0]. Notes: - The second parameter is treated as a boolean selector (0 or 1). Internally it indexes a 2-slot graphics/appearance bank (base + 0xB... */
    export function getPedDirtLevel(ped: number, useCompositeLayer: boolean): number;
    /** Returns ped drunk level _H* or _I* */
    export function getPedDrunkness(ped: number): number;
    export function getPedGrappler(ped: number): number;
    export function getPedGrappleFlag(ped: number): number;
    export function getPedGrappleStyle(ped: number): number;
    /** Returns whether given ped has recently interacted with a player in a specific way or not (determined by the given flag) flags: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/0x947... */
    export function getPedHasInteractedWithPlayer(targetPed: number, player: number, flag: number, durationMs: number): boolean;
    export function getPedHasSimplePlayerMemoryChanged(ped: number, memoryType: number, ms: number): boolean;
    export function getPedHeight(ped: number): number;
    export function getPedIdRange(ped: number): number;
    export function getPedIncapacitationHealth(ped: number): number;
    /** Used for AUDIO / ANIMSCENE (REFERENCE_REGIONAL_CHARACTER) Params: p1 = 0 */
    export function getPedIndexFromPerscharHash(persCharHash: number, p1: number): number;
    export function getPedInteractionPersonality(ped: number): number;
    /** https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/CLassoHogtieFlags__Flags https://github.com/femga/rdr3_discoveries/tree/master/AI/LASSO_HOGTIE_FLAG */
    export function getPedLassoHogtieFlag(ped: number, flagId: number): boolean;
    export function getPedLastDroppedHat(ped: number): number;
    export function getPedLodMultiplier(ped: number): number;
    export function getPedMaxStamina(ped: number): number;
    export function getPedMeleeActionPhase(ped: number): number;
    export function getPedMetaOutfitHash(ped: number): number;
    /** PS_SMALL = 0, PS_MEDIUM, PS_MEDIUM_LARGE, PS_LARGE, PS_EXTRA_LARGE */
    export function getPedModelSizeFromHash(modelHash: number): number;
    /** If targetPed is set to 0 the ped motivationState affects everyone */
    export function getPedMotivation(ped: number, motivationState: number, targetPed: number): number;
    /** Returns Ped Quality to be used to calculate Skinning Quality  enum ePedQuality { 	PQ_INVALID = -1, 	PQ_LOW, 	PQ_MEDIUM, 	PQ_HIGH, 	PQ_MAX }; */
    export function getPedQuality(ped: number): number;
    /** Returns boneIndex */
    export function getPedRagdollBoneIndex(ped: number, boneId: number): number;
    /** Gets a registered/attached prop entity for a particular ped. Second parameter will detach the prop entity from the ped if true. Props primarily appear to come from scenarios, such as a broom or hay... */
    export function getPedRegisterProp(ped: number, propName: string, detachProp: boolean): number;
    /** normalized / non normalized 0.0        / 1000.0         STARTED IN WRITHE STAGE 1.0        / 0.0            END OF WRITHE, DEAD -1.0                        DEAD  Returns some value from AI task 562... */
    export function getPedRemainingRevivalTime(ped: number, normalized: boolean): number;
    /** Checks weather a speech relationship hash was applied on given ped or not. _GET_PED_G* - _GET_PED_I* (INTERACTION?) */
    export function getPedSpeechRelationship(ped: number, p1: number): boolean;
    export function getPedStamina(ped: number): number;
    /** Returns stamina normalizedValue / normalizedUnlockedMax */
    export function getPedStaminaNormalized(ped: number): number;
    export function getPedTranquilizer(ped: number): number;
    export function getPedWhoHogitiedThisPed(ped: number): number;
    /** Returns peltId */
    export function getPeltFromHorse(horse: number, index: number): number;
    /** Returns animal skin quality modifier */
    export function getPlayerCurrentAnimalDamageModifier(player: number): number;
    export function getPlayerDismountTimestamp(mount: number, player: number): number;
    /** Returns how deep the water is below the ped (if in water) -1.0f = Not in water 10.0f = Max water depth */
    export function getPlayerPedWaterDepth(ped: number): number;
    export function getRiderOfMount(mount: number, p1: boolean): number;
    export function getShopItemBaseLayers(shopItem: number, p1: any, ped: number, metapedType: number, p4: boolean, drawable: number, albedo: number, normal: number, material: number, p9: number, p10: number, p11: number, p12: number): boolean;
    /** Returns 0 if index invalid/unresolvable; else the shop component hash. resolveSelection: true -> run the resolver (rebuild from ped meta/outfit - MP “net shop” style); false -> use cached entry onl... */
    export function getShopItemComponentAtIndex(ped: number, index: number, resolveSelection: boolean, outStatusFlag: boolean, outWearableState: number): number;
    export function getShopItemComponentCategory(componentHash: number, metapedType: number, isMP: boolean): number;
    export function getShopItemHatComponent(ped: number, metapedType: number, p2: boolean): any;
    /** Returns the number of wearable states available for a shop item / component. p2 seems to be true in scripts.  For use with 0x6243635AF2F1B826 (_GET_SHOP_ITEM_AVAILABLE_WEARABLE_STATE_BY_INDEX) */
    export function getShopItemNumWearableStates(componentHash: number, isMpFemale: boolean, p2: boolean): number;
    /** Gets an available wearable state by index for a shop item / component - it does not retreive what the current state is. p3 seems to be true in scripts.  Use 0xFFCC2DB2D9953401 (_GET_SHOP_ITEM_NUM_W... */
    export function getShopItemWearableStateByIndex(componentHash: number, wearableStateIndex: number, isMpFemale: boolean, p3: boolean): number;
    export function getStaminaDepletionMultiplier(ped: number): number;
    export function getStaminaRechargeMultiplier(ped: number): number;
    /** _GET_WA* */
    export function getTotalPedDamageFromAi(ped: number): number;
    export function getTransportPedIsSeatedOn(ped: number): number;
    export function getVehicleDraftHorseIsAttachedTo(horse: number): number;
    /** Ped Command Hash are special commands, that can be activated to change conditional anim variations or trigger transitions between conditional anims. https://github.com/femga/rdr3_discoveries/blob/m... */
    export function givePedHashCommand(ped: number, commandHash: number, activationDuration: number): void;
    export function givePedScenarioProp(ped: number, object: number, conditionalAnim: string, p3: string, p4: string, p5: boolean): boolean;
    /** Only used in SP R* Script rcm_jack2 */
    export function givePedScenarioPropDynamic(ped: number, object: number, p2: string, p3: string, p4: boolean): boolean;
    export function hasMetaPedAssetLoaded(requestId: number): boolean;
    export function hasMetaPedOutfitLoaded(requestId: number): boolean;
    export function hasMetaPedRequestLoaded(requestId: number): boolean;
    export function hasPedBeenShovedRecently(ped: number, ms: number): boolean;
    /** See _REQUEST_PED_EMOTIONAL_PRESET */
    export function hasPedEmotionalPresetLoaded(ped: number, name: string): boolean;
    /** True if the ped fired a weapon within the last `seconds` (seconds -> compared in ms internally). Returns false if no recent shot is recorded. */
    export function hasPedShotRecently(ped: number, seconds: number): boolean;
    /** limb: 3 = Left Hand, 4 = Left Arm, 6 = Right Hand, 7 = Right Arm, 9 = Left Foot, 10 = Left Leg, 12 = Right Foot, 13 = Right Leg, 37 = Head */
    export function hasPedTakenGoreDamage(ped: number, limb: number): boolean;
    /** _H* - _I* */
    export function horseAgitate(mount: number, kickOffRider: boolean): void;
    export function incapacitatedRevive(ped: number, ped2: number): void;
    /** Returns true only if it's a player ped and an animal as well. _IS_ANY_* - _IS_CONTROL_* */
    export function isAnimalControlledByAPlayer(ped: number): boolean;
    export function isAnimalInteractionRunning(ped: number): boolean;
    export function isAnimalSkinned(ped: number): boolean;
    export function isMetaPedAssetValid(requestId: number): boolean;
    /** Returns true if given ped is a fish. _IS_ME* - _IS_MO* */
    export function isMetaPedFish(ped: number): boolean;
    /** Used in script function HORSE_IS_META_PED_OUTFIT_SADDLE_EQUIPPED */
    export function isMetaPedOutfitEquipped(ped: number, outfit: number): boolean;
    export function isMetaPedOutfitRequestValid(requestId: number): boolean;
    export function isMetaPedRequestValid(requestId: number): boolean;
    export function isMetaPedUsingComponent(ped: number, component: number): boolean;
    export function isMountSeatFree(mount: number, seat: number): boolean;
    export function isPedActionDisableFlagEnabled(ped: number, actionDisableFlag: number): boolean;
    export function isPedChild(ped: number): boolean;
    export function isPedClimbingLadder(ped: number): boolean;
    export function isPedCowering(ped: number): boolean;
    export function isPedDoingScenarioTransition(ped: number): boolean;
    export function isPedDragging(ped: number): boolean;
    export function isPedDrunk(ped: number): boolean;
    export function isPedGroupLeader(ped: number, groupId: number): boolean;
    export function isPedIntimidated(ped: number): boolean;
    /** _IS_PED_IN* */
    export function isPedInvestigating(ped: number): boolean;
    /** Detects if ped is afloat in water like swimming or in a boat (driving or standing on it) */
    export function isPedInNavigableWater(ped: number): boolean;
    /** If returned true: There are enemy peds near friendly turn in ped. Going to aggro. If returned false: Moving back to idle as there aren't any remaining enemy peds near ped _IS_PED_IN_* */
    export function isPedInPoint(ped: number, x: number, y: number, z: number, radius: number, p5: boolean): boolean;
    /** _IS_PED_L* - _IS_PED_M* */
    export function isPedLeadingAnyGroup(ped: number): boolean;
    export function isPedModelSuppressed(model: number): boolean;
    export function isPedQueuedForDeletion(ped: number): boolean;
    export function isPedSliding(ped: number): boolean;
    export function isPedTargetActionDisableFlagEnabled(ped: number, actionDisableFlag: number): boolean;
    export function isPedUsingActionMode2(ped: number): boolean;
    export function isPedVisibilityTracked(ped: number): boolean;
    export function isScenarioBlockingAreaValid(p0: any): boolean;
    export function isTarget(ped: number, targetPed: number): boolean;
    export function isTextureValid(textureId: number): boolean;
    export function isThisModelAHorse(model: number): boolean;
    export function isTrackedPedVisibilityPercentageNotLessThan(ped: number, percent: number): boolean;
    /** _IS_TRACKED_* - IS_V* */
    export function isUsingSlipstream(ped: number): boolean;
    /** Applies speech relationship hash on given ped. */
    export function pedApplySpeechRelationship(ped: number, p1: number): boolean;
    export function pedClearLocoMotion(ped: number): void;
    export function pedDuelingDidPlayerHeadshotOpponent(ped: number): boolean;
    /** target: 0 affects everyone duration: -1 indefinite flag: always 4 in R* Scripts */
    export function pedEmotionalPresetLocoMotion(ped: number, presetName: string, targetPed: number, duration: number, flag: number): void;
    /** Removes speech relationship hash on given ped. */
    export function pedRemoveSpeechRelationship(ped: number, p1: number): void;
    /** memoryType: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/_PED_SET_SIMPLE_PLAYER_MEMORY */
    export function pedSetSimplePlayerMemory(ped: number, memoryType: number): void;
    export function pedWasKilledByHeadshot(ped: number): boolean;
    /** Plays a conditional locomotion animation with a prop item (commonly used for carry/attach interactions like crates).  Notes: - propItemId is a ConditionalAnims.propitem identifier (e.g. "P_CS_CRATE... */
    export function propitemPlayConditionalAnimWithObject(ped: number, targetEntity: number, propItemId: string, conditionalAnimName: string): void;
    /** Outputs the carried ped. Params: p2 is alsways 2, p3 is always false. */
    export function refreshCarriedPedForPed(ped: number, carriedPed: number, p2: number, p3: boolean): void;
    /** Queries/refreshes the current 'carry' action for a ped and (optionally) returns involved entities. Usage/Example: https://pastebin.com/qVstcVfz */
    export function refreshCarryStateForPed(ped: number, carryType: number, outEnts: any, outEntsCount: number, filterFlags: number): number;
    /** Returns loot state enum eLootState { 	LAP_NONE, 	LAP_RESUMING, 	LAP_GETTING_ON_FOOT, 	LAP_DISTANT_NAV, 	LAP_CHOOSING_ACTION, 	LAP_APPROACHING, 	LAP_ENTERING, 	LAP_LOOTING, 	LAP_EXITING };  _POSSE_*... */
    export function refreshLootStateForPed(ped: number, p1: number, lootTarget: number, p3: number, p4: number): number;
    /** p1 is always 1 */
    export function refreshMetaPedShopItems(ped: number, p1: number): void;
    export function registerHatedTargetsInArea(ped: number, x: number, y: number, z: number, radius: number): void;
    export function releaseMetaPedAssetRequest(requestId: number): void;
    export function releaseMetaPedOutfitRequest(requestId: number): void;
    export function releaseMetaPedRequest(requestId: number): void;
    /** Removes a texture created by 0xC5E7204F322E49EB. */
    export function releaseTexture(textureId: number): void;
    /** Stops and clears a running conditional locomotion animation state on the ped for the given prop item id (started via _PROPITEM_PLAY_CONDITIONAL_ANIM_WITH_OBJECT).  Notes: - Ends the conditional loc... */
    export function removeConditionalAnimPropitem(ped: number, propItemId: string): void;
    /** Removes gravity well by handle returned from 0x4F5EBE70081E5A20 */
    export function removeGravityWell(handle: number): void;
    export function removeMotionTypeAsset(nameHash: number, ped: number): void;
    export function removePedBlackboardBool(ped: number, variableName: string): void;
    export function removePedBlackboardFloat(ped: number, variableName: string): void;
    export function removePedBlackboardHash(ped: number, variableName: string): void;
    export function removePedBlackboardInt(ped: number, variableName: string): void;
    /** See _REQUEST_PED_EMOTIONAL_PRESET */
    export function removePedEmotionalPreset(ped: number, name: string): void;
    export function removePedFromMount(ped: number, p1: boolean, p2: boolean): void;
    export function removePedOverlay(textureId: number, overlayId: number): void;
    export function removePedStayOutVolume(ped: number, volume: number): boolean;
    export function removePedSubscribeToLegendaryBlips(ped: number): boolean;
    /** Directly removes a shop item component from a ped Params: p2 and p3 are always 0 */
    export function removeShopItemFromPed(ped: number, componentHash: number, p2: number, p3: boolean): void;
    export function removeTarget(ped: number, targetPed: number): void;
    /** Returns requestId Params: p1 = 1 in R* Scripts (Used in SP only) */
    export function requestMetaPed(model: number, p1: number): number;
    /** Returns requestId Params: p1 = 1 in R* Scripts */
    export function requestMetaPedAssetBundle(asset: number, p1: number): number;
    export function requestMetaPedComponent(metaPedType: number, p1: any, p2: number, p3: number, p4: number): any;
    /** https://github.com/femga/rdr3_discoveries/blob/master/clothes/metaped_outfits.lua Returns requestId, to be used with 0x74F512E29CB717E2 */
    export function requestMetaPedOutfit(model: number, outfit: number): number;
    export function requestMotionTypeAsset(nameHash: number, ped: number): void;
    /** For more information, see common:/data/emotional_presets.meta */
    export function requestPedEmotionalPreset(ped: number, name: string): void;
    /** mood: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/fwFacialAnimRequest__Mood Params: p2 = 6 in R* Scripts */
    export function requestPedFacialMoodThisFrame(ped: number, mood: number, p2: number): void;
    export function requestPedForScenarioType(ped: number, object: number, p2: string, scenarioType: number, p4: string, p5: boolean): any;
    /** Known get up animation types: REAR, FRONT */
    export function requestPedGetupAnimation(ped: number, getUpType: string): void;
    export function requestPropScenarioPed(ped: number, object: number, p2: string, p3: string, p4: string, p5: boolean): any;
    /** Creates a texture override data for ped and returns it's index. So you can replace any texture of any ped's component. Also, you can add overlays on it, such as aging, lipstick and more. Textures c... */
    export function requestTexture(albedoHash: number, normalHash: number, materialHash: number): number;
    export function reserveAmbientPeds(numPeds: number): void;
    export function reserveAmbientPedsTotal(numPeds: number): void;
    export function resetPedComponents(ped: number): void;
    export function resetPedIncapacitationBleedOutDuration(ped: number): void;
    export function resetPedLadderMovementSpeedModifier(ped: number): void;
    /** Seems to set the peds stamina to 30% */
    export function resetPedStamina(ped: number): void;
    /** Removes every texture layer but the base layer Clearing texture's data: setting params to default values, but keep overlays. */
    export function resetPedTexture(textureId: number): void;
    /** 0.0 <= stamina <= 100.0 */
    export function restorePedStamina(ped: number, stamina: number): void;
    export function setAccuracyAgainstLocalPlayerModifier(ped: number, modifier: number): void;
    /** Related to _0x704C908E9C405136 for component loading Can be used to fix missing outfit changes, always paired with _UPDATE_PED_VARIATION Doesn't actually return anything. */
    export function setActiveMetaPedComponentsUpdated(ped: number, isMP: boolean): any;
    export function setAmbientAnimalDensityMultiplierThisFrame(multiplier: number): void;
    export function setAmbientHumanDensityMultiplierThisFrame(multiplier: number): void;
    export function setAmbientPedDensityMultiplierThisFrame(multiplier: number): void;
    /** Sets MetaPedExpression at index specified. Morphs components, such as changing body size or facial features.  Note: You have to update the ped's variation (using 0xCC8CA3E88256E58F) after calling t... */
    export function setCharExpression(ped: number, index: number, value: number): void;
    export function setCurrentDefenseAgainstPlayersModifier(horse: number, modifier: number): void;
    export function setDefenseModifierForPed(ped: number, modifier: number): void;
    export function setFormationAutoAssignPosition(groupId: number, toggle: boolean): void;
    export function setHealthRechargeMultiplier(ped: number, multiplier: number): void;
    /** Sets an internal horse-component flag (bit 0x04) used for scripted control of mounts (e.g., after placing/rider-seating). Pass true to set, false to clear. No effect on non-horses. */
    export function setHorseScriptedFlag(ped: number, toggle: boolean): void;
    export function setInteractionLockonFlag(ped: number, player: number, flag: number, enable: boolean): void;
    /** Use to apply metaped player components Replaces asset, alternatively you can remove assets using REMOVE_TAG_FROM_META_PED */
    export function setMetaPedTag(ped: number, drawable: number, albedo: number, normal: number, material: number, palette: number, tint0: number, tint1: number, tint2: number): void;
    /** Sets ped eye redness, weariness: 0.f to 1.f */
    export function setMetaPedWeariness(ped: number, weariness: number): void;
    export function setMinPedHealthThreshold(ped: number, healthAmount: number): void;
    export function setMountBondingLevel(ped: number, bondingLevel: number): void;
    /** Note: this native was added in build 1232.40 */
    export function setMountSecurityEnabled(ped: number, toggle: boolean): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/AI/COMBAT_ACTION_DISABLE_FLAGS */
    export function setPedActionDisableFlag(ped: number, actionDisableFlag: number): void;
    /** bloodFountainPressure: visible effect from 0.0 till 20.0 yaw: visible effect from -3.0 till 3.0 bloodFountainDirection: 1.0 left side, -1.0 right side bloodFountainPulse: from 0.1 (low) till 1.0 (f... */
    export function setPedActivateWoundEffect(ped: number, p1: number, boneId: number, moveWoundLeftRight: number, bloodFountainPressure: number, yaw: number, bloodFountainDirection: number, bloodFountainPulse: number, p8: number, p9: number): void;
    /** Params: hash - ARTHUR or JOHN _SET_PED_(A-D)* */
    export function setPedActivePlayerType(ped: number, playerType: number): void;
    export function setPedAnimalDetectionModifier(ped: number, modifier: number): void;
    /** NET_FETCH_CLIENT_UPDATE_PED_FIGHT_PROFICIENCY: Changing parry multiplier for ped */
    export function setPedBeatMultiplier(ped: number, p1: number): void;
    export function setPedBlackboardBool(ped: number, variableName: string, value: boolean, removeTimer: number): void;
    export function setPedBlackboardFloat(ped: number, variableName: string, value: number, removeTimer: number): void;
    /** p1: BodyPartChained OverloadMostInjuredBodyPart  p2: LeftLeg Legs RightArm */
    export function setPedBlackboardHash(ped: number, variableName: string, value: string, removeTimer: number): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/AI/BLACKBOARDS Blackboard natives allow you to apply and check certain data to/for peds. Blackboard bools, floats and strings are subdivided in... */
    export function setPedBlackboardInt(ped: number, variableName: string, value: number, removeTimer: number): void;
    /** Bleedout profiles: Animal_FastBleedout Animal_Generic Human_FastBleedout Human_Generic Human_Mission  For more information, see common/data/ai/peddamageinfo.meta */
    export function setPedBleedoutProfile(ped: number, bleedoutProfile: number): void;
    /** brawlingStyle: enum eBrawlingStyle : Hash { 	BS_AI = 0x802C604D, 	BS_AI_BARBRAWL = 0x4FF5F0C7, 	BS_AI_DEFENSIVE = 0xD888F2FD, 	BS_AI_MOONSHINE_BARBRAWL = 0xA01B433A, 	BS_ALLIGATOR = 0x7A5548ED, 	BS... */
    export function setPedBrawlingStyle(ped: number, brawlingStyle: number): void;
    /** SET_PED_CAN_* */
    export function setPedCanBeLassoed(ped: number, toggle: boolean): void;
    /** _SET_PED_CAN_(?)_IK* */
    export function setPedCanUnkBodypartIk(ped: number, toggle: boolean): void;
    /** Hashes: GUARD, COMBAT_ANIMAL, LAW, LAW_SHERIFF _SET_PED_COMBAT_A* - _SET_PED_COMBAT_M* */
    export function setPedCombatAttributeHash(ped: number, p1: number): void;
    export function setPedCombatBehaviour(ped: number, behaviour: number): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/AI/COMBAT_STYLES Params: p2 is usually 1, sometimes 0 or 2 duration in seconds, -1.0 = forever */
    export function setPedCombatStyle(ped: number, combatStyleHash: number, p2: number, duration: number): void;
    /** duration in seconds, -1.0 = forever */
    export function setPedCombatStyleMod(ped: number, combatStyleModHash: number, duration: number): void;
    export function setPedCrouchMovement(ped: number, state: boolean, p2: number, immediately: boolean): void;
    /** The higher the multiplier the less the engine renders culls (https://docs.unity3d.com/Manual/OcclusionCulling.html) */
    export function setPedCullRange(ped: number, p1: number, p2: number): void;
    export function setPedDamaged(ped: number, damaged: boolean): void;
    /** damageCleanliness: see _GET_PED_DAMAGE_CLEANLINESS */
    export function setPedDamageCleanliness(ped: number, damageCleanliness: number): void;
    /** _SET_PED_(A?)* */
    export function setPedDefensiveAreaToAngledArea(ped: number, x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, p7: any, p8: boolean, p9: boolean, entity: number, p11: boolean): void;
    export function setPedDefensiveSphereAttachedToEntity(ped: number, entity: number, x: number, y: number, z: number, radius: number, p6: number, p7: boolean): void;
    /** Seems to set the ped's loco type. Values used in the scripts: algie angry_female arthur_healthy cowboy cowboy_f default default_female free_slave_01 free_slave_02 gold_panner guard_lantern injured_... */
    export function setPedDesiredLocoForModel(ped: number, locomotionArchetype: string): void;
    /** Sets peds motion type */
    export function setPedDesiredLocoMotionType(ped: number, locoMotionType: string): void;
    export function setPedDesiresGroup(ped: number, toggle: boolean): void;
    /** Params: ped, 0f, -1, true, true in R* MP Scripts _SET_PED_DE* - _SET_PED_F* */
    export function setPedDirtCleaned(ped: number, p1: number, p2: number, p3: boolean, p4: boolean): void;
    /** Disables being able to kick move ped. */
    export function setPedDisableKickMove(ped: number, disable: boolean): void;
    /** SOBER = 0.0f, SLIGHTLY_DRUNK = 0.25f, MODERATELY_DRUNK = 0.5f, VERY_DRUNK = 1.0f */
    export function setPedDrunkness(ped: number, enabled: boolean, drunknessLevel: number): void;
    /** Used in R* MP Script fm_mission_controller and various R* SP Scripts for ambush* */
    export function setPedFiringPattern2(ped: number, patternHash: number): void;
    /** Only used in R* MP Script fm_mission_controller */
    export function setPedFiringPattern3(ped: number, patternHash: number): void;
    export function setPedFormationPosition(ped: number, position: number, toggle: boolean): void;
    export function setPedGetupAnimation(ped: number, animName: string, p2: boolean): void;
    export function setPedGrappleAction(ped: number, grappleAction: number): void;
    export function setPedGrappleAnimation(ped: number, grappleAnim: number): void;
    export function setPedGrappleEffectMultiplier(ped: number, multiplier: number): any;
    export function setPedGrappleFlag(ped: number, flag: number, enable: boolean): void;
    export function setPedGrappleSequence(ped: number, grappleSequence: string): void;
    /** Hashes: GS_DRAGGING, GS_FACE_TO_BACK, GS_FACE_TO_FACE, GS_FACE_TO_FACE_WALL, GS_MOUNTED */
    export function setPedGrappleStyle(ped: number, style: number): any;
    export function setPedHeadshotDamageMultiplier(ped: number, multiplier: number): void;
    /** configHash: see pedhealth.meta */
    export function setPedHealthConfig(ped: number, configHash: number): void;
    /** Only used in R* Script beat_sharp_shooter Blocks ped from swimming underwater */
    export function setPedImmersionFlag(ped: number, toggle: boolean): void;
    export function setPedIncapacitationFlags(ped: number, flags: number): void;
    export function setPedIncapacitationModifiers(ped: number, canBeIncapacitated: boolean, threshold: number, bleedoutTime: number, p4: number): void;
    export function setPedIncapacitationTotalBleedOutDuration(ped: number, duration: number): void;
    export function setPedInteractionNegativeResponse(ped: number, speech: string): void;
    /** personality (script_mp_rel): NONE, AGGRESSIVE, TIMID (non-aggressive), CRIPPS, SCRIPTEDINTIMIDATION, MAGGIE, MARCEL, SCRIPTEDSALOON personality (script_rel): AVOID, SCRIPTEDOUTLAW, TIMIDGUARDDOG, S... */
    export function setPedInteractionPersonality(ped: number, personality: number): void;
    export function setPedInteractionPositiveResponse(ped: number, speech: string): void;
    export function setPedKnockedByOneHit(ped: number, p1: number): void;
    export function setPedLadderMovementSpeedModifier(ped: number, p1: number): void;
    export function setPedLights(ped: number, toggle: boolean): void;
    /** If the ped has an active TASK_MELEE, force-keep it active for durationMs starting now; returns true on success (no effect if melee task not running). */
    export function setPedMeleeForcedDuration(ped: number, durationMs: number): boolean;
    /** enum eMotivationState { 	TOILET_STATE, 	FEAR_STATE, 	ANGRY_STATE, 	AGITATION_STATE, 	HUNGRY_STATE, 	TIRED_STATE, 	SAD_STATE, 	BRAVE_STATE, 	OFFER_ITEM_STATE, 	SUSPICION, 	DRUNK_STATE };  If targetP... */
    export function setPedMotivation(ped: number, motivationState: number, threshold: number, targetPed: number): void;
    /** The higher the modifier, the slower the motivationState value will decrease */
    export function setPedMotivationModifier(ped: number, motivationState: number, modifier: number): void;
    /** motivationState: see _SET_PED_MOTIVATION */
    export function setPedMotivationStateOverride(ped: number, motivationState: number, enabled: boolean): void;
    /** Hashes: STANDARD_PED_AGRO_GUARD, BOUNTY_HUNTER, PLAYER_HORSE, LAW_POLICE, GUARD_DOG, ATTACK_DOG Personalities can also be found in common:/data/ai/interactionpersonalities */
    export function setPedPersonality(ped: number, personality: number): void;
    export function setPedPromptName(ped: number, name: string): void;
    export function setPedPromptName2(ped: number, name: string): void;
    export function setPedPromptNameFromGxtEntry(ped: number, gxtEntryHash: number): void;
    export function setPedPromptNameFromGxtEntry2(ped: number, gxtEntryHash: number): void;
    /** quality: see _GET_PED_QUALITY */
    export function setPedQuality(ped: number, quality: number): void;
    export function setPedScale(ped: number, scale: number): void;
    /** 0.0 - 1.0 Modifies the "scent line" on the ped's body when using Eagle Eye. */
    export function setPedScent(ped: number, scent: number): void;
    export function setPedTargetActionDisableFlag(ped: number, actionDisableFlag: number): void;
    export function setPedToBeRemoved(ped: number, p1: number, p2: number, p3: number, p4: any): void;
    export function setPedToDisableRagdoll(ped: number, toggle: boolean): void;
    /** duration in seconds */
    export function setPedTrailEffect(ped: number, p1: boolean, duration: number): void;
    /** Doesn't actually return anything. */
    export function setPedUseHorseMapCollision(ped: number, toggle: boolean): any;
    /** _SET_PED_S* - _SET_PED_T* */
    export function setPedVoiceVolume(ped: number, volume: number): void;
    /** To be used with SET_PED_WETNESS_HEIGHT, see R* Scripts */
    export function setPedWetnessAmount(ped: number, amount: number): void;
    export function setPedWrithingDuration(ped: number, writhingDuration1: number, writhingDuration2: number, p3: number): void;
    export function setPeltForHorse(horse: number, peltId: number): void;
    /** Adds a pelt to a horse from an inventoryItem hash and albedoHash, optionally normalHash textures. */
    export function setPeltForHorseByInventoryItem(horse: number, inventoryItem: number, albedoHash: number, normalHash: number, p4: boolean): void;
    export function setPlayerAntagonizeDisabledForPed(ped: number, player: number, duration: number): void;
    /** Animal Skin Quality Modifier Params: p2 = 2, p3 = 3 in R* Scripts */
    export function setPlayerCurrentAnimalDamageModifier(player: number, modifier: number, p2: number, p3: number): any;
    export function setPlayerDismountTimestamp(mount: number, player: number, dismountedTimestamp: number): void;
    export function setPlayerGreetDisabledForPed(ped: number, player: number, duration: number): void;
    export function setRandomOutfitVariation(ped: number, p1: boolean): void;
    /** Registers/unregisters a relationship group as a script resource (type 0x24). Pass false to register, true to unregister. */
    export function setRelationshipGroupScriptRegistered(group: number, unregister: boolean): void;
    export function setRemovePedNetworked(ped: number, p1: number): void;
    export function setScenarioAnimalDensityMultiplierThisFrame(multiplier: number): void;
    export function setScenarioHumanDensityMultiplierThisFrame(multiplier: number): void;
    /** Sets the scenario ped density to the given config.  Valid configs: - BLACKWATER - DEFAULT - NEWBORDEAUX - RHODES - STRAWBERRY - TUMBLEWEED - VALENTINE - VANHORN  See common/data/ai/densityscoringco... */
    export function setScenarioPedDensityThisFrame(configHash: number): void;
    export function setScenarioPedRangeMultiplierThisFrame(multiplier: number): void;
    /** Registers the given volume with ped/scenario systems (stores a global ref). Used by SP scripts to keep a volume alive; calling again replaces the previous ref. */
    export function setScenarioPedVolumeReference(volume: number): void;
    /** Sets ScriptData bit 0x8000 on the ped (one-way). Used by scripts to mark a staged/special ped (e.g., scripted corpse) affecting cleanup/interaction. */
    export function setStagedPedFlag(ped: number): void;
    export function setStaminaDepletionMultiplier(ped: number, multiplier: number): void;
    export function setStaminaRechargeMultiplier(ped: number, multiplier: number): void;
    /** Size will be permanent */
    export function setTankAttributeSize(ped: number, attributeIndex: number, size: number): void;
    export function setTextureLayerAlpha(textureId: number, layerId: number, texAlpha: number): void;
    export function setTextureLayerMod(textureId: number, layerId: number, modTextureHash: number, modAlpha: number, modChannel: number): void;
    /** paletteHash: https://raw.githubusercontent.com/femga/rdr3_discoveries/master/clothes/cloth_color_palletes.lua */
    export function setTextureLayerPallete(textureId: number, layerId: number, paletteHash: number): void;
    export function setTextureLayerRoughness(textureId: number, layerId: number, texRough: number): void;
    export function setTextureLayerSheetGridIndex(textureId: number, layerId: number, sheetGridIndex: number): void;
    export function setTextureLayerTextureMap(textureId: number, layerId: number, albedoHash: number, normalHash: number, materialHash: number): void;
    /** Seem color is not RGB or HSV */
    export function setTextureLayerTint(textureId: number, layerId: number, tint0: number, tint1: number, tint2: number): void;
    /** Used in script function METAPED_CLOTHING__XML__APPLY_OUTFIT_TINTS_TO_PED */
    export function setTextureOutfitTints(ped: number, componentCategory: number, palette: number, tint0: number, tint1: number, tint2: number): void;
    /** _SET_W(EAPON?)* */
    export function setTotalPedDamageFalloffBonus(ped: number, bonus: number): void;
    /** _SET_W(EAPON?)* */
    export function setTotalPedDamageFromAi(ped: number, totalDamage: number): void;
    /** Triggers a gunshot Params: p5 = -1 in R* Scripts */
    export function shootTriggerAtCoords(ped: number, x: number, y: number, z: number, p4: number, p5: number, p6: number, p7: number): any;
    export function spawnpointsStartSearchWithVolume(volume: number, spawnpointsFlag: number, p2: number, duration: number, p4: number): void;
    /** _TOGGLE_S* - _UPDATE_* */
    export function togglePlayerPedFlinch(ped: number, x: number, y: number, z: number, scale: number): void;
    export function unreserveAmbientPeds(numPeds: number): void;
    export function updateAnimalDamageModifier(player: number): void;
    /** Should be called at least once for any new texture override. Otherwise component textures will be just black. Also needs to be called for updating any ped overlays to apply the changes. */
    export function updatePedTexture(textureId: number): void;
    /** Update variation on ped, needed after first creation, or when component or texture/overlay is changed */
    export function updatePedVariation(ped: number, p1: boolean, p2: boolean, p3: boolean, p4: boolean, p5: boolean): void;
    /** Params: 0.0f to remove wound effects */
    export function updatePedWoundEffect(ped: number, value: number): void;
    /** Possible way to pull up / down bandana for male and female models: https://imgur.com/a/Zr4pjze Params: p3 = 0, p4 = true, p5 = 1 wearableState: https://github.com/Jump-On-Studios/RedM-jo_libs/blob/... */
    export function updateShopItemWearableState(ped: number, componentHash: number, wearableState: number, p3: number, isMp: boolean, p5: number): void;
    export function warpPedOutOfVehicle(ped: number): void;

    // PERSCHAR
    export function createPersistentCharacter(hash: number): number;
    export function deletePerschar(persChar: number): void;
    export function forceDespawnPerschar(persChar: number): void;
    export function forceSpawnPerschar(persChar: number, p1: boolean): number;
    export function getPerscharIndexFromPedIndex(ped: number): number;
    export function getPerscharModelName(persCharHash: number): number;
    export function getPerscharOutfit(persCharHash: number): number;
    export function getPerscharPedIndex(persChar: number): number;
    export function isPersistentCharacterDead(persChar: number): boolean;
    export function isPersistentCharacterValid(persChar: number): boolean;
    export function resetPerscharSchedule(persCharHash: number): void;
    export function retaskPersistentCharacter(persChar: number): void;
    export function revivePerschar(persChar: number): boolean;
    export function setPerscharOutfit(persCharHash: number, outfit: number): void;
    export function setPerscharSchedule(persCharHash: number, schedule: string): void;

    // PERSISTENCE
    export function persistenceAddScenarioLooted(scenario: number): void;
    export function persistenceRemoveAllEntitiesInArea(x: number, y: number, z: number, radius: number): void;
    export function persistenceIsScenarioMarkedAsLooted(scenario: number): boolean;
    export function persistenceIsScenarioMarkedAsLootedAtCoords(x: number, y: number, z: number): boolean;
    export function persistenceIsScenarioMarkedAsLootedAtCoordsWithModel(x: number, y: number, z: number, model: number): boolean;
    /** Only used in R* script long_update.ysc in script function REFRESH_CLOSEST_TOWN */
    export function persistenceRefreshTownVolume(volume: number): void;

    // PHYSICS
    export function activatePhysics(entity: number): void;
    /** There are 19 types of rope, from type = 0 to type = 18 Rope definitions are stored in ropedata.xml Rope types 0, 15 and 18 have proper physics for hanging objects (taut, do not sag, small to medium... */
    export function addRope(x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, length: number, ropeType: number, maxLength: number, minLength: number, p10: number, p11: boolean, p12: boolean, rigid: boolean, p14: number, breakWhenShot: boolean, unkPtr: any, p17: boolean): number;
    /** Attaches entity 1 to entity 2. If you use a boneName (p12/p13) make sure boneId (p15/p16) is set to -1. */
    export function attachEntitiesToRope(ropeId: number, entity1: number, entity2: number, ent1X: number, ent1Y: number, ent1Z: number, ent2X: number, ent2Y: number, ent2Z: number, length: number, alwaysZero1: number, alwaysZero2: number, boneName1: string, boneName2: string, p14: boolean, boneId1: number, boneId2: number, alwaysZero3: number, alwaysZero4: number, p19: boolean, p20: boolean): void;
    export function breakEntityGlass(entity: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: any, p10: boolean): void;
    export function deleteChildRope(ropeId: number): void;
    export function deleteRope(ropeId: number): void;
    export function detachRopeFromEntity(ropeId: number, entity: number): void;
    export function doesRopeExist(ropeId: number): boolean;
    export function getRopeLastVertexCoord(ropeId: number): Vector3;
    export function getRopeVertexCoord(ropeId: number, vertex: number): Vector3;
    export function getRopeVertexCount(ropeId: number): number;
    export function ropeDrawShadowEnabled(ropeId: number, toggle: boolean): void;
    /** Forces a rope to a certain length. */
    export function ropeForceLength(ropeId: number, length: number): void;
    export function ropeSetUpdateOrder(ropeId: number, p1: any): void;
    export function setDamping(entity: number, vertex: number, value: number): void;
    export function setDisableBreaking(object: number, toggle: boolean): void;
    export function setDisableFragDamage(object: number, toggle: boolean): void;
    export function startRopeUnwindingFront(ropeId: number): void;
    export function startRopeWinding(ropeId: number): void;
    export function stopRopeUnwindingFront(ropeId: number): void;
    export function stopRopeWinding(ropeId: number): void;
    export function addRope2(x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, length: number, ropeType: number, isNetworked: boolean, p9: number, p10: number): number;
    export function attachEntitesToRope3(ropeId: number, entity1: number, entity2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: any, p10: any): void;
    /** Attaches a rope to two entities: binds two bones from two entities; one entity can be an object, i.e. a suspension point, the other an NPC bone */
    export function attachEntitiesToRope2(ropeId: number, entity1: number, entity2: number, ent1X: number, ent1Y: number, ent1Z: number, ent2X: number, ent2Y: number, ent2Z: number, boneName1: string, boneName2: string): void;
    /** ropeTop returns top half of rope, ropeBottom returns bottom half of rope */
    export function breakRope(ropeId: number, ropeTop: number, ropeBottom: number, offsetX: number, offsetY: number, offsetZ: number, p6: number): void;
    /** Combining this with ADD_ROPE enables winding p1: mostly empty (0) ropeModelType: RB_L_Wrist02, RB_R_Wrist02, ropeAttach, noose01x_Rope_03, SKEL_Neck0, SKEL_L_FOOT, SKEL_Neck1, Root_s_meatbit_Chunck... */
    export function createRopeWindingAbility(ropeId: number, p1: string, ropeModelType: string, length: number, p4: boolean): void;
    export function hitchHorse(horse: number, x: number, y: number, z: number): void;
    export function isRopeAttachedToEntity(ropeId: number, entity: number): boolean;
    export function isRopeBroken(ropeId: number): boolean;
    export function releaseRope(ropeId: number): void;
    export function ropeChangeVisibility(ropeId: number, visible: boolean): void;
    export function ropeGetBreakerOfRope(ropeId: number): number;
    export function ropeGetForcedLength(ropeId: number): number;
    export function startRopeUnwindingBack(ropeId: number): void;
    export function stopRopeUnwindingBack(ropeId: number): void;
    export function unhitchHorse(horse: number): void;

    // PLAYER
    export function boostPlayerHorseSpeedForTime(player: number, speedBoost: number, duration: number): void;
    export function canPlayerStartMission(player: number): boolean;
    export function clearPlayerHasDamagedAtLeastOneNonAnimalPed(player: number): void;
    export function clearPlayerHasDamagedAtLeastOnePed(player: number): void;
    /** nullsub, doesn't do anything */
    export function clearPlayerWantedLevel(player: number): void;
    /** Inhibits the player from using any method of combat including melee and firearms.  NOTE: Only disables the firing for one frame */
    export function disablePlayerFiring(player: number, toggle: boolean): void;
    export function eagleEyeSetCustomEntityTint(entity: number, red: number, green: number, blue: number): void;
    export function forceCleanup(cleanupFlags: number): void;
    export function forceCleanupForAllThreadsWithThisName(name: string, cleanupFlags: number): void;
    export function forceCleanupForThreadWithThisId(id: number, cleanupFlags: number): void;
    export function getCauseOfMostRecentForceCleanup(): number;
    /** Returns name hash (name) and outHash includes the type. */
    export function getDiscoverableNameHashAndTypeForEntity(entity: number, type: number): number;
    export function getEntityPlayerIsFreeAimingAt(player: number, entity: number): boolean;
    export function getHasPlayerDiscoveredCharacterNameMp(discoveryHash: number): boolean;
    export function getIsPlayerUiPromptActive(player: number, p1: number): boolean;
    /** Gets the maximum wanted level the player can get. Ranges from 0 to 5. */
    export function getMaxWantedLevel(): number;
    export function getMountOwnedByPlayer(player: number): number;
    export function getPlayersLastVehicle(): number;
    export function getPlayerCurrentStealthNoise(player: number): number;
    /** Returns the group ID the player is member of. */
    export function getPlayerGroup(player: number): number;
    /** Returns the same as PLAYER_ID and NETWORK_PLAYER_ID_TO_INT */
    export function getPlayerIndex(): number;
    export function getPlayerInteractionTargetEntity(player: number, outEntity: number, p2: boolean, p3: boolean): boolean;
    /** Returns the player's invincibility status. */
    export function getPlayerInvincible(player: number): boolean;
    export function getPlayerName(player: number): NativeString;
    export function getPlayerPed(player: number): number;
    /** Does the same like PLAYER::GET_PLAYER_PED */
    export function getPlayerPedScriptIndex(player: number): number;
    export function getPlayerReceivedBattleEventRecently(player: number, p1: number, p2: boolean): boolean;
    export function getPlayerTargetEntity(player: number, entity: number): boolean;
    /** Gets the player's team. Returns -1 in singleplayer. */
    export function getPlayerTeam(player: number): number;
    export function getPlayerWantedLevel(player: number): number;
    export function getTargetCharacterNameForLocalPlayer(ped: number): number;
    export function getTargetCharacterNameScriptOverrideHash(ped: number): number;
    export function getTargetCharacterNameScriptOverrideRawString(ped: number): NativeString;
    export function getWantedLevelRadius(p0: number): number;
    export function getWantedLevelThreshold(wantedLevel: number): number;
    export function hasForceCleanupOccurred(cleanupFlags: number): boolean;
    export function hasPlayerBeenSpottedInStolenVehicle(player: number): boolean;
    export function hasPlayerDamagedAtLeastOneNonAnimalPed(player: number): boolean;
    export function hasPlayerDamagedAtLeastOnePed(player: number): boolean;
    /** Simply returns whatever is passed to it (Regardless of whether the handle is valid or not). */
    export function intToParticipantindex(value: number): number;
    /** Simply returns whatever is passed to it (Regardless of whether the handle is valid or not). */
    export function intToPlayerindex(value: number): number;
    /** Return true while player is being arrested / busted.  If atArresting is set to 1, this function will return 1 when player is being arrested (while player is putting his hand up, but still have cont... */
    export function isPlayerBeingArrested(player: number, atArresting: boolean): boolean;
    /** Returns TRUE if the player ('s ped) is climbing at the moment. */
    export function isPlayerClimbing(player: number): boolean;
    /** Returns whether the player can control himself. */
    export function isPlayerControlOn(player: number): boolean;
    export function isPlayerDead(player: number): boolean;
    /** Gets a value indicating whether the specified player is currently aiming freely. */
    export function isPlayerFreeAiming(player: number): boolean;
    /** Gets a value indicating whether the specified player is currently aiming freely at the specified entity. */
    export function isPlayerFreeAimingAtEntity(player: number, entity: number): boolean;
    /** Checks whether the specified player has a Ped, the Ped is not dead, is not injured and is not arrested. */
    export function isPlayerPlaying(player: number): boolean;
    export function isPlayerReadyForCutscene(player: number): boolean;
    /** Returns true if the player is riding a train. */
    export function isPlayerRidingTrain(player: number): boolean;
    export function isPlayerScriptControlOn(player: number): boolean;
    export function isPlayerTargettingAnything(player: number): boolean;
    export function isPlayerTargettingEntity(player: number, entity: number, p2: boolean): boolean;
    export function isPlayerTeleportActive(): boolean;
    export function isPlayerWantedLevelGreater(player: number, wantedLevel: number): boolean;
    export function isSystemUiBeingDisplayed(): boolean;
    /** Does exactly the same thing as PLAYER_ID() */
    export function networkPlayerIdToInt(): number;
    /** This returns YOUR 'identity' as a Player type.  Always returns 0 in story mode. */
    export function playerId(): number;
    /** Returns current player ped */
    export function playerPedId(): number;
    export function reportPoliceSpottedPlayer(player: number): void;
    export function resetLawResponseDelayOverride(): void;
    export function resetPlayerArrestState(player: number): void;
    export function resetPlayerInputGait(player: number): void;
    export function resetWantedLevelDifficulty(player: number): void;
    export function restorePlayerStamina(player: number, p1: number): void;
    /** This can be between 1.0f - 50.0f */
    export function setAirDragMultiplierForPlayersVehicle(player: number, multiplier: number): void;
    export function setAllNeutralRandomPedsFleeThisFrame(player: number): void;
    /** Sets whether all random peds will run away from player if they are agitated (threatened) (bool=true), or some peds can stand up for themselves (bool=false). */
    export function setAllRandomPedsFlee(player: number, toggle: boolean): void;
    export function setAllRandomPedsFleeThisFrame(player: number): void;
    export function setEveryoneIgnorePlayer(player: number, toggle: boolean): void;
    export function setLawResponseDelayOverride(p0: number): void;
    export function setLockonToFriendlyPlayers(player: number, toggle: boolean): void;
    export function setMaxWantedLevel(maxWantedLevel: number): void;
    export function setMinTimeBeforeHorseBucking(mount: number, iMinBuckTime: number): void;
    export function setPedAsTempPlayerHorse(player: number, horse: number): boolean;
    /** Sets whether this player can be hassled by gangs. */
    export function setPlayerCanBeHassledByGangs(player: number, toggle: boolean): void;
    /** Sets whether this player can take cover. */
    export function setPlayerCanUseCover(player: number, toggle: boolean): void;
    export function setPlayerClothPinFrames(ped: number, p1: number): void;
    /** flags: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eSetPlayerControlFlags */
    export function setPlayerControl(player: number, toggle: boolean, flags: number, bPreventHeadingChange: boolean): void;
    export function setPlayerForcedAim(player: number, toggle: boolean, ped: number, p3: number, p4: boolean): void;
    export function setPlayerHealthRechargeMultiplier(player: number, regenRate: number): void;
    /** Simply sets you as invincible (Health will not deplete). */
    export function setPlayerInvincible(player: number, toggle: boolean): void;
    export function setPlayerLockon(player: number, toggle: boolean): void;
    /** Affects the range of auto aim target. */
    export function setPlayerLockonRangeOverride(player: number, range: number): void;
    export function setPlayerMayNotEnterAnyVehicle(player: number): void;
    export function setPlayerMayOnlyEnterThisVehicle(player: number, vehicle: number): void;
    export function setPlayerMeleeWeaponDamageModifier(player: number, modifier: number): void;
    /** Make sure to request the model first and wait until it has loaded. */
    export function setPlayerModel(player: number, modelHash: number, p2: boolean): void;
    export function setPlayerNoiseMultiplier(player: number, multiplier: number): void;
    export function setPlayerSimulateAiming(player: number, toggle: boolean): void;
    export function setPlayerSneakingNoiseMultiplier(player: number, multiplier: number): void;
    export function setPlayerStaminaRechargeMultiplier(player: number, multiplier: number): void;
    /** Sets your targeting mode for when you're on foot. enum eTargetingMode { 	TARGETING_MODE_INVALID = -1, 	TARGETING_MODE_CAUSAL, (Wide) 	TARGETING_MODE_NORMAL, 	TARGETING_MODE_HARD, (Narrow) 	TARGETIN... */
    export function setPlayerTargetingMode(targetMode: number): void;
    /** Sets the player's team. */
    export function setPlayerTeam(player: number, team: number, bRestrictToThisScript: boolean): void;
    /** nullsub, doesn't do anything */
    export function setPlayerWantedLevel(player: number, wantedLevel: number, disableNoMission: boolean): void;
    /** This modifies the damage value of your weapon. Whether it is a multiplier or base damage is unknown. */
    export function setPlayerWeaponDamageModifier(player: number, modifier: number): void;
    export function setPlayerWeaponDefenseModifier(player: number, modifier: number): void;
    export function setPlayerWeaponTypeDamageModifier(player: number, weaponHash: number, damageModifier: number): void;
    /** If toggle is set to false:  The police won't be shown on the (mini)map  If toggle is set to true:  The police will be shown on the (mini)map */
    export function setPoliceRadarBlips(toggle: boolean): void;
    /** Swim speed multiplier. Multiplier goes up to 1.49f */
    export function setSwimMultiplierForPlayer(player: number, multiplier: number): void;
    export function setWantedLevelMultiplier(multiplier: number): void;
    export function simulatePlayerInputGait(player: number, speed: number, duration: number, heading: number, p4: boolean, p5: boolean): void;
    export function startPlayerTeleport(player: number, x: number, y: number, z: number, heading: number, p5: boolean, p6: boolean, p7: boolean, p8: boolean): void;
    /** Disables the player's teleportation */
    export function stopPlayerTeleport(): void;
    export function suppressWitnessesCallingPoliceThisFrame(player: number): void;
    export function updatePlayerTeleport(player: number): boolean;
    export function updateWantedPositionThisFrame(player: number): void;
    /** Associates a specific "interactive focus mode preset" between a player and a ped, with a specified location and target entity. To access all available presets, refer to the file located at: `\updat... */
    export function addAmbientPlayerInteractiveFocusPreset(player: number, ped: number, preset: string, x: number, y: number, z: number, targetEntity: number, name: string): void;
    /** Adds an "interactive focus mode preset" between a player and a specific set of coordinates with a target entity. To access all available presets, refer to the file located at: `\update_1.rpf\common... */
    export function addAmbientPlayerInteractiveFocusPresetAtCoords(player: number, x1: number, y1: number, z1: number, preset: string, x2: number, y2: number, z2: number, targetEntity: number, name: string): void;
    /** Used in script function: NET_AUTO_FOLLOW_UPDATE_LEADER_VALUES followMode: HORSEFOLLOWMODE_AUTO = 0, HORSEFOLLOWMODE_SIDE_ONLY, HORSEFOLLOWMODE_BEHIND_ONLY, HORSEFOLLOWMODE_BEHIND_AND_SIDE, HORSEFOL... */
    export function addPlayerAsFollowTarget(player: number, ped: number, p2: number, p3: number, followMode: number, followPriority: number, p6: boolean): void;
    export function clearBountyTarget(player: number): void;
    /** Clears the intensity of aura effects applied to entities for a specific player in Deadeye mode based on a flag parameter. This function is used to reset any intensity modifications set by `PLAYER::... */
    export function clearDeadeyeAuraEffectIntensity(player: number, flag: number): void;
    /** Clears all eagle eye trails that were registered for peds (maybe also other entities?) associated with specified player. Video: https://imgur.com/a/uvCTPei _CLEAR_FACIAL_* - _CLEAR_PED_BLOOD* */
    export function clearPedEagleEyeTrailsForPlayer(player: number): void;
    /** Disables the previously set "interactive focus mode preset" for a given player. Example usage: PLAYER::_DISABLE_PLAYER_INTERACTIVE_FOCUS_PRESET(Player::PLAYER_ID(), "qadr_");  This example disables... */
    export function disablePlayerInteractiveFocusPreset(player: number, name: string): void;
    /** Enable/disable the focus skill on given entity in eagle eye mode, which makes the entity glow up. Video: https://imgur.com/a/e1ph146 */
    export function eagleEyeAddFocusSkill(entity: number, enable: boolean): void;
    /** Retrieves whether all trails are currently hidden during Eagle Eye mode for the specified player. Images: - https://imgur.com/gallery/0x330ca55a3647fa1c-0xa62bbaae67a05bb0-Lpzt2Yi - https://imgur.c... */
    export function eagleEyeAreAllTrailsHidden(player: number): boolean;
    /** Checks if the player can focus on tracks while in Eagle Eye mode. Returns true if the player is able to focus on a track, otherwise false. Example usage: if (PLAYER::_EAGLE_EYE_CAN_PLAYER_FOCUS_ON_... */
    export function eagleEyeCanPlayerFocusOnTrack(player: number): boolean;
    export function eagleEyeDisableTrackingTrail(entity: number, trail: string, p2: any, p3: any): void;
    /** Retrieves the ID of the ped that the specified player is currently tracking while in Eagle Eye mode. Images: - https://imgur.com/gallery/0x3813e11a378958a5-CHoJVRu - https://imgur.com/gallery/0x381... */
    export function eagleEyeGetTrackedPedId(player: number): number;
    /** Clears yellow indicator particle effects from given entity. */
    export function eagleEyeRemoveParticleFxFromEntity(entity1: number, entity2: number, p2: number): void;
    /** false: default eagleeye color true: green eagleeye color */
    export function eagleEyeSetColor(player: number, p1: boolean, p2: any): void;
    export function eagleEyeSetCustomDistance(entity: number, distance: number): void;
    export function eagleEyeSetDrainRateModifier(player: number, modifier: number): void;
    export function eagleEyeSetFocusOnAssociatedClueTrail(player: number, linkedWaypointPed: number): void;
    /** Sets whether all trails are hidden during Eagle Eye mode. Example usage: Hide all trails in Eagle Eye mode for the current player PLAYER::_EAGLE_EYE_SET_HIDE_ALL_TRAILS(PLAYER::PLAYER_ID(), true); ... */
    export function eagleEyeSetHideAllTrails(player: number, hideTrails: boolean): void;
    /** Adds yellow indicator particle effects to given entity. Image: https://imgur.com/a/cbldo35 */
    export function eagleEyeSetParticleFxToEntity(entity1: number, entity2: number, p2: number, heading: number): void;
    /** Sets the behavior of sprinting while the eagle eye feature is active, determining whether sprinting cancels the effect based on the specified parameter. disabled = true: sprinting will cancel the e... */
    export function eagleEyeSetPlusFlagDisabled(player: number, disabled: boolean): void;
    export function eagleEyeSetRange(player: number, range: number): void;
    export function eagleEyeSetTrackingUpgrade(player: number, p1: number): void;
    export function eagleEyeSetTrackingUpgrade2(player: number, p1: number): void;
    export function enableCustomDeadeyeAbility(player: number, enable: boolean): void;
    /** (Un)lock Eagle Eye functionality */
    export function enableEagleeye(player: number, enable: boolean): void;
    export function forceRestScenario(toggle: boolean): void;
    export function formatPlayerNameString(string: string): NativeString;
    export function getActiveHorseForPlayer(player: number): number;
    export function getAiPlayerDefenseModifierAgainstAi(player: number): number;
    /** p0: mostly Ped Hashes */
    export function getConstructedDiscoveredCharacterName(p0: number, model: boolean, outfit: boolean): number;
    /** Returns the depletion delay value for the Deadeye ability that was previously set using `PLAYER::_SET_DEADEYE_ABILITY_DEPLETION_DELAY` (0x870634493CB4372C). This function provides a float value rep... */
    export function getDeadeyeAbilityDepletionDelay(player: number): number;
    export function getDeadeyeAbilityLevel(player: number): number;
    export function getHasPlayerDiscoveredCharacterNameSp(player: number, p1: number, discoveryHash: number): boolean;
    export function getIsDeadeyeTaggingEnabled(player: number): boolean;
    /** Retrieves the number of marks placed on a PED when Deadeye mode is active for the specified player. Example usage: int marksCount = PLAYER::_GET_NUM_DEADEYE_MARKS_ON_PED(PLAYER::PLAYER_ID(), pedHan... */
    export function getNumDeadeyeMarksOnPed(player: number, ped: number): number;
    export function getNumMarkedDeadeyeTargets(player: number): number;
    /** Alternate name: _GET_PEDS_DAMAGED_BY_PLAYER Returns an array of peds the player has recently attacked in a combo, tracking up to three consecutive peds. Video: https://imgur.com/gallery/0x1a6e84f13... */
    export function getPedsInCombatWithRecently(player: number, recentlyMs: number, outArray: any): boolean;
    export function getPlayerCachedDeadEyeAmount(player: number): number;
    export function getPlayerDeadEye(player: number): number;
    export function getPlayerDeadEyeMeterLevel(player: number, p1: boolean): number;
    export function getPlayerHealth(player: number): number;
    export function getPlayerHealthRechargeMultiplier(player: number): number;
    export function getPlayerHuntingWagon(player: number): number;
    /** Get the entity the player is aiming at with/without weapon. */
    export function getPlayerInteractionAimEntity(player: number, outEntity: number): boolean;
    export function getPlayerMaxDeadEye(player: number, p1: any): number;
    /** See _SET_PLAYER_MOOD */
    export function getPlayerMood(player: number): number;
    /** Checks if the player is sprinting on a road while riding a horse. This function only checks sprinting status when the player is on a road.  Video: https://youtu.be/cGyh0AXPu1E */
    export function getPlayerMountIsSprintingOnRoad(player: number): boolean;
    export function getPlayerOwnerOfMount(mount: number): number;
    export function getPlayerOwnerOfVehicle(vehicle: number): number;
    export function getPlayerPed2(player: number): number;
    /** If player has less Dead Eye than required, Dead Eye cant be triggered. */
    export function getPlayerRequiredDeadEyeAmount(player: number): number;
    /** playerResetFlag: See 0x9F9A829C6751F3C7 */
    export function getPlayerResetFlag(player: number, playerResetFlag: number): boolean;
    export function getPlayerSpecialAbilityMultiplier(player: number): number;
    export function getPlayerStamina(player: number): number;
    export function getPlayerStaminaDepletionMultiplier(player: number): number;
    export function getPlayerStaminaRechargeMultiplier(player: number): number;
    /** Returns true if PromptType is enabled for ped (mount) Params: See 0x0751D461F06E41CE */
    export function getPlayerUiPromptForPedIsEnabled(player: number, ped: number, promptType: number, promptMode: number): boolean;
    /** Returns false if PromptType is enabled Params: See 0x0751D461F06E41CE */
    export function getPlayerUiPromptIsDisabled(player: number, promptType: number, promptMode: number): boolean;
    export function getPlayerWeaponDamage(player: number, weaponHash: number): number;
    export function getSaddleHorseForPlayer(player: number): number;
    export function getTempPlayerHorse(player: number): number;
    export function getVehicleOwnedByPlayer(player: number): number;
    /** Returns -1.0f if no multiplier has been set */
    export function getWantedLevelMultiplier(): number;
    /** Checks if the player has damaged the ped they recently attacked. Useful for determining if the player's recent attack on a ped resulted in damage. */
    export function hasPlayerDamagedRecentlyAttackedPed(player: number, recentlyMs: number): boolean;
    export function isDeadeyeAbilityLocked(player: number, abilityType: number): boolean;
    export function isEagleEyeRegisteredForEntity(player: number, entity: number): boolean;
    export function isPlayerFollowingTarget(player: number, ped: number): boolean;
    /** Checks if player is focused on any entity */
    export function isPlayerFreeFocusing(player: number): boolean;
    export function isPlayerInScope(player: number): boolean;
    /** Checks if the player has locked onto an entity while on horseback. This function checks only the lock-on status and does not trigger any additional behavior. Images: https://imgur.com/gallery/0x200... */
    export function isPlayerLockedOnEntityOnHorse(player: number): boolean;
    export function isSecondarySpecialAbilityActive(player: number): boolean;
    /** Returns true if eagle eye is enabled for the player */
    export function isSecondarySpecialAbilityEnabled(player: number): boolean;
    export function isSpecialAbilityActive(player: number): boolean;
    /** Checks if the player's Deadeye ability is enabled.  Example usage:  if (PLAYER::_IS_SPECIAL_ABILITY_ENABLED(PLAYER::PLAYER_ID())) {     // Execute logic when Deadeye is enabled }  This function doe... */
    export function isSpecialAbilityEnabled(player: number): boolean;
    /** Toggle handles wether Deadeye and Eagleeye are infinite or not. */
    export function modifyInfiniteTrailVision(player: number, toggle: boolean): void;
    export function modifyPlayerDiscoveredCharacterNameMpSetUndiscovered(discoveryHash: number): void;
    /** Params: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/ePromptType promptType is mostly 34 (PP_TRACK_ANIMAL), promptMode = 0 (PP_MODE_BLOCK) in R* Scripts */
    export function modifyPlayerUiPrompt(player: number, promptType: number, promptMode: number, disabled: boolean): void;
    /** Params: See 0x0751D461F06E41CE */
    export function modifyPlayerUiPromptForPed(player: number, ped: number, promptType: number, promptMode: number, enabled: boolean): void;
    /** Returns true if the given player has a valid ped. */
    export function networkHasPlayerValidPed(player: number): boolean;
    /** Used for setting up eagle eye for entity Params: p2 = re-register or not? */
    export function registerEagleEyeForEntity(player: number, entity: number, p2: boolean): void;
    export function registerEagleEyeTrailsForEntity(player: number, entity: number, p2: any): void;
    export function removePlayerAsFollowTarget(player: number, ped: number): void;
    /** Resets any aura effects applied to entities for a specific player in Deadeye mode, returning all aura-related visuals to their default state. This function is primarily used to remove any highlight... */
    export function resetDeadeyeAuraEffect(player: number): void;
    /** Activates EagleEye, called together with 0x28A13BF6B05C3D83 */
    export function secondarySpecialAbilitySetActive(player: number): void;
    /** Deactivates EagleEye, called together with 0xC0B21F235C02139C */
    export function secondarySpecialAbilitySetDisabled(player: number, disabled: boolean): void;
    /** Sets Player's Defense against AI modifier */
    export function setAiPlayerDefenseModifierAgainstAi(player: number, modifier: number): void;
    export function setBountyTarget(player: number, target: number): void;
    export function setBowDrawReductionTimeInDeadeye(player: number, drawReductionTime: number): void;
    /** Decreases Stamina bar drain speed by % when drawing a bow. */
    export function setBowStaminaDrainSpeed(player: number, staminaDrain: number): void;
    export function setDamageCloseDistanceBonus(player: number, closeRangeLowerBound: number, closeRangeUpperBound: number): void;
    export function setDamageCloseDistanceBonusTotal(player: number, closeDamageBonus: number): void;
    export function setDamageFarDistanceBonus(player: number, farRangeLowerBound: number, farRangeUpperBound: number): void;
    export function setDamageFarDistanceBonusTotal(player: number, farDamageBonus: number): void;
    /** Only used in R* SP Script short_update */
    export function setDeadeyeAbilityDepletionDelay(player: number, delay: number): void;
    /** Max level is 5. */
    export function setDeadeyeAbilityLevel(player: number, level: number): void;
    export function setDeadeyeAbilityLocked(player: number, abilityType: number, toggle: boolean): void;
    /** Applies an aura effect to nearby entities when Deadeye is active, based on a flag parameter. This includes humans, animals, vehicles, and horses pulling those vehicles. Additionally, depending on t... */
    export function setDeadeyeEntityAuraEffect(player: number, flag: number): void;
    /** Applies a customizable aura effect to nearby entities when Deadeye is active, with control over aura intensity and additional behavior based on a flag parameter.  auraIntensity: maximum value of 1.... */
    export function setDeadeyeEntityAuraEffectIntensity(player: number, p1: number, p2: number, p3: number, auraIntensity: number, flag: number): void;
    export function setDeadeyeTaggingConfig(player: number, filter: number): void;
    export function setDeadeyeTaggingEnabled(player: number, toggle: boolean): void;
    /** Disables the players ability to be wanted by lawmen */
    export function setDisablePlayerWantedLevel(player: number, disable: boolean): void;
    /** see personaabilities.meta enum ePersonaAbilityFlag { 	PERSONA_CAN_AUTOESCAPE_FROM_LASSO, 	PERSONA_HAT_BLOCKS_FIRST_HEADSHOT, 	PERSONA_FULL_AUTO_FOR_ALL_WEAPONS, 	PERSONA_MIGHT_LIVE_AFTER_DEADLY_DAM... */
    export function setLocalPlayerPersonaAbilityFlag(flagId: number, toggle: boolean): void;
    /** Focus Fire VFX start for player: p1 = focusfire */
    export function setLockonFocusFireVfx(player: number, p1: string): void;
    export function setMaxWantedLevel2(maxWantedLevel: number): void;
    export function setMountPromptDisabled(disabled: boolean): void;
    /** Seems to work similar to 0xD2CB0FB0FDCB473D */
    export function setPedActivePlayerHorse(player: number, horse: number): void;
    export function setPedAsSaddleHorseForPlayer(player: number, mount: number): void;
    /** Sets the weapon that the specified player will aim with. The weapon must already be assigned to the PED. This also determines the weapon order, specifying which weapon the player will automatically... */
    export function setPlayerAimWeapon(player: number, weapon: number, weaponDrawOrder: number): void;
    export function setPlayerCanMercyKill(player: number, toggle: boolean): void;
    /** Shows or hides all "Pick Up" prompts for the specified player, including the prompt for picking up hats from the ground. When set to true, the player will see "Pick Up" prompts for all nearby items... */
    export function setPlayerCanPickupAbility(player: number, isVisible: boolean): void;
    /** Enables or disables the "Pick Up" prompt for a hat on the ground for the specified player. When set to true, the player will see a prompt to pick up the hat if they are near it. Video: https://imgu... */
    export function setPlayerCanPickupHat(player: number, enable: boolean): void;
    /** Activates the "Surrender" prompt for the specified player in the current frame. Notes: - Continuous Activation: Must be called every frame to keep the "Surrender" prompt active. - Prompt Grouping: ... */
    export function setPlayerCooperatePromptThisFrame(player: number, targetPed: number, promptOrder: number, unknownFlag: boolean): void;
    /** damageInfo: STANDARD_PED_DAMAGE, STANDARD_FEMALE_PED_DAMAGE, STANDARD_PLAYER_PED_DAMAGE_MP, STANDARD_FEMALE_PLAYER_PED_DAMAGE_MP */
    export function setPlayerDamageInfoOverride(player: number, damageInfo: string): void;
    /** Sets the aura color for entities that the player can target in Deadeye mode, based on a specific hash value. Known hash :  - 1014693585  - 1936842089  - 1979474018 Example usage: PLAYER::_SET_PLAYE... */
    export function setPlayerDeadEyeAuraByHash(player: number, auraHash: number): void;
    /** Sets stamina core drains peed using ranged damage scale and melee damage scale */
    export function setPlayerDefenseModifier(player: number, weaponDefenseMod: number, meleeDefenseMod: number): void;
    /** bullet damage modifier: type = 4 explosive damage Defense mod: type = 7 fire damage Defense mod: type = 8, 15 */
    export function setPlayerDefenseTypeModifier(player: number, type: number, defenseModifier: number): void;
    export function setPlayerExplosiveWeaponDamageModifier(player: number, modifier: number): void;
    export function setPlayerHasDiscoveredCharacterNameMp(discoveryHash: number): void;
    export function setPlayerHasDiscoveredCharacterNameSp(player: number, p1: number, discoveryHash: number): void;
    /** Sets the player's ability to wear hats based on the specified flag. The flag value determines whether the player can wear all hats or only the ones they own.  If the flag is set to 15 and `allow` i... */
    export function setPlayerHatAccess(player: number, flag: number, enable: boolean): void;
    /** Setting player's Health recharge time to zero forces immediate health regen */
    export function setPlayerHealthRechargeTimeModifier(player: number, modifier: number): void;
    /** Only applies to HUNTERCART01 */
    export function setPlayerHuntingWagon(player: number, wagon: number): void;
    export function setPlayerInteractionNegativeResponse(player: number, speech: string): void;
    export function setPlayerInteractionPositiveResponse(player: number, speech: string): void;
    /** Sets your targeting mode for when you're in a vehicle (perhaps a mount/horse). see SET_PLAYER_TARGETING_MODE for eTargetingMode */
    export function setPlayerInVehicleTargetingMode(targetMode: number): void;
    /** _SET_PLAYER_A* - _SET_PLAYER_C* */
    export function setPlayerLassoDamagePerSecond(player: number, damage: number): void;
    export function setPlayerLocalAccuracyFloorModifier(player: number, accuracy: number): void;
    export function setPlayerManageBuffSuperJump(player: number, p1: number): void;
    export function setPlayerMaxAmmoOverrideForAmmoType(player: number, ammoType: number, amount: number): void;
    /** mood: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/ePedMood */
    export function setPlayerMood(player: number, mood: number): void;
    /** Name could potentially be inaccurate. Used in Script Function HORSE_SETUP_PLAYER_HORSE_ATTRIBUTES (p1 = true) _SET_PLAYER_L* - _SET_PLAYER_M* */
    export function setPlayerMountStateActive(player: number, active: boolean): void;
    /** Seems to enable active horse equipment prompt when being near it and enables the control that opens the inventory as well */
    export function setPlayerOwnsMount(player: number, mount: number): void;
    export function setPlayerOwnsVehicle(player: number, vehicle: number): void;
    /** Sets the stand prompt for a specific player using a predefined text entry. Example usage: PLAYER::_SET_PLAYER_PROMPT_LEAVE_TEXT(PLAYER::PLAYER_ID(), MISC::VAR_STRING(10, "LITERAL_STRING", "Get on y... */
    export function setPlayerPromptLeaveText(player: number, promptTextKey: string): void;
    /** Sets the melee combat prompt for a specific player using a predefined text entry. Example usage: PLAYER::_SET_PLAYER_PROMPT_MELEE_TEXT(PLAYER::PLAYER_ID(), MISC::VAR_STRING(10, "LITERAL_STRING", "T... */
    export function setPlayerPromptMeleeText(player: number, promptTextKey: string): void;
    /** Sets the sit prompt for a specific player using a predefined text entry. Example usage: PLAYER::_SET_PLAYER_PROMPT_SIT_TEXT(PLAYER::PLAYER_ID(), MISC::VAR_STRING(10, "LITERAL_STRING", "Take a Seat"... */
    export function setPlayerPromptSitText(player: number, promptTextKey: string): void;
    export function setPlayerRemoteAccuracyFloorModifier(player: number, accuracy: number): void;
    /** https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/ePlayerResetFlags https://github.com/femga/rdr3_discoveries/tree/master/AI/PLAYER_RESET_FLAGS */
    export function setPlayerResetFlag(player: number, playerResetFlag: number, p2: boolean): void;
    export function setPlayerStaminaSprintDepletionMultiplier(player: number, multiplier: number): void;
    export function setPlayerTotalAccuracyModifier(player: number, accuracy: number): void;
    export function setPlayerTrampleDamageModifier(player: number, modifier: number): void;
    export function setPlayerWeaponGroupAsInstantKill(player: number, weaponGroup: number, toggle: boolean): void;
    export function setPlayerWeaponGroupDamageModifier(player: number, weaponGroup: number, modifier: number): void;
    /** Decreases the damage the player receives while on horseback Previous name: _SET_RECEIVED_HORSEBACK_DAMAGE_DECREASE */
    export function setReceivedDamageTakenOnHorsebackModifier(player: number, damageDecrease: number): void;
    export function setShowInfoCard(player: number, showingInfoCard: boolean): void;
    export function setSpecialAbilityActivationCost(player: number, activationCost: number, p2: number): void;
    /** Only used in R* SP Script short_update */
    export function setSpecialAbilityDisableTimer(player: number, timer: number): void;
    /** durationCost: per second */
    export function setSpecialAbilityDurationCost(player: number, durationCost: number): void;
    export function setSpecialAbilityMultiplier(player: number, multiplier: number): void;
    /** SPECIAL_ABILITY_NONE = -1, SPECIAL_ABILITY_CAR_SLOWDOWN, SPECIAL_ABILITY_RAGE, SPECIAL_ABILITY_BULLET_TIME, SPECIAL_ABILITY_SNAPSHOT, SPECIAL_ABILITY_INSULT, SPECIAL_ABILITY_DEADEYE, SPECIAL_ABILIT... */
    export function setSpecialAbilityType(player: number, type: number): void;
    export function setUsedItemEffect(health: number, stamina: number, deadeye: number, healthCore: number, staminaCore: number, deadeyeCore: number): void;
    export function setWeaponDegradationModifier(player: number, modifier: number): void;
    export function setWeaponDrawSpeed(player: number, weaponHash: number, modifier: number): void;
    /** Drains Deadeye by given amount. */
    export function specialAbilityDrainByAmount(player: number, amount: number, p2: any): void;
    /** Returns Deadeye value from player */
    export function specialAbilityGetAmountCached(player: number): number;
    /** Restores Deadeye by given amount. Params: p2, p3, p4 = 0, 0, 1 in R* Scripts */
    export function specialAbilityRestoreByAmount(player: number, amount: number, p2: number, p3: number, p4: number): void;
    /** Only used in R* SP Script short_update Restores Deadeye Outer Ring */
    export function specialAbilityRestoreOuterRing(player: number, amount: number): void;
    /** Activates the special ability for the specified player. Example usage:  Activate the special ability for the current player PLAYER::_SPECIAL_ABILITY_SET_ACTIVATE(PLAYER::PLAYER_ID());  Video: https... */
    export function specialAbilitySetActivate(player: number): void;
    export function specialAbilitySetDisabled(player: number, disabled: boolean): void;
    export function specialAbilitySetEagleEyeDisabled(player: number): void;
    /** Params: p1 = -1 in R* Scripts */
    export function specialAbilityStartRestore(player: number, abilityType: number, p2: boolean): void;
    export function unregisterEagleEyeForEntity(player: number, entity: number): void;
    export function unregisterEagleEyeTrailsForEntity(player: number, entity: number, p2: any): void;

    // POPULATION
    export function clearSpawnerInfoPriority(p0: number, p1: number): void;
    export function disableAmbientRoadPopulation(unk: boolean): void;
    export function enableAmbientRoadPopulation(): void;
    export function getNumModelsInPopulationSet(popSetHash: number): number;
    export function getPedModelNameInPopulationSet(popSetHash: number, index: number): number;
    export function getRandomModelFromPopulationSet(popSetHash: number, flags: number, p2: number, p3: boolean, p4: boolean, x: number, y: number, z: number): number;
    export function setPopzonePopulationSet(popZone: number, populationSetHash: number): void;
    export function setSpawnerInfoPriority(p0: number, p1: number, priority: number): void;
    /** flags: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/PedFilterFlags */
    export function addAmbientAvoidanceRestriction(volume: number, includeFlags: number, excludeFlags: number, p3: number, p4: number, p5: number, p6: number): void;
    /** flags: see 0xB56D41A694E42E86 */
    export function addAmbientSpawnRestriction(volume: number, includeFlags: number, excludeFlags: number, p3: number, p4: number, p5: number, p6: number): void;
    export function createPopzoneFromVolume(volume: number): number;
    export function deleteScriptPopzone(popZone: number): void;
    /** Returns model hash of the closest fish */
    export function getRandomFishTypeForLocation(): number;
    export function isPopzoneValid(popZone: number): boolean;
    /** flags: see 0xB56D41A694E42E86 */
    export function removeAmbientAvoidanceRestriction(volume: number): void;
    export function removeAmbientSpawnRestriction(volume: number): void;
    /** Params: p1 = 1 & 2 in R* Scripts, 0 = Disable avoidance, 1 = Enabled avoidance, 2 = Enabled avoidance (?) */
    export function setPedShouldIgnoreAvoidanceVolumes(ped: number, p1: number): void;

    // POSSE
    export function posseGetPosseMembershipCount(): number;

    // PROPSET
    export function createPropSetInstanceAttachedToEntity(hash: number, x: number, y: number, z: number, entity: number, p5: number, p6: boolean, p7: number, p8: boolean): number;
    export function doesPropSetExist(propSet: number): boolean;
    export function isPropSetFullyLoaded(propSet: number): boolean;
    /** https://github.com/femga/rdr3_discoveries/blob/master/vehicles/vehicle_modding/vehicle_propsets.lua */
    export function addAdditionalPropSetForVehicle(vehicle: number, propset: number): void;
    /** To remove propsets either parse a zero as hash or call 0xE31C0CB1C3186D40 0xA6A9712955F53D9C returns lightPropset Hashes https://github.com/femga/rdr3_discoveries/blob/master/vehicles/vehicle_moddi... */
    export function addLightPropSetToVehicle(vehicle: number, lightPropset: number): void;
    /** List of vehicle propsets (wagons & trains): https://pastebin.com/1CsnvGLu / https://pastebin.com/v7TtqTgE */
    export function addPropSetForVehicle(vehicle: number, propset: number): void;
    /** propsetType: https://github.com/femga/rdr3_discoveries/blob/master/objects/propsets_list.lua placementType: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/PlacementType */
    export function createPropSet(propsetType: number, x: number, y: number, z: number, placementType: number, heading: number, zProbe: number, p7: boolean, useVegMod: boolean): number;
    /** Same as _CREATE_PROP_SET */
    export function createPropSet2(propsetType: number, x: number, y: number, z: number, placementType: number, heading: number, zProbe: number, p7: boolean, useVegMod: boolean): number;
    /** Same as CREATE_PROP_SET_INSTANCE_ATTACHED_TO_ENTITY */
    export function createPropSetInstanceAttachedToEntity2(hash: number, x: number, y: number, z: number, entity: number, p5: number, p6: boolean, p7: number, p8: boolean): number;
    export function deletePropSet(propSet: number, p1: boolean, p2: boolean): void;
    export function doesPropSetOfTypeExistNearCoords(propsetHash: number, x: number, y: number, z: number): boolean;
    export function doesVehicleHaveAnyLightPropSet(vehicle: number): boolean;
    export function doesVehicleHaveAnyPropSet(vehicle: number): boolean;
    export function getEntitiesFromPropSet(propSet: number, itemSet: number, model: number, p3: boolean, p4: boolean): number;
    export function getPropSetAtCoords(propsetHash: number, x: number, y: number, z: number): number;
    export function getPropSetModel(propSet: number): number;
    /** Example before/after deleting a train carriage's propset: https://imgur.com/a/qRNrIrK */
    export function getTrainCarriagePropSet(trainCarriage: number): number;
    /** Returns PropSet handle to be used with _GET_PROP_SET_MODEL */
    export function getVehicleLightPropSet(vehicle: number): number;
    export function getVehiclePropSet(vehicle: number): number;
    export function getVehiclePropSetHash(vehicle: number): number;
    export function hasPropSetLoaded(hash: number): boolean;
    /** Same as _HAS_PROP_SET_LOADED */
    export function hasPropSetLoaded2(hash: number): boolean;
    export function hasVehicleTrailerPropSetLoaded(vehicle: number, wagonIndex: number): boolean;
    export function isPropSetVisible(propSet: number): boolean;
    export function isVehicleLightPropSetLoaded(vehicle: number): boolean;
    export function isVehiclePropSetLoaded(vehicle: number): boolean;
    export function isVehiclePropSetLoadedAdditional(vehicle: number): boolean;
    /** Relocates an existing prop set to specified coordinates and adjusts its heading (rotation) without affecting the prop set's internal layout or structure. When `onGroundProperly` is true, the prop s... */
    export function movePropsetCoordsAndHeading(propSet: number, x: number, y: number, z: number, onGroundProperly: boolean, heading: number): void;
    export function releasePropSet(hash: number): boolean;
    export function removeVehicleLightPropSets(vehicle: number): void;
    export function removeVehiclePropSets(vehicle: number): void;
    export function requestPropSet(hash: number): boolean;
    /** Same as _REQUEST_PROP_SET */
    export function requestPropSet2(hash: number): boolean;
    export function setPropSetAsNoLongerNeeded(propSet: number): void;
    export function setPropSetFlag(propSet: number, flag: number): void;
    export function setPropSetVisible(propSet: number, toggle: boolean): void;

    // QUEUE
    export function eventQueueIsEmpty(hash: number): boolean;
    export function eventQueuePop(hash: number): void;

    // RECORDING
    /** nullsub, doesn't do anything  Old name: _STOP_RECORDING_THIS_FRAME */
    export function replayPreventRecordingThisFrame(): void;

    // REPLAY
    /** Hardcoded to return true. */
    export function closeVideoEditor(p0: any): boolean;
    /** Hardcoded to return false. */
    export function isVideoEditorRunning(): boolean;
    /** Hardcoded to return true. */
    export function openVideoEditor(): boolean;
    /** Hardcoded to return false.  Old name: _IS_INTERIOR_RENDERING_DISABLED */
    export function replaySystemHasRequestedAScriptCleanup(): boolean;
    /** nullsub, doesn't do anything */
    export function setScriptsHaveCleanedUpForReplaySystem(): void;

    // SAVE
    export function savegameIsSavePending(): boolean;
    /** See SAVEGAME_SAVE_SP */
    export function savegameSaveMp(savegameType: number): boolean;
    /** enum eSavegameType : Hash { 	SAVEGAMETYPE_AMBIENT = 0x3CA4E1F8, 	SAVEGAMETYPE_DEFAULT = 0xCB6ED080, 	SAVEGAMETYPE_DELETE_CHAR = 0xCD35F947, 	SAVEGAMETYPE_END_CREATE_NEWCHAR = 0x4C50A3CE, 	SAVEGAMET... */
    export function savegameSaveSp(savegameType: number): boolean;
    /** Does the exact same as 0x529B9CCD0972AF4E */
    export function savegameGetBool(p0: any, variableName: string): void;
    export function savegameGetFloat(p0: any, variableName: string): void;
    export function savegameGetInt(p0: any, variableName: string): void;
    /** Does the exact same as 0x529B9CCD0972AF4E Commonly used with time/timestamps */
    export function savegameGetInt2(p0: any, variableName: string): void;
    /** Does the exact same as 0x529B9CCD0972AF4E Commonly used with enums and flags */
    export function savegameGetInt3(p0: any, variableName: string): void;
    export function savegameGetTextLabel23(p0: any, variableName: string): void;
    export function savegameGetTextLabel31(p0: any, variableName: string): void;
    export function savegameGetTextLabel63(p0: any, variableName: string): void;
    export function savegameHasSaveFailed(): boolean;

    // SCRIPTS
    export function awardsGetResultItem(rpcGuid: any, awardHash: number, itemIndex: number, outResultItem: any): boolean;
    export function bailToLandingPage(bailCode: number): void;
    export function bailWithPassThroughParams(params: string): void;
    export function bgDoesLaunchParamExist(scriptIndex: number, p1: string): boolean;
    /** Deletes the given context from the background scripts context map. */
    export function bgEndContext(contextName: string): void;
    /** Hashed version of BG_END_CONTEXT */
    export function bgEndContextHash(contextHash: number): void;
    export function bgGetLaunchParamValue(scriptIndex: number, p1: string): number;
    export function bgGetScriptIdFromNameHash(p0: number): number;
    /** Returns true if GtaThread+0x77C is equal to 1.  Old name: _BG_EXITED_BECAUSE_BACKGROUND_THREAD_STOPPED */
    export function bgIsExitflagSet(): boolean;
    /** Sets bit 0 in GtaThread+0x784 */
    export function bgSetExitflagResponse(): void;
    /** Inserts the given context into the background scripts context map. */
    export function bgStartContext(contextName: string): void;
    /** Hashed version of BG_START_CONTEXT */
    export function bgStartContextHash(contextHash: number): void;
    export function countParticipantBits(value: any): number;
    export function countPlayerBits(value: any): number;
    export function doesScriptExist(scriptName: string): boolean;
    export function doesScriptWithNameHashExist(scriptHash: number): boolean;
    export function doesThreadExist(threadId: number): boolean;
    export function getBlockOfPlayerBits(value: any, p1: number): number;
    /** eventGroup: 0 = SCRIPT_EVENT_QUEUE_AI (CEventGroupScriptAI), 1 = SCRIPT_EVENT_QUEUE_NETWORK (CEventGroupScriptNetwork), 2 = unk, 3 = unk, 4 = SCRIPT_EVENT_QUEUE_SCRIPT_ERRORS (CEventGroupScriptErro... */
    export function getEventAtIndex(eventGroup: number, eventIndex: number): number;
    /** eventGroup: 0 = SCRIPT_EVENT_QUEUE_AI (CEventGroupScriptAI), 1 = SCRIPT_EVENT_QUEUE_NETWORK (CEventGroupScriptNetwork), 2 = unk, 3 = unk, 4 = SCRIPT_EVENT_QUEUE_SCRIPT_ERRORS (CEventGroupScriptErro... */
    export function getEventData(eventGroup: number, eventIndex: number, eventData: any, eventDataSize: number): boolean;
    /** eventGroup: 0 = SCRIPT_EVENT_QUEUE_AI (CEventGroupScriptAI), 1 = SCRIPT_EVENT_QUEUE_NETWORK (CEventGroupScriptNetwork), 2 = unk, 3 = unk, 4 = SCRIPT_EVENT_QUEUE_SCRIPT_ERRORS (CEventGroupScriptErrors) */
    export function getEventExists(eventGroup: number, eventType: number): boolean;
    export function getHashOfThisScriptName(): number;
    export function getIdOfThisThread(): number;
    export function getNoLoadingScreen(): boolean;
    /** eventGroup: 0 = SCRIPT_EVENT_QUEUE_AI (CEventGroupScriptAI), 1 = SCRIPT_EVENT_QUEUE_NETWORK (CEventGroupScriptNetwork), 2 = unk, 3 = unk, 4 = SCRIPT_EVENT_QUEUE_ERRORS (CEventGroupScriptErrors) */
    export function getNumberOfEvents(eventGroup: number): number;
    /** Gets the number of instances of the specified script is currently running.  Actually returns numRefs - 1. if (program) 	v3 = rage::scrProgram::GetNumRefs(program) - 1; return v3;  Old name: _GET_NU... */
    export function getNumberOfThreadsRunningTheScriptWithThisHash(scriptHash: number): number;
    export function getThreadExistenceDetails(threadId: number, threadExists: boolean, hasScriptHandler: boolean): void;
    /** Returns if a script has been loaded into the game. Used to see if a script was loaded after requesting. */
    export function hasScriptLoaded(scriptName: string): boolean;
    export function hasScriptWithNameHashLoaded(scriptHash: number): boolean;
    /** Waiting for child scripts to terminate / waiting for collapse of child scripts */
    export function haveAllChildScriptsTerminated(p0: number): boolean;
    /** Same as GET_IS_LOADING_SCREEN_ACTIVE */
    export function isLoadingScreenVisible(): boolean;
    export function isThreadActive(threadId: number, ignoreKilledState: boolean): boolean;
    export function isThreadExitRequested(): boolean;
    export function requestScript(scriptName: string): void;
    export function requestScriptWithNameHash(scriptHash: number): void;
    /** If the function returns 0, the end of the iteration has been reached. */
    export function scriptThreadIteratorGetNextThreadId(): number;
    /** Starts a new iteration of the current threads. Call this first, then SCRIPT_THREAD_ITERATOR_GET_NEXT_THREAD_ID (0x30B4FA1C82DD4B9F) */
    export function scriptThreadIteratorReset(): void;
    export function setBlockOfPlayerBits(value: any, p1: number, p2: number): void;
    export function setEventFlagForDeletion(eventGroup: number, eventIndex: number, p2: boolean): void;
    export function setNoLoadingScreen(toggle: boolean): void;
    export function setScriptAsNoLongerNeeded(scriptName: string): void;
    export function setScriptWithNameHashAsNoLongerNeeded(scriptHash: number): void;
    export function shutdownLoadingScreen(): void;
    export function startNewScript(scriptName: string, stackSize: number): number;
    /** return : script thread id, 0 if failed Pass pointer to struct of args in p1, size of struct goes into p2 */
    export function startNewScriptWithArgs(scriptName: string, args: any, argCount: number, stackSize: number): number;
    export function startNewScriptWithNameHash(scriptHash: number, stackSize: number): number;
    export function startNewScriptWithNameHashAndArgs(scriptHash: number, args: any, argCount: number, stackSize: number): number;
    export function stopDisplayingMpTransitionLoadingScreens(p0: any): void;
    export function terminateThisThread(): void;
    export function terminateThread(threadId: number): void;
    /** eventGroup: 0 = SCRIPT_EVENT_QUEUE_AI (CEventGroupScriptAI), 1 = SCRIPT_EVENT_QUEUE_NETWORK (CEventGroupScriptNetwork), 2 = unk, 3 = unk, 4 = SCRIPT_EVENT_QUEUE_SCRIPT_ERRORS (CEventGroupScriptErro... */
    export function triggerScriptEvent(eventGroup: number, eventData: any, eventDataSize: number, scriptMetadataIndex: number, playerBits: number): void;
    /** goalContext: see <availableContexts> in common/data/stats_and_challenges/goals_*.meta */
    export function activateGoalContext(goalContext: number): void;
    export function awardsGetUnlockClaimData(rpcGuid: any, awardHash: number, dataIndex: number, outUnlockData: any): boolean;
    export function bgReloadAllBackgroundScripts(): void;
    export function clearAllPlayerBits(value: any): void;
    export function clearPlayerBitAtIndex(value: any, bitIndex: number): void;
    /** goalContext: see _ACTIVATE_GOAL_CONTEXT */
    export function deactivateGoalContext(goalContext: number): void;
    export function displayLoadingScreens(p0: number, p1: number, p2: number, gamemodeName: string, title: string, subtitle: string): void;
    export function doesCompressedGlobalBlockBufferExist(index: number): boolean;
    export function getGlobalBlockCanBeAccessed(index: number): boolean;
    export function getHashOfThread(threadId: number): number;
    /** enum eThreadExitReason { 	THREAD_EXIT_REASON_NONE, 	THREAD_EXIT_REASON_BACKGROUND_THREAD_STOPPED, 	THREAD_EXIT_REASON_SESSION_MERGE, 	THREAD_EXIT_REASON_SCENARIO_OUT_OF_SCOPE, 	THREAD_EXIT_REASON_R... */
    export function getThreadExitReason(): number;
    export function isAnyPlayerBitSet(playerBits: number): boolean;
    export function isBackgroundScript(threadId: number): boolean;
    /** goalContext: see _ACTIVATE_GOAL_CONTEXT */
    export function isGoalContextActive(goalContext: number): boolean;
    export function isPlayerBitSetAtIndex(value: any, bitIndex: number): boolean;
    export function isThreadExitRequestedForThreadWithThisId(threadId: number): boolean;
    export function lootGetLootClaimData(rpcGuid: any, dataIndex: number, outLootData: any): boolean;
    export function lootGetResultItem(rpcGuid: any, itemIndex: number, outResultItem: any): boolean;
    /** Returns "INVALID_NET_RPC_GUID" if netRpcGuid is invalid. */
    export function netRpcGuidToString(netRpcGuid: any): NativeString;
    export function requestThreadExit(threadId: number): void;
    export function requestThreadExitForAllThreadsWithThisName(nameHash: number): void;
    export function restoreGlobalBlock(index: number): boolean;
    export function setAllGlobalBlocksHaveBeenLoaded(toggle: boolean): void;
    export function setAllPlayerBits(value: any): void;
    export function setGlobalBlockCanBeAccessed(index: number, toggle: boolean): void;
    export function setPlayerBitAtIndex(value: any, bitIndex: number): void;
    export function storeGlobalBlock(index: number): boolean;
    export function triggerScriptEvent2(eventData: any, eventDataSize: number, scriptMetadataIndex: number, threadId: number): void;

    // SHAPETEST
    /** Returns the result of a shape test: 0 if the handle is invalid, 1 if the shape test is still pending, or 2 if the shape test has completed, and the handle should be invalidated.  When used with an ... */
    export function getShapeTestResult(shapeTestHandle: number, hit: boolean, endCoords: Vector3, surfaceNormal: Vector3, entityHit: number): number;
    /** Does the same as 0x7EE9F5D83DD4F90E, except blocking until the shape test completes. */
    export function startExpensiveSynchronousShapeTestLosProbe(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, flags: number, entityToIgnore: number, p8: number): number;
    export function startShapeTestBox(posX: number, posY: number, posZ: number, dimensionsX: number, dimensionsY: number, dimensionsZ: number, rotX: number, rotY: number, rotZ: number, rotationOrder: number, flags: number, entityToIgnore: number, options: number): number;
    export function startShapeTestCapsule(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, radius: number, flags: number, entityToIgnore: number, p9: number): number;
    /** Asynchronously starts a line-of-sight (raycast) world probe shape test.  Use the handle with 0x3D87450E15D98694 or 0x65287525D951F6BE until it returns 0 or 2.  p8 is a bit mask with bits 1, 2 and/o... */
    export function startShapeTestLosProbe(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, flags: number, entity: number, p8: number): number;
    /** Old name: _START_SHAPE_TEST_SURROUNDING_COORDS */
    export function startShapeTestMouseCursorLosProbe(pVec1: Vector3, pVec2: Vector3, flag: number, entity: number, flag2: number): number;
    export function startShapeTestSweptSphere(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, radius: number, flags: number, entity: number, p9: any): number;

    // SOCIALCLUB
    export function scCommunityEventGetDisplayName(p0: string): boolean;
    export function scCommunityEventGetDisplayNameById(p0: number, p1: string): boolean;
    export function scCommunityEventGetDisplayNameForType(p0: string, p1: string): boolean;
    export function scCommunityEventGetEventId(): number;
    export function scCommunityEventGetEventIdForType(p0: string): number;
    export function scCommunityEventGetExtraDataFloat(p0: string, p1: number): boolean;
    export function scCommunityEventGetExtraDataFloatById(p0: number, p1: string, p2: number): boolean;
    export function scCommunityEventGetExtraDataFloatForType(p0: string, p1: number, p2: string): boolean;
    export function scCommunityEventGetExtraDataInt(p0: string, p1: number): boolean;
    export function scCommunityEventGetExtraDataIntById(p0: number, p1: string, p2: number): boolean;
    export function scCommunityEventGetExtraDataIntForType(p0: string, p1: number, p2: string): boolean;
    export function scCommunityEventGetExtraDataString(p0: string, p1: string): boolean;
    export function scCommunityEventGetExtraDataStringById(p0: number, p1: string, p2: string): boolean;
    export function scCommunityEventGetExtraDataStringForType(p0: string, p1: string, p2: string): boolean;
    export function scCommunityEventIsActive(): boolean;
    export function scCommunityEventIsActiveById(p0: number): boolean;
    export function scCommunityEventIsActiveForType(p0: string): boolean;
    export function scInboxGetMessageIsReadAtIndex(msgIndex: number): boolean;
    export function scInboxGetMessageTypeAtIndex(msgIndex: number): number;
    export function scInboxGetTotalNumMessages(): number;
    export function scInboxMessageGetDataInt(p0: number, context: string, out_: number): boolean;
    export function scInboxMessageGetDataString(p0: number, context: string, out_: string): boolean;
    export function scInboxMessageGetRawTypeAtIndex(p0: number): NativeString;
    export function scInboxSetMessageAsReadAtIndex(msgIndex: number): boolean;
    export function scPresenceAttrSetFloat(attrHash: number, value: number): boolean;
    export function scPresenceAttrSetFloatEx(attrName: string, value: number, p2: boolean): boolean;
    export function scPresenceAttrSetIntEx(attrName: string, value: number, p2: boolean): boolean;
    export function scPresenceAttrSetStringEx(attrName: string, value: string, p2: boolean): boolean;
    /** Starts a task to check an entered string for profanity on the ROS/Social Club services. */
    export function scProfanityCheckString(string: string, token: number): boolean;
    export function scProfanityGetCheckIsPending(token: number): boolean;
    export function scProfanityGetCheckIsValid(token: number): boolean;
    export function scProfanityGetStringPassed(token: number): boolean;
    export function scProfanityGetStringStatus(token: number): number;

    // SOCIALCLUBFEED
    export function scFeedHubHasNewData(): boolean;
    export function scFeedSubmitPresetMessage(type: number, subType: number): number;

    // SPACTIONPROXY
    export function spactionproxyGetNextPendingBuyAction(data: any): boolean;
    export function spactionproxyGetNextPendingCraftingAction(data: any): boolean;
    export function spactionproxyManagerIsFailed(): boolean;
    export function spactionproxyManagerIsReady(): boolean;
    export function spactionproxyProcessAction(p0: any, p1: boolean): boolean;
    export function spactionproxyStartManager(): boolean;

    // STATS
    export function chalAchievementGetProgressInt(p0: number, p1: number): number;
    export function chalAchievementIsComplete(p0: number, p1: number): boolean;
    export function chalAddGoalProgressFloat(chalHash: number, goalHash: number, value: number): void;
    export function chalAddGoalProgressFloatByScoreId(p0: number, value: number): void;
    export function chalAddGoalProgressInt(chalHash: number, goalHash: number, value: number): void;
    export function chalAddGoalProgressIntByScoreId(p0: number, value: number): void;
    export function chalGetMaxRanks(chalHash: number): number;
    export function chalGetNumRanksCompleted(chalHash: number): number;
    /** https://github.com/femga/rdr3_discoveries/blob/master/AI/EVENTS/challenge_goals.lua */
    export function chalIsGoalActive(chalHash: number, goalHash: number): boolean;
    export function chalMissionAddGoalProgressInt(missionHash: number, goalHash: number, value: number): void;
    export function chalMissionGetNumGoals(missionHash: number): number;
    export function chalMissionGetNumGoalsComplete(missionHash: number): number;
    export function chalMissionIsGoalComplete(missionHash: number, goalHash: number): boolean;
    export function chalNetStartChal(chalHash: number): void;
    export function chalNetStartGoal(chalHash: number, goalHash: number): void;
    export function chalNetStopChal(chalHash: number): void;
    export function chalNetStopGoal(chalHash: number, goalHash: number): void;
    export function chalSetGoalDisabled(chalHash: number, goalHash: number, disabled: boolean): void;
    export function chalSetGoalProgressInt(chalHash: number, goalHash: number, value: number): void;
    export function statstrackerDeedStarted(p0: number, p1: any): void;
    export function statstrackerIsInitialized(p0: number): boolean;
    export function statAddBountyTarget(unlockHash: number, ped: number): void;
    export function statBountyCaptured(entity: number): void;
    export function statBountyEscaped(ped: number): void;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdGetBool(statId: any, value: boolean): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdGetDate(statId: any, date: any): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdGetFloat(statId: any, value: number): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdGetInt(statId: any, p1: number): boolean;
    /** struct StatId { 	alignas(8) Hash BaseId; 	alignas(8) Hash PermutationId; } */
    export function statIdIsValid(statId: any): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdSetBool(statId: any, value: boolean, p2: boolean): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdSetDate(statId: any, date: any, p2: boolean): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdSetFloat(statId: any, value: number, p2: boolean): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdSetGxtLabel(statId: any, label: string, p2: boolean): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdSetInt(statId: any, value: number, p2: boolean): boolean;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdSetToPosseId(statId: any): void;
    export function statPhotographTaken(itemset: number): void;
    export function statRegisterLegendaryAnimalDeed(deedHash: number): void;
    export function weeklyCollectibleGetItemInSet(chalHash: number, setIndex: number, itemIndex: number, p3: number, p4: number): boolean;
    export function weeklyCollectibleGetItemSetBuyAward(chalHash: number, index: number): number;
    export function weeklyCollectibleGetItemSetLabel(chalHash: number, index: number): number;
    export function weeklyCollectibleGetNumItemsInSet(chalHash: number, index: number): number;
    export function weeklyCollectibleGetNumSets(chalHash: number): number;
    export function statstrackerDeedStatus(deedType: number, deedHash: number, missionStatus: number, data: any): void;
    /** Related to animal tagging */
    export function statAddAnimalSampleTarget(animalType: number): void;
    /** Calculation: (value / 1000) / 60 % 60 */
    export function statCalculateCooldown(value: number): number;
    export function statCarriedSatchelItemFromPed(ped: number): void;
    export function statDonateIncrementItem(item: number, slot: number, p2: any, p3: any): void;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdDecrementInt(statId: any, value: number): void;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdIncrementFloat(statId: any, value: number): void;
    /** statId: see STAT_ID_IS_VALID */
    export function statIdIncrementInt(statId: any, value: number): void;
    export function statItemFishCaught(fish: number, weight: number, category: number, subcategory: number): void;
    /** statId: see STAT_ID_IS_VALID */
    export function statPheromoneCooldownLegendaryAnimal(entity: number, statId: any): boolean;

    // STREAMING
    export function beginSrl(): void;
    export function clearFocus(): void;
    export function clearHdArea(): void;
    export function doesAnimDictExist(animDict: string): boolean;
    export function endSrl(): void;
    export function getNumberOfStreamingRequests(): number;
    export function getPopulationBudgetMultiplier(): number;
    export function hasAnimDictLoaded(animDict: string): boolean;
    /** Alias for HAS_ANIM_SET_LOADED. */
    export function hasClipSetLoaded(clipSet: string): boolean;
    export function hasCollisionForModelLoaded(model: number): boolean;
    /** Checks if the specified model has loaded into memory. */
    export function hasModelLoaded(model: number): boolean;
    export function hasMoveNetworkDefLoaded(name: string): boolean;
    export function hasNamedPtfxAssetLoaded(fxNameHash: number): boolean;
    export function hasPtfxAssetLoaded(): boolean;
    export function iplGroupSwapCancel(): void;
    export function iplGroupSwapFinish(): void;
    export function iplGroupSwapIsActive(): boolean;
    export function iplGroupSwapIsReady(): boolean;
    export function iplGroupSwapStart(iplName1: string, iplName2: string): void;
    export function isEntityFocus(entity: number): boolean;
    /** Old name: _IS_IMAP_ACTIVE_2 */
    export function isIplActiveByHash(iplHash: number): boolean;
    /** Old name: _IS_IMAP_ACTIVE */
    export function isIplActiveHash(iplHash: number): boolean;
    export function isLoadSceneActive(): boolean;
    export function isLoadSceneLoaded(): boolean;
    export function isModelAPed(model: number): boolean;
    /** Returns whether the specified model represents a vehicle. */
    export function isModelAVehicle(model: number): boolean;
    /** Returns whether the specified model exists in the game. */
    export function isModelInCdimage(model: number): boolean;
    /** Returns whether the specified model is valid */
    export function isModelValid(model: number): boolean;
    export function isPlayerSwitchInProgress(): boolean;
    export function isRenderedSceneLoaded(): boolean;
    export function isSrlLoaded(): boolean;
    export function loadSceneStart(posX: number, posY: number, posZ: number, offsetX: number, offsetY: number, offsetZ: number, radius: number, p7: number): boolean;
    export function loadSceneStartSphere(x: number, y: number, z: number, radius: number, p4: any): boolean;
    export function loadSceneStop(): void;
    export function prefetchSrl(srl: string): void;
    export function removeAnimDict(animDict: string): void;
    /** Alias for REMOVE_ANIM_SET. */
    export function removeClipSet(clipSet: string): void;
    /** Old name: _REMOVE_IMAP_2 */
    export function removeIplByHash(iplHash: number): void;
    /** Old name: _REMOVE_IMAP */
    export function removeIplHash(iplHash: number): void;
    export function removeMoveNetworkDef(name: string): void;
    export function removeNamedPtfxAsset(fxNameHash: number): void;
    export function removePtfxAsset(): void;
    export function requestAdditionalCollisionAtCoord(x: number, y: number, z: number): void;
    export function requestAnimDict(animDict: string): void;
    export function requestClipSet(clipSet: string): void;
    export function requestCollisionAtCoord(x: number, y: number, z: number): void;
    export function requestCollisionForModel(model: number): void;
    /** Old name: _REQUEST_IMAP_2 */
    export function requestIplByHash(iplHash: number): void;
    /** Old name: _REQUEST_IMAP */
    export function requestIplHash(iplHash: number): void;
    /** Request a model to be loaded into memory. */
    export function requestModel(model: number, p1: boolean): void;
    export function requestMoveNetworkDef(name: string): void;
    export function requestNamedPtfxAsset(fxNameHash: number): void;
    export function requestPtfxAsset(): void;
    /** nullsub, doesn't do anything */
    export function setAllMapdataCulled(p0: any): void;
    /** It seems to make the entity's coords mark the point from which LOD-distances are measured. In my testing, setting a vehicle as the focus entity and moving that vehicle more than 300 distance units ... */
    export function setFocusEntity(entity: number): void;
    export function setFocusPosAndVel(x: number, y: number, z: number, offsetX: number, offsetY: number, offsetZ: number): void;
    export function setGamePausesForStreaming(toggle: boolean): void;
    export function setHdArea(x: number, y: number, z: number, radius: number): void;
    export function setMapdatacullboxEnabled(name: string, toggle: boolean): void;
    /** Marks the model as no longer needed. */
    export function setModelAsNoLongerNeeded(model: number): void;
    export function setPopulationBudgetMultiplier(fBudgetMultiplier: number): void;
    export function setSceneStreamingTracksCamPosThisFrame(): void;
    export function setSrlLongJumpMode(p0: boolean): void;
    export function setSrlReadaheadTimes(p0: number, p1: number, p2: number, p3: number): void;
    export function setSrlTime(p0: number): void;
    /** Outputs IPL position and radius (previously wrongly named heading) https://github.com/femga/rdr3_discoveries/blob/master/imaps/imaps_with_coords_and_heading.lua */
    export function getIplBoundingSphere(iplHash: number, position: Vector3, radius: number): boolean;
    export function hasCollisionLoadedAtCoord(x: number, y: number, z: number): boolean;
    export function hasScenarioTypeLoaded(scenarioType: number, p1: boolean): boolean;
    export function isModelAnObject(model: number): boolean;
    /** Returns true if IPL is streamed in (?) */
    export function isPositionInsideIplStreamingExtents(iplHash: number, x: number, y: number, z: number): boolean;
    export function removeScenarioAsset(scenarioType: number): any;
    export function requestClipSetByHash(clipSetHash: number): void;
    export function requestMetadataAtCoord(x: number, y: number, z: number): void;
    /** entityModel can be 0 or using Hash or using GET_ENTITY_MODEL conditionalAnim can be 0 or using Hash also accepts GET_ACTIVE_DYNAMIC_SCENARIO. */
    export function requestScenarioType(scenarioType: number, p1: number, entityModel: number, conditionalAnim: number): number;
    export function setGuarmaWorldhorizonActive(toggle: boolean): void;

    // TASK
    export function addCoverPoint(p0: number, p1: number, p2: number, p3: number, p4: any, p5: any, p6: any, p7: boolean): number;
    /** Params: p2 is always -1.f in R* Scripts */
    export function addFleeTargetPed(ped: number, targetPed: number, p2: number): void;
    export function addPatrolRouteLink(node1: number, node2: number): void;
    export function addPatrolRouteNode(nodeId: number, scenarioName: string, x: number, y: number, z: number, lookPosX: number, lookPosY: number, lookPosZ: number, duration: number, p9: boolean): void;
    /** Returns true when requested asset is loaded */
    export function areCompositeLootableEntityDefAssetsLoaded(asset: number): boolean;
    export function assistedMovementIsRouteLoaded(route: string): boolean;
    export function assistedMovementRemoveRoute(route: string): void;
    export function assistedMovementSetRouteProperties(route: string, props: number): void;
    export function canStartItemInteraction(ped: number, itemHash: number, interactionAnimHash: number, p3: number): boolean;
    export function clearDrivebyTaskUnderneathDrivingTask(ped: number): void;
    export function clearPedSecondaryTask(ped: number): void;
    export function clearPedTasks(ped: number, p1: boolean, p2: boolean): void;
    /** Immediately stops the pedestrian from whatever it's doing. They stop fighting, animations, etc. they forget what they were doing.  resetCrouch TRUE = ped will stand up if crouching, FALSE = ped wil... */
    export function clearPedTasksImmediately(ped: number, p1: boolean, resetCrouch: boolean): void;
    export function clearSequenceTask(taskSequenceId: number): void;
    export function closePatrolRoute(): void;
    export function closeSequenceTask(taskSequenceId: number): void;
    export function createPatrolRoute(): void;
    /** Returns scenario */
    export function createScenarioPointHash(scenarioHash: number, x: number, y: number, z: number, heading: number, p5: any, p6: any, p7: boolean): number;
    /** Returns scenario */
    export function createScenarioPointHashAttachedToEntity(entity: number, scenarioHash: number, x: number, y: number, z: number, heading: number, p6: any, p7: any, p8: boolean): number;
    export function deletePatrolRoute(patrolRoute: string): void;
    export function doesScenarioExistInArea(x: number, y: number, z: number, radius: number, p4: boolean, p5: any, p6: boolean): boolean;
    export function doesScenarioGroupExist(scenarioGroup: string): boolean;
    export function doesScenarioOfTypeExistInAreaHash(x: number, y: number, z: number, typeHash: number, radius: number, p5: boolean): boolean;
    export function doesScenarioPointExist(scenario: number): boolean;
    /** Checks if there is a cover point at position */
    export function doesScriptedCoverPointExistAtCoords(p0: any, p1: any, p2: any, p3: any): boolean;
    export function endDuel(ped: number, p1: boolean, p2: number): void;
    export function findScenarioOfTypeHash(xPos: number, yPos: number, zPos: number, scenarioType: number, distance: number, p5: any, p6: boolean): number;
    export function forceScenarioGroupPriority(p0: any, p1: any): void;
    export function getActiveVehicleMissionType(vehicle: number): number;
    export function getIsCarriableEntity(entity: number): boolean;
    export function getIsPedAimingInTheAir(ped: number): boolean;
    export function getIsTaskActive(ped: number, taskIndex: number): boolean;
    export function getIsWaypointRecordingLoaded(waypointRecording: string): boolean;
    export function getItemInteractionItemId(ped: number): number;
    export function getItemInteractionPromptProgress(ped: number, inputContext: number): number;
    export function getItemInteractionState(ped: number): number;
    export function getPedDesiredMoveBlendRatio(ped: number): number;
    export function getPedWaypointDistance(ped: number): number;
    export function getPedWaypointOverrideSpeed(ped: number): number;
    export function getPedWaypointProgress(ped: number): number;
    /** Old name: _GET_SCENARIO_POINT_ENTITY */
    export function getPropForScenarioPoint(scenarioPoint: number, name: string): number;
    export function getRansackScenarioPointPedIsUsing(ped: number): any;
    /** Note: scenariosInRadius is an array, and its size and values should be aligned to 8 bytes. */
    export function getScenarioPointsInArea(posX: number, posY: number, posZ: number, radius: number, scenariosInRadius: any, size: number): number;
    export function getScriptedCoverPointCoords(coverpoint: number): Vector3;
    /** Gets the status of a script-assigned task, and returns an int between 0-8 taskHash: https://alloc8or.re/rdr3/doc/enums/eScriptTaskHash.txt   WAITING_TO_START_TASK = 0, PERFORMING_TASK DORMANT_TASK ... */
    export function getScriptTaskStatus(ped: number, taskHash: number, p2: boolean): number;
    /** returned values: 0 to 7 = task that's currently in progress, 0 meaning the first one. -1 no task sequence in progress. */
    export function getSequenceProgress(ped: number): number;
    export function getTaskMoveNetworkEvent(ped: number, eventName: string): boolean;
    export function getTaskMoveNetworkState(ped: number): NativeString;
    export function getVehicleWaypointPlaybackOverrideSpeed(p0: any): any;
    export function getVehicleWaypointProgress(vehicle: number): number;
    export function getVehicleWaypointTargetPoint(vehicle: number): number;
    export function getWaypointDistanceAlongRoute(waypointRecording: string, p1: number): number;
    export function isDrivebyTaskUnderneathDrivingTask(ped: number): boolean;
    export function isEmoteTaskRunning(ped: number, p1: any): boolean;
    export function isMountedWeaponTaskUnderneathDrivingTask(ped: number): boolean;
    export function isMoveBlendRatioRunning(moveBlendRatio: number): boolean;
    export function isMoveBlendRatioSprinting(moveBlendRatio: number): boolean;
    export function isMoveBlendRatioStill(moveBlendRatio: number): boolean;
    export function isMoveBlendRatioWalking(moveBlendRatio: number): boolean;
    export function isPedActiveInScenario(ped: number, scenario: number): boolean;
    /** This function is hard-coded to always return false. */
    export function isPedBeingArrested(ped: number): boolean;
    export function isPedCuffed(ped: number): boolean;
    export function isPedExitingScenario(ped: number, p1: boolean): boolean;
    export function isPedGettingUp(ped: number): boolean;
    export function isPedInHitReact(ped: number): boolean;
    /** This native checks if a ped is on the ground, in pain from a (gunshot) wound. */
    export function isPedInWrithe(ped: number): boolean;
    export function isPedRunning(ped: number): boolean;
    export function isPedRunningInspectionTask(ped: number): boolean;
    export function isPedRunningTaskItemInteraction(ped: number): boolean;
    export function isPedScenarioReactLooking(ped: number, p1: boolean): boolean;
    export function isPedSprinting(ped: number): boolean;
    export function isPedStill(ped: number): boolean;
    export function isPedWalking(ped: number): boolean;
    export function isScenarioGroupEnabled(scenarioGroup: string): boolean;
    export function isScenarioOccupied(p0: number, p1: number, p2: number, p3: number, p4: boolean): boolean;
    export function isScenarioTypeEnabled(scenarioType: string): boolean;
    export function isTaskMoveNetworkActive(ped: number): boolean;
    export function isTaskMoveNetworkReadyForTransition(ped: number): boolean;
    export function isTeamCarriableEntity(p0: any, p1: any): boolean;
    export function isWaypointPlaybackGoingOnForPed(ped: number, waypointRecording: string): boolean;
    export function isWaypointPlaybackGoingOnForVehicle(p0: any, p1: any): boolean;
    export function makeObjectNotCarriable(object: number): void;
    /** Note: patrolRoute must be prefixed with 'miss_' for it to be valid */
    export function openPatrolRoute(patrolRoute: string): void;
    export function openSequenceTask(taskSequenceId: number): void;
    export function pedHasUseScenarioTask(ped: number): boolean;
    export function playAnimOnRunningScenario(ped: number, animDict: string, animName: string): void;
    export function playEntityScriptedAnim(entity: number, args: any): void;
    /** lookIntensity: see SET_PED_SHOULD_PLAY_FLEE_SCENARIO_EXIT  exitAnimation: LOOK_RETURN_GENERIC = 1, LOOK_RETURN_DISMISSIVE = 2, LOOK_RETURN_RELIEVED = 3 */
    export function reactLookAt(ped: number, targetPed: number, lookIntensity: number, exitAnimation: number, duration: number, p5: number, targetPed2: number, p7: any, p8: any): void;
    export function reactLookAtEnd(ped: number, exitAnimation: number, p2: boolean): void;
    export function removeAllCoverBlockingAreas(): void;
    export function removeCoverPoint(coverpoint: number): void;
    export function removeWaypointRecording(waypointRecording: string): void;
    export function requestTaskMoveNetworkStateTransition(ped: number, name: string): void;
    export function requestWaypointRecording(waypointRecording: string): void;
    export function resetScenarioGroupsEnabled(): void;
    export function resetScenarioTypesEnabled(): void;
    export function setAnimFilter(p0: any, p1: any, p2: any, p3: any): void;
    export function setAnimRate(p0: any, p1: number, p2: any, p3: boolean): void;
    export function setDrivebyTaskTarget(shootingPed: number, targetPed: number, targetVehicle: number, x: number, y: number, z: number): void;
    export function setDriveTaskCruiseSpeed(driver: number, cruiseSpeed: number): void;
    /** Not implemented. */
    export function setDriveTaskMaxCruiseSpeed(ped: number, maxCruiseSpeed: number): void;
    export function setEnableSpeedRestrainForWaypointRecordingLeader(p0: any, p1: any): void;
    /** clipset: CLIPSET@MECH_HOGTIE@HUMAN@BREAKOUT_MG@GROUND, CLIPSET@MECH_HOGTIE@HUMAN@BREAKOUT_MG@SHOULDER, CLIPSET@MECH_HOGTIE@HUMAN@BREAKOUT_MG@MOUNT clipset can also be 0 */
    export function setEnhancedBreakFree(ped: number, p1: boolean, clipset: string): boolean;
    /** Makes the ped ragdoll like when falling from a great height */
    export function setHighFallTask(ped: number, p1: number, p2: number, p3: number): void;
    export function setPedDesiredMoveBlendRatio(ped: number, p1: number): void;
    export function setPedPathAvoidFire(ped: number, avoidFire: boolean): void;
    export function setPedPathCanDropFromHeight(ped: number, toggle: boolean): void;
    export function setPedPathCanUseClimbovers(ped: number, toggle: boolean): void;
    export function setPedPathCanUseLadders(ped: number, toggle: boolean): void;
    export function setPedPathClimbCostModifier(ped: number, modifier: number): void;
    export function setPedPathDeepSnowCostModifier(ped: number, modifier: number): void;
    export function setPedPathFoliageCostModifier(ped: number, modifier: number): void;
    export function setPedPathMayEnterWater(ped: number, mayEnterWater: boolean): void;
    export function setPedPathPreferToAvoidWater(ped: number, avoidWater: boolean, p2: number): void;
    export function setPedWaypointRouteOffset(ped: number, p1: number, p2: number, p3: number): boolean;
    export function setScenarioGroupEnabled(scenarioGroup: string, toggle: boolean): void;
    export function setScenarioTypeEnabled(scenarioType: string, toggle: boolean): void;
    /** repeatMode: 0 = REPEAT_NOT; 1 = REPEAT_FOREVER */
    export function setSequenceToRepeat(taskSequenceId: number, repeatMode: number): void;
    export function setTaskMoveNetworkSignalBool(ped: number, signalName: string, value: boolean): void;
    export function setTaskMoveNetworkSignalFloat(ped: number, signalName: string, value: number): void;
    export function setTeamCarriableEntity(p0: any, p1: any, p2: any): void;
    export function setUpSpeedRestrainInformationForPlayerFollower(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any): void;
    /** Params: p3 = 0, 1; p5 = 0.0f, -1.0f https://github.com/femga/rdr3_discoveries/tree/master/tasks/TASK_ITEM_INTERACTION */
    export function startTaskItemInteraction(ped: number, itemHash: number, interactionAnimHash: number, p3: number, flag: number, p5: number): void;
    export function stopAnimPlayback(ped: number, p1: number, p2: boolean): void;
    export function stopAnimTask(ped: number, animDictionary: string, animationName: string, p3: number): void;
    /** Makes the specified ped achieve the specified heading.  pedHandle: The handle of the ped to assign the task to. heading: The desired heading. timeout: The time, in milliseconds, to allow the task t... */
    export function taskAchieveHeading(ped: number, heading: number, timeout: number): void;
    export function taskAimAtCoord(ped: number, x: number, y: number, z: number, time: number, p5: boolean, p6: boolean): void;
    export function taskAimAtEntity(ped: number, targetEntity: number, time: number, p3: boolean, p4: boolean): void;
    export function taskAimGunAtCoord(ped: number, x: number, y: number, z: number, time: number, p5: boolean, p6: boolean): void;
    /** duration: the amount of time in milliseconds to do the task.  -1 will keep the task going until either another task is applied, or CLEAR_ALL_TASKS() is called with the ped */
    export function taskAimGunAtEntity(ped: number, targetEntity: number, duration: number, p3: boolean, p4: number): void;
    export function taskAmbientAnimalHunt(ped: number, p1: any, p2: any): void;
    export function taskAmbientAnimalStalk(ped: number, p1: any, p2: any): void;
    export function taskAnimalAlerted(ped: number, p1: any, p2: any): void;
    export function taskAnimalFlee(ped: number, targetPed: number, p2: any): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/tasks/TASK_ANIMAL_INTERACTION */
    export function taskAnimalInteraction(ped: number, targetPed: number, interactionType: number, interactionModel: number, skipIdleAnimationClip: boolean): void;
    export function taskAnimalUnalerted(ped: number, p1: any, p2: any, p3: any, p4: any): void;
    export function taskAnimalWrithe(ped: number, p1: any, p2: any): void;
    export function taskArrestPed(ped: number, target: number): void;
    export function taskBark(ped: number, barkAtTarget: number, mood: number): void;
    export function taskBoatMission(pedDriver: number, boat: number, p2: any, p3: any, x: number, y: number, z: number, p7: any, maxSpeed: number, drivingStyle: number, p10: number, p11: any): void;
    export function taskBreakVehicleDoorLock(ped: number, vehicle: number): void;
    /** carriableSlot:  7 > Back of a horse  6 > Right side of a horse  5 > Left side of a horse flags:  512: enables the prompt being the name of the item when using a generic item */
    export function taskCarriable(entity: number, carryConfig: number, carrier: number, carriableSlot: number, flags: number): void;
    export function taskClearDefensiveArea(ped: number): void;
    export function taskClearLookAt(ped: number): void;
    /** Climbs or vaults the nearest thing. */
    export function taskClimb(ped: number, unused: boolean): void;
    export function taskClimbLadder(ped: number, p1: number, p2: boolean, p3: boolean): void;
    export function taskCombatAnimalChargePed(ped: number, targetPed: number, p2: boolean, p3: any, p4: any, p5: any, p6: any): void;
    export function taskCombatAnimalWarn(ped: number, p1: any, p2: any): void;
    export function taskCombatHatedTargets(ped: number, radius: number): void;
    /** Despite its name, it only attacks ONE hated target. The one closest hated target. */
    export function taskCombatHatedTargetsAroundPed(ped: number, radius: number, flags: number, p3: any): void;
    export function taskCombatHatedTargetsAroundPedTimed(ped: number, radius: number, time: number, flags: number): void;
    /** Despite its name, it only attacks ONE hated target. The one closest to the specified position. */
    export function taskCombatHatedTargetsInArea(ped: number, x: number, y: number, z: number, radius: number, flags: number, p6: any): void;
    export function taskCombatHatedTargetsNoLosTest(ped: number, radius: number): void;
    export function taskCombatPed(ped: number, targetPed: number, p2: number, p3: number): void;
    export function taskCombatPedTimed(ped: number, targetPed: number, p2: number, p3: any): void;
    export function taskCompanionAmbient(ped: number, p1: any): void;
    export function taskConfront(ped: number, targetPed: number, p2: number): boolean;
    export function taskCower(ped: number, duration: number, pedToCowerFrom: number, p3: string): void;
    /** flags: See TASK_ENTER_VEHICLE */
    export function taskDisembarkNearestTrainCarriage(ped: number, p1: number, flags: number): void;
    /** Dismounts the ped from the animal it's mounted on. taskFlag affects what side the rider gets off. p2-p5 are almost always 0. flags: See TASK_ENTER_VEHICLE */
    export function taskDismountAnimal(rider: number, taskFlag: number, p2: any, p3: any, p4: any, targetPed: number): void;
    export function taskDriveBy(driverPed: number, targetPed: number, targetVehicle: number, targetX: number, targetY: number, targetZ: number, distanceToShoot: number, pedAccuracy: number, p8: boolean, firingPattern: number): void;
    export function taskDuck(ped: number, p1: number): void;
    /** Params: p4 either 0.2f, 0.25f, 0.31f, 0.4f */
    export function taskDuel(ped: number, p1: any, p2: number, entity: number, p4: number, p5: number, vPosOpponentX: number, vPosOpponentY: number, vPosOpponentZ: number, fOpponentHead: number, p10: number): void;
    export function taskDumpCarriableFromParent(ped: number, ped2: number, entity: number): void;
    export function taskEat(ped: number, p1: any, p2: any): void;
    /** flags: MOVE_WHILST_WAITING_FOR_PATH = (1 << 0), DO_NOT_STAND_STILL_AT_END_OF_PATH = (1 << 1), SKIP_NAVIGATION = (1 << 2), TEASF_AUTO_START_ANIM_SCENE = (1 << 3), FORCE_STAND_STILL_AT_END_OF_PATH = ... */
    export function taskEnterAnimScene(ped: number, animScene: number, entityName: string, playbackListName: string, enterSpeed: number, bAutoStart: boolean, flag: number, p7: number, p8: number): void;
    /** flags: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eEnterExitVehicleFlags */
    export function taskEnterVehicle(ped: number, vehicle: number, timeout: number, seat: number, speed: number, flag: number, p6: any): void;
    /** Params: p2 is returned by BUILTIN::SHIFT_LEFT */
    export function taskEvasiveAnim(ped1: number, ped2: number, p2: number): void;
    export function taskEveryoneLeaveVehicleInOrder(vehicle: number, p1: boolean): void;
    /** Adds a new point to the current point route. Call TASK_FLUSH_ROUTE before the first call to this. Call TASK_FOLLOW_POINT_ROUTE to make the Ped go the route.  A maximum of 8 points can be added. */
    export function taskExtendRoute(x: number, y: number, z: number): void;
    /** Params: p5 = some flag?, p6 = -1.0f, p8 = 0 in R* Scripts fleeStyle: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eFleeStyle */
    export function taskFleeCoord(ped: number, x: number, y: number, z: number, fleeStyle: number, p5: number, p6: number, duration: number, p8: number): void;
    export function taskFleeCoordVia(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any): void;
    /** Params: p4 = -1.0f, p5 = -1, p6 = 0 in R* Scripts fleeStyle: see TASK_FLEE_COORD */
    export function taskFleePed(ped: number, fleeFromTarget: number, fleeStyle: number, flag: number, p4: number, p5: number, p6: number): void;
    export function taskFleePedVia(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any): void;
    /** Clears the current point route. Call this before TASK_EXTEND_ROUTE and TASK_FOLLOW_POINT_ROUTE. */
    export function taskFlushRoute(): void;
    export function taskFlyingCircle(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any): void;
    export function taskFlyAway(ped: number, fleeFromTarget: number): void;
    export function taskFlyToCoord(ped: number, travelMbr: number, x: number, y: number, z: number, p5: boolean, p6: boolean): void;
    export function taskFollowAndConverseWithPed(ped: number, targetPed: number, p2: any, p3: any, p4: number, p5: number, p6: number, p7: any, p8: any, p9: number, p10: number): void;
    export function taskFollowEntityAlongWaypointRecordingAtOffset(ped0: number, ped1: number, waypointRecording: string, p3: number, p4: number, p5: number, p6: number, p7: number, p8: boolean): void;
    export function taskFollowEntityWhileAimingAtEntity(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any): void;
    /** If no timeout, set timeout to -1. */
    export function taskFollowNavMeshToCoord(ped: number, x: number, y: number, z: number, speedMultiplier: number, timeout: number, stoppingRange: number, flags: number, heading: number): void;
    export function taskFollowNavMeshToCoordAdvanced(ped: number, x: number, y: number, z: number, speedMultiplier: number, timeout: number, stoppingRange: number, flags: number, p8: number, p9: number, p10: number, entity: number, unk: number): void;
    export function taskFollowPavementToCoord(ped: number, args: any): void;
    export function taskFollowPointRoute(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any): void;
    export function taskFollowToOffsetOfCoord(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any): void;
    export function taskFollowToOffsetOfEntity(ped: number, entity: number, offsetX: number, offsetY: number, offsetZ: number, movementSpeed: number, timeout: number, stoppingRange: number, persistFollowing: boolean, p9: boolean, walkOnly: boolean, p11: boolean, p12: boolean, p13: boolean): void;
    /** Follow a loaded waypoint recording. startIndex/endIndex bound the segment; patrol makes it loop back-and-forth; aimWeapon sets aiming stance; durationMs=-1 for natural pacing, otherwise caps time (... */
    export function taskFollowWaypointRecording(ped: number, waypointRecording: string, startIndex: number, flags: number, endIndex: number, patrol: boolean, aimWeapon: boolean, durationMs: number): void;
    export function taskFollowWaypointRecordingAdvanced(ped: number, p1: any): void;
    export function taskFollowWaypointRecordingAtOffset(ped: number, waypointRecording: string, p2: number, p3: number, p4: number, p5: number, p6: boolean): void;
    /** motionStateHash: see FORCE_PED_MOTION_STATE */
    export function taskForceMotionState(ped: number, motionStateHash: number, p2: boolean): void;
    /** ped = Ped you want to perform this task. target = the Entity they should aim at. distanceToStopAt = distance from the target, where the ped should stop to aim. StartAimingDist = distance where the ... */
    export function taskGotoEntityAiming(ped: number, target: number, distanceToStopAt: number, StartAimingDist: number): void;
    export function taskGotoEntityOffset(ped: number, entity: number, p2: any, x: number, y: number, z: number, duration: number): void;
    export function taskGotoEntityOffsetXy(ped: number, entity: number, duration: number, targetRadius: number, xOffset: number, yOffset: number, moveBlendRatio: number, offsetFlags: number): void;
    export function taskGotoEntityOffsetXyz(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any): void;
    export function taskGotoEntityOffsetXyzAiming(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any): void;
    export function taskGotoEntityOffsetXyAiming(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any): void;
    /** Go to coords wihtout using navmesh, if timeBeforeTeleport is -1 then it never teleports p8 is 1 or 0 still unknown. */
    export function taskGoStraightToCoord(ped: number, x: number, y: number, z: number, moveBlendSpeedY: number, timeBeforeTeleport: number, finalHeading: number, targetRadius: number, p8: number): void;
    /** Go to coords relative to entity wihtout using navmesh, if timeBeforeTeleport is -1 then it never teleports; p7 is 1 or 0 still unknown. */
    export function taskGoStraightToCoordRelativeToEntity(ped: number, entity: number, xOffset: number, yOffset: number, zOffset: number, moveBlendRatio: number, timeBeforeTeleport: number, p7: number): void;
    export function taskGoToCoordAndAimAtHatedEntitiesNearCoord(ped: number, goToLocationX: number, goToLocationY: number, goToLocationZ: number, focusLocationX: number, focusLocationY: number, focusLocationZ: number, speed: number, shootAtEnemies: boolean, distanceToStopAt: number, noRoadsDistance: number, unkTrue: boolean, unkFlag: number, aimingFlag: number, firingPattern: number): void;
    export function taskGoToCoordAndAimAtHatedEntitiesNearCoordUsingCombatStyle(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any): void;
    export function taskGoToCoordAnyMeans(ped: number, x: number, y: number, z: number, speed: number, entity: number, p6: boolean, walkingStyle: number, p8: number): void;
    export function taskGoToCoordAnyMeansExtraParams(ped: number, x: number, y: number, z: number, speed: number, p5: any, p6: boolean, walkingStyle: number, p8: number, p9: any, p10: any, p11: any, p12: any): void;
    export function taskGoToCoordAnyMeansExtraParamsWithCruiseSpeed(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any): void;
    export function taskGoToCoordWhileAimingAtCoord(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any, p15: any): void;
    export function taskGoToCoordWhileAimingAtCoordUsingCombatStyle(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any, p15: any): void;
    export function taskGoToCoordWhileAimingAtEntity(ped1: number, x: number, y: number, z: number, ped2: number, p5: number, p6: any, p7: number, p8: number, p9: any, p10: any, p11: any, firingPattern: number, p13: number, p14: any): void;
    export function taskGoToCoordWhileAimingAtEntityUsingCombatStyle(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any): void;
    export function taskGoToEntity(ped: number, target: number, duration: number, distance: number, speed: number, p5: number, p6: number): void;
    /** shootatEntity: If true, peds will shoot at Entity till it is dead. If false, peds will just walk till they reach the entity and will cease shooting. */
    export function taskGoToEntityWhileAimingAtEntity(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any): void;
    export function taskGoToEntityWhileAimingAtEntityUsingCombatStyle(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any): void;
    /** enum eWhistleType { 	WHISTLE_MAIN, 	WHISTLE_SECONDARY, 	WHISTLE_DOUBLE, 	WHISTLE_URGENT, 	WHISTLE_LONG }; */
    export function taskGoToWhistle(ped: number, p1: number, whistleType: number): void;
    /** grappleStyle: AR_GRAPPLE_MOUNT_STANDING_FROM_FRONT, AR_GRAPPLE_MOUNT_STANDING_FROM_RIGHT, AR_GRAPPLE_MOUNT_STANDING_FROM_BACK, AR_GRAPPLE_MOUNT_STANDING_FROM_LEFT, AR_GRAPPLE_MOUNT_FROM_FRONT, AR_W... */
    export function taskGrapple(ped: number, targetPed: number, grappleStyle: number, p3: number, p4: number, p5: number, p6: number): boolean;
    export function taskGuard(ped: number, p1: any, p2: any): void;
    export function taskGuardAssignedDefensiveArea(ped: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: any): void;
    export function taskGuardCurrentPosition(ped: number, p1: number, p2: number, p3: boolean): void;
    /** flags: 0 = HANDS_UP_NOTHING; 1 = HANDS_UP_STRAIGHT_TO_LOOP */
    export function taskHandsUp(ped: number, duration: number, facingPed: number, timeToFacePed: number, flags: number): void;
    export function taskHitchAnimal(ped: number, scenarioPoint: number, flag: number): void;
    export function taskHogtieable(ped: number): void;
    export function taskHogtieTargetPed(ped: number, targetPed: number): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/tasks/TASK_HORSE_ACTION Params: p2, p3 are set to 0 in R* Scripts */
    export function taskHorseAction(ped: number, action: number, targetPed: number, p3: any): void;
    export function taskInvestigate(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any): void;
    export function taskJump(ped: number, unused: boolean): void;
    export function taskKnockedOut(ped: number, p1: number, permanently: boolean): void;
    /** koTimeOffset (seconds): offset applied to the knockout timer—positive delays recovery (longer KO), negative brings recovery sooner, 0.0 initializes with no extension (immediate baseline). flags (bi... */
    export function taskKnockedOutAndHogtied(ped: number, koTimeOffset: number, flags: number): void;
    export function taskLassoPed(ped: number, targetPed: number): void;
    export function taskLeadAndConverse(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any): void;
    export function taskLeadHorse(ped: number, horse: number): void;
    /** flags: See TASK_ENTER_VEHICLE */
    export function taskLeaveAnyVehicle(ped: number, p1: number, taskFlag: number): void;
    /** flags: See TASK_ENTER_VEHICLE */
    export function taskLeaveVehicle(ped: number, vehicle: number, flags: number, unkPed: number): void;
    export function taskLookAtCoord(ped: number, x: number, y: number, z: number, duration: number, flags: number, p6: number, p7: boolean): void;
    /** param3: duration in ms, use -1 to look forever param4: using 2048 is fine param5: using 3 is fine */
    export function taskLookAtEntity(ped: number, lookAtTarget: number, duration: number, p3: number, p4: number, p5: number): void;
    export function taskLootEntity(ped: number, entity: number): void;
    export function taskLootNearestEntity(ped: number, x: number, y: number, z: number, p4: number, p5: number): void;
    /** Params: p2: AR_TAKEDOWN_FRONT, AR_EXECUTION_FRONT, 0 in R* Scripts */
    export function taskMelee(ped: number, targetPed: number, p2: number, p3: any, p4: any, p5: number, p6: any, p7: number): boolean;
    /** timer: in ms, if it reaches 0 it will auto warp the ped on the horse mountStyle: See TASK_ENTER_VEHICLE Flags will still apply to mountStyle */
    export function taskMountAnimal(ped: number, mount: number, timer: number, seatIndex: number, pedSpeed: number, mountStyle: number, p6: any, p7: any): void;
    export function taskMoveBeInFormation(ped: number, p1: any, p2: number, p3: number, p4: number, p5: number, p6: any): void;
    /** Params: moveBlendRatio commonly 1.25f, p5 is always 0 in R* Scripts */
    export function taskMoveFollowRoadUsingNavmesh(ped: number, moveBlendRatio: number, x: number, y: number, z: number, p5: any): void;
    export function taskMoveInTraffic(ped: number, p1: number, p2: any, p3: any): void;
    export function taskMoveInTrafficAwayFromEntity(ped: number, p1: any, p2: any, p3: any, p4: any): void;
    export function taskMoveInTrafficToDestination(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any): void;
    export function taskMoveNetworkAdvancedByNameWithInitParams(ped: number, moveNetworkDefName: string, taskData: any, xPos: number, yPos: number, zPos: number, xRot: number, yRot: number, zRot: number, p9: number, p10: number, p11: number, p12: number, flag: number, p14: number): void;
    export function taskMoveNetworkAdvancedByNameWithInitParamsAttached(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any, p15: any, p16: any, p17: any): void;
    export function taskMoveNetworkByName(ped: number, task: string, multiplier: number, p3: boolean, animDict: string, flags: number): void;
    export function taskMoveNetworkByNameWithInitParams(ped: number, moveNetworkDefName: string, taskData: any, p3: number, p4: boolean, animDict: string, flags: number): void;
    export function taskPatrol(ped: number, patrolRoute: string, p2: any, p3: boolean, p4: boolean): void;
    /** This tasks the ped to do nothing for the specified amount of milliseconds. This is useful if you want to add a delay between tasks when using a sequence task. */
    export function taskPause(ped: number, ms: number): void;
    export function taskPedSlideToCoord(ped: number, x: number, y: number, z: number, heading: number, p5: number): void;
    export function taskPerformSequence(ped: number, taskSequenceId: number): void;
    export function taskPerformSequenceFromProgress(ped: number, p1: any, p2: any, p3: any): void;
    export function taskPersistentCharacter(ped: number): void;
    export function taskPickupCarriableEntity(ped: number, entity: number): void;
    export function taskPickUpWeapon(ped: number, p1: any): void;
    export function taskPlaceCarriedEntityAtCoord(ped: number, entity: number, x: number, y: number, z: number, p5: number, flags: number): void;
    export function taskPlaceCarriedEntityOnMount(ped: number, entity: number, mount: number, p3: number): void;
    export function taskPlantBomb(ped: number, x: number, y: number, z: number, heading: number): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/animations flags: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eScriptedAnimFlags ikFlags: https://github.com/Halen84/RDR3-... */
    export function taskPlayAnim(ped: number, animDict: string, animName: string, speed: number, speedMultiplier: number, duration: number, flags: number, playbackRate: number, p8: boolean, ikFlags: number, p10: boolean, taskFilter: string, p12: boolean): void;
    /** flags: see TASK_PLAY_ANIM ikFlags: see TASK_PLAY_ANIM */
    export function taskPlayAnimAdvanced(ped: number, animDict: string, animName: string, posX: number, posY: number, posZ: number, rotX: number, rotY: number, rotZ: number, speed: number, speedMultiplier: number, duration: number, flags: number, p13: number, p14: number, p15: number, p16: number): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/animations/kit_emotes_list.lua emote: https://alloc8or.re/rdr3/doc/enums/eEmote.txt  enum eEmoteType { 	EMOTE_TYPE_INVALID = -1, 	EMOTE_TYPE_RE... */
    export function taskPlayEmoteWithHash(ped: number, emoteType: number, playbackMode: number, emote: number, isSecondaryTask: boolean, canBreakOut: boolean, disableEarlyOutAnimTag: boolean, ignoreInvalidMainTask: boolean, destroyProps: boolean): void;
    export function taskPlayUpperAnimFacingEntity(ped: number, animDict: string, animName: string, entity: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: boolean, p10: boolean, p11: number, p12: string, p13: number, p14: number): void;
    export function taskPolice(ped: number, p1: boolean): boolean;
    export function taskPutPedDirectlyIntoCover(ped: number, x: number, y: number, z: number, timeout: number, p5: boolean, p6: number, p7: any, p8: any, coverpoint: number, p10: boolean, p11: boolean, p12: any): void;
    /** grappleStyle: AR_GRAPPLE_STRUGGLE, AR_ALLIGATOR_LEG_GRAB_CHALLENGE_FAIL, AR_GRAPPLE_BACK_FROM_BACK, AR_GRAPPLE_BACK_DEFEND, AR_GRAPPLE_FRONT_FROM_FRONT */
    export function taskPutPedDirectlyIntoGrapple(ped: number, grappleTarget: number, grappleStyle: number, p3: number, p4: number, p5: boolean, p6: number): void;
    /** meleeStyles: AR_GRAPPLE_BACK_FROM_BACK, AR_GRAPPLE_MOUNT_FACEDOWN_FROM_FRONT, AR_ALLIGATOR_LEAPKILL, AR_ALLIGATOR_WAIST_AUTOKILL_FRONT */
    export function taskPutPedDirectlyIntoMelee(ped: number, meleeTarget: number, meleeStyle: number, p3: number, animBlendRatio: number, p5: boolean, p6: number): void;
    /** Makes a ped react to an entity. Params: reactingTo Entity can be 0, p8 is always 4 */
    export function taskReact(ped: number, reactingTo: number, x: number, y: number, z: number, reactionName: string, p6: number, p7: number, p8: number): void;
    export function taskReloadWeapon(ped: number, unused: boolean): void;
    export function taskReviveTarget(ped: number, reviver: number, tool: number): void;
    export function taskRideTrain(ped: number, train: number, scenarioPoint: number, scenarioHash: number): void;
    export function taskRobPed(ped: number, target: number, p2: number, flag: number, p4: number): void;
    export function taskScriptedAnimation(ped: number, args: any): void;
    export function taskSeekClearLosToEntity(ped: number, entity: number, p2: number, p3: number, p4: number): void;
    export function taskSeekCoverFromPed(ped: number, fromPed: number, duration: number, p3: any, p4: any, p5: any): void;
    export function taskSeekCoverFromPos(ped: number, x: number, y: number, z: number, duration: number, p5: any, p6: any, p7: any): void;
    export function taskSeekCoverToCoords(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any): void;
    export function taskSeekCoverToCoverPoint(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any): void;
    export function taskSetBlockingOfNonTemporaryEvents(ped: number, toggle: boolean): void;
    export function taskSetCrouchMovement(ped: number, p1: boolean, p2: any, p3: boolean): void;
    export function taskSetSphereDefensiveArea(ped: number, p1: number, p2: number, p3: number, p4: number): void;
    export function taskSetStealthMovement(ped: number, p1: boolean, p2: any, p3: boolean): void;
    export function taskShockingEventReact(ped: number, p1: any, p2: any): void;
    export function taskShootAtCoord(ped: number, x: number, y: number, z: number, duration: number, firingPattern: number, p6: any): void;
    export function taskShootAtEntity(entity: number, targetEntity: number, duration: number, firingPattern: number, affectCockedState: boolean): void;
    export function taskShootWithWeapon(ped: number, args: any): void;
    /** Makes the specified ped shuffle to the next vehicle seat. The ped MUST be in a vehicle and the vehicle parameter MUST be the ped's current vehicle. */
    export function taskShuffleToNextVehicleSeat(ped: number, vehicle: number): void;
    /** Makes the specified ped flee the specified distance from the specified position. fleeType: see TASK_FLEE_COORD */
    export function taskSmartFleeCoord(ped: number, x: number, y: number, z: number, distance: number, time: number, fleeType: number, fleeSpeed: number): void;
    /** Makes a ped run away from another ped (fleeFromTarget)  fleeDistance = ped will flee this distance fleeTime = ped will flee for this amount of time, set to "-1" to flee forever fleeType = see TASK_... */
    export function taskSmartFleePed(ped: number, fleeFromTarget: number, fleeDistance: number, fleeTime: number, fleeType: number, fleeSpeed: number, targetPed: number): void;
    export function taskStandGuard(ped: number, x: number, y: number, z: number, heading: number, scenarioName: string): void;
    /** Makes the specified ped stand still for (time) milliseconds. */
    export function taskStandStill(ped: number, time: number): void;
    export function taskStartScenarioAtPosition(ped: number, scenarioHash: number, x: number, y: number, z: number, heading: number, duration: number, sittingScenario: boolean, teleport: boolean, p9: string, p10: number, p11: boolean): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/animations/scenarios Params: duration in milliseconds  conditionalHash (optionally): 0 = play random conditional anim. Every conditional anim h... */
    export function taskStartScenarioInPlaceHash(ped: number, scenarioHash: number, duration: number, playEnterAnim: boolean, conditionalHash: number, heading: number, p6: boolean): void;
    /** Makes the ped run to take cover */
    export function taskStayInCover(ped: number): void;
    export function taskStopLeadingHorse(ped: number): void;
    /** Baits: see 0x9B0C7FA063E67629 */
    export function taskSwapFishingBait(ped: number, bait: string, withoutBuoy: boolean): void;
    export function taskSwapWeapon(ped: number, p1: any, p2: any, p3: any, p4: any): void;
    export function taskThrowProjectile(ped: number, p1: any, p2: any, p3: any): void;
    /** duration in milliseconds */
    export function taskTurnPedToFaceCoord(ped: number, x: number, y: number, z: number, duration: number): void;
    /** duration: the amount of time in milliseconds to do the task. -1 will keep the task going until either another task is applied, or CLEAR_ALL_TASKS() is called with the ped */
    export function taskTurnPedToFaceEntity(ped: number, targetEntity: number, duration: number, p3: number, p4: number, p5: number): void;
    export function taskTurnToFaceClosestPed(ped: number, p1: number, p2: number, p3: number): void;
    export function taskUseNearestScenarioChainToCoord(ped: number, x: number, y: number, z: number, distance: number, p5: boolean, p6: boolean, p7: boolean, p8: boolean): void;
    export function taskUseNearestScenarioChainToCoordWarp(ped: number, x: number, y: number, z: number, distance: number, p5: boolean, p6: boolean, p7: boolean, p8: boolean): void;
    export function taskUseNearestScenarioToCoordWarp(ped: number, x: number, y: number, z: number, distance: number, duration: number, p6: boolean, p7: boolean, p8: boolean, p9: boolean): void;
    export function taskUseNearestTrainScenarioToCoordWarp(ped: number, x: number, y: number, z: number, distance: number): void;
    export function taskUseRandomScenarioInGroup(ped: number, p1: any, p2: any, p3: any, p4: any): void;
    export function taskUseScenarioPoint(ped: number, scenario: number, conditionalAnim: string, p3: number, p4: boolean, p5: boolean, p6: number, p7: boolean, p8: number, p9: boolean): void;
    export function taskVehicleAimAtCoord(ped: number, x: number, y: number, z: number): void;
    export function taskVehicleAimAtPed(ped: number, target: number): void;
    /** Old name: _TASK_VEHICLE_DRIVE_TO_POINT flag: 524419 and 0 in shop_horse_shop R* Script */
    export function taskVehicleDriveStraightToPoint(driver: number, vehicle: number, x: number, y: number, z: number, p5: number, p6: number, flag: number): void;
    /** stopRange: how close vehicle will get to destination before stopping, default 4.0 straightLineDist: distance at which AI switches to heading for target directly instead of following nodes, default -1 */
    export function taskVehicleDriveToCoord(ped: number, vehicle: number, x: number, y: number, z: number, speed: number, drivingStyle: number, vehicleModel: number, drivingMode: number, stopRange: number, straightLineDist: number): void;
    /** flags: 67108864, 2097152, 524564, 524675 (eDrivingFlags) p7 = 6 or 3 p8 = x coordinate p9 - 8.f p10 = false */
    export function taskVehicleDriveToDestination(driver: number, vehicle: number, x: number, y: number, z: number, speed: number, drivingFlags: number, p7: number, stoppingRange1: number, stoppingRange2: number, p10: boolean): void;
    export function taskVehicleDriveWander(ped: number, vehicle: number, speed: number, drivingStyle: number): void;
    export function taskVehicleEscort(ped: number, vehicle: number, targetVehicle: number, mode: number, speed: number, drivingStyle: number, minDistance: number, p7: number, noRoadsDistance: number): void;
    export function taskVehicleFollowWaypointRecording(ped: number, vehicle: number, waypointRecording: string, drivingMode: number, p4: any, eWaypoint: number, flag: number, p7: number, p8: boolean, stoppingDist: number, p10: any): void;
    export function taskVehicleGotoNavmesh(ped: number, vehicle: number, x: number, y: number, z: number, speed: number, behaviorFlag: number, stoppingRange: number): void;
    export function taskVehicleMission(driver: number, vehicle: number, vehicleTarget: number, missionType: number, p4: number, p5: any, p6: number, p7: number, DriveAgainstTraffic: boolean): void;
    /** See TASK_VEHICLE_MISSION */
    export function taskVehicleMissionPedTarget(ped: number, vehicle: number, pedTarget: number, mode: number, maxSpeed: number, drivingStyle: number, minDistance: number, p7: number, DriveAgainstTraffic: boolean): void;
    export function taskVehicleShootAtCoord(ped: number, x: number, y: number, z: number, p4: number): void;
    export function taskVehicleShootAtPed(ped: number, target: number, p2: number): void;
    /** Documentation from GTA V, might be the same in RDR:  '1 - brake '3 - brake + reverse '4 - turn left 90 + braking '5 - turn right 90 + braking '6 - brake strong (handbrake?) until time ends '7 - tur... */
    export function taskVehicleTempAction(driver: number, vehicle: number, action: number, time: number): void;
    export function taskWalkAway(ped: number, entity: number): void;
    export function taskWanderAndConverseWithPed(ped: number, p1: any, p2: any, p3: any): void;
    export function taskWanderInArea(ped: number, x: number, y: number, z: number, radius: number, p5: number, p6: number, p7: number): void;
    export function taskWanderInVolume(ped: number, volume: number, p2: number, p3: number, p4: number): void;
    /** Makes ped walk around the area.  set p1 to 10.0f and p2 to 10 if you want the ped to walk anywhere without a duration. */
    export function taskWanderStandard(ped: number, p1: number, p2: number): void;
    export function taskWanderSwim(ped: number, p1: any): void;
    export function taskWarpPedIntoVehicle(ped: number, vehicle: number, seat: number): void;
    export function taskWeapon(ped: number): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/AI/EVENTS/aud_ped_whistle_types.lua p2: UNSPECIFIED */
    export function taskWhistleAnim(ped: number, audPedWhistleType: number, p2: number): void;
    export function uncuffPed(ped: number): void;
    /** getupSetHash: see nm_blend_out_sets.meta */
    export function unhogtiePed(ped: number, flags: number, getupSetHash: number, p3: string, p4: string, p5: number): void;
    export function updateTaskHandsUpDuration(ped: number, duration: number): void;
    export function useWaypointRecordingAsAssistedMovementRoute(waypointRecording: string, p1: boolean, p2: number, p3: number, p4: boolean): void;
    export function vehicleWaypointPlaybackGetIsPaused(p0: any): any;
    export function vehicleWaypointPlaybackOverrideSpeed(vehicle: number, speed: number): void;
    export function vehicleWaypointPlaybackPause(vehicle: number): void;
    export function vehicleWaypointPlaybackResume(vehicle: number): void;
    export function vehicleWaypointPlaybackUseDefaultSpeed(vehicle: number): void;
    export function waypointPlaybackGetIsAiming(ped: number): boolean;
    export function waypointPlaybackGetIsPaused(ped: number): boolean;
    export function waypointPlaybackGetIsShooting(ped: number): boolean;
    export function waypointPlaybackOverrideSpeed(ped: number, speed: number, p2: any, p3: number, p4: any): void;
    export function waypointPlaybackPause(ped: number, p1: any, p2: any, p3: any): void;
    export function waypointPlaybackResume(ped: number, p1: boolean, p2: number, p3: number): void;
    export function waypointPlaybackStartAimingAtCoord(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any): void;
    export function waypointPlaybackStartAimingAtEntity(p0: any, p1: any, p2: any, p3: any): void;
    export function waypointPlaybackStartAimingAtPed(p0: any, p1: any, p2: any, p3: any): void;
    export function waypointPlaybackStartShootingAtCoord(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any): void;
    export function waypointPlaybackStartShootingAtEntity(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    export function waypointPlaybackStartShootingAtPed(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    export function waypointPlaybackStopAimingOrShooting(p0: any): void;
    export function waypointPlaybackUseDefaultSpeed(ped: number): void;
    export function waypointRecordingGetClosestWaypoint(waypointRecording: string, x: number, y: number, z: number, point: number): boolean;
    export function waypointRecordingGetCoord(waypointRecording: string, point: number, coord: Vector3): boolean;
    export function waypointRecordingGetNumPoints(waypointRecording: string, points: number): boolean;
    export function waypointRecordingGetSpeedAtPoint(waypointRecording: string, point: number): number;
    export function addCoverBlockingVolume(volume: number, p1: boolean, p2: boolean, p3: boolean, p4: boolean): void;
    /** Returns the entity coverpoint with offset. */
    export function addCoverPointForEntity(entity: number, xOffset: number, yOffset: number, zOffset: number, heading: number, p5: number, p6: number, p7: number, p8: number): number;
    export function addFleeTargetCoords(ped: number, x: number, y: number, z: number, p4: number): void;
    export function associatePropWithScenario(scenario: number, entity: number, propName: string, p3: boolean): boolean;
    /** Signed arclength (meters) from the start of a loaded waypoint recording to the point on the path nearest to (x,y,z). Negative before the first node; clamped to total length past the last node. Reco... */
    export function calculateWaypointDistanceFromStart(waypointRecording: string, x: number, y: number, z: number): number;
    /** Clears all active tasks assigned to the specified vehicle. This cancels ongoing behaviors such as TASK_VEHICLE_DRIVE_TO_DESTINATION_2. And even tasks triggered by horses pulling the vehicle when th... */
    export function clearVehicleTasks(vehicle: number): void;
    /** Clears the vehicle's secondary/aux AI task slot (behaviors/overlays). In R* Scripts, this is often called right after _CLEAR_VEHICLE_TASKS to fully stop/flush vehicle behavior. */
    export function clearVehicleTasksSecondary(vehicle: number): void;
    /** groundSetting: 0: spawn on ground, 2 (1?): do not spawn on ground p7: -1 in R* Scripts Returns compositeId */
    export function createHerbComposites(asset: number, x: number, y: number, z: number, heading: number, groundSetting: number, p6: any, p7: number): number;
    export function createWaypointPath(pathName: string, p1: any, nodes: number, p3: number): boolean;
    export function cuffPed(ped: number): void;
    /** Params: p1 is always false except in script nb_egg_protector */
    export function deletePatchObjectsFromHerbComposites(compositeId: number, p1: boolean): void;
    export function deleteScenarioPoint(scenario: number): void;
    export function detachCarriablePed(ped: number): void;
    export function disassociatePropFromScenario(scenario: number, propName: string): boolean;
    export function doesScenarioGroupExistHash(scenarioGroup: number): boolean;
    export function doesScenarioPointHaveProps(scenario: number): boolean;
    export function emitPedCarriableStruggleDirection(ped: number, direction: number): void;
    export function emitPedCarriableStruggleIntensity(ped: number, intensity: number): void;
    export function evaluatePedCarriableStruggleAvailable(ped: number): boolean;
    export function findModelForItem(item: number): number;
    /** Animals only. Returns the nearest ped around `animalPed` matching the life-state filters (e.g., use (false,true,0) to find a nearby corpse for TASK_EAT). Last flag appears to bias predators/fish to... */
    export function findNearestPedAroundAnimal(animalPed: number, aliveOnly: boolean, deadOnly: boolean, preferDeadPredators: boolean): number;
    /** Flowers, Stalks or whatever the composite has */
    export function getHerbCompositeNumEntities(compositeId: number, outEntities: any): number;
    export function getHogtieEscapeTimer(ped: number): number;
    /** Returns whether the “Hold to Reel [Fishing]” gameplay setting is currently enabled. */
    export function getHoldToReelSettingEnabled(): boolean;
    /** item hashes: PRIMARYITEM, P_MUGCOFFEE01X_PH_R_HAND, P_BOTTLEBEER01X_PH_R_HAND http://prntscr.com/1qtp3bz https://github.com/femga/rdr3_discoveries/tree/master/tasks/TASK_ITEM_INTERACTION */
    export function getItemInteractionEntityFromPed(ped: number, item: number): number;
    export function getLedHorseFromPed(ped: number): number;
    /** Fills an output array with scenario points linked ("chained") to a given parent scenario point. Returns the number of linked points found. Writes up to maxPoints and zeroes remaining entries. Usefu... */
    export function getLinkedScenarioPoints(scenarioPoint: number, outPoints: number, maxPoints: number): number;
    export function getPedIsIgnoringDeadBodies(ped: number): boolean;
    export function getPedUsingScenarioPoint(scenario: number): number;
    /** Returns the current 'Break Free' prompt progress for a hogtied/knocked-out (writhing) ped. Range: 0.0-1.0 (hits 1.0 when the ped breaks free). Returns -1.0 if not applicable. */
    export function getPedWritheBreakFreeProgress(ped: number): number;
    /** Returns the total number of compartments (drawers, lids, etc.) the specified scenario container entity has. For example, a chest has 1 compartment, while a cabinet with 3 drawers returns 3. */
    export function getRansackScenarioContainerNumCompartments(entity: number): number;
    /** Returns the number of currently open compartments for the specified scenario container entity. If the container has closeable compartments (like drawers), this will return how many of them are curr... */
    export function getRansackScenarioContainerNumOpenCompartments(entity: number): number;
    /** Returns m_eContainerState */
    export function getRansackScenarioContainerOpeningState(entity: number): boolean;
    /** Returns the total number of lootable items currently inside the specified scenario container entity. This value decreases as items are looted.Before opening, it may return 0 because contents are no... */
    export function getRansackScenarioContainerRemainingLootCount(entity: number): number;
    /** Returns the targeted revivable horse (critically injured/writhing) when the revive prompt is active; 0 if none. */
    export function getRevivableHorse(): number;
    /** Params: p1 is always true in R* Scripts */
    export function getScenarioPointCoords(scenario: number, p1: boolean): Vector3;
    /** Note: The current name for this native is the old name of 0x295514F198EFD0CA Old name for this native: _GET_ENTITY_SCENARIO_POINT_IS_ATTACHED_TO */
    export function getScenarioPointEntity(scenario: number): number;
    /** Params: p1 is always true in R* Scripts */
    export function getScenarioPointHeading(scenario: number, p1: boolean): number;
    export function getScenarioPointPedIsUsing(ped: number, p1: boolean): number;
    export function getScenarioPointRadius(scenario: number): number;
    export function getScenarioPointType(scenario: number): number;
    export function getScenarioPointTypePedIsUsing(ped: number): number;
    export function getScriptTaskActionTime(ped: number, task: number): number;
    /** Returns true if the ped is in a ranged-attack task and is about to fire (ready/primed to shoot or throw). Covers firearms and projectiles (throwables/molotov/poison), not melee. Useful to block oth... */
    export function getTaskCombatReadyToShoot(ped: number): boolean;
    /** Fishing Research: https://pastebin.com/NmK5ZLVs Only used in R* Scripts fishing_core and av_fishing_river */
    export function getTaskFishing(ped: number, p1: any): boolean;
    /** Returns hash of the underlying move network def, see move_networks.xml https://alloc8or.re/rdr3/doc/misc/move_networks.txt */
    export function getTaskMoveNetworkId(ped: number): number;
    export function getTaskMoveNetworkPhaseFloat(ped: number, phaseName: string): number;
    /** Returns 0.0-1.0 progress for the current mount-leap task, or -1.0f if no leap is active. Video demo: https://youtu.be/YZuw9lhqDms */
    export function getTaskPedMountLeapProgress(ped: number): number;
    /** Returns a coarse state for the mount-leap task (jumping from your mount onto another mount/wagon/train). -1 = no task; 0 = in-air/ongoing; 1 = boarded/mounted; 2 = boarded rear train trailer. Use w... */
    export function getTaskPedMountLeapState(ped: number): number;
    /** Returns the maximum (target) whistle/call distance associated with the next horse bonding level. Used together with the current level's minimum to derive an effective whistle range based on the hor... */
    export function getWhistleRangeMaxForBondingLevel(bondingLevel: number): number;
    /** Returns the minimum (baseline) whistle/call distance for the given horse bonding level. This value represents the lower bound used when computing whether a horse is considered "near" or "far" relat... */
    export function getWhistleRangeMinForBondingLevel(bondingLevel: number): number;
    /** carriableConfig: see _REQUEST_CARRIABLE_CONFIG */
    export function hasRequestedCarriableConfigLoaded(carriableConfig: number): boolean;
    export function isEntityRevivable(ped: number): boolean;
    /** Returns true while a hat is being picked up _IS_A* - _IS_D* */
    export function isHatBeingPickedUp(hatObject: number): boolean;
    /** Returns true while a hat is being picked up. Similar to 0x11CD066F54DA0133 _IS_A* - _IS_D* */
    export function isHatBeingPickedUp2(hatObject: number): boolean;
    export function isPedArrestingAnyPed(ped: number): boolean;
    /** Returns true if the given ped (usually a horse) is currently being led by a ped (lead/rope). Mirrors usage with _IS_PED_LEADING_HORSE(ped) and _GET_LED_HORSE_FROM_PED(ped). */
    export function isPedBeingLed(ped: number): boolean;
    export function isPedDuelling(ped: number): boolean;
    export function isPedLeadingHorse(ped: number): boolean;
    /** Returns true if the ped's current 'IK look-at' target is within `radius` of (x, y, z). This checks the active look-at point (head/eyes) — not LOS or heading — and returns false if the ped has no ac... */
    export function isPedLookingAtCoord(ped: number, x: number, y: number, z: number, radius: number): boolean;
    /** Checks for the revive-horse prompt. strict=true: prompt must be usable (enabled and inputs not blocked). strict=false: true if the prompt simply exists (may be disabled). */
    export function isRevivableHorsePromptVisible(strict: boolean): boolean;
    export function isScenarioGroupEnabledHash(scenarioGroup: number): boolean;
    /** Checks whether a specified scenario is currently being used (actively played) by any entity (player or ped). Returns true if the scenario is already occupied, otherwise false. */
    export function isScenarioInUse(scenario: number): boolean;
    export function isScenarioPointActive(scenario: number): boolean;
    export function isScenarioPointFlagSet(scenario: number, flag: number): boolean;
    export function makeObjectCarriable(object: number): void;
    export function pedFishingrodHookEntity(ped: number, entity: number): void;
    /** Used with 'P_BODYPARTARMFLOAT02X' model in fishing_core.c */
    export function pedFishingrodHookObject(ped: number, object: number): void;
    export function pedIsInScenarioBase(ped: number): boolean;
    /** carriableConfig: see _REQUEST_CARRIABLE_CONFIG */
    export function removeCarriableConfig(carriableConfig: number): void;
    /** Config: https://pastebin.com/gZvuq7fV */
    export function requestCarriableConfig(carriableConfig: number): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/objects/composites */
    export function requestHerbCompositeAsset(asset: number): boolean;
    export function resetScenarioForEntity(scenario: number, entity: number): void;
    export function resetScenarioScript(scenario: number): void;
    /** Set a ped's boat-local offset and/or facing (degrees). Boats only. Flags: 0=apply both; 1=heading only (lock offset); 2=offset only (lock heading); 3=apply neither. */
    export function setAboardPedBoatPose(ped: number, boat: number, offsetX: number, offsetY: number, offsetZ: number, heading: number, flags: number): void;
    /** Enables or disables the interaction prompt for a given carriable config (e.g. DEAD_CARRIABLE_HUMAN). Use after loading the config; when disabled, pickup/use prompts will not appear. carriableConfig... */
    export function setCarriableConfigPromptEnabled(carriableConfig: number, toggle: boolean): void;
    /** Baits: p_fishHook02x, p_baitBread01x, p_baitCorn01x, p_baitCheese01x, p_baitWorm01x, p_baitCricket01x, p_crawdad01x, p_finisheDragonfly01x, p_finisdFishlure01x, p_finishdCrawd01x, p_finisheDragonfl... */
    export function setFishingBait(ped: number, bait: string, withoutBuoy: boolean, instantly: boolean): void;
    /** Sets the time it takes for a hogtied ped to escape -1.0f for ped to never escape */
    export function setHogtieEscapeTimer(ped: number, time: number): void;
    /** Controls intimidated/hogtied ped facing. If useLimits=false, always face the player; if =true, clamp facing within [minAngle, maxAngle] degrees. Angle note: the range defines the allowed yaw cone a... */
    export function setIntimidatedFacingAngle(ped: number, useLimits: boolean, minAngle: number, maxAngle: number): void;
    /** All Interaction states https://github.com/abdulkadiraktas/rdr3_discoveries/tree/master/tasks/ItemInteraction#4-item_interaction_state_name--item_interaction_propid */
    export function setItemInteractionState(ped: number, itemInteractionState: number, p2: number): void;
    export function setPedClearAimingInTheAir(ped: number, p1: any): void;
    export function setPedIgnoreDeadBodies(ped: number, toggle: boolean): void;
    export function setPedPathAvoidTraffic(ped: number, avoidTraffic: boolean): void;
    /** _SET_PED_PATH_P* */
    export function setPedPathLadderCostModifier(ped: number, modifier: number): void;
    export function setPedPathMayEnterDeepWater(ped: number, mayEnterDeepWater: boolean): void;
    export function setPedPathMayUseSlidingSurfaces(ped: number, useSlidingSurfaces: boolean): void;
    export function setPedPathNeverUseInteriors(ped: number, neverUseInteriors: boolean): void;
    export function setPedPathPreferHorseWalkable(ped: number, preferHorseWalkable: boolean, p2: number): void;
    export function setPedPathPreferStayInWater(ped: number, preferStayInWater: boolean, p2: number): void;
    export function setPedPathPreferToAvoidFoliage(ped: number, preferAvoidFoliage: boolean, p2: number): void;
    export function setPedPathPreferToAvoidMud(ped: number, preferAvoidMud: boolean, p2: number): void;
    /** Opens/closes containers: ChestDugUp */
    export function setRansackScenarioContainerOpeningState(entity: number, open: boolean): void;
    export function setScenarioGroupEnabledHash(scenarioGroup: number, toggle: boolean): void;
    export function setScenarioPointActive(scenario: number, active: boolean): void;
    export function setScenarioPointCoords(scenario: number, xPos: number, yPos: number, zPos: number, p4: boolean): void;
    /** flag: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/CScenarioPointFlags__Flags */
    export function setScenarioPointFlag(scenario: number, flag: number, value: boolean): void;
    export function setScenarioPointHeading(scenario: number, heading: number, p2: boolean): void;
    export function setScenarioPointRadius(scenario: number, radius: number): void;
    export function setScenarioTypeEnabledHash(scenarioType: number, toggle: boolean): void;
    /** Only used in R* Scripts fishing_core and av_fishing_river */
    export function setTaskFishing(ped: number, p1: any): boolean;
    export function setTaskMoveNetworkSignalFloat2(ped: number, signalName: string, value: number): void;
    export function setTaskMoveNetworkSignalVector(ped: number, signalName: string, x: number, y: number, z: number): void;
    /** Swaps the wagon/coach reins control between the ped and their adjacent front-seat partner. */
    export function swapReinsForPeds(ped: number): void;
    export function taskAnimalBleedOut(ped: number, killer: number, flee: boolean, weaponHash: number, p4: number, boneId: number): void;
    export function taskBoardVehicle(ped: number, vehicle: number, p2: number, p3: any, speed: number, boardingFlags: number): void;
    export function taskBoardVehicle2(ped: number, p1: any, p2: number, speed: number, boardingFlags: number): void;
    export function taskClimb2(ped: number, heading: number): void;
    /** Coords: volume coords used in R* Script smuggler2 p4/p5 = 0 in R* Scripts */
    export function taskCombatPedAtCoords(ped: number, x: number, y: number, z: number, p4: number, p5: number): void;
    export function taskCutFreeHogtiedTargetPed(ped: number, targetPed: number): void;
    export function taskCutFreeHogtiedTargetPed2(ped: number, targetPed: number, p2: number): void;
    export function taskDisembarkVehicle(p0: any, vehicle: number, p2: number, p3: any, p4: number, p5: any): void;
    /** Triggers the 'action / flourish' sub-clip of the ped's currently playing emote. Returns true on success, false if no valid emote state. Observed in ingame UIs to fire a flourish while an emote loop... */
    export function taskEmoteAction(ped: number): boolean;
    export function taskEmoteOutro(ped: number): void;
    /** _A* */
    export function taskEquipHat(hatObject: number, ped: number): void;
    /** fleeType: see TASK_FLEE_COORD */
    export function taskFleeFromCoord(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any): void;
    /** fleeType: see TASK_FLEE_COORD */
    export function taskFleeFromPed(ped: number, fleeFromTarget: number, x: number, y: number, z: number, distance: number, p6: number, p7: number, p8: number, targetPed: number): void;
    /** Makes a ped that is already aiming keep firing at `targetEntity` for `durationMs`. Uses projectile logic if holding a throwable/bow, otherwise gun logic. `p3` unused; `p4` is an extra mode flag (ob... */
    export function taskForceFireAtEntityWhileAiming(ped: number, targetEntity: number, durationMs: number, p3: any, p4: boolean): void;
    export function taskGuardAssignedDefensiveArea2(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any): void;
    /** Only used in R* SP Script homeinvasion: Params p2, p3, p4: 0, 0, 1 */
    export function taskIntimidated(p0: any, ped: number, p2: any, p3: any, p4: any): boolean;
    export function taskIntimidated2(victim: number, attacker: number, p2: number, p3: boolean, p4: boolean, everyFrame: boolean, p6: boolean, p7: boolean, flag: number): boolean;
    export function taskItemInteraction2(ped: number, propNameGxt: number, prop: number, propId: number, itemInteractionState: number, p5: number, p6: any, p7: number): void;
    /** Params: p3, p4, p5, p6: 0, 0, 0, -1.0f in R* Scripts */
    export function taskItemInteraction3(ped: number, item: number, guid: any, p3: any, p4: any, p5: any, p6: number): void;
    export function taskJump2(ped: number, x: number, y: number, z: number, entity: number): void;
    /** Sets the knockout timer in seconds for a ped that is currently in the knocked-out state. */
    export function taskKnockedOutSetDuration(ped: number, koTimeDuration: number): void;
    /** Sets an unknown float tuning setting for a ped that is currently in the knocked-out state. */
    export function taskKnockedOutSetTuning(ped: number, tuning: number): void;
    export function taskPatrol2(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any): void;
    export function taskPerformSequence2(p0: any, p1: any, p2: any, p3: any): void;
    /** Similar to 0xB31A277C1AC7B7FF but checks if the ped's inventory contains the specified emote kit. */
    export function taskPlayEmote(ped: number, emoteType: number, playbackMode: number, emote: number, isSecondaryTask: boolean, canBreakOut: boolean, disableEarlyOutAnimTag: boolean, ignoreInvalidMainTask: boolean, destroyProps: boolean): void;
    export function taskPointAtEntity(ped: number, targetEntity: number, durationMs: number): void;
    export function taskPutPedDirectlyIntoCoverFromCoords(ped: number, x: number, y: number, z: number, fromX: number, fromY: number, fromZ: number, timeout: number, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any, p15: any, p16: any, p17: any): void;
    /** Takes scenario point handle instead of hash */
    export function taskStartScenarioInPlace2(ped: number, p1: any, p2: string, p3: number, p4: boolean, p5: number, p6: boolean): void;
    export function taskThrowProjectile2(p0: any, p1: any, p2: any, p3: any): void;
    export function taskUseNearestScenarioToCoord(ped: number, x: number, y: number, z: number, distance: number, duration: number, p6: boolean, p7: boolean, p8: boolean, p9: boolean): void;
    export function taskUseScenarioPoint2(ped: number, ped2: number, p2: any, p3: string, p4: number, p5: number, p6: number, p7: boolean): void;
    /** Adds a waypoint to an AI vehicle's active drive-to-destination task; only the last 3 points are kept (ignored if no such task). */
    export function taskVehicleAddNextDestination(vehicle: number, x: number, y: number, z: number): void;
    export function taskVehicleDriveToCoord2(ped: number, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any): void;
    /** Tasks vehicle towards owner */
    export function taskVehicleDriveToDestination2(vehicle: number, x: number, y: number, z: number, speed: number, p5: number, p6: number, p7: number, p8: number): void;
    /** Params: p4 = 3.f or 8.f, p5 = 0.25f, p6 = 0 in R* Scripts */
    export function taskVehicleDriveToPoint2(vehicle: number, x: number, y: number, z: number, p4: number, p5: number, p6: any): void;
    /** Vehicle Auto Drive (?) p1/p2/p3: usually 1f, 1f, 0f or 0f, 0f, 0f Speed: usually 8f Types: 1148979456 (task with flee), 1148979587 (dismissing the vehicle) */
    export function taskVehicleFleeOnCleanup(vehicle: number, p1: number, p2: number, p3: number, speed: number, type: number): void;
    export function taskVehicleFollowWaypointRecording2(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any): void;
    /** Returns true if the vehicle's current drive-to task is targeting the given coordinates (i.e., its active destination matches x,y,z). Useful to avoid reissuing TASK_VEHICLE_DRIVE_TO_DESTINATION_2 wh... */
    export function taskVehicleIsHeadingToCoords(vehicle: number, x: number, y: number, z: number): boolean;
    /** Smoothly transitions an active scenario actor (ped) into a specific conditional / clipset defined in the scenario's conditional-anim graph, breaking or restarting the scenario. Returns `true` if th... */
    export function transitionScenarioToConditionalAnim(ped: number, scenarioPoint: number, clipsetDict: string, clipName: string, fromConditionalAnim: string, flags: number): boolean;
    /** In-place update for a running follow-to-offset/go-to task: sets new target coords + local offset, with speed and arrival tolerance (foot or mount; no effect if no compatible task). */
    export function updateTaskGoToCoordWithOffset(ped: number, targetX: number, targetY: number, targetZ: number, offsetX: number, offsetY: number, offsetZ: number, speed: number, tolerance: number): void;

    // TELEMETRY
    export function analyticsPlaytimeFreemodeEnd(): void;
    export function analyticsPlaytimeFreemodeStart(): void;
    export function telemetryCampDonate(transactionId: any, p1: any, p2: any, p3: any, p4: any, slotId: number, p6: number, p7: any, p8: boolean): void;
    export function telemetryPersonalVehicleMount(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryPlayerMenuPin(p0: any, p1: any, p2: any, p3: any): void;
    export function clearTelemetryShopUi(): void;
    export function telemetryAmbientVignette(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any): void;
    export function telemetryAnimalSkinned(type: number, items: any): void;
    export function telemetryBountyTarget(data: any): void;
    export function telemetryCampCreated(p0: any): void;
    export function telemetryCampSupplies(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    export function telemetryCharCreator(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any): void;
    export function telemetryCollect(transactionId: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any): void;
    export function telemetryCoupon(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any): void;
    export function telemetryCraftItem(p0: any, p1: any, p2: any, quantity: any): void;
    /** Works in MP only. */
    export function telemetryCreateUuid(uuid: any): boolean;
    export function telemetryCustom(args: any): void;
    export function telemetryDefensive(p0: any, p1: any, p2: any): void;
    export function telemetryDiscoverable(p0: any): void;
    export function telemetryEmoteAddCategoryToSave(p0: any, p1: any, emote: number): void;
    export function telemetryFastTravel(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    export function telemetryFavorEmote(p0: any, p1: any, p2: any): void;
    export function telemetryGameProgress(p0: any, p1: any): void;
    export function telemetryGangShares(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryGoldStore(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryGunLocker(): void;
    export function telemetryGunLockerWeaponRemoved(p0: number): void;
    export function telemetryGunLockerWeaponStored(p0: number): void;
    export function telemetryHerbPicked(herbType: number): void;
    export function telemetryHonor(p0: any, p1: any): void;
    export function telemetryHubNavigation(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryHubOffers(couponItem: any, p1: any): void;
    export function telemetryIntroSkip(p0: any, p1: any, p2: any): void;
    export function telemetryLobbyProgression(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryLoot(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryMatchNomination(args: any): void;
    export function telemetryMatchOver(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    export function telemetryMatchQueue(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any): void;
    export function telemetryMatchStarted(p0: any, p1: any): void;
    export function telemetryMatchVote(p0: any, p1: any): void;
    export function telemetryMenuNavigation(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryMissionCheckpoint(p0: any, p1: any, p2: any): void;
    /** _TELEMETRY_C* - _TELEMETRY_G* */
    export function telemetryMissionFailedToLaunch(p0: any, p1: any, x: number, y: number, z: number, reason: number): void;
    export function telemetryMissionIloOption(p0: any, p1: any): void;
    export function telemetryMissionOver(p0: any, p1: any): void;
    export function telemetryMissionStarted(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryMoonshineBrew(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any): void;
    export function telemetryNetCamp(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any): void;
    export function telemetryNotoriety(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryParleyFeud(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    export function telemetryPersonalVehicleWagon(p0: any, p1: any, p2: any): void;
    export function telemetryPhoto(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryPhotoCam(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any): void;
    export function telemetryPlayerSpawned(ped: number): void;
    export function telemetryPokerOver(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any): void;
    export function telemetryPrison(transactionId: any, bountyAmount: any, ped: number, completionType: any, jailTimeServed: any, jailTimeLeft: any, posseRole: any): void;
    export function telemetryRegion(regionHash: number): void;
    export function telemetryRoleBounty(p0: any): void;
    export function telemetryRoleCollector(transactionId: any, collectible: any, category: any, p3: any, p4: any, p5: any, p6: any): void;
    export function telemetryRoleMoonshiner(p0: any, transactionId: any): void;
    export function telemetryRoleNaturalist(transactionId: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any): void;
    export function telemetryRoleTokenTransaction(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any): void;
    export function telemetryRoleTrader(p0: any, transactionId: any): void;
    /** Creation of the metric is related to attribute filling, i. e. at camp fires, when the ped is resting. _TELEMETRY_C* - _TELEMETRY_P* */
    export function telemetryRpgGlobalCalculateAttributeCoreDelta(): void;
    export function telemetrySample(transactionId: any, animal: any, p2: any, bSampled: any, bTranq: boolean): void;
    export function telemetrySetIsFlow(toggle: boolean): void;
    export function telemetrySetShopForTransaction(transactionId: number, p1: number, p2: number): void;
    export function telemetryShopCutscene(p0: any, p1: any, p2: any, p3: any): void;
    export function telemetryShopEntry(shopType: any, shopRegion: any, region: any, p3: any, p4: any, p5: any): void;
    export function telemetryShopExit(p0: any, p1: any): void;
    export function telemetryShopPurchase(p0: any, p1: any, p2: any, p3: any, p4: any): void;
    export function telemetryShopSell(p0: any, p1: any, p2: any, p3: any, centSalePrice: number): void;
    export function telemetrySleep(p0: any): void;
    export function telemetryStartGunLockerInteraction(): void;
    /** Returns false when transaction request is failing */
    export function telemetryTriggerTransactionRequest(requestId: any, transactionId: any): boolean;
    export function tryGetTelemetryIdFromTransactionId(transactionId: any, requestId: any): boolean;

    // TXD
    export function doesStreamedTextureDictExist(textureDict: string): boolean;
    export function doesStreamedTxdExist(txdHash: number): boolean;
    export function hasStreamedTextureDictLoaded(textureDict: string): boolean;
    export function hasStreamedTxdLoaded(txdHash: number): boolean;
    export function requestStreamedTextureDict(textureDict: string, p1: boolean): void;
    export function requestStreamedTxd(txdHash: number, p1: boolean): void;
    export function setStreamedTextureDictAsNoLongerNeeded(textureDict: string): void;
    export function setStreamedTxdAsNoLongerNeeded(txdHash: number): void;

    // UIAPPS
    export function canLaunchUiappByHash(appNameHash: number): boolean;
    export function canLaunchUiappByHashWithEntry(appNameHash: number, entryHash: number): boolean;
    export function isAnyUiappActive(): boolean;
    export function isAnyUiappRunning(): boolean;
    export function isUiappActiveByHash(appNameHash: number): boolean;
    export function isUiappRunning(appName: string): boolean;
    export function isUiappRunningByHash(appNameHash: number): boolean;
    export function isUiappTransitioningByHash(appNameHash: number): boolean;
    export function launchUiappByHash(appNameHash: number): number;
    export function launchUiappByHashWithEntry(appNameHash: number, entryHash: number): number;
    export function launchUiappWithEntry(appName: string, entry: string): number;
    export function requestUiappTransitionByHash(appNameHash: number, transitionHash: number): boolean;
    export function closeAllUiapps(): void;
    export function closeAllUiappsImmediate(): void;
    export function closeUiapp(appName: string): void;
    export function closeUiappByHash(appNameHash: number): void;
    export function closeUiappByHashImmediate(appNameHash: number): void;
    export function closeUiappImmediate(appName: string): void;
    export function getUiappCurrentActivityByHash(appNameHash: number): number;

    // UIDEBUG
    /** Note: you must use VAR_STRING */
    export function bgDisplayText(text: string, x: number, y: number): void;
    /** https://github.com/femga/rdr3_discoveries/tree/master/useful_info_from_rpfs/colours */
    export function bgSetTextColor(red: number, green: number, blue: number, alpha: number): void;
    export function bgSetTextScale(scaleX: number, scaleY: number): void;

    // UIEVENTS
    /** eventData: struct UI_SCRIPT_EVENT { 	alignas(8) eUIScriptEventType eventType; // https://alloc8or.re/rdr3/doc/enums/eUIScriptEventType.txt 	alignas(8) int intParam; 	alignas(8) Hash hashParam; 	ali... */
    export function eventsUiGetMessage(hash: number, eventData: any): boolean;
    /** Old name: _EVENT_MANAGER_IS_EVENT_PENDING */
    export function eventsUiIsPending(hash: number): boolean;
    /** eventData: see EVENTS_UI_GET_MESSAGE  Old name: _EVENT_MANAGER_PEEK_EVENT */
    export function eventsUiPeekMessage(hash: number, eventData: any): boolean;
    /** Old name: _EVENT_MANAGER_POP_EVENT */
    export function eventsUiPopMessage(hash: number): void;

    // UIFEED
    /** feedChannel: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eUIFeedChannel */
    export function uiFeedClearChannel(feedChannel: number, p1: boolean, p2: boolean): void;
    /** feedChannel: see UI_FEED_CLEAR_CHANNEL Returns feedMessage */
    export function uiFeedGetCurrentMessage(feedChannel: number): number;
    /** Hides Toast Notifications */
    export function uiFeedClearAllChannels(): void;
    /** Clears help text */
    export function uiFeedClearHelpTextFeed(feedMessage: number, p1: boolean): void;
    /** Returns messageState, see https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eUIMessageState */
    export function uiFeedGetMessageState(feedMessage: number): number;
    /** Display text on right of the screen, Example : https://pastebin.com/n1YmNe25 */
    export function uiFeedPostFeedTicker(p0: any, p1: any, p2: boolean): number;
    export function uiFeedPostGameUpdateShard(p0: any, p1: any, p2: boolean): number;
    /** Example : https://pastebin.com/GvdBp8Dh */
    export function uiFeedPostHelpText(p0: any, p1: any, p2: boolean): number;
    /** Example : https://pastebin.com/h1YzycuR */
    export function uiFeedPostLocationShard(duration: any, data: any, p2: boolean, p3: boolean): number;
    export function uiFeedPostMissionName(p0: any, p1: any, p2: boolean): number;
    /** Example : https://pastebin.com/13tuRa63 */
    export function uiFeedPostObjective(p0: any, p1: any, p2: boolean): number;
    export function uiFeedPostOneTextShard(p0: any, p1: any, p2: boolean): number;
    export function uiFeedPostRankupToast(p0: any, p1: any, p2: number, p3: number): number;
    export function uiFeedPostReticleMessage(p0: any, p1: any, p2: boolean): number;
    /** Example : https://pastebin.com/kAtEMQTD */
    export function uiFeedPostSampleNotification(p0: any, p1: any, p2: number, p3: number): number;
    /** Example : https://pastebin.com/YZMBkAmW */
    export function uiFeedPostSampleToast(p0: any, p1: any, p2: boolean, p3: boolean): number;
    export function uiFeedPostSampleToastRight(p0: any, p1: any, p2: boolean): number;
    export function uiFeedPostSampleToastWithAppLink(p0: any, p1: any, p2: boolean, p3: boolean, p4: boolean): number;
    export function uiFeedPostThreeTextShard(p0: any, p1: any, p2: boolean, p3: boolean, p4: boolean): number;
    export function uiFeedPostTwoTextShard(p0: any, p1: any, p2: boolean, p3: boolean): number;
    export function uiFeedPostVoiceChatFeed(p0: any, p1: any, p2: boolean): number;

    // UILOG
    export function uilogAddEntryHash(p0: number, p1: number, x: number, y: number, z: number, p5: number, p6: number, p7: any): void;
    export function uilogAddItemToTaskList(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any): void;
    export function uilogAddOrUpdateObjective(p0: number, p1: number, p2: number, p3: string, p4: boolean, p5: boolean, p6: boolean): void;
    export function uilogAddTotalTakeEntry(p0: number, p1: number, p2: string, p3: string, p4: number): void;
    export function uilogClearAllEntries(): void;
    export function uilogClearCachedObjective(): void;
    export function uilogClearHasDisplayedCachedObjective(): void;
    export function uilogGetCachedObjective(): NativeString;
    export function uilogHasDisplayedCachedObjective(): boolean;
    export function uilogIsEntryRegistered(p0: number, p1: number): boolean;
    export function uilogMarkAllEntriesAvailability(p0: number, p1: string): void;
    export function uilogMarkEntryAvailability(p0: number, p1: number, p2: number, p3: string): void;
    export function uilogMarkMissionCompleted(p0: number): void;
    export function uilogPostNotification(data: any): number;
    export function uilogPrintCachedObjective(): void;
    export function uilogRemoveEntry(p0: number, p1: number): void;
    export function uilogSetCachedObjective(p0: string): void;
    export function uilogSetDisplayCompletionRating(logEntryType: number, p1: number, p2: boolean): void;
    export function uilogSetEntryBriefTexture(p0: number, p1: number, texture: number, textureDictionary: number): void;
    export function uilogSetEntryIconTexture(p0: number, p1: number, icon: number, iconDictionary: number): void;
    export function uilogSetEntryPinned(p0: number, p1: number, p2: boolean): void;
    export function uilogSetHasDisplayedCachedObjective(): void;
    export function uilogSetPendingDetailsId(p0: number, p1: number): any;
    export function uilogSetTotalTakeSummary(p0: string, p1: string): void;
    export function uilogUpdateEntrySubheader(p0: number, p1: number, p2: string): void;

    // UIPINNING
    export function uipinningGetTooltipText(hash: number): NativeString;

    // UISTATEMACHINE
    export function uiStateMachineCanRequestTransition(p0: any): any;
    export function uiStateMachineCreate(p0: any, p1: any): any;
    export function uiStateMachineDestroy(p0: any): void;
    export function uiStateMachineDestroyAndClear(p0: any): void;
    export function uiStateMachineExists(p0: any): any;
    export function uiStateMachineRequestExit(p0: any, p1: any): void;
    export function uiStateMachineRequestTransition(p0: any, p1: any): any;
    export function uiflowblockEnter(p0: any, p1: any): any;
    export function uiflowblockIsLoaded(p0: any): any;
    export function uiflowblockRelease(p0: any): void;
    export function uiflowblockRequest(p0: any): any;
    /** It's either EXITED or EXITING */
    export function uiStateMachineIsExited(p0: number): boolean;

    // UISTICKYFEED
    export function uiStickyFeedClearMessage(msgId: number): void;
    /** Example: https://pastebin.com/JygJShNU */
    export function uiStickyFeedCreateDeathFailMessage(p0: any, p1: any, p2: boolean): number;
    /** Example: https://pastebin.com/EJD7ytnz */
    export function uiStickyFeedCreateErrorMessage(p0: any, p1: any, p2: boolean): number;
    /** Example: https://pastebin.com/6mLtee2S */
    export function uiStickyFeedCreateWarningMessage(p0: any, p1: any, p2: boolean): number;
    /** Returns state of sticky feed message, see 0x59FA676177DBE4C9 */
    export function uiStickyFeedGetMessageState(msgId: number): number;
    export function uiStickyFeedIsAlertScreenActive(): boolean;
    /** stickyFeedChannel: https://github.com/Halen84/RDR3-Native-Flags-And-Enums/tree/main/eUIStickyFeedChannel */
    export function uiStickyFeedIsChannelActive(stickyFeedChannel: number): boolean;
    /** Seems to only update _UI_STICKY_FEED_CREATE_ERROR_MESSAGE(0x9F2CC2439A04E7BA) and _UI_STICKY_FEED_CREATE_DEATH_FAIL_MESSAGE(0x815C4065AE6E6071) message. Example: https://pastebin.com/nDrJyWq2 */
    export function uiStickyFeedUpdateMessage(msgId: number, p1: any, p2: boolean): void;

    // UITUTORIAL
    export function uitutorialGetIsThreatIndicatorCapableRadarShown(): boolean;
    export function uitutorialGetIsThreatIndicatorOn(): boolean;
    /** enum eRpgIcons { 	ICON_STAMINA, 	ICON_STAMINA_CORE, 	ICON_DEADEYE, 	ICON_DEADEYE_CORE, 	ICON_HEALTH, 	ICON_HEALTH_CORE, 	ICON_HORSE_HEALTH, 	ICON_HORSE_HEALTH_CORE, 	ICON_HORSE_STAMINA, 	ICON_HORSE... */
    export function uitutorialSetRpgIconVisibility(rpgIcon: number, visibility: number): void;

    // UNLOCK
    export function unlockIsUnlocked(unlockHash: number): boolean;
    export function unlockIsVisible(unlockHash: number): boolean;
    export function unlockSetUnlocked(unlockHash: number, toggle: boolean): void;
    export function unlockSetVisible(unlockHash: number, toggle: boolean): void;
    export function unlockGetItemRoleUnlockInfo(unlockHash: number, outData: any): void;
    export function unlockIsLootable(unlockHash: number): boolean;
    export function unlockIsNew(unlockHash: number): boolean;
    export function unlockIsUnlockFlagSet(unlockHash: number, flag: number): boolean;
    export function unlockSetNew(unlockHash: number, toggle: boolean): void;

    // VEHICLE
    export function addRoadNodeSpeedZone(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any): number;
    /** Returns false if every seat is occupied. */
    export function areAnyVehicleSeatsFree(vehicle: number): boolean;
    /** This native makes the vehicle stop immediately  distance defines how far it will travel until stopping. */
    export function bringVehicleToHalt(vehicle: number, distance: number, duration: number, unknown: boolean): void;
    export function canAnchorBoatHere(vehicle: number): boolean;
    /** seatIndex: see CREATE_PED_INSIDE_VEHICLE */
    export function canShuffleSeat(vehicle: number, seatIndex: number): boolean;
    export function clearLastDrivenVehicle(): void;
    /** Copies sourceVehicle's damage (broken bumpers, broken lights, etc.) to targetVehicle. */
    export function copyVehicleDamages(sourceVehicle: number, targetVehicle: number): void;
    export function createVehicle(modelHash: number, x: number, y: number, z: number, heading: number, isNetwork: boolean, bScriptHostVeh: boolean, bDontAutoCreateDraftAnimals: boolean, p8: boolean): number;
    export function deleteAllTrains(): void;
    export function deleteMissionTrain(train: number): void;
    /** Deletes a vehicle. The vehicle must be a mission entity to delete, so call this before deleting: SET_ENTITY_AS_MISSION_ENTITY(vehicle, true, true);  eg how to use: SET_ENTITY_AS_MISSION_ENTITY(vehi... */
    export function deleteVehicle(vehicle: number): void;
    export function disableVehicleWeapon(disabled: boolean, weaponHash: number, vehicle: number, owner: number): void;
    export function doesExtraExist(vehicle: number, extraId: number): boolean;
    /** Explodes a selected vehicle.  Vehicle vehicle = Vehicle you want to explode. BOOL isAudible = If explosion makes a sound. BOOL isInvisible = If the explosion is invisible or not.  First BOOL does n... */
    export function explodeVehicle(vehicle: number, isAudible: boolean, isInvisible: boolean, p3: any, p4: any): void;
    /** Often called after START_PLAYBACK_RECORDED_VEHICLE and SKIP_TIME_IN_PLAYBACK_RECORDED_VEHICLE; similar in use to FORCE_ENTITY_AI_AND_ANIMATION_UPDATE. */
    export function forcePlaybackRecordedVehicleUpdate(vehicle: number, p1: boolean): void;
    export function getClosestVehicle(x: number, y: number, z: number, radius: number, modelHash: number, flags: number): number;
    /** Returns p1 for 0xBA958F68031DDBFC (stationIndex) */
    export function getCurrentStationForTrain(train: number): number;
    export function getDraftAnimalCount(vehicle: number, expected: number, actual: number): boolean;
    export function getDriverOfVehicle(vehicle: number): number;
    export function getLastDrivenVehicle(): number;
    /** seatIndex: see CREATE_PED_INSIDE_VEHICLE */
    export function getLastPedInVehicleSeat(vehicle: number, seatIndex: number): number;
    /** seatIndex: see CREATE_PED_INSIDE_VEHICLE */
    export function getPedInVehicleSeat(vehicle: number, seatIndex: number): number;
    /** This native does no interpolation between pathpoints. The same position will be returned for all times up to the next pathpoint in the recording.  See REQUEST_VEHICLE_RECORDING */
    export function getPositionOfVehicleRecordingAtTime(recording: number, time: number, script: string): Vector3;
    /** This native does no interpolation between pathpoints. The same rotation will be returned for all times up to the next pathpoint in the recording.  See REQUEST_VEHICLE_RECORDING */
    export function getRotationOfVehicleRecordingAtTime(recording: number, time: number, script: string): Vector3;
    export function getTimePositionInRecording(vehicle: number): number;
    export function getTrackIndexOfTrain(train: number): number;
    export function getTrainCarriage(train: number, trailerNumber: number): number;
    /** Seems related to vehicle health, like the one in IV. Max 1000, min 0. Vehicle does not necessarily explode or become undrivable at 0. */
    export function getVehicleBodyHealth(vehicle: number): number;
    export function getVehicleDoorsLockedForPlayer(vehicle: number, player: number): boolean;
    export function getVehicleDoorLockStatus(vehicle: number): number;
    /** Returns 1000.0 if the function is unable to get the address of the specified vehicle or if it's not a vehicle.  Minimum: -4000 Maximum: 1000  -4000: Engine is destroyed 0 and below: Engine catches ... */
    export function getVehicleEngineHealth(vehicle: number): number;
    export function getVehicleEstimatedMaxSpeed(vehicle: number): number;
    export function getVehicleMaxNumberOfPassengers(vehicle: number): number;
    export function getVehicleModelNumberOfSeats(modelHash: number): number;
    /** Gets the number of passengers, NOT including the driver. Use IS_VEHICLE_SEAT_FREE(Vehicle, -1) to also check for the driver */
    export function getVehicleNumberOfPassengers(vehicle: number): number;
    /** 1000 is max health */
    export function getVehiclePetrolTankHealth(vehicle: number): number;
    /** Gets the trailer of a vehicle and puts it into the trailer parameter. */
    export function getVehicleTrailerVehicle(vehicle: number, trailer: number): boolean;
    export function hasInstantFillVehiclePopulationFinished(): boolean;
    export function hasVehicleAssetLoaded(vehicleAsset: number): boolean;
    /** See REQUEST_VEHICLE_RECORDING */
    export function hasVehicleRecordingBeenLoaded(recording: number, script: string): boolean;
    export function instantlyFillVehiclePopulation(): void;
    export function isAnyVehicleNearPoint(x: number, y: number, z: number, radius: number): boolean;
    export function isDraftVehicle(vehicle: number): boolean;
    export function isEntryPointForSeatClear(ped: number, vehicle: number, seatIndex: number, side: boolean, onEnter: boolean): boolean;
    export function isPlaybackGoingOnForVehicle(vehicle: number): boolean;
    export function isPlaybackUsingAiGoingOnForVehicle(vehicle: number): boolean;
    /** seatIndex: see CREATE_PED_INSIDE_VEHICLE */
    export function isSeatWarpOnly(vehicle: number, seatIndex: number): boolean;
    export function isThisModelABoat(model: number): boolean;
    export function isThisModelATrain(model: number): boolean;
    export function isTrainWaitingAtStation(train: number): boolean;
    /** doorId: see SET_VEHICLE_DOOR_SHUT */
    export function isVehicleDoorFullyOpen(vehicle: number, doorId: number): boolean;
    export function isVehicleDriveable(vehicle: number, p1: boolean, p2: boolean): boolean;
    export function isVehicleExtraTurnedOn(vehicle: number, extraId: number): boolean;
    export function isVehicleInBurnout(vehicle: number): boolean;
    export function isVehicleModel(vehicle: number, model: number): boolean;
    export function isVehicleOnAllWheels(vehicle: number): boolean;
    /** seatIndex: see CREATE_PED_INSIDE_VEHICLE Use GET_VEHICLE_MAX_NUMBER_OF_PASSENGERS(vehicle) - 1 for last seat index. */
    export function isVehicleSeatFree(vehicle: number, seatIndex: number): boolean;
    /** Returns true if the vehicle's current speed is less than, or equal to 0.0025f.  For some vehicles it returns true if the current speed is <= 0.00039999999. */
    export function isVehicleStopped(vehicle: number): boolean;
    /** VEH_STUCK_ON_ROOF = 0, VEH_STUCK_ON_SIDE, VEH_STUCK_HUNG_UP, VEH_STUCK_JAMMED */
    export function isVehicleStuckTimerUp(vehicle: number, stuckType: number, ms: number): boolean;
    /** Requires a visibility tracker on the vehicle (TRACK_VEHICLE_VISIBILITY) */
    export function isVehicleVisible(vehicle: number): boolean;
    export function isVehicleWindowIntact(vehicle: number, windowIndex: number): boolean;
    export function isVehicleWrecked(vehicle: number): boolean;
    export function lockDoorsWhenNoLongerNeeded(vehicle: number): void;
    export function modifyVehicleTopSpeed(vehicle: number, value: number): void;
    export function removeRoadNodeSpeedZone(speedzone: number): boolean;
    export function removeVehiclesFromGeneratorsInArea(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any): void;
    export function removeVehicleAsset(vehicleAsset: number): void;
    /** See REQUEST_VEHICLE_RECORDING */
    export function removeVehicleRecording(p0: any, p1: any): void;
    /** windowIndex: 0 = Front Right Window 1 = Front Left Window 2 = Back Right Window 3 = Back Left Window */
    export function removeVehicleWindow(vehicle: number, windowIndex: number): void;
    export function requestVehicleAsset(vehicleHash: number, vehicleAsset: number): void;
    export function requestVehicleHighDetailModel(vehicle: number): void;
    /** Request the vehicle recording defined by the lowercase format string "%s%03d.yvr". For example, REQUEST_VEHICLE_RECORDING(1, "FBIs1UBER") corresponds to fbis1uber001.yvr. For all vehicle recording/... */
    export function requestVehicleRecording(recording: number, script: string): void;
    export function resetVehicleStuckTimer(vehicle: number, nullAttributes: number): void;
    export function setAllowVehicleExplodesOnContact(vehicle: number, p1: boolean): void;
    export function setAllVehicleGeneratorsActive(): void;
    export function setAllVehicleGeneratorsActiveInArea(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, p6: boolean, p7: boolean): void;
    export function setBoatAnchor(vehicle: number, toggle: boolean): void;
    /** Value: mostly 99999.9f  Old name: _SET_BOAT_MOVEMENT_RESISTANCE */
    export function setBoatLowLodAnchorDistance(vehicle: number, value: number): void;
    /** Old name: _SET_BOAT_FROZEN_WHEN_ANCHORED */
    export function setBoatRemainsAnchoredWhilePlayerIsDriver(vehicle: number, p1: boolean, p2: boolean): void;
    export function setBoatSinksWhenWrecked(vehicle: number, toggle: boolean): void;
    export function setBreakableVehicleLocksUnbreakable(vehicle: number, toggle: boolean): void;
    /** nullsub, doesn't do anything */
    export function setDisableRandomTrainsThisFrame(toggle: boolean): void;
    /** Old name: _SET_DISABLE_SUPERDUMMY_MODE */
    export function setDisableSuperdummy(vehicle: number, disable: boolean): void;
    export function setDisableVehicleEngineFires(vehicle: number, p1: boolean): void;
    export function setDisableVehiclePetrolTankDamage(vehicle: number, toggle: boolean): void;
    export function setDisableVehiclePetrolTankFires(vehicle: number, toggle: boolean): void;
    export function setDontAllowPlayerToEnterVehicleIfLockedForPlayer(vehicle: number, p1: boolean): void;
    /** doorId: see SET_VEHICLE_DOOR_SHUT  Old name: _SET_VEHICLE_DOOR_CAN_BREAK */
    export function setDoorAllowedToBeBrokenOff(vehicle: number, doorId: number, isBreakable: boolean): void;
    export function setEnableVehicleSlipstreaming(p0: boolean): void;
    export function setForceHdVehicle(vehicle: number, toggle: boolean): void;
    /** Sets boat to be anchored on spawn, called together with SET_BOAT_ANCHOR and _SET_BOAT_ANCHOR_BUOYANCY_COEFFICIENT */
    export function setForceLowLodAnchorMode(vehicle: number, p1: boolean): void;
    export function setForceVehicleEngineDamageByBullet(vehicle: number, toggle: boolean): void;
    /** flags = 0: DEFAULT; 1: KEEP_OLD_SPEED */
    export function setMissionTrainAsNoLongerNeeded(train: number, flags: number): void;
    export function setMissionTrainCoords(train: number, x: number, y: number, z: number): void;
    export function setParkedVehicleDensityMultiplierThisFrame(multiplier: number): void;
    export function setPedOwnsVehicle(ped: number, vehicle: number): void;
    export function setPlaybackSpeed(vehicle: number, speed: number): void;
    export function setRandomBoats(toggle: boolean): void;
    export function setRandomTrains(toggle: boolean): void;
    export function setRandomVehicleDensityMultiplierThisFrame(multiplier: number): void;
    export function setTrainCruiseSpeed(train: number, speed: number): void;
    export function setTrainOffsetFromStation(train: number, offset: number): void;
    export function setTrainSpeed(train: number, speed: number): void;
    /** Used to be incorrectly named SET_VEHICLE_EXCLUSIVE_DRIVER */
    export function setVehicleAiCanUseExclusiveSeats(vehicle: number, toggle: boolean): void;
    export function setVehicleAllowHomingMissleLockon(vehicle: number, toggle: boolean): void;
    /** Makes the vehicle accept no passengers. */
    export function setVehicleAllowNoPassengersLockon(vehicle: number, toggle: boolean): void;
    export function setVehicleAutomaticallyAttaches(vehicle: number, p1: boolean, p2: any): any;
    export function setVehicleBodyHealth(vehicle: number, value: number): void;
    export function setVehicleBrokenPartsDontAffectAiHandling(vehicle: number, p1: boolean): void;
    export function setVehicleCanBeTargetted(vehicle: number, state: boolean): void;
    export function setVehicleCanBeUsedByFleeingPeds(vehicle: number, toggle: boolean): void;
    export function setVehicleCanBeVisiblyDamaged(vehicle: number, state: boolean): void;
    export function setVehicleCanBreak(vehicle: number, toggle: boolean): void;
    export function setVehicleCanEjectPassengersIfLocked(vehicle: number, p1: boolean): void;
    /** Apply damage to vehicle at a location. Location is relative to vehicle model (not world).  Radius of effect damage applied in a sphere at impact location */
    export function setVehicleDamage(vehicle: number, xOffset: number, yOffset: number, zOffset: number, damage: number, radius: number, p6: boolean): void;
    export function setVehicleDensityMultiplierThisFrame(multiplier: number): void;
    export function setVehicleDirtLevel(vehicle: number, dirtLevel: number): void;
    export function setVehicleDoorsLocked(vehicle: number, doorLockStatus: number): void;
    export function setVehicleDoorsLockedForAllPlayers(vehicle: number, toggle: boolean): void;
    export function setVehicleDoorsLockedForPlayer(vehicle: number, player: number, toggle: boolean): void;
    export function setVehicleDoorsLockedForTeam(vehicle: number, team: number, toggle: boolean): void;
    /** Closes all doors of a vehicle: */
    export function setVehicleDoorsShut(vehicle: number, closeInstantly: boolean): void;
    export function setVehicleDoorsToOpenAtAnyDistance(vehicle: number, toggle: boolean): void;
    /** doorId: see SET_VEHICLE_DOOR_SHUT */
    export function setVehicleDoorBroken(vehicle: number, doorId: number, deleteDoor: boolean): void;
    /** doorId: see SET_VEHICLE_DOOR_SHUT */
    export function setVehicleDoorControl(vehicle: number, doorId: number, speed: number, angle: number): void;
    /** doorId: see SET_VEHICLE_DOOR_SHUT */
    export function setVehicleDoorLatched(vehicle: number, doorId: number, p2: boolean, p3: boolean, p4: boolean): void;
    /** doorId: see SET_VEHICLE_DOOR_SHUT Can also be used on trains and its wagons */
    export function setVehicleDoorOpen(vehicle: number, doorId: number, loose: boolean, openInstantly: boolean): void;
    /** doorId: enum eDoorId { 	VEH_EXT_DOOR_INVALID_ID = -1, 	VEH_EXT_DOOR_DSIDE_F, 	VEH_EXT_DOOR_DSIDE_M, 	VEH_EXT_DOOR_DSIDE_M1, 	VEH_EXT_DOOR_DSIDE_M2, 	VEH_EXT_DOOR_DSIDE_R, 	VEH_EXT_DOOR_PSIDE_F, 	VE... */
    export function setVehicleDoorShut(vehicle: number, doorId: number, closeInstantly: boolean): void;
    export function setVehicleEngineCanDegrade(vehicle: number, toggle: boolean): void;
    /** 1000 is max health Begins leaking gas at around 650 health -999.90002441406 appears to be minimum health, although nothing special occurs <- false statement  ------------------------- Minimum: -400... */
    export function setVehicleEngineHealth(vehicle: number, health: number): void;
    /** Starts or stops the engine on the specified vehicle.  vehicle: The vehicle to start or stop the engine on. value: true to turn the vehicle on; false to turn it off. instantly: if true, the vehicle ... */
    export function setVehicleEngineOn(vehicle: number, value: boolean, instantly: boolean): void;
    /** index: 0 - 1  Used to be incorrectly named _SET_VEHICLE_EXCLUSIVE_DRIVER_2 */
    export function setVehicleExclusiveDriver(vehicle: number, ped: number, index: number): void;
    /** Sets a vehicle to be strongly resistant to explosions. p0 is the vehicle; set p1 to false to toggle the effect on/off. */
    export function setVehicleExplodesOnHighExplosionDamage(vehicle: number, toggle: boolean): void;
    /** Note: only some vehicle have extras https://github.com/femga/rdr3_discoveries/blob/master/vehicles/vehicle_modding/vehicle_extras.lua */
    export function setVehicleExtra(vehicle: number, extraId: number, disable: boolean): void;
    /** This fixes a vehicle. If the vehicle's engine's broken then you cannot fix it with this native. */
    export function setVehicleFixed(vehicle: number): void;
    export function setVehicleForwardSpeed(vehicle: number, speed: number): void;
    export function setVehicleHandbrake(vehicle: number, toggle: boolean): void;
    export function setVehicleHasBeenOwnedByPlayer(vehicle: number, owned: boolean): void;
    /** if true, axles won't bend. */
    export function setVehicleHasStrongAxles(vehicle: number, toggle: boolean): void;
    export function setVehicleHasUnbreakableLights(vehicle: number, p1: boolean): void;
    export function setVehicleInactiveDuringPlayback(vehicle: number, toggle: boolean): void;
    /** doorId: see SET_VEHICLE_DOOR_SHUT */
    export function setVehicleIndividualDoorsLocked(vehicle: number, doorId: number, doorLockStatus: number): void;
    export function setVehicleInfluencesWantedLevel(vehicle: number, toggle: boolean): void;
    /** Setting this to false, makes the specified vehicle to where if you press Y your character doesn't even attempt the animation to enter the vehicle. Hence it's not considered aka ignored. */
    export function setVehicleIsConsideredByPlayer(vehicle: number, toggle: boolean): void;
    export function setVehicleIsStolen(vehicle: number, isStolen: boolean): void;
    export function setVehicleKeepEngineOnWhenAbandoned(vehicle: number, toggle: boolean): void;
    /** Sets the vehicle's lights state. */
    export function setVehicleLights(vehicle: number, state: number): void;
    /** _SET_VEHICLE_LI* */
    export function setVehicleLimitSpeedWhenPlayerInactive(vehicle: number, p1: boolean): void;
    export function setVehicleLodMultiplier(vehicle: number, multiplier: number): void;
    export function setVehicleMayBeUsedByGotoPointAnyMeans(vehicle: number, p1: boolean): void;
    export function setVehicleNotStealableAmbiently(vehicle: number, p1: boolean): void;
    export function setVehicleOnGroundProperly(vehicle: number, p1: boolean): boolean;
    /** 1000 is max health */
    export function setVehiclePetrolTankHealth(vehicle: number, health: number): void;
    export function setVehicleProvidesCover(vehicle: number, toggle: boolean): void;
    export function setVehicleRespectsLocksWhenHasDriver(vehicle: number, p1: boolean): void;
    export function setVehicleShootAtTarget(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any): void;
    export function setVehicleStaysFrozenWhenCleanedUp(vehicle: number, toggle: boolean): void;
    /** Locks the vehicle's steering to the desired angle, explained below.  Requires to be called onTick. Steering is unlocked the moment the function stops being called on the vehicle.  Steer bias: -1.0 ... */
    export function setVehicleSteerBias(vehicle: number, value: number): void;
    export function setVehicleStopInstantlyWhenPlayerInactive(vehicle: number, p1: boolean): void;
    /** If set to true, vehicle will not take crash damage, but is still susceptible to damage from bullets and explosives */
    export function setVehicleStrong(vehicle: number, toggle: boolean): void;
    /** Allows you to toggle bulletproof tires. */
    export function setVehicleTyresCanBurst(vehicle: number, toggle: boolean): void;
    export function setVehicleUndriveable(vehicle: number, toggle: boolean): void;
    export function setVehicleWheelsCanBreak(vehicle: number, enabled: boolean): void;
    export function setVehicleWheelsCanBreakOffWhenBlowUp(vehicle: number, toggle: boolean): void;
    /** SET_TIME_POSITION_IN_RECORDING can be emulated by: desired_time - GET_TIME_POSITION_IN_RECORDING(vehicle) */
    export function skipTimeInPlaybackRecordedVehicle(vehicle: number, time: number): void;
    /** p3 is some flag related to 'trailers' (invokes CVehicle::GetTrailer).  See REQUEST_VEHICLE_RECORDING */
    export function startPlaybackRecordedVehicle(vehicle: number, recording: number, script: string, p3: boolean): void;
    /** Sounds the horn for the specified vehicle.  vehicle: The vehicle to activate the horn for. mode: The hash of "NORMAL" or "HELDDOWN". Can be 0. duration: The duration to sound the horn, in milliseco... */
    export function startVehicleHorn(vehicle: number, duration: number, mode: number, forever: boolean): void;
    /** Old name: _STOP_BRING_VEHICLE_TO_HALT */
    export function stopBringingVehicleToHalt(vehicle: number): void;
    export function stopPlaybackRecordedVehicle(vehicle: number): void;
    export function trackVehicleVisibility(vehicle: number): void;
    export function addTrainTemporaryStop(train: number, trackIndex: number, x: number, y: number, z: number): void;
    /** Returns true if any wheel is destroyed IS_VEHICLE_DRIVEABLE will still return true even though a wheel is destroyed, like vehicles with 4 wheels. */
    export function areAnyVehicleWheelsDestroyed(vehicle: number): boolean;
    export function attachDraftVehicleHarnessPed(mount: number, draft: number, harnessId: number): boolean;
    /** Params: destroyingForce is usually 100f in R* Scripts Similar to 0xD4F5EFB55769D272, _A* */
    export function breakOffDraftWheel(vehicle: number, wheelIndex: number, destroyingForce: number): void;
    /** wheelIndex 0: left, wheelIndex 1: right, 4 & 5: unknown */
    export function breakOffVehicleWheel(vehicle: number, wheelIndex: number): number;
    /** Only used to break draft vehicle log straps. Coords is always equal to the vehicle coords. */
    export function breakVehicleStraps(vehicle: number, x: number, y: number, z: number): void;
    /** Identical to CREATE_VEHICLE but allows to set draftAnimalPopGroup (see popgroups.#mt for DRAFT_HORSES_*) */
    export function createDraftVehicle(modelHash: number, x: number, y: number, z: number, heading: number, isNetwork: boolean, bScriptHostVeh: boolean, bDontAutoCreateDraftAnimals: boolean, draftAnimalPopGroup: number, p9: boolean): number;
    /** configHash: https://alloc8or.re/rdr3/doc/enums/eTrainConfig.txt For more information, see trainconfigs.ymt To make the train AI controlled, set conductor to true and set the speed once. */
    export function createMissionTrain(configHash: number, x: number, y: number, z: number, direction: boolean, passengers: boolean, p6: boolean, conductor: boolean): number;
    /** Spawn without lanterns set */
    export function deleteVehicleLanterns(vehicle: number): boolean;
    export function detachDraftVehicleHarnessFromIndex(draft: number, harnessId: number): boolean;
    export function detachDraftVehicleHarnessPed(draft: number, ped: number): boolean;
    /** Only used in train_robbery4 R* Script _C* - _DEL* */
    export function detachWagonEntityFromTrain(entity: number): void;
    export function doesTrainExistOnTrack(trackIndex: number): boolean;
    export function fadeAndDestroyVehicle(vehicle: number): void;
    /** Collects all passenger peds (excluding the driver) from the specified wagon-type vehicle (train wagon) and appends them to itemSet as indexed items. Returns the number of passengers added (0 if non... */
    export function getAllWagonPassengers(wagon: number, itemSet: number): number;
    /** Returns the balloon OBJECT entity attached to a hot air balloon vehicle.  Returns 0 if: 	- vehicle is not a hot air balloon type, or 	- the balloon object is not present.  Notes: 	- Returned handle... */
    export function getBalloonObjectFromVehicle(vehicle: number): number;
    export function getBreakableVehicleLocksState(vehicle: number): number;
    export function getBreakableVehicleLockObject(vehicle: number, index: number): number;
    export function getCheckpointTrainSpawnLocation(trackIndex: number, x: number, y: number, z: number, distance: number, direction: boolean): Vector3;
    /** Returns p0 for 0xBA958F68031DDBFC (trackIndex) */
    export function getCurrentTrackForTrain(train: number): number;
    /** Returns rage::NumericLimits<float>::kMax (3.402823466e+38) if vehicle is not a valid vehicle of type VEHICLE_TYPE_DRAFT. */
    export function getDraftVehicleDesiredSpeed(vehicle: number): number;
    /** Returns the world coordinates of a junction node for the given train track configuration. trainTrack: see 0x09034479E6E3E269. */
    export function getJunctionCoordsForTrainTrack(trainTrack: number, junctionIndex: number): Vector3;
    export function getNearestTrainTrackPosition(x: number, y: number, z: number): Vector3;
    export function getNumBreakableVehicleLockObjects(vehicle: number): number;
    /** Returns amount for CAN_REGISTER_MISSION_VEHICLES */
    export function getNumCarsFromTrainConfig(trainConfig: number): number;
    /** Returns number of horses a wagon can have */
    export function getNumDraftVehicleHarnessPed(modelHash: number): number;
    /** Return the number of logs on a draft vehicle. Video demo: https://imgur.com/a/5JEeOij */
    export function getNumDraftVehicleLogs(vehicle: number): number;
    /** Return the number of straps that hold the logs of a draft vehicle. Video demo: https://imgur.com/a/5JEeOij */
    export function getNumDraftVehicleStraps(vehicle: number): number;
    /** enum eDraftHarness { 	DRAFT_HARNESS_LR, 	DRAFT_HARNESS_RR, 	DRAFT_HARNESS_LM, 	DRAFT_HARNESS_RM, 	DRAFT_HARNESS_LF, 	DRAFT_HARNESS_RF, 	DRAFT_HARNESS_COUNT }; */
    export function getPedInDraftHarness(vehicle: number, harnessId: number): number;
    /** Returns handles of boat paddles entities. */
    export function getRowingOars(vehicle: number, left: number, right: number): void;
    /** Returns the station hash for a given train track and station index. Notes: - trackIndex is typically 0..24 and stationIndex 0..7. - Returns 0 if the pair is invalid/out of range. */
    export function getStationAtIndex(trackIndex: number, stationIndex: number): number;
    /** Returns Coords of vStation p0 - NET_TRAIN_MANAGER_GET_TRAIN_STATION_DATA _GET_P* - _GET_T* */
    export function getStationCoordsFromTrainStationData(trackIndex: number, stationIndex: number): Vector3;
    /** Requires a visibility tracker on the vehicle (TRACK_VEHICLE_VISIBILITY) */
    export function getTrackAmountOfVisiblePixels(vehicle: number): number;
    /** Returns trackIndex _E* - _F* */
    export function getTrackIndexFromCoords(x: number, y: number, z: number): number;
    /** Returns train car, use GET_TRAIN_CARRIAGE when trailerNumber is bigger than 0 */
    export function getTrainCar(train: number): number;
    /** Returns iNumCars - to be used with GET_TRAIN_CARRIAGE (trailerNumber) _C* (O, P, Q, R) */
    export function getTrainCarriageTrailerNumber(train: number): number;
    export function getTrainDirection(train: number): boolean;
    /** https://i.imgur.com/1rHibjW.jpg */
    export function getTrainDirectionFromIndex(trackIndex: number): boolean;
    /** Returns modelHash */
    export function getTrainModelFromTrainConfigByCarIndex(trainConfig: number, trainCarIndex: number): number;
    export function getTrainPositionOnTrack(trackIndex: number): Vector3;
    /** Returns trackIndex */
    export function getTrainTrackFromTrainVehicle(train: number): number;
    /** Outputs junctionIndex, to be used with 0xE6C5E2125EB210C1. trainTrack: see 0x09034479E6E3E269. */
    export function getTrainTrackJunctionAtCoords(trainTrack: number, x: number, y: number, z: number, junctionIndex: number): boolean;
    /** Returns train */
    export function getTrainVehicleFromTrackIndex(trackIndex: number): number;
    export function getVehicleDoorsLockedForTeam(vehicle: number, team: number): boolean;
    export function getVehicleIsPropSetApplied(vehicle: number): boolean;
    export function getVehicleLivery(vehicle: number): number;
    export function getVehicleOwner(vehicle: number): number;
    export function getVehicleTint(vehicle: number): number;
    export function getVehicleTurretSeat(vehicle: number, seatIndex: number): boolean;
    export function hasTrainLoaded(train: number): boolean;
    export function hideHorseReins(vehicle: number): void;
    export function isBoatGrounded(vehicle: number): boolean;
    export function isPedExclusiveDriverOfVehicle(ped: number, vehicle: number, outIndex: number): boolean;
    export function isThisModelADraftVehicle(model: number): boolean;
    /** Only returns true if BRING_VEHICLE_TO_HALT is called on vehicle beforehand */
    export function isVehicleBroughtToHalt(vehicle: number): boolean;
    /** doorId: see SET_VEHICLE_DOOR_SHUT */
    export function isVehicleDoorBroken(vehicle: number, doorId: number): boolean;
    export function isVehicleFadingOut(vehicle: number): boolean;
    export function isVehicleOnFire(vehicle: number): boolean;
    export function isVehicleWheelDestroyed(vehicle: number, wheel: number): boolean;
    /** Returns the log prop entity that is currently detaching/falling from a draft (log) wagon. Returns 0 if no log is in the falling phase. R* scripts call this repeatedly to fetch each fallen piece, th... */
    export function recoverDraftVehicleFallingLog(vehicle: number): number;
    export function requestVehicleAssetAnims(ped: number, entity: number, vehicleAsset: number): void;
    /** Outputs track hash and junction index on given train vehicle handle. trainTrack: https://pastebin.com/mhy0dTXs */
    export function returnTrainInfoFromHandle(train: number, trainTrack: number, junctionIndex: number): boolean;
    export function setAllJunctionsCleared(): void;
    export function setAllVehicleGeneratorsDisabledForVolume(volume: number, toggle: boolean): void;
    /** Params: 1.0f will make balloon hover */
    export function setBalloonHoverState(balloon: number, p1: number): void;
    /** Total height is calculated using: cargo ratio + pelt ratio (by pelt count) Screenshot: https://imgur.com/a/nsomtiv */
    export function setBatchTarpHeight(vehicle: number, height: number, immediately: boolean): void;
    export function setDraftAnimalRandomSeed(vehicle: number, seed: number): void;
    export function setDraftVehicleAllowDraftAnimalAutoCreation(vehicle: number, allow: boolean): void;
    export function setDraftVehicleAnimalsCanDetach(draft: number, canDetach: boolean): void;
    export function setDraftVehicleDesiredSpeed(vehicle: number, speed: number): void;
    export function setDraftVehicleYokeCanBreak(draft: number, canBreak: boolean): void;
    /** Hashes: COACH2_BOOT_LOOT_ITEMS_COACHROB_RSC, COACH2_BOOT_LOOT_ITEMS_COACHROB, COACH2_MARY3 */
    export function setForceCoachRobberyLoot(vehicle: number, coachrobberyLoot: number): void;
    export function setForceHighLodVehicle(vehicle: number, p1: boolean): void;
    export function setHorseTrafficGroupingDistribution(p0: any, p1: any, p2: any, p3: any): void;
    /** Notice: BOOL p4 was wrongly named takePassengers (?) Can be used to rotate the train by setting the BOOL direction */
    export function setMissionTrainWarpToCoords(train: number, x: number, y: number, z: number, direction: boolean): void;
    /** Enables/disables the whistle on a specific train entity. */
    export function setRandomTrainsWhistleEnabled(train: number, enabled: boolean): void;
    /** Trains only. Enables/disables damage/explosion flags on the engine and all attached cars; typically set true before EXPLODE_VEHICLE. */
    export function setTrainDestructionEnabled(train: number, enabled: boolean): void;
    export function setTrainHalt(train: number): void;
    /** Restarts the train */
    export function setTrainLeaveStation(train: number): void;
    /** Maximum possible speed is 30.0 (108 km/h) */
    export function setTrainMaxSpeed(train: number, speed: number): void;
    export function setTrainStopsForStations(train: number, toggle: boolean): void;
    /** trainTrack: see 0x09034479E6E3E269. */
    export function setTrainTrackJunctionSwitch(trainTrack: number, junctionIndex: number, enabled: boolean): void;
    export function setVehicleDeterioration(vehicle: number, amount: number, p2: number, p3: boolean): void;
    /** dirtLevel: 0.0 - 1.0 */
    export function setVehicleDirtLevel2(vehicle: number, dirtLevel: number): void;
    export function setVehicleIsInHurry(vehicle: number, enabled: boolean): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/vehicles/vehicle_modding/vehicle_liveries.lua */
    export function setVehicleLivery(vehicle: number, liveryIndex: number): void;
    /** Ranges from -1 to 2? (internal type is int8) https://imgur.com/a/bPzHcft */
    export function setVehicleLodLevel(vehicle: number, lodLevel: number): void;
    /** mudLevel: 0.0 - 1.0 */
    export function setVehicleMudLevel(vehicle: number, mudLevel: number): void;
    /** Picks the road/path link nearest (start to end) and stores it on the vehicle's driving component (used by R* Scripts to choose an exit link). */
    export function setVehicleRoadLinkForced(vehicle: number, startX: number, startY: number, startZ: number, endX: number, endY: number, endZ: number): void;
    /** snowLevel: 0.0 - 1.0 */
    export function setVehicleSnowLevel(vehicle: number, snowLevel: number): void;
    /** https://github.com/femga/rdr3_discoveries/blob/master/vehicles/vehicle_modding/vehicle_tints.lua */
    export function setVehicleTint(vehicle: number, tintId: number): void;
    /** wetLevel: 0.0 - 1.0 */
    export function setVehicleWetLevel(vehicle: number, wetLevel: number): void;
    export function showHorseReins(vehicle: number): void;
    /** whistleSequence: ACKNOWLEDGE, BACKING_UP, CROSSING, DANGER, MOVING, NEXT_STATION, PASSING, STOPPED p2 = true seems to mute the sound */
    export function triggerTrainWhistle(train: number, whistleSequence: string, p2: boolean, p3: boolean): void;

    // VOICE

    // VOLUME
    export function createVolumeAggregate(): number;
    export function createVolumeBox(x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number): number;
    export function createVolumeCylinder(x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number): number;
    export function createVolumeSphere(x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number): number;
    export function deleteVolume(volume: number): void;
    export function doesVolumeCollideWithAnyVolumeLock(x: number, y: number, z: number, radius: number, p4: boolean, p5: number, p6: number): boolean;
    export function doesVolumeExist(volume: number): boolean;
    export function getVolumeCoords(volume: number): Vector3;
    /** enum eVolumeLockRequestStatus { 	VOLUME_LOCK_REQUEST_STATUS_INVALID, 	VOLUME_LOCK_REQUEST_STATUS_READY, 	VOLUME_LOCK_REQUEST_STATUS_IN_PROGRESS, 	VOLUME_LOCK_REQUEST_STATUS_SUCCEEDED, 	VOLUME_LOCK_... */
    export function getVolumeLockRequestStatus(volLockRequestId: number): number;
    export function getVolumeRotation(volume: number): Vector3;
    export function getVolumeScale(volume: number): Vector3;
    /** Old name: _IS_POSITION_INSIDE_VOLUME */
    export function isPointInVolume(volume: number, x: number, y: number, z: number): boolean;
    export function isVolumeLockRequestValid(volLockRequestId: number): boolean;
    export function requestVolumeLock(x: number, y: number, z: number, radius: number, p4: number, p5: number): number;
    export function requestVolumeLockWithArgs(args: any): number;
    export function setVolumeCoords(volume: number, posX: number, posY: number, posZ: number): boolean;
    export function setVolumeOwnerPersistentCharacter(volume: number, persChar: number, p2: boolean): void;
    export function setVolumeRotation(volume: number, rotX: number, rotY: number, rotZ: number): boolean;
    export function setVolumeScale(volume: number, scaleX: number, scaleY: number, scaleZ: number): boolean;
    /** _ADD_R* - _ADD_V(OLUME?)* */
    export function addBoundsToAggregateVolume(volume: number, aggregate: number): void;
    export function addBoxVolumeToVolumeAggregate(aggregate: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: number): void;
    export function addCylinderVolumeToVolumeAggregate(aggregate: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: number): void;
    export function addEntryVolumeLock(args: any): boolean;
    export function addSphereVolumeToVolumeAggregate(aggregate: number, p1: number, p2: number, p3: number, p4: number, p5: number, p6: number, p7: number, p8: number, p9: number): void;
    export function addVolumeToVolumeAggregate(aggregate: number, typeHash: number, x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number): void;
    export function createAntiGriefVolume(volumeType: number, x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number): number;
    export function createSpeedVolume(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any, p13: any, p14: any): number;
    export function createVolumeAggregateWithCustomName(name: string): number;
    export function createVolumeBoxWithCustomName(x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number, name: string): number;
    export function createVolumeByHash(volumeType: number, x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number): number;
    export function createVolumeByHashWithCustomName(volumeType: number, x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number, name: string): number;
    export function createVolumeCylinderWithCustomName(x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number, name: string): number;
    /** Params: p5 is always 0 */
    export function createVolumeLock(x: number, y: number, z: number, radius: number, flag: number, p5: any): number;
    /** Params: p3 is always 0 */
    export function createVolumeLockAttachedToEntity(entity: number, radius: number, flag: number, p3: any): number;
    export function createVolumeSphereWithCustomName(x: number, y: number, z: number, rotX: number, rotY: number, rotZ: number, scaleX: number, scaleY: number, scaleZ: number, name: string): number;
    export function createWalkAndTalkVolume(p0: any, p1: any, p2: any, p3: any, p4: any, p5: any, p6: any, p7: any, p8: any, p9: any, p10: any, p11: any, p12: any): number;
    export function findVolumeLockRequestIdWithArgs(args: any): number;
    /** Indexes items (including entyties and peds) in a set volume Counts up as its the return value of how many items it writes to given itemSet */
    export function getVolumeAmountOfIndexedItems(x: number, y: number, z: number, radius: number, itemSet: number): number;
    export function getVolumeBounds(volume: number, min: Vector3, max: Vector3): void;
    /** Returns relationshipGroup Hash */
    export function getVolumeRelationship(volume: number): number;
    export function isAggregateVolume(volume: number): boolean;
    export function isPointNearVolumeLockCenter(x: number, y: number, z: number, radius: number, p4: number, p5: number, flags: number): boolean;
    export function isVolumeLockRequestValid2(volLockRequestId: number): boolean;
    export function modifyVolumeLockLocation(volLock: number, x: number, y: number, z: number): void;
    export function releaseLockVolume(volLockRequestId: number): void;
    /** _REMOVE_E* - _REMOVE_R* */
    export function removeBoundsFromAggregateVolume(volume: number, aggregate: number): void;
    export function setAntiGriefVolumeBlocksHorse(volume: number, toggle: boolean): void;
    export function setAntiGriefVolumeBlocksPlayer(volume: number, toggle: boolean): void;
    export function setVolumeRelationship(volume: number, relationshipGroup: number): void;

    // WATER
    /** Must be called every frame to take full effect. */
    export function disableWaterLookup(): void;
    export function enableWaterLookup(): void;
    /** Checks against a global variable that is set by _SET_WORLD_WATER_TYPE. If that is set to one it will fail. Likely not the only issue but part of it. */
    export function getWaterHeight(x: number, y: number, z: number, height: number): boolean;
    export function getWaterHeightNoWaves(x: number, y: number, z: number, height: number): boolean;
    /** Only used in rcm_crackpot1 R* Script: p0 = 0 */
    export function removeExtraCalmingQuad(index: number): void;
    /** enum eScriptWaterTestResult { 	SCRIPT_WATER_TEST_RESULT_NONE, 	SCRIPT_WATER_TEST_RESULT_WATER, 	SCRIPT_WATER_TEST_RESULT_BLOCKED, }; */
    export function testProbeAgainstAllWater(x1: number, y1: number, z1: number, x2: number, y2: number, z2: number, flags: number, intersectionPos: Vector3): number;
    /** Checks against a global variable that is set by _SET_WORLD_WATER_TYPE. If it's set to 1 (Guarma) it will fail.  See TEST_PROBE_AGAINST_ALL_WATER. */
    export function testVerticalProbeAgainstAllWater(x: number, y: number, z: number, flags: number, waterHeight: number): number;
    export function getWorldWaterType(): number;
    /** Only used in guama1 / guama3 R* Script _REQUEST_* or _RESET_* */
    export function resetGuarmaWaterState(): void;
    /** Only used in R* Script guama1 */
    export function setOceanGuarmaWaterQuadrant(wavesHeight: number, p1: number, wavesDirection: number, p3: number, wavesAmount: number, p5: number, wavesSpeed: number, wavesStrength: number, ignoreHeight: boolean): void;
    /** 0 = World 1 = Guarma */
    export function setWorldWaterType(waterType: number): void;

    // WEAPON
    export function getAllowDualWield(ped: number): boolean;
    export function getAmmoInClip(ped: number, ammo: number, weaponHash: number): boolean;
    export function getAmmoInPedWeapon(ped: number, weaponHash: number): number;
    export function getBestPedShortarmGuid(ped: number, outGUID: any, p2: boolean, p3: boolean): void;
    export function getBestPedWeapon(ped: number, p1: boolean, p2: boolean): number;
    export function getCurrentPedVehicleWeapon(ped: number, weaponHash: number): boolean;
    /** attachPoint: see SET_CURRENT_PED_WEAPON */
    export function getCurrentPedWeapon(ped: number, weaponHash: number, p2: boolean, attachPoint: number, p4: boolean): boolean;
    /** Returns weaponObject, attachPoint: see SET_CURRENT_PED_WEAPON */
    export function getCurrentPedWeaponEntityIndex(ped: number, attachPoint: number): number;
    export function getMaxAmmo(ped: number, ammo: number, weaponHash: number): boolean;
    export function getMaxAmmoInClip(ped: number, weaponHash: number, p2: boolean): number;
    export function getPedAmmoByType(ped: number, ammoType: number): number;
    /** Returns the current ammo type of the specified ped's specified weapon. */
    export function getPedAmmoTypeFromWeapon(ped: number, weaponHash: number): number;
    export function getPedBackupWeapon(ped: number, p1: boolean): number;
    export function getPedLastWeaponImpactCoord(ped: number, coords: Vector3): boolean;
    export function getPedWeaponGuidAtAttachPoint(ped: number, attachPoint: number, weaponGuid: any): boolean;
    export function getWeapontypeGroup(weaponHash: number): number;
    export function getWeaponClipSize(weaponHash: number): number;
    /** 0.0: good condition, 1.0: poor condition */
    export function getWeaponDegradation(weaponObject: number): number;
    /** Related to rust of weapons */
    export function getWeaponPermanentDegradation(weaponObject: number): number;
    /** addReason: see _ADD_AMMO_TO_PED */
    export function giveDelayedWeaponToPed(ped: number, weaponHash: number, ammoCount: number, p3: boolean, addReason: number): void;
    /** Gives the ped the weapon. List: https://github.com/femga/rdr3_discoveries/blob/master/weapons/weapons.lua  Params: p7 is 0.5f, and p8 is 1.0f. p11 and p12 are both 0 in R* Scripts attachPoint: see ... */
    export function giveWeaponToPed(ped: number, weaponHash: number, ammoCount: number, bForceInHand: boolean, bForceInHolster: boolean, attachPoint: number, bAllowMultipleCopies: boolean, p7: number, p8: number, addReason: number, bIgnoreUnlocks: boolean, permanentDegradation: number, p12: boolean): number;
    export function giveWeaponToPedWithOptions(ped: number, inData: any, outData: any): boolean;
    /** onlyCheckPlayerInventory: If true, it will only check the players current inventory. If false, it also checks your horse inventory */
    export function hasPedGotWeapon(ped: number, weaponHash: number, p2: number, onlyCheckPlayerInventory: boolean): boolean;
    export function hasWeaponGotWeaponComponent(weapon: number, addonHash: number): boolean;
    /** Hides the ped's weapon during a cutscene. */
    export function hidePedWeaponForScriptedCutscene(ped: number, toggle: boolean): void;
    export function isPedArmed(ped: number, flags: number): boolean;
    export function isPedCarryingWeapon(ped: number, weaponHash: number): boolean;
    export function isPedWeaponReadyToShoot(ped: number): boolean;
    /** Returns true if CWeaponInfoFlags::Flags::Gun is set. */
    export function isWeaponAGun(weaponHash: number): boolean;
    export function isWeaponBow(weaponHash: number): boolean;
    export function isWeaponMeleeWeapon(weaponHash: number): boolean;
    export function isWeaponPistol(weaponHash: number): boolean;
    export function isWeaponRepeater(weaponHash: number): boolean;
    export function isWeaponRevolver(weaponHash: number): boolean;
    export function isWeaponRifle(weaponHash: number): boolean;
    export function isWeaponShotgun(weaponHash: number): boolean;
    export function isWeaponValid(weaponHash: number): boolean;
    /** Old name: _DROP_CURRENT_PED_WEAPON */
    export function makePedDropWeapon(ped: number, p1: boolean, attachPoint: number, p3: boolean, p4: boolean): number;
    export function removeAllPedWeapons(ped: number, p1: boolean, p2: boolean): void;
    export function removeWeaponComponentFromWeaponObject(weaponObject: number, component: number): void;
    /** removeReason: enum eRemoveItemReason : Hash { 	REMOVE_REASON_CLIENT_PURGED = 0x4A4E94DC, 	REMOVE_REASON_COALESCE = 0x2ABE393E, 	REMOVE_REASON_DEBUG = 0xA07362E6, 	REMOVE_REASON_DEFAULT = 0xF77DE93D... */
    export function removeWeaponFromPed(ped: number, weaponHash: number, p2: boolean, removeReason: number): void;
    export function setAllowAnyWeaponDrop(ped: number, toggle: boolean): void;
    export function setAmmoInClip(ped: number, weaponHash: number, ammo: number): boolean;
    export function setCurrentPedVehicleWeapon(ped: number, weaponHash: number): boolean;
    /** attachPoint: enum eWeaponAttachPoint { 	WEAPON_ATTACH_POINT_INVALID = -1, 	WEAPON_ATTACH_POINT_HAND_PRIMARY = 0, 	WEAPON_ATTACH_POINT_HAND_SECONDARY = 1, 	WEAPON_ATTACH_POINT_PISTOL_R = 2, 	MAX_HAN... */
    export function setCurrentPedWeapon(ped: number, weaponHash: number, equipNow: boolean, attachPoint: number, p4: boolean, p5: boolean): void;
    /** Equips a weapon from a weaponItem, similar to GIVE_WEAPON_TO_PED */
    export function setCurrentPedWeaponByGuid(ped: number, weaponUid: any, p2: boolean, p3: boolean, p4: boolean, p5: boolean): void;
    export function setInstantlyEquipWeaponPickups(ped: number, toggle: boolean): void;
    export function setPedAmmo(ped: number, weaponHash: number, ammo: number): void;
    export function setPedAmmoByType(ped: number, ammoType: number, ammo: number): void;
    export function setPedAmmoToDrop(ped: number, p1: number, p2: number): void;
    export function setPedCurrentWeaponVisible(ped: number, visible: boolean, deselectWeapon: boolean, p3: boolean, p4: boolean): void;
    export function setPedDropsInventoryWeapon(ped: number, weaponHash: number, xOffset: number, yOffset: number, zOffset: number, ammoCount: number): void;
    export function setPedDropsWeaponsWhenDead(ped: number, toggle: boolean): void;
    export function setPedInfiniteAmmo(ped: number, toggle: boolean, weaponHash: number): void;
    export function setPlayerPedQuickSwapWeaponByGuid(ped: number, guidPrimary: any, guidSecondary: any): void;
    export function setVehicleWeaponHeading(vehicle: number, seatIndex: number, heading: number, p3: boolean): void;
    export function shouldWeaponBeDiscardedWhenSwapped(weaponHash: number): boolean;
    /** addReason: enum eAddItemReason : Hash { 	ADD_REASON_AWARDS = 0xB784AD1E, 	ADD_REASON_CREATE_CHARACTER = 0xE2C4FF71, 	ADD_REASON_DEBUG = 0x5C05C64D, 	ADD_REASON_DEFAULT = 0x2CD419DC, 	ADD_REASON_GET... */
    export function addAmmoToPed(ped: number, weaponHash: number, amount: number, addReason: number): void;
    /** addReason: see _ADD_AMMO_TO_PED */
    export function addAmmoToPedByType(ped: number, ammoType: number, amount: number, addReason: number): void;
    /** Visually attaches the specified weapon to a horse holster/rack (shows the weapon model on the horse), even when the caller is not near the horse.  Typical flow: 	1) Move/assign the weapon into the ... */
    export function attachWeaponToHorseHolster(horse: number, weaponHash: number, ownerPed: number): void;
    /** True if the ped can access their *owned* mount/horse inventory for saddle weapon stow/retrieve (used to gate longarm-slot selection in the weapon wheel). Notes: does not require holding a weapon; r... */
    export function canPedAccessMountWeapons(ped: number): boolean;
    export function clearPedLastWeaponDamage(ped: number): void;
    export function createWeaponObject(weaponHash: number, ammoCount: number, x: number, y: number, z: number, showWorldModel: boolean, scale: number): number;
    /** Deletes all visible weapon PROP objects attached to a horse's holsters/rack. This removes only the *rendered/attached objects*; it does not remove the weapons from inventory and they remain usable.... */
    export function deleteWeaponObjectsOnHorse(horse: number): void;
    /** Disables all special ammo variants for the given weapon on the specified ped, forcing regular/basic ammo only.  Notes: 	- Higher-level override vs. per-ammo-type disables (targets all special varia... */
    export function disableAllSpecialAmmoForPed(ped: number, weaponHash: number): void;
    export function disableAmmoTypeForPed(ped: number, ammoHash: number): void;
    export function disableAmmoTypeForPedWeapon(ped: number, weaponHash: number, ammoHash: number): void;
    export function doesPedHavePistol(ped: number, p1: boolean): boolean;
    export function doesPedHaveRepeater(ped: number, p1: boolean): boolean;
    /** Preview: https://imgur.com/a/U8Q04Xu */
    export function doesPedHaveRevolver(ped: number, p1: boolean): boolean;
    export function doesPedHaveRifle(ped: number, p1: boolean): boolean;
    export function doesPedHaveShotgun(ped: number, p1: boolean): boolean;
    export function doesPedHaveSniper(ped: number, p1: boolean): boolean;
    /** Re-enables all special ammo variants for the given weapon on the specified ped (inverse of _DISABLE_ALL_SPECIAL_AMMO_FOR_PED).  Notes: 	- Restores access to any supported special ammo types for tha... */
    export function enableAllSpecialAmmoForPed(ped: number, weaponHash: number): void;
    export function enableAmmoTypeForPed(ped: number, weaponHash: number): void;
    export function enableAmmoTypeForPedWeapon(ped: number, weaponHash: number, ammoHash: number): void;
    export function enableWeaponRestore(ped: number): boolean;
    /** Triggers detonation/effect for a specific throwable ammo type associated with the given ped (commonly used to remotely detonate placed dynamite). Known ammo hashes seen in scripts: - AMMO_DYNAMITE ... */
    export function explodePedAmmoType(ped: number, ammoHash: number): boolean;
    export function getAmmoInClipByInventoryUid(ped: number, ammo: number, inventoryUid: any): boolean;
    /** Return total ammo in ped weapon from its guid, or false if 0 Example: https://pastebin.com/u2Hcah3C */
    export function getAmmoInPedWeaponFromGuid(ped: number, guid: any): number;
    /** Identical to _GET_AMMO_TYPE_FOR_WEAPON (0x5C2EA6C44F515F34) -> same native handler address Technically returns the first type specified for a weapon Example: local ammoType = _GET_AMMO_RECOMMENDED_... */
    export function getAmmoRecommendedTypeForWeapon(weaponHash: number): number;
    /** Identical to _GET_AMMO_RECOMMENDED_TYPE_FOR_WEAPON (0xEC97101A8F311282) -> same native handler address */
    export function getAmmoTypeForWeapon(weaponHash: number): number;
    /** If near your horse when called, weapons stored on your horse will be considered Returns weaponHash */
    export function getBestPedWeaponInGroup(ped: number, weaponGroup: number, p2: boolean, p3: boolean): number;
    export function getBestPedWeaponInInventory(ped: number, p1: any, guidPrimary: any): any;
    /** Returns whether the ped is currently allowed to switch weapons (weapon switching not locked by internal code/state).  Script evidence: used as an additional gate in NPLOI__CAN_GUN_SPIN_PREVIEW; whe... */
    export function getCanSwitchWeapon(ped: number): boolean;
    export function getCanTwirlWeapon(weaponHash: number): boolean;
    /** _GET_BEST_* - _GET_CLOSEST_* */
    export function getCorrectKitEmoteTwirlGun(ped: number, weaponGuid: any): boolean;
    /** Returns ammoHash */
    export function getCurrentAmmoTypeFromGuid(ped: number, weaponGuid: any): number;
    /** Returns ammoHash from weaponObject (Returned by 0x6CA484C9A7377E4F) */
    export function getCurrentPedWeaponAmmoType(ped: number, weaponObject: number): number;
    /** Returns weaponCollection Hash Example: RE_POLICECHASE_MALES_01: Carbine Repeater + Knife, LO_AGRO_PED */
    export function getDefaultPedWeaponCollection(pedModel: number): number;
    /** Returns the ped's default unarmed weapon hash as defined in CPedModelInfo (DefaultUnarmedWeapon). Falls back to WEAPON_UNARMED if the ped doesn't have a valid model info pointer, or 0 if the ped do... */
    export function getDefaultUnarmedWeaponHash(ped: number): number;
    /** Returns default attach point for given weapon hash. Returns -1 for melee as they can't be attached. Example: https://imgur.com/a/mUE5fug */
    export function getDefaultWeaponAttachPoint(weaponHash: number): number;
    /** True if the ped is currently being damaged by poison gas/fog (used by scripts to block actions like crafting). May be false if damage is suppressed/immune even while inside the fog. */
    export function getIsPedTakingPoisonGasDamage(ped: number): boolean;
    /** Returns the current weapon's lock-on/aim-assist range for this ped. Internally selects a different range when the ped is mounted or in a vehicle vs on foot. Returns -1.0 if the ped/weapon data is u... */
    export function getLockonRangeCurrentWeapon(ped: number): number;
    /** Gets the ped setting that controls whether longarms are instantly stored on the mount when dismounting.  Notes: - p1 selects one of two internal bits (WeaponComponent+0x1B5); scripts consistently p... */
    export function getLongarmsInstantlyStoreOnDismount(ped: number, p1: number): boolean;
    export function getMaxLockonDistanceOfCurrentPedWeapon(ped: number): number;
    export function getNumPedsRestrainedFromBolas(ped: number): number;
    /** Returns eCurrentHeldWeapon _GET_R* - _GET_T* */
    export function getPedCurrentHeldWeapon(ped: number): number;
    /** Returns emote Hash */
    export function getPedGunSpinningEquippedKitEmoteTwirl(ped: number): number;
    /** Returns iSpinHash / iVariationSpin */
    export function getPedGunSpinningHashFromWeaponEmoteVariation(ped: number, weaponEmoteVariation: number): number;
    export function getPedHogtieWeapon(ped: number): number;
    /** slotHash is usually just the weaponHash name, but WEAPON_* is replaced with SLOT_* */
    export function getPedWeaponInSlot(ped: number, slotHash: number): number;
    /** _GET_M* - _GET_PED_A* */
    export function getPedWeaponObject(ped: number, p1: boolean): number;
    export function getPedWorstWeapon(ped: number, p1: boolean, p2: boolean, p3: boolean): number;
    /** Outputs cached guids */
    export function getPlayerPedQuickSwapWeaponByGuid(ped: number, guidPrimary: any, guidSecondary: any): void;
    /** Finds an ignited (lit/fused) explosive projectile inside a Volume.  Parameters: - volume: Search volume. - outEntity: [out] Receives the projectile Entity handle.  Returns: - BOOL: true if an ignit... */
    export function getProjectileIgnitedInVolume(volume: number, outEntity: number): boolean;
    /** Finds a projectile entity inside the given Volume and writes its handle to the out pointer.  Parameters: - volume: Volume to search in. - outEntity: [out] Receives the found projectile entity handl... */
    export function getProjectileInVolume(volume: number, outEntity: number): boolean;
    /** Gets the model hash from the weapon hash. */
    export function getWeapontypeModel(weaponHash: number): number;
    /** Returns hash where WEAPON_ is replaced with SLOT_ */
    export function getWeapontypeSlot(weaponHash: number): number;
    /** Returns WeaponAttachPoint */
    export function getWeaponAttachPoint(ped: number, attachPoint: number): number;
    export function getWeaponComponentTypeModel(componentHash: number): number;
    /** Related to weapon visual damage, not actual damage. */
    export function getWeaponDamage(weaponObject: number): number;
    export function getWeaponDirt(weaponObject: number): number;
    /** Returns weaponEmoteVariation  WEAPON_EMOTE_VARIATION_INVALID = -2, WEAPON_EMOTE_VARIATION_BASE, WEAPON_EMOTE_VARIATION_A, WEAPON_EMOTE_VARIATION_B, WEAPON_EMOTE_VARIATION_C, WEAPON_EMOTE_VARIATION_... */
    export function getWeaponEmoteVariation(ped: number, variation: number): number;
    /** Returns a random weaponHash from default ped weapon collection (see _GET_DEFAULT_PED_WEAPON_COLLECTION). */
    export function getWeaponFromDefaultPedWeaponCollection(weaponCollection: number, weaponGroup: number): number;
    /** Returns the weapon hash in the horse/mount's first holster slot, or 0 if empty/invalid. */
    export function getWeaponFromHorseHolster(horse: number): number;
    /** Returns iSpinHash */
    export function getWeaponGunSpinningWeaponEmoteTrickTypeHash(emote: number, weaponEmoteTrickType: number): number;
    /** Example: https://imgur.com/a/fCaPJ1x */
    export function getWeaponHasMultipleAmmoTypes(weaponHash: number): boolean;
    /** Returns "WNS_INVALID" if the weapon is invalid/doesn't exist. */
    export function getWeaponName(weaponHash: number): NativeString;
    export function getWeaponName2(weaponHash: number): NativeString;
    export function getWeaponNameWithPermanentDegradation(weaponHash: number, permanentDegradationLevel: number): NativeString;
    /** Detaches the weapon from the ped and actually removes the ped's weapon */
    export function getWeaponObjectFromPed(ped: number, p1: boolean): number;
    /** Returns the last weapon hash that was replaced due to a slot/holster swap when giving/equipping a weapon (set internally during weapon give/equip when another weapon gets displaced). If clear==true... */
    export function getWeaponReplacedHash(clear: boolean): number;
    export function getWeaponScale(weaponObject: number): number;
    export function getWeaponSoot(weaponObject: number): number;
    export function getWeaponStatId(weaponHash: number): number;
    export function getWeaponTypeFromAmmoType(ammoType: number): number;
    export function getWeaponUnlock(weaponHash: number): number;
    export function giveWeaponCollectionToPed(ped: number, weaponCollection: number): void;
    /** entity can be a ped or weapon object. */
    export function giveWeaponComponentToEntity(entity: number, componentHash: number, weaponHash: number, p3: boolean): void;
    export function giveWeaponComponentToWeaponObject(weaponObject: number, ped: number, componentHash: number, p3: boolean): void;
    export function hasEntityBeenDamagedByWeapon(entity: number, weaponName: number, weaponType: number): boolean;
    export function hasEntityBeenDamagedByWeaponRecently(entity: number, weaponHash: number, ms: number): boolean;
    export function hasPedGotWeaponComponent(ped: number, componentHash: number, weaponHash: number): boolean;
    export function hasWeaponAssetLoaded(weaponHash: number): boolean;
    /** Unequip current weapon and set current weapon to WEAPON_UNARMED. p0 usually 2 in R* scripts. Doesn't seem to have any effect if changed.... immediately: if true it will instantly switch to unarmed */
    export function hidePedWeapons(ped: number, p0: number, immediately: boolean): void;
    export function holsterPedWeapons(ped: number, p1: boolean, p2: boolean, p3: boolean, immediately: boolean): void;
    export function isAmmoSilent(ammoHash: number): boolean;
    export function isAmmoSilent2(ammoHash: number): boolean;
    export function isAmmoTypeValidForWeapon(weaponHash: number, ammoHash: number): boolean;
    export function isAmmoValid(ammoHash: number): boolean;
    /** Checks whether the weapon stored/equipped at the specified ped attach point is classified as a sniper weapon.  Params: - ped: Target ped. - attachPoint: Attach point / weapon slot to check.  Return... */
    export function isPedCarryingWeaponSniperAtAttachPoint(ped: number, attachPoint: number): boolean;
    export function isPedCurrentWeaponHolstered(ped: number): boolean;
    export function isTargetPedConstrainedByPedUsingBolas(ped: number, targetPed: number): boolean;
    export function isWeaponBinoculars(weaponHash: number): boolean;
    /** Returns true for lassos, melee, thrown weapons (machetes and unarmed return false) Returns false for all guns, bows and animal weapons _IS_WEAPON_S* - _IS_WEAPON_V* */
    export function isWeaponCloseRange(weaponHash: number): boolean;
    /** Returns true if the ped is currently holstering or unholstering a weapon */
    export function isWeaponHolsterStateChanging(ped: number): boolean;
    /** Returns true when the weapon passed is either a lasso, the camera or the binoculars _IS_WEAPON_M* - _IS_WEAPON_P* */
    export function isWeaponKit(weaponHash: number): boolean;
    /** Returns true when the weapon passed is either the fishingrod, a lasso, the camera or the binoculars _IS_WEAPON_M* - _IS_WEAPON_P* */
    export function isWeaponKit2(weaponHash: number): boolean;
    export function isWeaponKnife(weaponHash: number): boolean;
    export function isWeaponLantern(weaponHash: number): boolean;
    export function isWeaponLasso(weaponHash: number): boolean;
    export function isWeaponOneHanded(weaponHash: number): boolean;
    export function isWeaponSilent(weaponHash: number): boolean;
    export function isWeaponSniper(weaponHash: number): boolean;
    export function isWeaponThrowable(weaponHash: number): boolean;
    export function isWeaponTorch(weaponHash: number): boolean;
    export function isWeaponTwoHanded(weaponHash: number): boolean;
    export function listenProjectileHitEvents(listen: boolean): void;
    export function makePedReload(ped: number): any;
    export function refillAmmoInClip(ped: number, clipInventoryUid: any, p2: number): any;
    export function refillAmmoInCurrentPedWeapon(ped: number): any;
    /** Registers a spawned weapon object as ignitable (enables fuse/lighting behavior).  Parameters: - weaponObject: Weapon OBJECT entity (e.g., from CREATE_WEAPON_OBJECT).  Notes: - Does not ignite insta... */
    export function registerWeaponObjectForIgnition(weaponObject: number): void;
    export function removeAllPedAmmo(ped: number): void;
    /** removeReason must be REMOVE_REASON_USED, REMOVE_REASON_GIVEN, REMOVE_REASON_DROPPED or REMOVE_REASON_DEBUG, unless amount is -1  removeReason: see REMOVE_WEAPON_FROM_PED */
    export function removeAmmoFromPed(ped: number, weaponHash: number, amount: number, removeReason: number): void;
    /** removeReason must be REMOVE_REASON_USED, REMOVE_REASON_GIVEN, REMOVE_REASON_DROPPED or REMOVE_REASON_DEBUG, unless amount is -1  removeReason: see REMOVE_WEAPON_FROM_PED */
    export function removeAmmoFromPedByType(ped: number, ammoHash: number, amount: number, removeReason: number): void;
    export function removeWeaponAsset(weaponHash: number): void;
    export function removeWeaponComponentFromPed(ped: number, componentHash: number, weaponHash: number): void;
    export function removeWeaponFromPedByGuid(ped: number, weaponGuid: any, removeReason: number): void;
    export function requestWeaponAsset(weaponHash: number, p1: number, p2: boolean): void;
    /** Appears to just send specified weapon to your horse holster without having to be close However, the weapon is not visible on the horse holster, but you can reach the weapon on the weapon wheel */
    export function sendWeaponToInventory(ped: number, weaponHash: number): void;
    /** emote hashes: KIT_EMOTE_TWIRL_GUN, KIT_EMOTE_TWIRL_GUN_LEFT_HOLSTER, KIT_EMOTE_TWIRL_GUN_DUAL, 0 (to unequip) */
    export function setActiveGunSpinningEquipKitEmoteTwirl(ped: number, emote: number): void;
    /** spinHash can be -1, 0 to disable */
    export function setActiveGunSpinningKitEmoteTwirl(ped: number, weaponEmoteTrickType: number, spin: number): void;
    export function setAllowDualWield(ped: number, allow: boolean): void;
    /** turretHash: WEAPON_TURRET_MAXIUM, WEAPON_TURRET_GATLING, WEAPON_TURRET_CANNON, WEAPON_TURRET_REVOLVING_CANNON */
    export function setAmmoInTurret(vehicle: number, turretHash: number, ammo: number): void;
    export function setAmmoTypeForPedWeapon(ped: number, weaponHash: number, ammoHash: number): void;
    export function setAmmoTypeForPedWeaponInventory(ped: number, weaponInventoryUid: any, ammoHash: number): void;
    /** Sets the arrow trail FX preset for arrows fired from a bow by this ped (applies to arrows fired after the call). trailHash: 658521773 is used by R* to enable a sparkly trail in some MP modes and to... */
    export function setArrowTrailFx(ped: number, trailHash: number): void;
    export function setForceAutoEquip(ped: number, toggle: boolean): void;
    export function setForceCurrentWeaponIntoCockedState(ped: number, attachPoint: number): void;
    /** _STOP_* - _TEST_* */
    export function setGunSpinningInventorySlotIdActivate(ped: number, emoteType: number): void;
    /** Stores longarms to your horse on dismount Params: p2 = 0 SET_[I - M]* */
    export function setInstantlyStoreLongarmsOnDismount(ped: number, storeLongarms: boolean, p2: number): void;
    export function setPedAllWeaponsVisibility(ped: number, visible: boolean): void;
    export function setPedInfiniteAmmoClip(ped: number, toggle: boolean): void;
    /** attachPoint: see SET_CURRENT_PED_WEAPON */
    export function setPedWeaponAttachPointVisibility(ped: number, attachPoint: number, visible: boolean): void;
    /** Sets the explosion/impact effect radius for an existing projectile entity (e.g., thrown dynamite).  Notes: - Set before detonation/impact; no effect after. - Does not ignite/detonate; it only chang... */
    export function setProjectileEffectRadius(projectile: number, radius: number): void;
    /** Sets the remaining fuse time (seconds) for an ignited explosive projectile.  Parameters: - projectile: Projectile entity (e.g., lit dynamite). - time: Remaining fuse time in seconds (0.0 = immediat... */
    export function setProjectileFuseTime(projectile: number, time: number): void;
    export function setVehicleWeaponHeadingLimits(vehicle: number, p1: number, minHeading: number, maxHeading: number): void;
    export function setVehicleWeaponHeadingLimits2(vehicle: number, p1: number, minHeading: number, maxHeading: number): any;
    /** Toggles reload behavior for certain vehicle-mounted cannons.  Params: - vehicle: Vehicle with the mounted cannon. - noReload: true = disable reload (continuous fire), false = normal reload. - p2: u... */
    export function setVehicleWeaponReloadMode(vehicle: number, noReload: boolean, p2: number): void;
    /** Related to weapon visual damage, not actual damage. */
    export function setWeaponDamage(weaponObject: number, level: number, p2: boolean): void;
    export function setWeaponDegradation(weaponObject: number, level: number): void;
    export function setWeaponDirt(weaponObject: number, level: number, p2: boolean): void;
    export function setWeaponHolstered(ped: number, disableAnim: boolean): void;
    /** every other level will have the max value of (brokeLevel - threshold) */
    export function setWeaponLevelThreshold(weaponObject: number, threshold: number): void;
    export function setWeaponScale(weaponObject: number, scale: number): void;
    export function setWeaponSoot(weaponObject: number, level: number, p2: boolean): void;

    // ZONE
    /** Returns name hash, see common:/data/levels/rdr3/mapzones.meta  type (-1 matches any type): class CMapZone { public: 	enum class Type 	{ 		STATE, 		TOWN, 		LAKE, 		RIVER, 		OIL_SPILL, 		SWAMP, 		OCE... */
    export function getMapZoneAtCoords(x: number, y: number, z: number, type: number): number;
    /** Returns the zone's name hash if its type matches one of the following: - LAKE - RIVER - OIL_SPILL - SWAMP - OCEAN - CREEK - POND - GLACIER */
    export function getWaterMapZoneAtCoords(x: number, y: number, z: number): number;

    // _NAMESPACE4
    /** nullsub, doesn't do anything however it is being used in tty scripts: [NET_BAD_SPORT_REPORT_PLAYER] Detected bad sport behavior from Player badSportBehavior: BS_QUITTER = 0, BS_VEHICLE_DESTRUCTION ... */
    export function reportPlayerBadSportBehavior(gamerHandle: any, badSportBehaviorType: number): void;

    const _default: {
        [key: string]: (...args: any[]) => any;
    };
    export default _default;
}
