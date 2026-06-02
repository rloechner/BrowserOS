diff --git a/chrome/browser/browseros/universe/stacker_universe_ui.h b/chrome/browser/browseros/universe/stacker_universe_ui.h
new file mode 100644
index 0000000000000..0000000000000
--- /dev/null
+++ b/chrome/browser/browseros/universe/stacker_universe_ui.h
@@ -0,0 +1,175 @@
+// Copyright 2026 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef CHROME_BROWSER_BROWSEROS_UNIVERSE_STACKER_UNIVERSE_UI_H_
+#define CHROME_BROWSER_BROWSEROS_UNIVERSE_STACKER_UNIVERSE_UI_H_
+
+#include <memory>
+#include <string>
+
+#include "base/functional/bind.h"
+#include "base/memory/raw_ptr.h"
+#include "base/strings/strcat.h"
+#include "base/strings/string_number_conversions.h"
+#include "base/strings/utf_string_conversions.h"
+#include "chrome/browser/profiles/profile.h"
+#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
+#include "chrome/browser/ui/browser_window/public/browser_window_interface_iterator.h"
+#include "chrome/browser/ui/tabs/tab_strip_model.h"
+#include "content/public/browser/web_contents.h"
+#include "ui/base/base_window.h"
+#include "ui/base/models/dialog_model.h"
+#include "ui/base/models/image_model.h"
+#include "ui/views/bubble/bubble_dialog_delegate_view.h"
+#include "ui/views/bubble/bubble_dialog_model_host.h"
+#include "ui/views/controls/button/label_button.h"
+#include "ui/views/view.h"
+#include "ui/views/widget/widget.h"
+
+namespace browseros::universe {
+
+namespace internal {
+
+inline int WindowIdFor(BrowserWindowInterface* browser_window) {
+  return browser_window ? browser_window->GetSessionID().id() : -1;
+}
+
+inline BrowserWindowInterface* FindBrowserWindow(int window_id) {
+  BrowserWindowInterface* result = nullptr;
+  ForEachCurrentBrowserWindowInterfaceOrderedByActivation(
+      [window_id, &result](BrowserWindowInterface* browser_window) {
+        if (WindowIdFor(browser_window) == window_id) {
+          result = browser_window;
+          return false;
+        }
+        return true;
+      });
+  return result;
+}
+
+inline std::u16string WindowTitle(BrowserWindowInterface* browser_window) {
+  if (!browser_window || !browser_window->GetTabStripModel()) {
+    return u"Window";
+  }
+
+  content::WebContents* active_contents =
+      browser_window->GetTabStripModel()->GetActiveWebContents();
+  if (!active_contents || active_contents->GetTitle().empty()) {
+    return u"New Window";
+  }
+  return active_contents->GetTitle();
+}
+
+inline std::u16string ProfileName(BrowserWindowInterface* browser_window) {
+  Profile* profile = browser_window ? browser_window->GetProfile() : nullptr;
+  if (!profile) {
+    return u"Profile";
+  }
+  return base::UTF8ToUTF16(profile->GetDebugName());
+}
+
+inline void ActivateWindow(int window_id, int event_flags) {
+  BrowserWindowInterface* browser_window = FindBrowserWindow(window_id);
+  if (browser_window && browser_window->GetWindow()) {
+    browser_window->GetWindow()->Activate();
+  }
+}
+
+inline void MinimizeWindow(int window_id) {
+  BrowserWindowInterface* browser_window = FindBrowserWindow(window_id);
+  if (browser_window && browser_window->GetWindow()) {
+    browser_window->GetWindow()->Minimize();
+  }
+}
+
+inline void RestoreWindow(int window_id) {
+  BrowserWindowInterface* browser_window = FindBrowserWindow(window_id);
+  if (browser_window && browser_window->GetWindow()) {
+    browser_window->GetWindow()->Restore();
+    browser_window->GetWindow()->Activate();
+  }
+}
+
+inline std::u16string WindowMenuLabel(BrowserWindowInterface* browser_window) {
+  return base::StrCat({WindowTitle(browser_window), u" - ",
+                       ProfileName(browser_window)});
+}
+
+inline std::unique_ptr<ui::DialogModel> CreateStackerUniverseDialogModel(
+    BrowserWindowInterface* browser_window) {
+  const int current_window_id = WindowIdFor(browser_window);
+  ui::DialogModel::Builder builder;
+  builder.SetInternalName("StackerUniverse")
+      .SetTitle(u"Stacker Universe")
+      .OverrideShowCloseButton(true)
+      .AddParagraph(ui::DialogModelLabel(
+          u"MVP universe controls for the current BrowserOS windows and "
+          u"profiles."));
+
+  if (current_window_id >= 0) {
+    builder.AddOkButton(
+        base::BindOnce(&RestoreWindow, current_window_id),
+        ui::DialogModel::Button::Params().SetLabel(u"Restore current"));
+    builder.AddCancelButton(
+        base::BindOnce(&MinimizeWindow, current_window_id),
+        ui::DialogModel::Button::Params().SetLabel(u"Collapse current"));
+  }
+
+  bool has_windows = false;
+  ForEachCurrentBrowserWindowInterfaceOrderedByActivation(
+      [&builder, &has_windows](BrowserWindowInterface* open_browser_window) {
+        has_windows = true;
+        builder.AddMenuItem(
+            ui::ImageModel(), WindowMenuLabel(open_browser_window),
+            base::BindRepeating(&ActivateWindow,
+                                WindowIdFor(open_browser_window)));
+        return true;
+      });
+
+  if (!has_windows) {
+    builder.AddParagraph(ui::DialogModelLabel(u"No BrowserOS windows found."));
+  }
+
+  return builder.Build();
+}
+
+inline void ShowStackerUniversePanel(views::View* anchor,
+                                     BrowserWindowInterface* browser_window) {
+  auto bubble = std::make_unique<views::BubbleDialogModelHost>(
+      CreateStackerUniverseDialogModel(browser_window), anchor,
+      views::BubbleBorder::TOP_LEFT);
+  views::BubbleDialogDelegate::CreateBubble(std::move(bubble))->Show();
+}
+
+}  // namespace internal
+
+class StackerUniverseButton : public views::LabelButton {
+ public:
+  StackerUniverseButton(BrowserWindowInterface* browser_window, bool compact)
+      : views::LabelButton(
+            base::BindRepeating(&StackerUniverseButton::ButtonPressed,
+                                base::Unretained(this)),
+            compact ? u"U" : u"Universe"),
+        browser_window_(browser_window) {
+    SetTooltipText(u"Stacker Universe");
+    GetViewAccessibility().SetName(u"Stacker Universe");
+  }
+
+ private:
+  void ButtonPressed() {
+    internal::ShowStackerUniversePanel(this, browser_window_);
+  }
+
+  raw_ptr<BrowserWindowInterface> browser_window_;
+};
+
+inline std::unique_ptr<views::LabelButton> CreateStackerUniverseButton(
+    BrowserWindowInterface* browser_window,
+    bool compact = false) {
+  return std::make_unique<StackerUniverseButton>(browser_window, compact);
+}
+
+}  // namespace browseros::universe
+
+#endif  // CHROME_BROWSER_BROWSEROS_UNIVERSE_STACKER_UNIVERSE_UI_H_
