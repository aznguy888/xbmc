/*
 *      Copyright (C) 2016 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "addons/AddonBuilder.h"
#include "addons/AudioDecoder.h"
#include "addons/AudioEncoder.h"
#include "addons/ContextMenuAddon.h"
#include "addons/ImageResource.h"
#include "addons/LanguageResource.h"
#include "addons/PluginSource.h"
#include "addons/Repository.h"
#include "addons/ScreenSaver.h"
#include "addons/Service.h"
#include "addons/Skin.h"
#include "addons/UISoundsResource.h"
#include "addons/Visualisation.h"
#include "addons/Webinterface.h"
#include "cores/AudioEngine/DSPAddons/ActiveAEDSPAddon.h"
#include "pvr/addons/PVRClient.h"


namespace ADDON
{

std::shared_ptr<IAddon> CAddonBuilder::Build()
{
  if (m_built)
    throw std::logic_error("Already built");

  if (m_props.id.empty())
    return nullptr;

  m_built = true;

  if (m_props.type == ADDON_UNKNOWN)
    return std::make_shared<CAddon>(std::move(m_props));

  if (m_extPoint == nullptr)
    return FromProps(std::move(m_props));

  const TYPE type(m_props.type);
  switch (type)
  {
    case ADDON_PLUGIN:
    case ADDON_SCRIPT:
      return CPluginSource::FromExtension(std::move(m_props), m_extPoint);
    case ADDON_SCRIPT_LIBRARY:
    case ADDON_SCRIPT_LYRICS:
    case ADDON_SCRIPT_MODULE:
    case ADDON_SUBTITLE_MODULE:
    case ADDON_SCRIPT_WEATHER:
      return std::make_shared<CAddon>(std::move(m_props));
    case ADDON_WEB_INTERFACE:
      return CWebinterface::FromExtension(std::move(m_props), m_extPoint);
    case ADDON_SERVICE:
      return CService::FromExtension(std::move(m_props), m_extPoint);
    case ADDON_SCRAPER_ALBUMS:
    case ADDON_SCRAPER_ARTISTS:
    case ADDON_SCRAPER_MOVIES:
    case ADDON_SCRAPER_MUSICVIDEOS:
    case ADDON_SCRAPER_TVSHOWS:
    case ADDON_SCRAPER_LIBRARY:
      return CScraper::FromExtension(std::move(m_props), m_extPoint);
    case ADDON_VIZ:
    case ADDON_SCREENSAVER:
    case ADDON_PVRDLL:
    case ADDON_ADSPDLL:
    case ADDON_AUDIOENCODER:
    case ADDON_AUDIODECODER:
    { // begin temporary platform handling for Dlls
      // ideally platforms issues will be handled by C-Pluff
      // this is not an attempt at a solution
      std::string value;
      if (type == ADDON_SCREENSAVER && 0 == strnicmp(m_extPoint->plugin->identifier, "screensaver.xbmc.builtin.", 25))
      { // built in screensaver
        return std::make_shared<CAddon>(std::move(m_props));
      }
      if (type == ADDON_SCREENSAVER)
      { // Python screensaver
        std::string library = CAddonMgr::GetInstance().GetExtValue(m_extPoint->configuration, "@library");
        if (URIUtils::HasExtension(library, ".py"))
          return std::make_shared<CScreenSaver>(std::move(m_props));
      }
      if (type == ADDON_AUDIOENCODER && 0 == strncmp(m_extPoint->plugin->identifier,
          "audioencoder.xbmc.builtin.", 26))
      { // built in audio encoder
        return CAudioEncoder::FromExtension(std::move(m_props), m_extPoint);
      }

      value = CAddonMgr::GetInstance().GetPlatformLibraryName(m_extPoint->plugin->extensions->configuration);
      if (value.empty())
        break;
      if (type == ADDON_VIZ)
      {
#if defined(HAS_VISUALISATION)
        return std::make_shared<CVisualisation>(std::move(m_props));
#endif
      }
      else if (type == ADDON_PVRDLL)
      {
#ifdef HAS_PVRCLIENTS
        return PVR::CPVRClient::FromExtension(std::move(m_props), m_extPoint);
#endif
      }
      else if (type == ADDON_ADSPDLL)
      {
        return std::make_shared<ActiveAE::CActiveAEDSPAddon>(std::move(m_props));
      }
      else if (type == ADDON_AUDIOENCODER)
        return CAudioEncoder::FromExtension(std::move(m_props), m_extPoint);
      else if (type == ADDON_AUDIODECODER)
        return CAudioDecoder::FromExtension(std::move(m_props), m_extPoint);
      else
        return std::make_shared<CScreenSaver>(std::move(m_props));;
    }
    case ADDON_SKIN:
      return CSkinInfo::FromExtension(std::move(m_props), m_extPoint);
    case ADDON_RESOURCE_IMAGES:
      return CImageResource::FromExtension(std::move(m_props), m_extPoint);
    case ADDON_RESOURCE_LANGUAGE:
      return CLanguageResource::FromExtension(std::move(m_props), m_extPoint);
    case ADDON_RESOURCE_UISOUNDS:
      return std::make_shared<CUISoundsResource>(std::move(m_props));
    case ADDON_REPOSITORY:
      return CRepository::FromExtension(std::move(m_props), m_extPoint);
    case ADDON_CONTEXT_ITEM:
      return CContextMenuAddon::FromExtension(std::move(m_props), m_extPoint);
    default:
      break;
  }
  return AddonPtr();
}


AddonPtr CAddonBuilder::FromProps(AddonProps addonProps)
{
  // FIXME: there is no need for this as none of the derived classes will contain any useful
  // information. We should return CAddon instances only, however there are several places that
  // down casts, which need to fixed first.
  switch (addonProps.type)
  {
    case ADDON_PLUGIN:
    case ADDON_SCRIPT:
      return AddonPtr(new CPluginSource(std::move(addonProps)));
    case ADDON_SCRIPT_LIBRARY:
    case ADDON_SCRIPT_LYRICS:
    case ADDON_SCRIPT_WEATHER:
    case ADDON_SCRIPT_MODULE:
    case ADDON_SUBTITLE_MODULE:
      return AddonPtr(new CAddon(std::move(addonProps)));
    case ADDON_WEB_INTERFACE:
      return AddonPtr(new CWebinterface(std::move(addonProps)));
    case ADDON_SERVICE:
      return AddonPtr(new CService(std::move(addonProps)));
    case ADDON_SCRAPER_ALBUMS:
    case ADDON_SCRAPER_ARTISTS:
    case ADDON_SCRAPER_MOVIES:
    case ADDON_SCRAPER_MUSICVIDEOS:
    case ADDON_SCRAPER_TVSHOWS:
    case ADDON_SCRAPER_LIBRARY:
      return AddonPtr(new CScraper(std::move(addonProps)));
    case ADDON_SKIN:
      return AddonPtr(new CSkinInfo(std::move(addonProps)));
#if defined(HAS_VISUALISATION)
    case ADDON_VIZ:
      return AddonPtr(new CVisualisation(std::move(addonProps)));
#endif
    case ADDON_SCREENSAVER:
      return AddonPtr(new CScreenSaver(std::move(addonProps)));
    case ADDON_PVRDLL:
      return AddonPtr(new PVR::CPVRClient(std::move(addonProps)));
    case ADDON_ADSPDLL:
      return AddonPtr(new ActiveAE::CActiveAEDSPAddon(std::move(addonProps)));
    case ADDON_AUDIOENCODER:
      return AddonPtr(new CAudioEncoder(std::move(addonProps)));
    case ADDON_AUDIODECODER:
      return AddonPtr(new CAudioDecoder(std::move(addonProps)));
    case ADDON_RESOURCE_IMAGES:
      return AddonPtr(new CImageResource(std::move(addonProps)));
    case ADDON_RESOURCE_LANGUAGE:
      return AddonPtr(new CLanguageResource(std::move(addonProps)));
    case ADDON_RESOURCE_UISOUNDS:
      return AddonPtr(new CUISoundsResource(std::move(addonProps)));
    case ADDON_REPOSITORY:
      return AddonPtr(new CRepository(std::move(addonProps)));
    case ADDON_CONTEXT_ITEM:
      return AddonPtr(new CContextMenuAddon(std::move(addonProps)));
    default:
      break;
  }
  return AddonPtr();
}
}