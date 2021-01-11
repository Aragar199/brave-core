/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BAT_ADS_INTERNAL_CATALOG_CATALOG_BRAVE_TODAY_AD_PAYLOAD_INFO_H_
#define BAT_ADS_INTERNAL_CATALOG_CATALOG_BRAVE_TODAY_AD_PAYLOAD_INFO_H_

#include <string>

namespace ads {

struct CatalogBraveTodayAdPayloadInfo {
  CatalogBraveTodayAdPayloadInfo();
  CatalogBraveTodayAdPayloadInfo(
      const CatalogBraveTodayAdPayloadInfo& info);
  ~CatalogBraveTodayAdPayloadInfo();

  bool operator==(
      const CatalogBraveTodayAdPayloadInfo& rhs) const;
  bool operator!=(
      const CatalogBraveTodayAdPayloadInfo& rhs) const;

  std::string company_name;
  std::string alt;
  std::string target_url;
};

}  // namespace ads

#endif  // BAT_ADS_INTERNAL_CATALOG_CATALOG_BRAVE_TODAY_AD_PAYLOAD_INFO_H_
