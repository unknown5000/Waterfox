/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/widget/NativeMenuSupport.h"

#include "mozilla/StaticPrefs_widget.h"
#include "MainThreadUtils.h"
#include "NativeMenuGtk.h"
#include "DBusMenu.h"
#include "nsWindow.h"
#include "nsINativeMenuService.h"
#include "nsServiceManagerUtils.h"

namespace mozilla::widget {

void NativeMenuSupport::CreateNativeMenuBar(nsIWidget* aParent,
                                            dom::Element* aMenuBarElement) {
  MOZ_RELEASE_ASSERT(NS_IsMainThread(),
                     "Attempting to create native menu bar on wrong thread!");

  nsCOMPtr<nsINativeMenuService> nms =
      do_GetService("@mozilla.org/widget/nativemenuservice;1");
  if (!nms) {
    return;
  }

  nms->CreateNativeMenuBar(aParent, aMenuBarElement);

#ifdef MOZ_ENABLE_DBUS
  if (aMenuBarElement && StaticPrefs::widget_gtk_global_menu_enabled() &&
      DBusMenuFunctions::Init()) {
    static_cast<nsWindow*>(aParent)->SetDBusMenuBar(
        DBusMenuBar::Create(aMenuBarElement));
  }
#endif
}

already_AddRefed<NativeMenu> NativeMenuSupport::CreateNativeContextMenu(
    dom::Element* aPopup) {
  return MakeAndAddRef<NativeMenuGtk>(aPopup);
}

bool NativeMenuSupport::ShouldUseNativeContextMenus() {
  return NativeMenuGtk::CanUse();
}

}  // namespace mozilla::widget
