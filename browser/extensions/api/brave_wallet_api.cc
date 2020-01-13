/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/extensions/api/brave_wallet_api.h"

#include <memory>
#include <string>

#include "base/environment.h"
#include "brave/browser/infobars/crypto_wallets_infobar_delegate.h"
#include "brave/browser/profiles/profile_util.h"
#include "brave/common/extensions/api/brave_wallet.h"
#include "brave/common/extensions/extension_constants.h"
#include "brave/common/pref_names.h"
#include "chrome/browser/extensions/api/tabs/tabs_constants.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "chrome/browser/infobars/infobar_service.h"
#include "chrome/browser/profiles/profile.h"
#include "components/prefs/pref_service.h"
#include "content/public/browser/web_contents.h"
#include "extensions/browser/extension_registry.h"
#include "extensions/browser/extension_util.h"
#include "brave/components/brave_wallet/browser/brave_wallet_service_factory.h"
#include "brave/components/brave_wallet/browser/brave_wallet_controller.h"
#include "brave/components/brave_wallet/browser/brave_wallet_service.h"

namespace {

BraveWalletController* GetBraveWalletController(
    content::BrowserContext* context) {
  return BraveWalletServiceFactory::GetInstance()
      ->GetForProfile(Profile::FromBrowserContext(context))
      ->controller();
}

}  // namespace

namespace extensions {
namespace api {

ExtensionFunction::ResponseAction
BraveWalletPromptToEnableWalletFunction::Run() {
  std::unique_ptr<brave_wallet::PromptToEnableWallet::Params> params(
      brave_wallet::PromptToEnableWallet::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params.get());

  Profile* profile = Profile::FromBrowserContext(browser_context());
  if (brave::IsTorProfile(profile)) {
    return RespondNow(Error("Not available in Tor profile"));
  }

  // Get web contents for this tab
  content::WebContents* contents = nullptr;
  if (!ExtensionTabUtil::GetTabById(
        params->tab_id,
        Profile::FromBrowserContext(browser_context()),
        include_incognito_information(),
        nullptr,
        nullptr,
        &contents,
        nullptr)) {
    return RespondNow(Error(tabs_constants::kTabNotFoundError,
                            base::NumberToString(params->tab_id)));
  }

  InfoBarService* infobar_service =
      InfoBarService::FromWebContents(contents);
  if (infobar_service) {
    auto* registry = extensions::ExtensionRegistry::Get(profile);
    bool metamask_enabled = !brave::IsTorProfile(profile) &&
      registry->ready_extensions().GetByID(metamask_extension_id);
    CryptoWalletsInfoBarDelegate::Create(infobar_service, metamask_enabled);
  }

  return RespondNow(NoArguments());
}

ExtensionFunction::ResponseAction
BraveWalletIsInstalledFunction::Run() {
  Profile* profile = Profile::FromBrowserContext(browser_context());
  auto* registry = extensions::ExtensionRegistry::Get(profile);
  bool enabled = !brave::IsTorProfile(profile) &&
    registry->ready_extensions().GetByID(ethereum_remote_client_extension_id);
  return RespondNow(OneArgument(std::make_unique<base::Value>(enabled)));
}

ExtensionFunction::ResponseAction
BraveWalletIsEnabledFunction::Run() {
  Profile* profile = Profile::FromBrowserContext(browser_context());
  bool enabled = !brave::IsTorProfile(profile) &&
    profile->GetPrefs()->GetBoolean(kBraveWalletEnabled);
  return RespondNow(OneArgument(std::make_unique<base::Value>(enabled)));
}

ExtensionFunction::ResponseAction
BraveWalletGetWalletSeedFunction::Run() {
  // make sure the passed in enryption key is 32 bytes.
  std::unique_ptr<brave_wallet::GetWalletSeed::Params> params(
    brave_wallet::GetWalletSeed::Params::Create(*args_));
  if (params->key.size() != 32) {
    return RespondNow(Error("Invalid input key size"));
  }

  auto* controller = GetBraveWalletController(browser_context());

  base::Value::BlobStorage blob;
  std::string derived = controller->GetWalletSeed(
      params->key);

  if (derived.empty()) {
    return RespondNow(Error("Error getting wallet seed"));
  }

  blob.assign(derived.begin(), derived.end());

  return RespondNow(OneArgument(
    std::make_unique<base::Value>(blob)));
}

ExtensionFunction::ResponseAction
BraveWalletGetProjectIDFunction::Run() {
  std::string project_id(BRAVE_INFURA_PROJECT_ID);
  std::unique_ptr<base::Environment> env(base::Environment::Create());
  if (env->HasVar("BRAVE_INFURA_PROJECT_ID")) {
    env->GetVar("BRAVE_INFURA_PROJECT_ID", &project_id);
  }
  return RespondNow(OneArgument(
      std::make_unique<base::Value>(project_id)));
}

ExtensionFunction::ResponseAction
BraveWalletResetWalletFunction::Run() {
  auto* controller = GetBraveWalletController(browser_context());
  controller->ResetCryptoWallets();
  return RespondNow(NoArguments());
}

}  // namespace api
}  // namespace extensions
