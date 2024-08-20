/*
 * Copyright 2023 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "discovery_impl.h"
#include "firebolt.h"


namespace Firebolt {
namespace Discovery {

    static void ProviderInvokeSession(std::string& methodName, JsonObject& jsonParameters, Firebolt::Error *err = nullptr)
    {
        Firebolt::Error status = Firebolt::Error::NotConnected;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {

            JsonObject jsonResult;
            status = transport->Invoke(methodName, jsonParameters, jsonResult);
            if (status == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "%s is successfully invoked", methodName.c_str());
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", status);
        }
        if (err != nullptr) {
            *err = status;
        }
    }
  
    class DiscoveryUserInterestSession : virtual public IProviderSession {
    public:
         std::string correlationId () const override
         {
             return _correlationId;
         }

    public:
        std::string _correlationId;
    };
    static void DiscoveryUserInterestSessionInnerCallback( void* provider, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Discovery::JsonData_Request>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Discovery::JsonData_Request>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Request request;

            request.correlationId = proxyResponse->CorrelationId;
            {
                request.parameters.type = (*proxyResponse).Parameters.Type;
                request.parameters.reason = (*proxyResponse).Parameters.Reason;
            }

            proxyResponse.Release();

            std::unique_ptr<IProviderSession> discoveryUserInterestSession = std::make_unique<DiscoveryUserInterestSession>();
            IDiscoveryProvider& discoveryProvider = *(reinterpret_cast<IDiscoveryProvider*>(provider));
            discoveryProvider.userInterest(request.parameters, std::move(discoveryUserInterestSession));
        }
    }


    // Methods
    /* clearContentAccess - Clear both availabilities and entitlements from the subscriber. This is equivalent of calling `Discovery.contentAccess({ availabilities: [], entitlements: []})`. This is typically called when the user signs out of an account. */
    void DiscoveryImpl::clearContentAccess( Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;

        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
    
            WPEFramework::Core::JSON::VariantContainer jsonResult;
            statusError = transport->Invoke("discovery.clearContentAccess", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.clearContentAccess is successfully invoked");
    
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return;
    }

    /* contentAccess - Inform the platform of what content the user can access either by discovering it or consuming it. Availabilities determine which content is discoverable to a user, while entitlements determine if the user can currently consume that content. Content can be available but not entitled, this means that user can see the content but when they try to open it they must gain an entitlement either through purchase or subscription upgrade. In case the access changed off-device, this API should be called any time the app comes to the foreground to refresh the access. This API should also be called any time the availabilities or entitlements change within the app for any reason. Typical reasons may include the user signing into an account or upgrading a subscription. Less common cases can cause availabilities to change, such as moving to a new service location. When availabilities or entitlements are removed from the subscriber (such as when the user signs out), then an empty array should be given. To clear both, use the Discovery.clearContentAccess convenience API. */
    void DiscoveryImpl::contentAccess( const ContentAccessIdentifiers& ids, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;

        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            auto element = ids;
            JsonData_ContentAccessIdentifiers idsContainer;
            {
                if (element.availabilities.has_value()) {
                    WPEFramework::Core::JSON::ArrayType<Firebolt::Discovery::JsonData_Availability> availabilitiesArray;
                    std::vector<Availability> availabilities = element.availabilities.value();
                    for (auto& element : availabilities) {
                        Firebolt::Discovery::JsonData_Availability availabilitiesContainer;
                        {
                            availabilitiesContainer.Type = element.type;
                            availabilitiesContainer.Id = element.id;
                            if (element.catalogId.has_value()) {
                                availabilitiesContainer.CatalogId = element.catalogId.value();
                            }
                            if (element.startTime.has_value()) {
                                availabilitiesContainer.StartTime = element.startTime.value();
                            }
                            if (element.endTime.has_value()) {
                                availabilitiesContainer.EndTime = element.endTime.value();
                            }
                        }
                        availabilitiesArray.Add() = availabilitiesContainer;
                    }
                    idsContainer.Availabilities = availabilitiesArray;
                }
                if (element.entitlements.has_value()) {
                    WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_Entitlement> entitlementsArray;
                    std::vector<Entertainment::Entitlement> entitlements = element.entitlements.value();
                    for (auto& element : entitlements) {
                        Firebolt::Entertainment::JsonData_Entitlement entitlementsContainer;
                        {
                            entitlementsContainer.EntitlementId = element.entitlementId;
                            if (element.startTime.has_value()) {
                                entitlementsContainer.StartTime = element.startTime.value();
                            }
                            if (element.endTime.has_value()) {
                                entitlementsContainer.EndTime = element.endTime.value();
                            }
                        }
                        entitlementsArray.Add() = entitlementsContainer;
                    }
                    idsContainer.Entitlements = entitlementsArray;
                }
            }
            string idsStr;
            idsContainer.ToString(idsStr);
            WPEFramework::Core::JSON::VariantContainer idsVariantContainer(idsStr);
            WPEFramework::Core::JSON::Variant idsVariant = idsVariantContainer;
            jsonParameters.Set(_T("ids"), idsVariant);
            WPEFramework::Core::JSON::VariantContainer jsonResult;
            statusError = transport->Invoke("discovery.contentAccess", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.contentAccess is successfully invoked");
    
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return;
    }

    /* entitlements - Inform the platform of the users latest entitlements w/in this app. */
    bool DiscoveryImpl::entitlements( const std::vector<Entertainment::Entitlement>& entitlements, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::ArrayType<WPEFramework::Core::JSON::Variant> entitlementsArray;
            for (auto& element : entitlements) {
                Firebolt::Entertainment::JsonData_Entitlement entitlementsContainer;
                {
                        entitlementsContainer.EntitlementId = element.entitlementId;
                        if (element.startTime.has_value()) {
                            entitlementsContainer.StartTime = element.startTime.value();
                        }
                        if (element.endTime.has_value()) {
                            entitlementsContainer.EndTime = element.endTime.value();
                        }
                }
                string entitlementsStr;
                entitlementsContainer.ToString(entitlementsStr);
                WPEFramework::Core::JSON::VariantContainer entitlementsVariantContainer(entitlementsStr);
                WPEFramework::Core::JSON::Variant entitlementsVariant = entitlementsVariantContainer;
                entitlementsArray.Add() = entitlementsVariant;
            }
            WPEFramework::Core::JSON::Variant entitlementsVariant;
            entitlementsVariant.Array(entitlementsArray);
            jsonParameters.Set(_T("entitlements"), entitlementsVariant);
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.entitlements", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.entitlements is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* entityInfo - Provide information about a program entity and its available watchable assets, such as entitlement status and price, via either a push or pull call flow. Includes information about the program entity and its relevant associated entities, such as extras, previews, and, in the case of TV series, seasons and episodes.

See the `EntityInfo` and `WayToWatch` data structures below for more information.

The app only needs to implement Pull support for `entityInfo` at this time. */
    bool DiscoveryImpl::entityInfo( const EntityInfoResult& result, Firebolt::Error *err ) 
    {
        Firebolt::Error status = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
            string correlationId = "";
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant correlationIdVariant(correlationId);
            jsonParameters.Set(_T("correlationId"), correlationIdVariant);
            auto element = result;
            JsonData_EntityInfoResult resultContainer;
            {
                resultContainer.Expires = element.expires;
                {
                    {
                        if (element.entity.identifiers.assetId.has_value()) {
                            resultContainer.Entity.Identifiers.AssetId = element.entity.identifiers.assetId.value();
                        }
                        if (element.entity.identifiers.entityId.has_value()) {
                            resultContainer.Entity.Identifiers.EntityId = element.entity.identifiers.entityId.value();
                        }
                        if (element.entity.identifiers.seasonId.has_value()) {
                            resultContainer.Entity.Identifiers.SeasonId = element.entity.identifiers.seasonId.value();
                        }
                        if (element.entity.identifiers.seriesId.has_value()) {
                            resultContainer.Entity.Identifiers.SeriesId = element.entity.identifiers.seriesId.value();
                        }
                        if (element.entity.identifiers.appContentData.has_value()) {
                            resultContainer.Entity.Identifiers.AppContentData = element.entity.identifiers.appContentData.value();
                        }
                    }
                    resultContainer.Entity.Title = element.entity.title;
                    resultContainer.Entity.EntityType = element.entity.entityType;
                    if (element.entity.programType.has_value()) {
                        resultContainer.Entity.ProgramType = element.entity.programType.value();
                    }
                    if (element.entity.musicType.has_value()) {
                        resultContainer.Entity.MusicType = element.entity.musicType.value();
                    }
                    if (element.entity.synopsis.has_value()) {
                        resultContainer.Entity.Synopsis = element.entity.synopsis.value();
                    }
                    if (element.entity.seasonNumber.has_value()) {
                        resultContainer.Entity.SeasonNumber = element.entity.seasonNumber.value();
                    }
                    if (element.entity.seasonCount.has_value()) {
                        resultContainer.Entity.SeasonCount = element.entity.seasonCount.value();
                    }
                    if (element.entity.episodeNumber.has_value()) {
                        resultContainer.Entity.EpisodeNumber = element.entity.episodeNumber.value();
                    }
                    if (element.entity.episodeCount.has_value()) {
                        resultContainer.Entity.EpisodeCount = element.entity.episodeCount.value();
                    }
                    if (element.entity.releaseDate.has_value()) {
                        resultContainer.Entity.ReleaseDate = element.entity.releaseDate.value();
                    }
                    if (element.entity.contentRatings.has_value()) {
                        WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_ContentRating> contentRatingsArray;
                        std::vector<Entertainment::ContentRating> contentRatings = element.entity.contentRatings.value();
                        for (auto& element : contentRatings) {
                            Firebolt::Entertainment::JsonData_ContentRating contentRatingsContainer;
                            {
                                contentRatingsContainer.Scheme = element.scheme;
                                contentRatingsContainer.Rating = element.rating;
                                if (element.advisories.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> advisoriesArray;
                                    std::vector<std::string> advisories = element.advisories.value();
                                    for (auto& element : advisories) {
                                        advisoriesArray.Add() = element;
                                    }
                                    contentRatingsContainer.Advisories = advisoriesArray;
                                }
                            }
                            contentRatingsArray.Add() = contentRatingsContainer;
                        }
                        resultContainer.Entity.ContentRatings = contentRatingsArray;
                    }
                    if (element.entity.waysToWatch.has_value()) {
                        WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatch> waysToWatchArray;
                        std::vector<Entertainment::WayToWatch> waysToWatch = element.entity.waysToWatch.value();
                        for (auto& element : waysToWatch) {
                            Firebolt::Entertainment::JsonData_WayToWatch waysToWatchContainer;
                            {
                                {
                                    if (element.identifiers.assetId.has_value()) {
                                        waysToWatchContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                    }
                                    if (element.identifiers.entityId.has_value()) {
                                        waysToWatchContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                    }
                                    if (element.identifiers.seasonId.has_value()) {
                                        waysToWatchContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                    }
                                    if (element.identifiers.seriesId.has_value()) {
                                        waysToWatchContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                    }
                                    if (element.identifiers.appContentData.has_value()) {
                                        waysToWatchContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                    }
                                }
                                if (element.expires.has_value()) {
                                    waysToWatchContainer.Expires = element.expires.value();
                                }
                                if (element.entitled.has_value()) {
                                    waysToWatchContainer.Entitled = element.entitled.value();
                                }
                                if (element.entitledExpires.has_value()) {
                                    waysToWatchContainer.EntitledExpires = element.entitledExpires.value();
                                }
                                if (element.offeringType.has_value()) {
                                    waysToWatchContainer.OfferingType = element.offeringType.value();
                                }
                                if (element.hasAds.has_value()) {
                                    waysToWatchContainer.HasAds = element.hasAds.value();
                                }
                                if (element.price.has_value()) {
                                    waysToWatchContainer.Price = element.price.value();
                                }
                                if (element.videoQuality.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatchVideoQuality> videoQualityArray;
                                    std::vector<Entertainment::WayToWatchVideoQuality> videoQuality = element.videoQuality.value();
                                    for (auto& element : videoQuality) {
                                        videoQualityArray.Add() = element;
                                    }
                                    waysToWatchContainer.VideoQuality = videoQualityArray;
                                }
                                WPEFramework::Core::JSON::ArrayType<Firebolt::Types::JsonData_AudioProfile> audioProfileArray;
                                std::vector<Types::AudioProfile> audioProfile = element.audioProfile;
                                for (auto& element : audioProfile) {
                                    audioProfileArray.Add() = element;
                                }
                                waysToWatchContainer.AudioProfile = audioProfileArray;
                                if (element.audioLanguages.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioLanguagesArray;
                                    std::vector<std::string> audioLanguages = element.audioLanguages.value();
                                    for (auto& element : audioLanguages) {
                                        audioLanguagesArray.Add() = element;
                                    }
                                    waysToWatchContainer.AudioLanguages = audioLanguagesArray;
                                }
                                if (element.closedCaptions.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> closedCaptionsArray;
                                    std::vector<std::string> closedCaptions = element.closedCaptions.value();
                                    for (auto& element : closedCaptions) {
                                        closedCaptionsArray.Add() = element;
                                    }
                                    waysToWatchContainer.ClosedCaptions = closedCaptionsArray;
                                }
                                if (element.subtitles.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> subtitlesArray;
                                    std::vector<std::string> subtitles = element.subtitles.value();
                                    for (auto& element : subtitles) {
                                        subtitlesArray.Add() = element;
                                    }
                                    waysToWatchContainer.Subtitles = subtitlesArray;
                                }
                                if (element.audioDescriptions.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioDescriptionsArray;
                                    std::vector<std::string> audioDescriptions = element.audioDescriptions.value();
                                    for (auto& element : audioDescriptions) {
                                        audioDescriptionsArray.Add() = element;
                                    }
                                    waysToWatchContainer.AudioDescriptions = audioDescriptionsArray;
                                }
                            }
                            waysToWatchArray.Add() = waysToWatchContainer;
                        }
                        resultContainer.Entity.WaysToWatch = waysToWatchArray;
                    }
                }
                if (element.related.has_value()) {
                    WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_EntityInfo> relatedArray;
                    std::vector<Entertainment::EntityInfo> related = element.related.value();
                    for (auto& element : related) {
                        Firebolt::Entertainment::JsonData_EntityInfo relatedContainer;
                        {
                            {
                                if (element.identifiers.assetId.has_value()) {
                                    relatedContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                }
                                if (element.identifiers.entityId.has_value()) {
                                    relatedContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                }
                                if (element.identifiers.seasonId.has_value()) {
                                    relatedContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                }
                                if (element.identifiers.seriesId.has_value()) {
                                    relatedContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                }
                                if (element.identifiers.appContentData.has_value()) {
                                    relatedContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                }
                            }
                            relatedContainer.Title = element.title;
                            relatedContainer.EntityType = element.entityType;
                            if (element.programType.has_value()) {
                                relatedContainer.ProgramType = element.programType.value();
                            }
                            if (element.musicType.has_value()) {
                                relatedContainer.MusicType = element.musicType.value();
                            }
                            if (element.synopsis.has_value()) {
                                relatedContainer.Synopsis = element.synopsis.value();
                            }
                            if (element.seasonNumber.has_value()) {
                                relatedContainer.SeasonNumber = element.seasonNumber.value();
                            }
                            if (element.seasonCount.has_value()) {
                                relatedContainer.SeasonCount = element.seasonCount.value();
                            }
                            if (element.episodeNumber.has_value()) {
                                relatedContainer.EpisodeNumber = element.episodeNumber.value();
                            }
                            if (element.episodeCount.has_value()) {
                                relatedContainer.EpisodeCount = element.episodeCount.value();
                            }
                            if (element.releaseDate.has_value()) {
                                relatedContainer.ReleaseDate = element.releaseDate.value();
                            }
                            if (element.contentRatings.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_ContentRating> contentRatingsArray;
                                std::vector<Entertainment::ContentRating> contentRatings = element.contentRatings.value();
                                for (auto& element : contentRatings) {
                                    Firebolt::Entertainment::JsonData_ContentRating contentRatingsContainer;
                                    {
                                        contentRatingsContainer.Scheme = element.scheme;
                                        contentRatingsContainer.Rating = element.rating;
                                        if (element.advisories.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> advisoriesArray;
                                            std::vector<std::string> advisories = element.advisories.value();
                                            for (auto& element : advisories) {
                                                advisoriesArray.Add() = element;
                                            }
                                            contentRatingsContainer.Advisories = advisoriesArray;
                                        }
                                    }
                                    contentRatingsArray.Add() = contentRatingsContainer;
                                }
                                relatedContainer.ContentRatings = contentRatingsArray;
                            }
                            if (element.waysToWatch.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatch> waysToWatchArray;
                                std::vector<Entertainment::WayToWatch> waysToWatch = element.waysToWatch.value();
                                for (auto& element : waysToWatch) {
                                    Firebolt::Entertainment::JsonData_WayToWatch waysToWatchContainer;
                                    {
                                        {
                                            if (element.identifiers.assetId.has_value()) {
                                                waysToWatchContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                            }
                                            if (element.identifiers.entityId.has_value()) {
                                                waysToWatchContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                            }
                                            if (element.identifiers.seasonId.has_value()) {
                                                waysToWatchContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                            }
                                            if (element.identifiers.seriesId.has_value()) {
                                                waysToWatchContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                            }
                                            if (element.identifiers.appContentData.has_value()) {
                                                waysToWatchContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                            }
                                        }
                                        if (element.expires.has_value()) {
                                            waysToWatchContainer.Expires = element.expires.value();
                                        }
                                        if (element.entitled.has_value()) {
                                            waysToWatchContainer.Entitled = element.entitled.value();
                                        }
                                        if (element.entitledExpires.has_value()) {
                                            waysToWatchContainer.EntitledExpires = element.entitledExpires.value();
                                        }
                                        if (element.offeringType.has_value()) {
                                            waysToWatchContainer.OfferingType = element.offeringType.value();
                                        }
                                        if (element.hasAds.has_value()) {
                                            waysToWatchContainer.HasAds = element.hasAds.value();
                                        }
                                        if (element.price.has_value()) {
                                            waysToWatchContainer.Price = element.price.value();
                                        }
                                        if (element.videoQuality.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatchVideoQuality> videoQualityArray;
                                            std::vector<Entertainment::WayToWatchVideoQuality> videoQuality = element.videoQuality.value();
                                            for (auto& element : videoQuality) {
                                                videoQualityArray.Add() = element;
                                            }
                                            waysToWatchContainer.VideoQuality = videoQualityArray;
                                        }
                                        WPEFramework::Core::JSON::ArrayType<Firebolt::Types::JsonData_AudioProfile> audioProfileArray;
                                        std::vector<Types::AudioProfile> audioProfile = element.audioProfile;
                                        for (auto& element : audioProfile) {
                                            audioProfileArray.Add() = element;
                                        }
                                        waysToWatchContainer.AudioProfile = audioProfileArray;
                                        if (element.audioLanguages.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioLanguagesArray;
                                            std::vector<std::string> audioLanguages = element.audioLanguages.value();
                                            for (auto& element : audioLanguages) {
                                                audioLanguagesArray.Add() = element;
                                            }
                                            waysToWatchContainer.AudioLanguages = audioLanguagesArray;
                                        }
                                        if (element.closedCaptions.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> closedCaptionsArray;
                                            std::vector<std::string> closedCaptions = element.closedCaptions.value();
                                            for (auto& element : closedCaptions) {
                                                closedCaptionsArray.Add() = element;
                                            }
                                            waysToWatchContainer.ClosedCaptions = closedCaptionsArray;
                                        }
                                        if (element.subtitles.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> subtitlesArray;
                                            std::vector<std::string> subtitles = element.subtitles.value();
                                            for (auto& element : subtitles) {
                                                subtitlesArray.Add() = element;
                                            }
                                            waysToWatchContainer.Subtitles = subtitlesArray;
                                        }
                                        if (element.audioDescriptions.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioDescriptionsArray;
                                            std::vector<std::string> audioDescriptions = element.audioDescriptions.value();
                                            for (auto& element : audioDescriptions) {
                                                audioDescriptionsArray.Add() = element;
                                            }
                                            waysToWatchContainer.AudioDescriptions = audioDescriptionsArray;
                                        }
                                    }
                                    waysToWatchArray.Add() = waysToWatchContainer;
                                }
                                relatedContainer.WaysToWatch = waysToWatchArray;
                            }
                        }
                        relatedArray.Add() = relatedContainer;
                    }
                    resultContainer.Related = relatedArray;
                }
            }
            string resultStr;
            resultContainer.ToString(resultStr);
            WPEFramework::Core::JSON::VariantContainer resultVariantContainer(resultStr);
            WPEFramework::Core::JSON::Variant resultVariant = resultVariantContainer;
            jsonParameters.Set(_T("result"), resultVariant);
            WPEFramework::Core::JSON::Boolean jsonResult;
            status = transport->Invoke("discovery.entityInfo", jsonParameters, jsonResult);
            if (status == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.entityInfo is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", status);
        }
        if (err != nullptr) {
            *err = status;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::HomeIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_HomeIntent intentContainer;
                intentContainer.Action = element.action;
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::LaunchIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_LaunchIntent intentContainer;
                intentContainer.Action = element.action;
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::EntityIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_EntityIntent intentContainer;
                intentContainer.Action = element.action;
                intentContainer.Data = element.data;
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::PlaybackIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_PlaybackIntent intentContainer;
                intentContainer.Action = element.action;
                intentContainer.Data = element.data;
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::SearchIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_SearchIntent intentContainer;
                intentContainer.Action = element.action;
                {
                    intentContainer.Data.Query = element.data.value().query;
                }
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::SectionIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_SectionIntent intentContainer;
                intentContainer.Action = element.action;
                {
                    intentContainer.Data.SectionName = element.data.sectionName;
                }
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::TuneIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_TuneIntent intentContainer;
                intentContainer.Action = element.action;
                {
                    {
                        intentContainer.Data.Entity.EntityType = element.data.entity.entityType;
                        intentContainer.Data.Entity.ChannelType = element.data.entity.channelType;
                        intentContainer.Data.Entity.EntityId = element.data.entity.entityId;
                        if (element.data.entity.appContentData.has_value()) {
                            intentContainer.Data.Entity.AppContentData = element.data.entity.appContentData.value();
                        }
                    }
                    {
                        if (element.data.options.value().assetId.has_value()) {
                            intentContainer.Data.Options.AssetId = element.data.options.value().assetId.value();
                        }
                        if (element.data.options.value().restartCurrentProgram.has_value()) {
                            intentContainer.Data.Options.RestartCurrentProgram = element.data.options.value().restartCurrentProgram.value();
                        }
                        if (element.data.options.value().time.has_value()) {
                            intentContainer.Data.Options.Time = element.data.options.value().time.value();
                        }
                    }
                }
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::PlayEntityIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_PlayEntityIntent intentContainer;
                intentContainer.Action = element.action;
                {
                    intentContainer.Data.Entity = element.data.entity;
                    {
                        if (element.data.options.value().playFirstId.has_value()) {
                            intentContainer.Data.Options.PlayFirstId = element.data.options.value().playFirstId.value();
                        }
                        if (element.data.options.value().playFirstTrack.has_value()) {
                            intentContainer.Data.Options.PlayFirstTrack = element.data.options.value().playFirstTrack.value();
                        }
                    }
                }
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* launch - Launch or foreground the specified app, and optionally instructs it to navigate to the specified user action. 
 For the Primary Experience, the appId can be any one of:  

 - xrn:firebolt:application-type:main 

 - xrn:firebolt:application-type:settings */
    bool DiscoveryImpl::launch( const std::string& appId, const std::optional<Intents::PlayQueryIntent>& intent, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant appIdVariant(appId);
            jsonParameters.Set(_T("appId"), appIdVariant);
            if (intent.has_value()) {
                auto element = intent.value();
                Firebolt::Intents::JsonData_PlayQueryIntent intentContainer;
                intentContainer.Action = element.action;
                {
                    intentContainer.Data.Query = element.data.query;
                    {
                        if (element.data.options.value().programTypes.has_value()) {
                            WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_ProgramType> programTypesArray;
                            std::vector<Entertainment::ProgramType> programTypes = element.data.options.value().programTypes.value();
                            for (auto& element : programTypes) {
                                programTypesArray.Add() = element;
                            }
                            intentContainer.Data.Options.ProgramTypes = programTypesArray;
                        }
                        if (element.data.options.value().musicTypes.has_value()) {
                            WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_MusicType> musicTypesArray;
                            std::vector<Entertainment::MusicType> musicTypes = element.data.options.value().musicTypes.value();
                            for (auto& element : musicTypes) {
                                musicTypesArray.Add() = element;
                            }
                            intentContainer.Data.Options.MusicTypes = musicTypesArray;
                        }
                    }
                }
                {
                    intentContainer.Context.Source = element.context.source;
                }
                string intentStr;
                intentContainer.ToString(intentStr);
                WPEFramework::Core::JSON::VariantContainer intentVariantContainer(intentStr);
                WPEFramework::Core::JSON::Variant intentVariant = intentVariantContainer;
                jsonParameters.Set(_T("intent"), intentVariant);
            }
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.launch", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.launch is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }

    /* policy - get the discovery policy */
    DiscoveryPolicy DiscoveryImpl::policy( Firebolt::Error *err ) const
    {
        const string method = _T("discovery.policy");
        
        
        JsonData_DiscoveryPolicy jsonResult;
        DiscoveryPolicy policy;
        
        Firebolt::Error status = FireboltSDK::Properties::Get(method, jsonResult);
        if (status == Firebolt::Error::None) {
            DiscoveryPolicy policyResult;
            policyResult.enableRecommendations = jsonResult.EnableRecommendations.Value();
            policyResult.shareWatchHistory = jsonResult.ShareWatchHistory.Value();
            policyResult.rememberWatchedPrograms = jsonResult.RememberWatchedPrograms.Value();
            policy = policyResult;
        }
        if (err != nullptr) {
            *err = status;
        }

        return policy;
    }

    void DiscoveryImpl::provide( IDiscoveryProvider& provider )
    {
        string eventName;
        Firebolt::Error status = Firebolt::Error::None;
        JsonObject jsonParameters;

        eventName = _T("discovery.onRequestUserInterest");
        status = FireboltSDK::Event::Instance().Subscribe<JsonData_Request>(eventName, jsonParameters, DiscoveryUserInterestSessionInnerCallback, reinterpret_cast<void*>(&provider), nullptr);
        if (status != Firebolt::Error::None) {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in %s subscribe = %d", eventName.c_str(), status);
        }

    }

    /* purchasedContent - Return content purchased by the user, such as rentals and electronic sell through purchases.

The app should return the user's 100 most recent purchases in `entries`. The total count of purchases must be provided in `count`. If `count` is greater than the total number of `entries`, the UI may provide a link into the app to see the complete purchase list.

The `EntityInfo` object returned is not required to have `waysToWatch` populated, but it is recommended that it do so in case the UI wants to surface additional information on the purchases screen.

The app should implement both Push and Pull methods for `purchasedContent`.

The app should actively push `purchasedContent` when:

*  The app becomes Active.
*  When the state of the purchasedContent set has changed.
*  The app goes into Inactive or Background state, if there is a chance a change event has been missed. */
    bool DiscoveryImpl::purchasedContent( const PurchasedContentResult& result, Firebolt::Error *err ) 
    {
        Firebolt::Error status = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
            string correlationId = "";
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant correlationIdVariant(correlationId);
            jsonParameters.Set(_T("correlationId"), correlationIdVariant);
            auto element = result;
            JsonData_PurchasedContentResult resultContainer;
            {
                resultContainer.Expires = element.expires;
                resultContainer.TotalCount = element.totalCount;
                WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_EntityInfo> entriesArray;
                std::vector<Entertainment::EntityInfo> entries = element.entries;
                for (auto& element : entries) {
                    Firebolt::Entertainment::JsonData_EntityInfo entriesContainer;
                    {
                        {
                            if (element.identifiers.assetId.has_value()) {
                                entriesContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                            }
                            if (element.identifiers.entityId.has_value()) {
                                entriesContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                            }
                            if (element.identifiers.seasonId.has_value()) {
                                entriesContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                            }
                            if (element.identifiers.seriesId.has_value()) {
                                entriesContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                            }
                            if (element.identifiers.appContentData.has_value()) {
                                entriesContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                            }
                        }
                        entriesContainer.Title = element.title;
                        entriesContainer.EntityType = element.entityType;
                        if (element.programType.has_value()) {
                            entriesContainer.ProgramType = element.programType.value();
                        }
                        if (element.musicType.has_value()) {
                            entriesContainer.MusicType = element.musicType.value();
                        }
                        if (element.synopsis.has_value()) {
                            entriesContainer.Synopsis = element.synopsis.value();
                        }
                        if (element.seasonNumber.has_value()) {
                            entriesContainer.SeasonNumber = element.seasonNumber.value();
                        }
                        if (element.seasonCount.has_value()) {
                            entriesContainer.SeasonCount = element.seasonCount.value();
                        }
                        if (element.episodeNumber.has_value()) {
                            entriesContainer.EpisodeNumber = element.episodeNumber.value();
                        }
                        if (element.episodeCount.has_value()) {
                            entriesContainer.EpisodeCount = element.episodeCount.value();
                        }
                        if (element.releaseDate.has_value()) {
                            entriesContainer.ReleaseDate = element.releaseDate.value();
                        }
                        if (element.contentRatings.has_value()) {
                            WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_ContentRating> contentRatingsArray;
                            std::vector<Entertainment::ContentRating> contentRatings = element.contentRatings.value();
                            for (auto& element : contentRatings) {
                                Firebolt::Entertainment::JsonData_ContentRating contentRatingsContainer;
                                {
                                    contentRatingsContainer.Scheme = element.scheme;
                                    contentRatingsContainer.Rating = element.rating;
                                    if (element.advisories.has_value()) {
                                        WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> advisoriesArray;
                                        std::vector<std::string> advisories = element.advisories.value();
                                        for (auto& element : advisories) {
                                            advisoriesArray.Add() = element;
                                        }
                                        contentRatingsContainer.Advisories = advisoriesArray;
                                    }
                                }
                                contentRatingsArray.Add() = contentRatingsContainer;
                            }
                            entriesContainer.ContentRatings = contentRatingsArray;
                        }
                        if (element.waysToWatch.has_value()) {
                            WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatch> waysToWatchArray;
                            std::vector<Entertainment::WayToWatch> waysToWatch = element.waysToWatch.value();
                            for (auto& element : waysToWatch) {
                                Firebolt::Entertainment::JsonData_WayToWatch waysToWatchContainer;
                                {
                                    {
                                        if (element.identifiers.assetId.has_value()) {
                                            waysToWatchContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                        }
                                        if (element.identifiers.entityId.has_value()) {
                                            waysToWatchContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                        }
                                        if (element.identifiers.seasonId.has_value()) {
                                            waysToWatchContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                        }
                                        if (element.identifiers.seriesId.has_value()) {
                                            waysToWatchContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                        }
                                        if (element.identifiers.appContentData.has_value()) {
                                            waysToWatchContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                        }
                                    }
                                    if (element.expires.has_value()) {
                                        waysToWatchContainer.Expires = element.expires.value();
                                    }
                                    if (element.entitled.has_value()) {
                                        waysToWatchContainer.Entitled = element.entitled.value();
                                    }
                                    if (element.entitledExpires.has_value()) {
                                        waysToWatchContainer.EntitledExpires = element.entitledExpires.value();
                                    }
                                    if (element.offeringType.has_value()) {
                                        waysToWatchContainer.OfferingType = element.offeringType.value();
                                    }
                                    if (element.hasAds.has_value()) {
                                        waysToWatchContainer.HasAds = element.hasAds.value();
                                    }
                                    if (element.price.has_value()) {
                                        waysToWatchContainer.Price = element.price.value();
                                    }
                                    if (element.videoQuality.has_value()) {
                                        WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatchVideoQuality> videoQualityArray;
                                        std::vector<Entertainment::WayToWatchVideoQuality> videoQuality = element.videoQuality.value();
                                        for (auto& element : videoQuality) {
                                            videoQualityArray.Add() = element;
                                        }
                                        waysToWatchContainer.VideoQuality = videoQualityArray;
                                    }
                                    WPEFramework::Core::JSON::ArrayType<Firebolt::Types::JsonData_AudioProfile> audioProfileArray;
                                    std::vector<Types::AudioProfile> audioProfile = element.audioProfile;
                                    for (auto& element : audioProfile) {
                                        audioProfileArray.Add() = element;
                                    }
                                    waysToWatchContainer.AudioProfile = audioProfileArray;
                                    if (element.audioLanguages.has_value()) {
                                        WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioLanguagesArray;
                                        std::vector<std::string> audioLanguages = element.audioLanguages.value();
                                        for (auto& element : audioLanguages) {
                                            audioLanguagesArray.Add() = element;
                                        }
                                        waysToWatchContainer.AudioLanguages = audioLanguagesArray;
                                    }
                                    if (element.closedCaptions.has_value()) {
                                        WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> closedCaptionsArray;
                                        std::vector<std::string> closedCaptions = element.closedCaptions.value();
                                        for (auto& element : closedCaptions) {
                                            closedCaptionsArray.Add() = element;
                                        }
                                        waysToWatchContainer.ClosedCaptions = closedCaptionsArray;
                                    }
                                    if (element.subtitles.has_value()) {
                                        WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> subtitlesArray;
                                        std::vector<std::string> subtitles = element.subtitles.value();
                                        for (auto& element : subtitles) {
                                            subtitlesArray.Add() = element;
                                        }
                                        waysToWatchContainer.Subtitles = subtitlesArray;
                                    }
                                    if (element.audioDescriptions.has_value()) {
                                        WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioDescriptionsArray;
                                        std::vector<std::string> audioDescriptions = element.audioDescriptions.value();
                                        for (auto& element : audioDescriptions) {
                                            audioDescriptionsArray.Add() = element;
                                        }
                                        waysToWatchContainer.AudioDescriptions = audioDescriptionsArray;
                                    }
                                }
                                waysToWatchArray.Add() = waysToWatchContainer;
                            }
                            entriesContainer.WaysToWatch = waysToWatchArray;
                        }
                    }
                    entriesArray.Add() = entriesContainer;
                }
                resultContainer.Entries = entriesArray;
            }
            string resultStr;
            resultContainer.ToString(resultStr);
            WPEFramework::Core::JSON::VariantContainer resultVariantContainer(resultStr);
            WPEFramework::Core::JSON::Variant resultVariant = resultVariantContainer;
            jsonParameters.Set(_T("result"), resultVariant);
            WPEFramework::Core::JSON::Boolean jsonResult;
            status = transport->Invoke("discovery.purchasedContent", jsonParameters, jsonResult);
            if (status == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.purchasedContent is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", status);
        }
        if (err != nullptr) {
            *err = status;
        }

        return success;
    }

    /* signIn - Inform the platform that your user is signed in, for increased visibility in search & discovery. Sign-in state is used separately from what content can be access through entitlements and availabilities. Sign-in state may be used when deciding whether to choose this app to handle a user intent. For instance, if the user tries to launch something generic like playing music from an artist, only a signed-in app will be chosen. If the user wants to tune to a channel, only a signed-in app will be chosen to handle that intent. While signIn can optionally include entitlements as those typically change at signIn time, it is recommended to make a separate call to Discovery.contentAccess for entitlements. signIn is not only for when a user explicitly enters login credentials. If an app does not require any credentials from the user to consume content, such as in a free app, then the app should call signIn immediately on launch. */
   static void signInDispatcher(const void* result) {
        // Accessing Metrics methods using singleton Instance
        Firebolt::IFireboltAccessor::Instance().Firebolt::IFireboltAccessor::MetricsInterface().signIn();
    }
    /* signIn - Inform the platform that your user is signed in, for increased visibility in search & discovery. Sign-in state is used separately from what content can be access through entitlements and availabilities. Sign-in state may be used when deciding whether to choose this app to handle a user intent. For instance, if the user tries to launch something generic like playing music from an artist, only a signed-in app will be chosen. If the user wants to tune to a channel, only a signed-in app will be chosen to handle that intent. While signIn can optionally include entitlements as those typically change at signIn time, it is recommended to make a separate call to Discovery.contentAccess for entitlements. signIn is not only for when a user explicitly enters login credentials. If an app does not require any credentials from the user to consume content, such as in a free app, then the app should call signIn immediately on launch. */
    bool DiscoveryImpl::signIn( const std::optional<std::vector<Entertainment::Entitlement>>& entitlements, Firebolt::Error *err ) 
    {
        Firebolt::Error status = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {

            JsonObject jsonParameters;
            WPEFramework::Core::JSON::ArrayType<WPEFramework::Core::JSON::Variant> entitlementsArray;
            if (entitlements.has_value()) {
                for (auto& element : entitlements.value()) {
                    Firebolt::Entertainment::JsonData_Entitlement entitlementsContainer;
                    {
                            entitlementsContainer.EntitlementId = element.entitlementId;
                            if (element.startTime.has_value()) {
                                entitlementsContainer.StartTime = element.startTime.value();
                            }
                            if (element.endTime.has_value()) {
                                entitlementsContainer.EndTime = element.endTime.value();
                            }
                    }
                    string entitlementsStr;
                    entitlementsContainer.ToString(entitlementsStr);
                    WPEFramework::Core::JSON::VariantContainer entitlementsVariantContainer(entitlementsStr);
                    WPEFramework::Core::JSON::Variant entitlementsVariant = entitlementsVariantContainer;
                    entitlementsArray.Add() = entitlementsVariant;
                }
            }
            WPEFramework::Core::JSON::Variant entitlementsVariant;
            entitlementsVariant.Array(entitlementsArray);
            jsonParameters.Set(_T("entitlements"), entitlementsVariant);
            WPEFramework::Core::JSON::Boolean jsonResult;
            status = transport->Invoke("discovery.signIn", jsonParameters, jsonResult);
            if (status == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.signIn is successfully invoked");
                success = jsonResult.Value();
                WPEFramework::Core::ProxyType<WPEFramework::Core::IDispatch> job = WPEFramework::Core::ProxyType<WPEFramework::Core::IDispatch>(WPEFramework::Core::ProxyType<FireboltSDK::Worker>::Create(signInDispatcher, nullptr));
                WPEFramework::Core::IWorkerPool::Instance().Submit(job);
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", status);
        }

        return success;
    }

    /* signOut - Inform the platform that your user has signed out. See `Discovery.signIn` for more details on how the sign-in state is used.signOut will NOT clear entitlements, the app should make a separate call to Discovery.clearContentAccess. Apps should also call signOut when a login token has expired and the user is now in a signed-out state. */
   static void signOutDispatcher(const void* result) {
        // Accessing Metrics methods using singleton Instance
        Firebolt::IFireboltAccessor::Instance().Firebolt::IFireboltAccessor::MetricsInterface().signOut();

    }
    /* signOut - Inform the platform that your user has signed out. See `Discovery.signIn` for more details on how the sign-in state is used.signOut will NOT clear entitlements, the app should make a separate call to Discovery.clearContentAccess. Apps should also call signOut when a login token has expired and the user is now in a signed-out state. */
    bool DiscoveryImpl::signOut( Firebolt::Error *err )  const
    {
        Firebolt::Error status = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {

            JsonObject jsonParameters;
    
            WPEFramework::Core::JSON::Boolean jsonResult;
            status = transport->Invoke("discovery.signOut", jsonParameters, jsonResult);
            if (status == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.signOut is successfully invoked");
                success = jsonResult.Value();
                WPEFramework::Core::ProxyType<WPEFramework::Core::IDispatch> job = WPEFramework::Core::ProxyType<WPEFramework::Core::IDispatch>(WPEFramework::Core::ProxyType<FireboltSDK::Worker>::Create(signOutDispatcher, nullptr));
                WPEFramework::Core::IWorkerPool::Instance().Submit(job);
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", status);
        }

        return success;
    }

    /* userInterest - Send an entity that the user has expressed interest in to the platform. */
    void DiscoveryImpl::userInterest( const InterestType& type, const InterestReason& reason, const Entity::EntityDetails& entity, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;

        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            JsonData_InterestType jsonType = type;
            WPEFramework::Core::JSON::Variant typeVariant(jsonType.Data());
            jsonParameters.Set(_T("type"), typeVariant);
            JsonData_InterestReason jsonReason = reason;
            WPEFramework::Core::JSON::Variant reasonVariant(jsonReason.Data());
            jsonParameters.Set(_T("reason"), reasonVariant);
            auto element = entity;
            Firebolt::Entity::JsonData_EntityDetails entityContainer;
            {
                entityContainer.Identifiers = element.identifiers;
                {
                    if (element.info.value().title.has_value()) {
                        entityContainer.Info.Title = element.info.value().title.value();
                    }
                    if (element.info.value().synopsis.has_value()) {
                        entityContainer.Info.Synopsis = element.info.value().synopsis.value();
                    }
                    if (element.info.value().seasonNumber.has_value()) {
                        entityContainer.Info.SeasonNumber = element.info.value().seasonNumber.value();
                    }
                    if (element.info.value().seasonCount.has_value()) {
                        entityContainer.Info.SeasonCount = element.info.value().seasonCount.value();
                    }
                    if (element.info.value().episodeNumber.has_value()) {
                        entityContainer.Info.EpisodeNumber = element.info.value().episodeNumber.value();
                    }
                    if (element.info.value().episodeCount.has_value()) {
                        entityContainer.Info.EpisodeCount = element.info.value().episodeCount.value();
                    }
                    if (element.info.value().releaseDate.has_value()) {
                        entityContainer.Info.ReleaseDate = element.info.value().releaseDate.value();
                    }
                    if (element.info.value().contentRatings.has_value()) {
                        WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_ContentRating> contentRatingsArray;
                        std::vector<Entertainment::ContentRating> contentRatings = element.info.value().contentRatings.value();
                        for (auto& element : contentRatings) {
                            Firebolt::Entertainment::JsonData_ContentRating contentRatingsContainer;
                            {
                                contentRatingsContainer.Scheme = element.scheme;
                                contentRatingsContainer.Rating = element.rating;
                                if (element.advisories.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> advisoriesArray;
                                    std::vector<std::string> advisories = element.advisories.value();
                                    for (auto& element : advisories) {
                                        advisoriesArray.Add() = element;
                                    }
                                    contentRatingsContainer.Advisories = advisoriesArray;
                                }
                            }
                            contentRatingsArray.Add() = contentRatingsContainer;
                        }
                        entityContainer.Info.ContentRatings = contentRatingsArray;
                    }
                }
                if (element.waysToWatch.has_value()) {
                    WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatch> waysToWatchArray;
                    std::vector<Entertainment::WayToWatch> waysToWatch = element.waysToWatch.value();
                    for (auto& element : waysToWatch) {
                        Firebolt::Entertainment::JsonData_WayToWatch waysToWatchContainer;
                        {
                            {
                                if (element.identifiers.assetId.has_value()) {
                                    waysToWatchContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                }
                                if (element.identifiers.entityId.has_value()) {
                                    waysToWatchContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                }
                                if (element.identifiers.seasonId.has_value()) {
                                    waysToWatchContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                }
                                if (element.identifiers.seriesId.has_value()) {
                                    waysToWatchContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                }
                                if (element.identifiers.appContentData.has_value()) {
                                    waysToWatchContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                }
                            }
                            if (element.expires.has_value()) {
                                waysToWatchContainer.Expires = element.expires.value();
                            }
                            if (element.entitled.has_value()) {
                                waysToWatchContainer.Entitled = element.entitled.value();
                            }
                            if (element.entitledExpires.has_value()) {
                                waysToWatchContainer.EntitledExpires = element.entitledExpires.value();
                            }
                            if (element.offeringType.has_value()) {
                                waysToWatchContainer.OfferingType = element.offeringType.value();
                            }
                            if (element.hasAds.has_value()) {
                                waysToWatchContainer.HasAds = element.hasAds.value();
                            }
                            if (element.price.has_value()) {
                                waysToWatchContainer.Price = element.price.value();
                            }
                            if (element.videoQuality.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatchVideoQuality> videoQualityArray;
                                std::vector<Entertainment::WayToWatchVideoQuality> videoQuality = element.videoQuality.value();
                                for (auto& element : videoQuality) {
                                    videoQualityArray.Add() = element;
                                }
                                waysToWatchContainer.VideoQuality = videoQualityArray;
                            }
                            WPEFramework::Core::JSON::ArrayType<Firebolt::Types::JsonData_AudioProfile> audioProfileArray;
                            std::vector<Types::AudioProfile> audioProfile = element.audioProfile;
                            for (auto& element : audioProfile) {
                                audioProfileArray.Add() = element;
                            }
                            waysToWatchContainer.AudioProfile = audioProfileArray;
                            if (element.audioLanguages.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioLanguagesArray;
                                std::vector<std::string> audioLanguages = element.audioLanguages.value();
                                for (auto& element : audioLanguages) {
                                    audioLanguagesArray.Add() = element;
                                }
                                waysToWatchContainer.AudioLanguages = audioLanguagesArray;
                            }
                            if (element.closedCaptions.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> closedCaptionsArray;
                                std::vector<std::string> closedCaptions = element.closedCaptions.value();
                                for (auto& element : closedCaptions) {
                                    closedCaptionsArray.Add() = element;
                                }
                                waysToWatchContainer.ClosedCaptions = closedCaptionsArray;
                            }
                            if (element.subtitles.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> subtitlesArray;
                                std::vector<std::string> subtitles = element.subtitles.value();
                                for (auto& element : subtitles) {
                                    subtitlesArray.Add() = element;
                                }
                                waysToWatchContainer.Subtitles = subtitlesArray;
                            }
                            if (element.audioDescriptions.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioDescriptionsArray;
                                std::vector<std::string> audioDescriptions = element.audioDescriptions.value();
                                for (auto& element : audioDescriptions) {
                                    audioDescriptionsArray.Add() = element;
                                }
                                waysToWatchContainer.AudioDescriptions = audioDescriptionsArray;
                            }
                        }
                        waysToWatchArray.Add() = waysToWatchContainer;
                    }
                    entityContainer.WaysToWatch = waysToWatchArray;
                }
            }
            string entityStr;
            entityContainer.ToString(entityStr);
            WPEFramework::Core::JSON::VariantContainer entityVariantContainer(entityStr);
            WPEFramework::Core::JSON::Variant entityVariant = entityVariantContainer;
            jsonParameters.Set(_T("entity"), entityVariant);
            WPEFramework::Core::JSON::VariantContainer jsonResult;
            statusError = transport->Invoke("discovery.userInterest", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.userInterest is successfully invoked");
    
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return;
    }

    /* watchNext - Suggest a call-to-action for this app on the platform home screen */
    bool DiscoveryImpl::watchNext( const std::string& title, const Entertainment::ContentIdentifiers& identifiers, const std::optional<std::string>& expires, const std::optional<Images>& images, Firebolt::Error *err ) 
    {
        Firebolt::Error statusError = Firebolt::Error::NotConnected;
        bool success = false;
        FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
        if (transport != nullptr) {
        
            JsonObject jsonParameters;
            WPEFramework::Core::JSON::Variant titleVariant(title);
            jsonParameters.Set(_T("title"), titleVariant);
            auto element = identifiers;
            Firebolt::Entertainment::JsonData_ContentIdentifiers identifiersContainer;
            {
                if (element.assetId.has_value()) {
                    identifiersContainer.AssetId = element.assetId.value();
                }
                if (element.entityId.has_value()) {
                    identifiersContainer.EntityId = element.entityId.value();
                }
                if (element.seasonId.has_value()) {
                    identifiersContainer.SeasonId = element.seasonId.value();
                }
                if (element.seriesId.has_value()) {
                    identifiersContainer.SeriesId = element.seriesId.value();
                }
                if (element.appContentData.has_value()) {
                    identifiersContainer.AppContentData = element.appContentData.value();
                }
            }
            string identifiersStr;
            identifiersContainer.ToString(identifiersStr);
            WPEFramework::Core::JSON::VariantContainer identifiersVariantContainer(identifiersStr);
            WPEFramework::Core::JSON::Variant identifiersVariant = identifiersVariantContainer;
            jsonParameters.Set(_T("identifiers"), identifiersVariant);
            if (expires.has_value()) {
                WPEFramework::Core::JSON::Variant expiresVariant(expires.value());
                jsonParameters.Set(_T("expires"), expiresVariant);
            }
            if (images.has_value()) {
                WPEFramework::Core::JSON::VariantContainer imagesContainer;
                imagesContainer.FromString(images.value());
                WPEFramework::Core::JSON::Variant imagesVariant(imagesContainer);
                jsonParameters.Set(_T("images"), imagesVariant);
            }
    
            WPEFramework::Core::JSON::Boolean jsonResult;
            statusError = transport->Invoke("discovery.watchNext", jsonParameters, jsonResult);
            if (statusError == Firebolt::Error::None) {
                FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.watchNext is successfully invoked");
                success = jsonResult.Value();
            }

        } else {
            FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", statusError);
        }
        if (err != nullptr) {
            *err = statusError;
        }

        return success;
    }


    // Events
    /* onNavigateToEntityIntent - listen to `navigateTo` events */
    static void onNavigateToEntityIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_EntityIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_EntityIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::EntityIntent value;

            value.action = (*proxyResponse).Action;
            string dataStr;
            (*proxyResponse).Data.ToString(dataStr);
            value.data = dataStr;
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToEntityIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToEntityIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToEntityIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_EntityIntent>(eventName, jsonParameters, onNavigateToEntityIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToEntityIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onNavigateToHomeIntent - listen to `navigateTo` events */
    static void onNavigateToHomeIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_HomeIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_HomeIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::HomeIntent value;

            value.action = (*proxyResponse).Action;
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToHomeIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToHomeIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToHomeIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_HomeIntent>(eventName, jsonParameters, onNavigateToHomeIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToHomeIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onNavigateToLaunchIntent - listen to `navigateTo` events */
    static void onNavigateToLaunchIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_LaunchIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_LaunchIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::LaunchIntent value;

            value.action = (*proxyResponse).Action;
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToLaunchIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToLaunchIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToLaunchIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_LaunchIntent>(eventName, jsonParameters, onNavigateToLaunchIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToLaunchIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onNavigateToPlaybackIntent - listen to `navigateTo` events */
    static void onNavigateToPlaybackIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_PlaybackIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_PlaybackIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::PlaybackIntent value;

            value.action = (*proxyResponse).Action;
            string dataStr;
            (*proxyResponse).Data.ToString(dataStr);
            value.data = dataStr;
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToPlaybackIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToPlaybackIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToPlaybackIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_PlaybackIntent>(eventName, jsonParameters, onNavigateToPlaybackIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToPlaybackIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onNavigateToPlayEntityIntent - listen to `navigateTo` events */
    static void onNavigateToPlayEntityIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_PlayEntityIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_PlayEntityIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::PlayEntityIntent value;

            value.action = (*proxyResponse).Action;
            {
                string entityStr;
                (*proxyResponse).Data.Entity.ToString(entityStr);
                value.data.entity = entityStr;
                if ((*proxyResponse).Data.Options.IsSet()) {
                    value.data.options = std::make_optional<Intents::PlayEntityIntentDataOptions>();
                    if ((*proxyResponse).Data.Options.PlayFirstId.IsSet()) {
                        value.data.options.value().playFirstId = (*proxyResponse).Data.Options.PlayFirstId;
                    }
                    if ((*proxyResponse).Data.Options.PlayFirstTrack.IsSet()) {
                        value.data.options.value().playFirstTrack = (*proxyResponse).Data.Options.PlayFirstTrack;
                    }
                }
            }
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToPlayEntityIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToPlayEntityIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToPlayEntityIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_PlayEntityIntent>(eventName, jsonParameters, onNavigateToPlayEntityIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToPlayEntityIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onNavigateToPlayQueryIntent - listen to `navigateTo` events */
    static void onNavigateToPlayQueryIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_PlayQueryIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_PlayQueryIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::PlayQueryIntent value;

            value.action = (*proxyResponse).Action;
            {
                value.data.query = (*proxyResponse).Data.Query;
                if ((*proxyResponse).Data.Options.IsSet()) {
                    value.data.options = std::make_optional<Intents::PlayQueryIntentDataOptions>();
                    if ((*proxyResponse).Data.Options.ProgramTypes.IsSet()) {
                        value.data.options.value().programTypes = std::make_optional<std::vector<Entertainment::ProgramType>>();
                        auto index((*proxyResponse).Data.Options.ProgramTypes.Elements());
                        while (index.Next() == true) {
                            value.data.options.value().programTypes.value().push_back(index.Current().Value());
                        }
                    }
                    if ((*proxyResponse).Data.Options.MusicTypes.IsSet()) {
                        value.data.options.value().musicTypes = std::make_optional<std::vector<Entertainment::MusicType>>();
                        auto index((*proxyResponse).Data.Options.MusicTypes.Elements());
                        while (index.Next() == true) {
                            value.data.options.value().musicTypes.value().push_back(index.Current().Value());
                        }
                    }
                }
            }
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToPlayQueryIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToPlayQueryIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToPlayQueryIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_PlayQueryIntent>(eventName, jsonParameters, onNavigateToPlayQueryIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToPlayQueryIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onNavigateToSearchIntent - listen to `navigateTo` events */
    static void onNavigateToSearchIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_SearchIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_SearchIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::SearchIntent value;

            value.action = (*proxyResponse).Action;
            if ((*proxyResponse).Data.IsSet()) {
                value.data = std::make_optional<Intents::SearchIntentData>();
                value.data.value().query = (*proxyResponse).Data.Query;
            }
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToSearchIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToSearchIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToSearchIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_SearchIntent>(eventName, jsonParameters, onNavigateToSearchIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToSearchIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onNavigateToSectionIntent - listen to `navigateTo` events */
    static void onNavigateToSectionIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_SectionIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_SectionIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::SectionIntent value;

            value.action = (*proxyResponse).Action;
            {
                value.data.sectionName = (*proxyResponse).Data.SectionName;
            }
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToSectionIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToSectionIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToSectionIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_SectionIntent>(eventName, jsonParameters, onNavigateToSectionIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToSectionIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onNavigateToTuneIntent - listen to `navigateTo` events */
    static void onNavigateToTuneIntentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_TuneIntent>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<Firebolt::Intents::JsonData_TuneIntent>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            Intents::TuneIntent value;

            value.action = (*proxyResponse).Action;
            {
                {
                    value.data.entity.entityType = (*proxyResponse).Data.Entity.EntityType;
                    value.data.entity.channelType = (*proxyResponse).Data.Entity.ChannelType;
                    value.data.entity.entityId = (*proxyResponse).Data.Entity.EntityId;
                    if ((*proxyResponse).Data.Entity.AppContentData.IsSet()) {
                        value.data.entity.appContentData = (*proxyResponse).Data.Entity.AppContentData;
                    }
                }
                if ((*proxyResponse).Data.Options.IsSet()) {
                    value.data.options = std::make_optional<Intents::TuneIntentDataOptions>();
                    if ((*proxyResponse).Data.Options.AssetId.IsSet()) {
                        value.data.options.value().assetId = (*proxyResponse).Data.Options.AssetId;
                    }
                    if ((*proxyResponse).Data.Options.RestartCurrentProgram.IsSet()) {
                        value.data.options.value().restartCurrentProgram = (*proxyResponse).Data.Options.RestartCurrentProgram;
                    }
                    if ((*proxyResponse).Data.Options.Time.IsSet()) {
                        value.data.options.value().time = (*proxyResponse).Data.Options.Time;
                    }
                }
            }
            {
                value.context.source = (*proxyResponse).Context.Source;
            }

            proxyResponse.Release();

            IDiscovery::IOnNavigateToTuneIntentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnNavigateToTuneIntentNotification*>(notification));
            notifier.onNavigateTo(value);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnNavigateToTuneIntentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onNavigateTo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<Firebolt::Intents::JsonData_TuneIntent>(eventName, jsonParameters, onNavigateToTuneIntentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnNavigateToTuneIntentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onNavigateTo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onPolicyChanged - get the discovery policy */
    static void onPolicyChangedInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<JsonData_DiscoveryPolicy>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<JsonData_DiscoveryPolicy>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            DiscoveryPolicy policy;

            policy.enableRecommendations = proxyResponse->EnableRecommendations;
            policy.shareWatchHistory = proxyResponse->ShareWatchHistory;
            policy.rememberWatchedPrograms = proxyResponse->RememberWatchedPrograms;

            proxyResponse.Release();

            IDiscovery::IOnPolicyChangedNotification& notifier = *(reinterpret_cast<IDiscovery::IOnPolicyChangedNotification*>(notification));
            notifier.onPolicyChanged(policy);
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnPolicyChangedNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onPolicyChanged");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;

        status = FireboltSDK::Event::Instance().Subscribe<JsonData_DiscoveryPolicy>(eventName, jsonParameters, onPolicyChangedInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnPolicyChangedNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onPolicyChanged"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onPullEntityInfo - Provide information about a program entity and its available watchable assets, such as entitlement status and price, via either a push or pull call flow. Includes information about the program entity and its relevant associated entities, such as extras, previews, and, in the case of TV series, seasons and episodes.

See the `EntityInfo` and `WayToWatch` data structures below for more information.

The app only needs to implement Pull support for `entityInfo` at this time. */
    static void onPullEntityInfoInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<JsonData_EntityInfoFederatedRequest>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<JsonData_EntityInfoFederatedRequest>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            JsonData_EntityInfoParameters jsonResult = proxyResponse->Parameters;
            EntityInfoParameters entityInfoParameters;
            EntityInfoParameters entityInfoParametersResult;
            entityInfoParametersResult.entityId = jsonResult.EntityId.Value();
            if (jsonResult.AssetId.IsSet()) {
                entityInfoParametersResult.assetId = jsonResult.AssetId.Value();
            }
            entityInfoParameters = entityInfoParametersResult;

            IDiscovery::IOnPullEntityInfoNotification& notifier = *(reinterpret_cast<IDiscovery::IOnPullEntityInfoNotification*>(notification));
            EntityInfoResult element = notifier.onPullEntityInfo(entityInfoParameters);
            Firebolt::Error status = Firebolt::Error::NotConnected;
            FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
            if (transport != nullptr) {
                JsonObject jsonParameters;
                WPEFramework::Core::JSON::Variant CorrelationId = proxyResponse->CorrelationId.Value();
                jsonParameters.Set(_T("correlationId"), CorrelationId);
                JsonData_EntityInfoResult entityInfoResultContainer;
                {
                    {
                        entityInfoResultContainer.Expires = element.expires;
                        {
                            {
                                if (element.entity.identifiers.assetId.has_value()) {
                                    entityInfoResultContainer.Entity.Identifiers.AssetId = element.entity.identifiers.assetId.value();
                                }
                                if (element.entity.identifiers.entityId.has_value()) {
                                    entityInfoResultContainer.Entity.Identifiers.EntityId = element.entity.identifiers.entityId.value();
                                }
                                if (element.entity.identifiers.seasonId.has_value()) {
                                    entityInfoResultContainer.Entity.Identifiers.SeasonId = element.entity.identifiers.seasonId.value();
                                }
                                if (element.entity.identifiers.seriesId.has_value()) {
                                    entityInfoResultContainer.Entity.Identifiers.SeriesId = element.entity.identifiers.seriesId.value();
                                }
                                if (element.entity.identifiers.appContentData.has_value()) {
                                    entityInfoResultContainer.Entity.Identifiers.AppContentData = element.entity.identifiers.appContentData.value();
                                }
                            }
                            entityInfoResultContainer.Entity.Title = element.entity.title;
                            entityInfoResultContainer.Entity.EntityType = element.entity.entityType;
                            if (element.entity.programType.has_value()) {
                                entityInfoResultContainer.Entity.ProgramType = element.entity.programType.value();
                            }
                            if (element.entity.musicType.has_value()) {
                                entityInfoResultContainer.Entity.MusicType = element.entity.musicType.value();
                            }
                            if (element.entity.synopsis.has_value()) {
                                entityInfoResultContainer.Entity.Synopsis = element.entity.synopsis.value();
                            }
                            if (element.entity.seasonNumber.has_value()) {
                                entityInfoResultContainer.Entity.SeasonNumber = element.entity.seasonNumber.value();
                            }
                            if (element.entity.seasonCount.has_value()) {
                                entityInfoResultContainer.Entity.SeasonCount = element.entity.seasonCount.value();
                            }
                            if (element.entity.episodeNumber.has_value()) {
                                entityInfoResultContainer.Entity.EpisodeNumber = element.entity.episodeNumber.value();
                            }
                            if (element.entity.episodeCount.has_value()) {
                                entityInfoResultContainer.Entity.EpisodeCount = element.entity.episodeCount.value();
                            }
                            if (element.entity.releaseDate.has_value()) {
                                entityInfoResultContainer.Entity.ReleaseDate = element.entity.releaseDate.value();
                            }
                            if (element.entity.contentRatings.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_ContentRating> contentRatingsArray;
                                std::vector<Entertainment::ContentRating> contentRatings = element.entity.contentRatings.value();
                                for (auto& element : contentRatings) {
                                    Firebolt::Entertainment::JsonData_ContentRating contentRatingsContainer;
                                    {
                                        contentRatingsContainer.Scheme = element.scheme;
                                        contentRatingsContainer.Rating = element.rating;
                                        if (element.advisories.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> advisoriesArray;
                                            std::vector<std::string> advisories = element.advisories.value();
                                            for (auto& element : advisories) {
                                                advisoriesArray.Add() = element;
                                            }
                                            contentRatingsContainer.Advisories = advisoriesArray;
                                        }
                                    }
                                    contentRatingsArray.Add() = contentRatingsContainer;
                                }
                                entityInfoResultContainer.Entity.ContentRatings = contentRatingsArray;
                            }
                            if (element.entity.waysToWatch.has_value()) {
                                WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatch> waysToWatchArray;
                                std::vector<Entertainment::WayToWatch> waysToWatch = element.entity.waysToWatch.value();
                                for (auto& element : waysToWatch) {
                                    Firebolt::Entertainment::JsonData_WayToWatch waysToWatchContainer;
                                    {
                                        {
                                            if (element.identifiers.assetId.has_value()) {
                                                waysToWatchContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                            }
                                            if (element.identifiers.entityId.has_value()) {
                                                waysToWatchContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                            }
                                            if (element.identifiers.seasonId.has_value()) {
                                                waysToWatchContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                            }
                                            if (element.identifiers.seriesId.has_value()) {
                                                waysToWatchContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                            }
                                            if (element.identifiers.appContentData.has_value()) {
                                                waysToWatchContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                            }
                                        }
                                        if (element.expires.has_value()) {
                                            waysToWatchContainer.Expires = element.expires.value();
                                        }
                                        if (element.entitled.has_value()) {
                                            waysToWatchContainer.Entitled = element.entitled.value();
                                        }
                                        if (element.entitledExpires.has_value()) {
                                            waysToWatchContainer.EntitledExpires = element.entitledExpires.value();
                                        }
                                        if (element.offeringType.has_value()) {
                                            waysToWatchContainer.OfferingType = element.offeringType.value();
                                        }
                                        if (element.hasAds.has_value()) {
                                            waysToWatchContainer.HasAds = element.hasAds.value();
                                        }
                                        if (element.price.has_value()) {
                                            waysToWatchContainer.Price = element.price.value();
                                        }
                                        if (element.videoQuality.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatchVideoQuality> videoQualityArray;
                                            std::vector<Entertainment::WayToWatchVideoQuality> videoQuality = element.videoQuality.value();
                                            for (auto& element : videoQuality) {
                                                videoQualityArray.Add() = element;
                                            }
                                            waysToWatchContainer.VideoQuality = videoQualityArray;
                                        }
                                        WPEFramework::Core::JSON::ArrayType<Firebolt::Types::JsonData_AudioProfile> audioProfileArray;
                                        std::vector<Types::AudioProfile> audioProfile = element.audioProfile;
                                        for (auto& element : audioProfile) {
                                            audioProfileArray.Add() = element;
                                        }
                                        waysToWatchContainer.AudioProfile = audioProfileArray;
                                        if (element.audioLanguages.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioLanguagesArray;
                                            std::vector<std::string> audioLanguages = element.audioLanguages.value();
                                            for (auto& element : audioLanguages) {
                                                audioLanguagesArray.Add() = element;
                                            }
                                            waysToWatchContainer.AudioLanguages = audioLanguagesArray;
                                        }
                                        if (element.closedCaptions.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> closedCaptionsArray;
                                            std::vector<std::string> closedCaptions = element.closedCaptions.value();
                                            for (auto& element : closedCaptions) {
                                                closedCaptionsArray.Add() = element;
                                            }
                                            waysToWatchContainer.ClosedCaptions = closedCaptionsArray;
                                        }
                                        if (element.subtitles.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> subtitlesArray;
                                            std::vector<std::string> subtitles = element.subtitles.value();
                                            for (auto& element : subtitles) {
                                                subtitlesArray.Add() = element;
                                            }
                                            waysToWatchContainer.Subtitles = subtitlesArray;
                                        }
                                        if (element.audioDescriptions.has_value()) {
                                            WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioDescriptionsArray;
                                            std::vector<std::string> audioDescriptions = element.audioDescriptions.value();
                                            for (auto& element : audioDescriptions) {
                                                audioDescriptionsArray.Add() = element;
                                            }
                                            waysToWatchContainer.AudioDescriptions = audioDescriptionsArray;
                                        }
                                    }
                                    waysToWatchArray.Add() = waysToWatchContainer;
                                }
                                entityInfoResultContainer.Entity.WaysToWatch = waysToWatchArray;
                            }
                        }
                        if (element.related.has_value()) {
                            WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_EntityInfo> relatedArray;
                            std::vector<Entertainment::EntityInfo> related = element.related.value();
                            for (auto& element : related) {
                                Firebolt::Entertainment::JsonData_EntityInfo relatedContainer;
                                {
                                    {
                                        if (element.identifiers.assetId.has_value()) {
                                            relatedContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                        }
                                        if (element.identifiers.entityId.has_value()) {
                                            relatedContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                        }
                                        if (element.identifiers.seasonId.has_value()) {
                                            relatedContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                        }
                                        if (element.identifiers.seriesId.has_value()) {
                                            relatedContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                        }
                                        if (element.identifiers.appContentData.has_value()) {
                                            relatedContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                        }
                                    }
                                    relatedContainer.Title = element.title;
                                    relatedContainer.EntityType = element.entityType;
                                    if (element.programType.has_value()) {
                                        relatedContainer.ProgramType = element.programType.value();
                                    }
                                    if (element.musicType.has_value()) {
                                        relatedContainer.MusicType = element.musicType.value();
                                    }
                                    if (element.synopsis.has_value()) {
                                        relatedContainer.Synopsis = element.synopsis.value();
                                    }
                                    if (element.seasonNumber.has_value()) {
                                        relatedContainer.SeasonNumber = element.seasonNumber.value();
                                    }
                                    if (element.seasonCount.has_value()) {
                                        relatedContainer.SeasonCount = element.seasonCount.value();
                                    }
                                    if (element.episodeNumber.has_value()) {
                                        relatedContainer.EpisodeNumber = element.episodeNumber.value();
                                    }
                                    if (element.episodeCount.has_value()) {
                                        relatedContainer.EpisodeCount = element.episodeCount.value();
                                    }
                                    if (element.releaseDate.has_value()) {
                                        relatedContainer.ReleaseDate = element.releaseDate.value();
                                    }
                                    if (element.contentRatings.has_value()) {
                                        WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_ContentRating> contentRatingsArray;
                                        std::vector<Entertainment::ContentRating> contentRatings = element.contentRatings.value();
                                        for (auto& element : contentRatings) {
                                            Firebolt::Entertainment::JsonData_ContentRating contentRatingsContainer;
                                            {
                                                contentRatingsContainer.Scheme = element.scheme;
                                                contentRatingsContainer.Rating = element.rating;
                                                if (element.advisories.has_value()) {
                                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> advisoriesArray;
                                                    std::vector<std::string> advisories = element.advisories.value();
                                                    for (auto& element : advisories) {
                                                        advisoriesArray.Add() = element;
                                                    }
                                                    contentRatingsContainer.Advisories = advisoriesArray;
                                                }
                                            }
                                            contentRatingsArray.Add() = contentRatingsContainer;
                                        }
                                        relatedContainer.ContentRatings = contentRatingsArray;
                                    }
                                    if (element.waysToWatch.has_value()) {
                                        WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatch> waysToWatchArray;
                                        std::vector<Entertainment::WayToWatch> waysToWatch = element.waysToWatch.value();
                                        for (auto& element : waysToWatch) {
                                            Firebolt::Entertainment::JsonData_WayToWatch waysToWatchContainer;
                                            {
                                                {
                                                    if (element.identifiers.assetId.has_value()) {
                                                        waysToWatchContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                                    }
                                                    if (element.identifiers.entityId.has_value()) {
                                                        waysToWatchContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                                    }
                                                    if (element.identifiers.seasonId.has_value()) {
                                                        waysToWatchContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                                    }
                                                    if (element.identifiers.seriesId.has_value()) {
                                                        waysToWatchContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                                    }
                                                    if (element.identifiers.appContentData.has_value()) {
                                                        waysToWatchContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                                    }
                                                }
                                                if (element.expires.has_value()) {
                                                    waysToWatchContainer.Expires = element.expires.value();
                                                }
                                                if (element.entitled.has_value()) {
                                                    waysToWatchContainer.Entitled = element.entitled.value();
                                                }
                                                if (element.entitledExpires.has_value()) {
                                                    waysToWatchContainer.EntitledExpires = element.entitledExpires.value();
                                                }
                                                if (element.offeringType.has_value()) {
                                                    waysToWatchContainer.OfferingType = element.offeringType.value();
                                                }
                                                if (element.hasAds.has_value()) {
                                                    waysToWatchContainer.HasAds = element.hasAds.value();
                                                }
                                                if (element.price.has_value()) {
                                                    waysToWatchContainer.Price = element.price.value();
                                                }
                                                if (element.videoQuality.has_value()) {
                                                    WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatchVideoQuality> videoQualityArray;
                                                    std::vector<Entertainment::WayToWatchVideoQuality> videoQuality = element.videoQuality.value();
                                                    for (auto& element : videoQuality) {
                                                        videoQualityArray.Add() = element;
                                                    }
                                                    waysToWatchContainer.VideoQuality = videoQualityArray;
                                                }
                                                WPEFramework::Core::JSON::ArrayType<Firebolt::Types::JsonData_AudioProfile> audioProfileArray;
                                                std::vector<Types::AudioProfile> audioProfile = element.audioProfile;
                                                for (auto& element : audioProfile) {
                                                    audioProfileArray.Add() = element;
                                                }
                                                waysToWatchContainer.AudioProfile = audioProfileArray;
                                                if (element.audioLanguages.has_value()) {
                                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioLanguagesArray;
                                                    std::vector<std::string> audioLanguages = element.audioLanguages.value();
                                                    for (auto& element : audioLanguages) {
                                                        audioLanguagesArray.Add() = element;
                                                    }
                                                    waysToWatchContainer.AudioLanguages = audioLanguagesArray;
                                                }
                                                if (element.closedCaptions.has_value()) {
                                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> closedCaptionsArray;
                                                    std::vector<std::string> closedCaptions = element.closedCaptions.value();
                                                    for (auto& element : closedCaptions) {
                                                        closedCaptionsArray.Add() = element;
                                                    }
                                                    waysToWatchContainer.ClosedCaptions = closedCaptionsArray;
                                                }
                                                if (element.subtitles.has_value()) {
                                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> subtitlesArray;
                                                    std::vector<std::string> subtitles = element.subtitles.value();
                                                    for (auto& element : subtitles) {
                                                        subtitlesArray.Add() = element;
                                                    }
                                                    waysToWatchContainer.Subtitles = subtitlesArray;
                                                }
                                                if (element.audioDescriptions.has_value()) {
                                                    WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioDescriptionsArray;
                                                    std::vector<std::string> audioDescriptions = element.audioDescriptions.value();
                                                    for (auto& element : audioDescriptions) {
                                                        audioDescriptionsArray.Add() = element;
                                                    }
                                                    waysToWatchContainer.AudioDescriptions = audioDescriptionsArray;
                                                }
                                            }
                                            waysToWatchArray.Add() = waysToWatchContainer;
                                        }
                                        relatedContainer.WaysToWatch = waysToWatchArray;
                                    }
                                }
                                relatedArray.Add() = relatedContainer;
                            }
                            entityInfoResultContainer.Related = relatedArray;
                        }
                    }
                }
                string resultStr;
                entityInfoResultContainer.ToString(resultStr);
                WPEFramework::Core::JSON::VariantContainer resultContainer(resultStr);
                WPEFramework::Core::JSON::Variant Result = resultContainer;
                jsonParameters.Set(_T("result"), Result);
                WPEFramework::Core::JSON::Boolean jsonResult;

                status = transport->Invoke("discovery.entityInfo", jsonParameters, jsonResult);
                if (status == Firebolt::Error::None) {
                    FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.onPullEntityInfo is successfully pushed with status as %d", jsonResult.Value());
                }

            } else {
                FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", status);
            }
            proxyResponse.Release();
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnPullEntityInfoNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onPullEntityInfo");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;
        status = FireboltSDK::Event::Instance().Subscribe<JsonData_EntityInfoFederatedRequest>(eventName, jsonParameters, onPullEntityInfoInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnPullEntityInfoNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onPullEntityInfo"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }

    /* onPullPurchasedContent - Return content purchased by the user, such as rentals and electronic sell through purchases.

The app should return the user's 100 most recent purchases in `entries`. The total count of purchases must be provided in `count`. If `count` is greater than the total number of `entries`, the UI may provide a link into the app to see the complete purchase list.

The `EntityInfo` object returned is not required to have `waysToWatch` populated, but it is recommended that it do so in case the UI wants to surface additional information on the purchases screen.

The app should implement both Push and Pull methods for `purchasedContent`.

The app should actively push `purchasedContent` when:

*  The app becomes Active.
*  When the state of the purchasedContent set has changed.
*  The app goes into Inactive or Background state, if there is a chance a change event has been missed. */
    static void onPullPurchasedContentInnerCallback( void* notification, const void* userData, void* jsonResponse )
    {
        WPEFramework::Core::ProxyType<JsonData_PurchasedContentFederatedRequest>& proxyResponse = *(reinterpret_cast<WPEFramework::Core::ProxyType<JsonData_PurchasedContentFederatedRequest>*>(jsonResponse));

        ASSERT(proxyResponse.IsValid() == true);

        if (proxyResponse.IsValid() == true) {
            JsonData_PurchasedContentParameters jsonResult = proxyResponse->Parameters;
            PurchasedContentParameters purchasedContentParameters;
            PurchasedContentParameters purchasedContentParametersResult;
            purchasedContentParametersResult.limit = jsonResult.Limit.Value();
            if (jsonResult.OfferingType.IsSet()) {
                purchasedContentParametersResult.offeringType = jsonResult.OfferingType.Value();
            }
            if (jsonResult.ProgramType.IsSet()) {
                purchasedContentParametersResult.programType = jsonResult.ProgramType.Value();
            }
            purchasedContentParameters = purchasedContentParametersResult;

            IDiscovery::IOnPullPurchasedContentNotification& notifier = *(reinterpret_cast<IDiscovery::IOnPullPurchasedContentNotification*>(notification));
            PurchasedContentResult element = notifier.onPullPurchasedContent(purchasedContentParameters);
            Firebolt::Error status = Firebolt::Error::NotConnected;
            FireboltSDK::Transport<WPEFramework::Core::JSON::IElement>* transport = FireboltSDK::Accessor::Instance().GetTransport();
            if (transport != nullptr) {
                JsonObject jsonParameters;
                WPEFramework::Core::JSON::Variant CorrelationId = proxyResponse->CorrelationId.Value();
                jsonParameters.Set(_T("correlationId"), CorrelationId);
                JsonData_PurchasedContentResult purchasedContentResultContainer;
                {
                    {
                        purchasedContentResultContainer.Expires = element.expires;
                        purchasedContentResultContainer.TotalCount = element.totalCount;
                        WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_EntityInfo> entriesArray;
                        std::vector<Entertainment::EntityInfo> entries = element.entries;
                        for (auto& element : entries) {
                            Firebolt::Entertainment::JsonData_EntityInfo entriesContainer;
                            {
                                {
                                    if (element.identifiers.assetId.has_value()) {
                                        entriesContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                    }
                                    if (element.identifiers.entityId.has_value()) {
                                        entriesContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                    }
                                    if (element.identifiers.seasonId.has_value()) {
                                        entriesContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                    }
                                    if (element.identifiers.seriesId.has_value()) {
                                        entriesContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                    }
                                    if (element.identifiers.appContentData.has_value()) {
                                        entriesContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                    }
                                }
                                entriesContainer.Title = element.title;
                                entriesContainer.EntityType = element.entityType;
                                if (element.programType.has_value()) {
                                    entriesContainer.ProgramType = element.programType.value();
                                }
                                if (element.musicType.has_value()) {
                                    entriesContainer.MusicType = element.musicType.value();
                                }
                                if (element.synopsis.has_value()) {
                                    entriesContainer.Synopsis = element.synopsis.value();
                                }
                                if (element.seasonNumber.has_value()) {
                                    entriesContainer.SeasonNumber = element.seasonNumber.value();
                                }
                                if (element.seasonCount.has_value()) {
                                    entriesContainer.SeasonCount = element.seasonCount.value();
                                }
                                if (element.episodeNumber.has_value()) {
                                    entriesContainer.EpisodeNumber = element.episodeNumber.value();
                                }
                                if (element.episodeCount.has_value()) {
                                    entriesContainer.EpisodeCount = element.episodeCount.value();
                                }
                                if (element.releaseDate.has_value()) {
                                    entriesContainer.ReleaseDate = element.releaseDate.value();
                                }
                                if (element.contentRatings.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_ContentRating> contentRatingsArray;
                                    std::vector<Entertainment::ContentRating> contentRatings = element.contentRatings.value();
                                    for (auto& element : contentRatings) {
                                        Firebolt::Entertainment::JsonData_ContentRating contentRatingsContainer;
                                        {
                                            contentRatingsContainer.Scheme = element.scheme;
                                            contentRatingsContainer.Rating = element.rating;
                                            if (element.advisories.has_value()) {
                                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> advisoriesArray;
                                                std::vector<std::string> advisories = element.advisories.value();
                                                for (auto& element : advisories) {
                                                    advisoriesArray.Add() = element;
                                                }
                                                contentRatingsContainer.Advisories = advisoriesArray;
                                            }
                                        }
                                        contentRatingsArray.Add() = contentRatingsContainer;
                                    }
                                    entriesContainer.ContentRatings = contentRatingsArray;
                                }
                                if (element.waysToWatch.has_value()) {
                                    WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatch> waysToWatchArray;
                                    std::vector<Entertainment::WayToWatch> waysToWatch = element.waysToWatch.value();
                                    for (auto& element : waysToWatch) {
                                        Firebolt::Entertainment::JsonData_WayToWatch waysToWatchContainer;
                                        {
                                            {
                                                if (element.identifiers.assetId.has_value()) {
                                                    waysToWatchContainer.Identifiers.AssetId = element.identifiers.assetId.value();
                                                }
                                                if (element.identifiers.entityId.has_value()) {
                                                    waysToWatchContainer.Identifiers.EntityId = element.identifiers.entityId.value();
                                                }
                                                if (element.identifiers.seasonId.has_value()) {
                                                    waysToWatchContainer.Identifiers.SeasonId = element.identifiers.seasonId.value();
                                                }
                                                if (element.identifiers.seriesId.has_value()) {
                                                    waysToWatchContainer.Identifiers.SeriesId = element.identifiers.seriesId.value();
                                                }
                                                if (element.identifiers.appContentData.has_value()) {
                                                    waysToWatchContainer.Identifiers.AppContentData = element.identifiers.appContentData.value();
                                                }
                                            }
                                            if (element.expires.has_value()) {
                                                waysToWatchContainer.Expires = element.expires.value();
                                            }
                                            if (element.entitled.has_value()) {
                                                waysToWatchContainer.Entitled = element.entitled.value();
                                            }
                                            if (element.entitledExpires.has_value()) {
                                                waysToWatchContainer.EntitledExpires = element.entitledExpires.value();
                                            }
                                            if (element.offeringType.has_value()) {
                                                waysToWatchContainer.OfferingType = element.offeringType.value();
                                            }
                                            if (element.hasAds.has_value()) {
                                                waysToWatchContainer.HasAds = element.hasAds.value();
                                            }
                                            if (element.price.has_value()) {
                                                waysToWatchContainer.Price = element.price.value();
                                            }
                                            if (element.videoQuality.has_value()) {
                                                WPEFramework::Core::JSON::ArrayType<Firebolt::Entertainment::JsonData_WayToWatchVideoQuality> videoQualityArray;
                                                std::vector<Entertainment::WayToWatchVideoQuality> videoQuality = element.videoQuality.value();
                                                for (auto& element : videoQuality) {
                                                    videoQualityArray.Add() = element;
                                                }
                                                waysToWatchContainer.VideoQuality = videoQualityArray;
                                            }
                                            WPEFramework::Core::JSON::ArrayType<Firebolt::Types::JsonData_AudioProfile> audioProfileArray;
                                            std::vector<Types::AudioProfile> audioProfile = element.audioProfile;
                                            for (auto& element : audioProfile) {
                                                audioProfileArray.Add() = element;
                                            }
                                            waysToWatchContainer.AudioProfile = audioProfileArray;
                                            if (element.audioLanguages.has_value()) {
                                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioLanguagesArray;
                                                std::vector<std::string> audioLanguages = element.audioLanguages.value();
                                                for (auto& element : audioLanguages) {
                                                    audioLanguagesArray.Add() = element;
                                                }
                                                waysToWatchContainer.AudioLanguages = audioLanguagesArray;
                                            }
                                            if (element.closedCaptions.has_value()) {
                                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> closedCaptionsArray;
                                                std::vector<std::string> closedCaptions = element.closedCaptions.value();
                                                for (auto& element : closedCaptions) {
                                                    closedCaptionsArray.Add() = element;
                                                }
                                                waysToWatchContainer.ClosedCaptions = closedCaptionsArray;
                                            }
                                            if (element.subtitles.has_value()) {
                                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> subtitlesArray;
                                                std::vector<std::string> subtitles = element.subtitles.value();
                                                for (auto& element : subtitles) {
                                                    subtitlesArray.Add() = element;
                                                }
                                                waysToWatchContainer.Subtitles = subtitlesArray;
                                            }
                                            if (element.audioDescriptions.has_value()) {
                                                WPEFramework::Core::JSON::ArrayType<FireboltSDK::JSON::String> audioDescriptionsArray;
                                                std::vector<std::string> audioDescriptions = element.audioDescriptions.value();
                                                for (auto& element : audioDescriptions) {
                                                    audioDescriptionsArray.Add() = element;
                                                }
                                                waysToWatchContainer.AudioDescriptions = audioDescriptionsArray;
                                            }
                                        }
                                        waysToWatchArray.Add() = waysToWatchContainer;
                                    }
                                    entriesContainer.WaysToWatch = waysToWatchArray;
                                }
                            }
                            entriesArray.Add() = entriesContainer;
                        }
                        purchasedContentResultContainer.Entries = entriesArray;
                    }
                }
                string resultStr;
                purchasedContentResultContainer.ToString(resultStr);
                WPEFramework::Core::JSON::VariantContainer resultContainer(resultStr);
                WPEFramework::Core::JSON::Variant Result = resultContainer;
                jsonParameters.Set(_T("result"), Result);
                WPEFramework::Core::JSON::Boolean jsonResult;

                status = transport->Invoke("discovery.purchasedContent", jsonParameters, jsonResult);
                if (status == Firebolt::Error::None) {
                    FIREBOLT_LOG_INFO(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Discovery.onPullPurchasedContent is successfully pushed with status as %d", jsonResult.Value());
                }

            } else {
                FIREBOLT_LOG_ERROR(FireboltSDK::Logger::Category::OpenRPC, FireboltSDK::Logger::Module<FireboltSDK::Accessor>(), "Error in getting Transport err = %d", status);
            }
            proxyResponse.Release();
        }
    }
    void DiscoveryImpl::subscribe( IDiscovery::IOnPullPurchasedContentNotification& notification, Firebolt::Error *err )
    {
        const string eventName = _T("discovery.onPullPurchasedContent");
        Firebolt::Error status = Firebolt::Error::None;

        JsonObject jsonParameters;
        status = FireboltSDK::Event::Instance().Subscribe<JsonData_PurchasedContentFederatedRequest>(eventName, jsonParameters, onPullPurchasedContentInnerCallback, reinterpret_cast<void*>(&notification), nullptr);

        if (err != nullptr) {
            *err = status;
        }
    }
    void DiscoveryImpl::unsubscribe( IDiscovery::IOnPullPurchasedContentNotification& notification, Firebolt::Error *err )
    {
        Firebolt::Error status = FireboltSDK::Event::Instance().Unsubscribe(_T("discovery.onPullPurchasedContent"), reinterpret_cast<void*>(&notification));

        if (err != nullptr) {
            *err = status;
        }
    }


}//namespace Discovery
}


namespace WPEFramework {

    /* AvailabilityType  */
    ENUM_CONVERSION_BEGIN(Firebolt::Discovery::AvailabilityType)
        { Firebolt::Discovery::AvailabilityType::CHANNEL_LINEUP, _T("channel-lineup") },
        { Firebolt::Discovery::AvailabilityType::PROGRAM_LINEUP, _T("program-lineup") },
    ENUM_CONVERSION_END(Firebolt::Discovery::AvailabilityType)

}