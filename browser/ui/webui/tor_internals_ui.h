/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_WEBUI_TOR_INTERNALS_UI_H_
#define BRAVE_BROWSER_UI_WEBUI_TOR_INTERNALS_UI_H_

#include <memory>
#include <string>
#include <vector>

#include "brave/browser/ui/webui/basic_ui.h"
#include "brave/components/tor/tor_launcher_observer.h"
#include "content/public/browser/web_ui_message_handler.h"

class TorLauncherFactory;

namespace content {
class BrowserContext;
}  // namespace content

class TorInternalsDOMHandler : public content::WebUIMessageHandler,
                               public TorLauncherObserver {
 public:
  TorInternalsDOMHandler(content::BrowserContext* context);
  ~TorInternalsDOMHandler() override;

  // WebUIMessageHandler implementation.
  void RegisterMessages() override;

 private:
  void HandleGetTorGeneralInfo(const base::ListValue* args);
  void HandleGetTorLog(const base::ListValue* args);

  void OnGetTorLog(bool success, const std::string& log);

  // tor::TorLauncherObserver:
  void OnTorCircuitEstablished(bool result) override;
  void OnTorInitializing(const std::string& percentage) override;
  void OnTorControlEvent(const std::string& event) override;

  TorLauncherFactory* tor_launcher_factory_ = nullptr;

  base::WeakPtrFactory<TorInternalsDOMHandler> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(TorInternalsDOMHandler);
};

class TorInternalsUI : public BasicUI {
 public:
  TorInternalsUI(content::WebUI* web_ui, const std::string& host);
  ~TorInternalsUI() override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TorInternalsUI);
};

#endif  // BRAVE_BROWSER_UI_WEBUI_TOR_INTERNALS_UI_H_
