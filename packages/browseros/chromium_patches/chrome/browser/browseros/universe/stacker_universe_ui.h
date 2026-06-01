diff --git a/chrome/browser/browseros/universe/stacker_universe_ui.h b/chrome/browser/browseros/universe/stacker_universe_ui.h
new file mode 100644
index 0000000000000..6a8128c7c2f3a
--- /dev/null
+++ b/chrome/browser/browseros/universe/stacker_universe_ui.h
@@ -0,0 +1,304 @@
+// Copyright 2026 The Chromium Authors
+// Use of this source code is governed by a BSD-style license that can be
+// found in the LICENSE file.
+
+#ifndef CHROME_BROWSER_BROWSEROS_UNIVERSE_STACKER_UNIVERSE_UI_H_
+#define CHROME_BROWSER_BROWSEROS_UNIVERSE_STACKER_UNIVERSE_UI_H_
+
+#include <algorithm>
+#include <memory>
+#include <string>
+#include <vector>
+
+#include "base/functional/bind.h"
+#include "base/memory/raw_ptr.h"
+#include "base/no_destructor.h"
+#include "base/strings/string_number_conversions.h"
+#include "base/strings/utf_string_conversions.h"
+#include "chrome/browser/profiles/profile.h"
+#include "chrome/browser/ui/browser.h"
+#include "chrome/browser/ui/browser_list.h"
+#include "chrome/browser/ui/browser_window/public/browser_window_interface.h"
+#include "chrome/browser/ui/browser_window/public/browser_window_interface_iterator.h"
+#include "chrome/browser/ui/tabs/tab_strip_model.h"
+#include "content/public/browser/web_contents.h"
+#include "ui/base/base_window.h"
+#include "ui/gfx/geometry/insets.h"
+#include "ui/views/bubble/bubble_dialog_delegate_view.h"
+#include "ui/views/controls/button/label_button.h"
+#include "ui/views/controls/button/md_text_button.h"
+#include "ui/views/controls/label.h"
+#include "ui/views/layout/box_layout.h"
+#include "ui/views/view.h"
+#include "ui/views/view_class_properties.h"
+#include "ui/views/widget/widget.h"
+
+namespace browseros::universe {
+
+namespace internal {
+
+inline BrowserWindowInterface* FindBrowserWindow(int window_id) {
+  BrowserWindowInterface* result = nullptr;
+  ForEachCurrentBrowserWindowInterfaceOrderedByActivation(
+      [window_id, &result](BrowserWindowInterface* browser_window) {
+        if (browser_window->GetSessionID().id() == window_id) {
+          result = browser_window;
+          return false;
+        }
+        return true;
+      });
+  return result;
+}
+
+inline int WindowIdFor(BrowserWindowInterface* browser_window) {
+  return browser_window ? browser_window->GetSessionID().id() : -1;
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
+}  // namespace internal
+
+struct StackerUniverseMember {
+  int window_id = -1;
+  std::u16string title;
+  std::u16string profile_name;
+  bool active = false;
+  bool minimized = false;
+};
+
+struct StackerUniverseSnapshot {
+  int id = 0;
+  std::u16string name;
+  bool collapsed = false;
+  std::vector<StackerUniverseMember> members;
+};
+
+class StackerUniverseRegistry {
+ public:
+  struct Universe {
+    int id = 0;
+    std::u16string name;
+    bool collapsed = false;
+    std::vector<int> window_ids;
+  };
+
+  static StackerUniverseRegistry& Get() {
+    static base::NoDestructor<StackerUniverseRegistry> instance;
+    return *instance;
+  }
+
+  int CreateUniverse(BrowserWindowInterface* initial_window) {
+    Universe universe;
+    universe.id = next_id_++;
+    universe.name = u"Universe " + base::NumberToString16(universe.id);
+    universes_.push_back(std::move(universe));
+    AddWindowToUniverse(universes_.back().id, initial_window);
+    return universes_.back().id;
+  }
+
+  void AddCurrentWindowToFirstUniverse(BrowserWindowInterface* browser_window) {
+    if (universes_.empty()) {
+      CreateUniverse(browser_window);
+      return;
+    }
+    AddWindowToUniverse(universes_.front().id, browser_window);
+  }
+
+  void AddWindowToUniverse(int universe_id,
+                           BrowserWindowInterface* browser_window) {
+    const int window_id = internal::WindowIdFor(browser_window);
+    if (window_id < 0) {
+      return;
+    }
+
+    RemoveWindowFromAllUniverses(window_id);
+    Universe* universe = FindUniverse(universe_id);
+    if (!universe) {
+      return;
+    }
+    universe->window_ids.push_back(window_id);
+  }
+
+  void ActivateWindow(int window_id) {
+    BrowserWindowInterface* browser_window =
+        internal::FindBrowserWindow(window_id);
+    if (!browser_window || !browser_window->GetWindow()) {
+      return;
+    }
+    browser_window->GetWindow()->Activate();
+  }
+
+  void ToggleCollapsed(int universe_id) {
+    Universe* universe = FindUniverse(universe_id);
+    if (!universe) {
+      return;
+    }
+
+    PruneMissingWindows(*universe);
+    universe->collapsed = !universe->collapsed;
+    for (int window_id : universe->window_ids) {
+      BrowserWindowInterface* browser_window =
+          internal::FindBrowserWindow(window_id);
+      if (!browser_window || !browser_window->GetWindow()) {
+        continue;
+      }
+      if (universe->collapsed) {
+        browser_window->GetWindow()->Minimize();
+      } else {
+        browser_window->GetWindow()->Restore();
+      }
+    }
+  }
+
+  void MoveWindow(int universe_id, int window_id, int offset) {
+    Universe* universe = FindUniverse(universe_id);
+    if (!universe) {
+      return;
+    }
+    auto it = std::ranges::find(universe->window_ids, window_id);
+    if (it == universe->window_ids.end()) {
+      return;
+    }
+    const int index = static_cast<int>(it - universe->window_ids.begin());
+    const int next_index = index + offset;
+    if (next_index < 0 ||
+        next_index >= static_cast<int>(universe->window_ids.size())) {
+      return;
+    }
+    std::iter_swap(universe->window_ids.begin() + index,
+                   universe->window_ids.begin() + next_index);
+  }
+
+  std::vector<StackerUniverseSnapshot> Snapshots() {
+    std::vector<StackerUniverseSnapshot> snapshots;
+    for (Universe& universe : universes_) {
+      PruneMissingWindows(universe);
+
+      StackerUniverseSnapshot snapshot;
+      snapshot.id = universe.id;
+      snapshot.name = universe.name;
+      snapshot.collapsed = universe.collapsed;
+
+      for (int window_id : universe.window_ids) {
+        BrowserWindowInterface* browser_window =
+            internal::FindBrowserWindow(window_id);
+        if (!browser_window) {
+          continue;
+        }
+        StackerUniverseMember member;
+        member.window_id = window_id;
+        member.title = internal::WindowTitle(browser_window);
+        member.profile_name = internal::ProfileName(browser_window);
+        member.active = browser_window->IsActive();
+        member.minimized = browser_window->GetWindow() &&
+                           browser_window->GetWindow()->IsMinimized();
+        snapshot.members.push_back(std::move(member));
+      }
+      snapshots.push_back(std::move(snapshot));
+    }
+    return snapshots;
+  }
+
+  size_t UniverseCount() {
+    PruneAllMissingWindows();
+    return universes_.size();
+  }
+
+ private:
+  friend class base::NoDestructor<StackerUniverseRegistry>;
+
+  StackerUniverseRegistry() = default;
+
+  Universe* FindUniverse(int universe_id) {
+    auto it = std::ranges::find_if(universes_, [universe_id](const Universe& u) {
+      return u.id == universe_id;
+    });
+    return it == universes_.end() ? nullptr : &*it;
+  }
+
+  void RemoveWindowFromAllUniverses(int window_id) {
+    for (Universe& universe : universes_) {
+      universe.window_ids.erase(
+          std::remove(universe.window_ids.begin(), universe.window_ids.end(),
+                      window_id),
+          universe.window_ids.end());
+    }
+  }
+
+  void PruneMissingWindows(Universe& universe) {
+    universe.window_ids.erase(
+        std::remove_if(universe.window_ids.begin(), universe.window_ids.end(),
+                       [](int window_id) {
+                         return !internal::FindBrowserWindow(window_id);
+                       }),
+        universe.window_ids.end());
+  }
+
+  void PruneAllMissingWindows() {
+    for (Universe& universe : universes_) {
+      PruneMissingWindows(universe);
+    }
+    universes_.erase(
+        std::remove_if(universes_.begin(), universes_.end(),
+                       [](const Universe& universe) {
+                         return universe.window_ids.empty();
+                       }),
+        universes_.end());
+  }
+
+  int next_id_ = 1;
+  std::vector<Universe> universes_;
+};
+
+class StackerUniverseBubble : public views::BubbleDialogDelegateView {
+ public:
+  StackerUniverseBubble(views::View* anchor,
+                        BrowserWindowInterface* browser_window)
+      : views::BubbleDialogDelegateView(anchor, views::BubbleBorder::TOP_LEFT),
+        browser_window_(browser_window) {
+    SetTitle(u"Stacker Universe");
+    SetShowCloseButton(true);
+    set_margins(gfx::Insets::VH(12, 14));
+    SetLayoutManager(std::make_unique<views::BoxLayout>(
+        views::BoxLayout::Orientation::kVertical, gfx::Insets(),
+        /*between_child_spacing=*/8));
+    BuildContents();
+  }
+
+ private:
+  void BuildContents() {
+    auto* create_button = AddChildView(views::MdTextButton::Create(
+        base::BindRepeating(&StackerUniverseBubble::CreateUniverse,
+                            base::Unretained(this)),
+        u"Create universe"));
+    create_button->SetHorizontalAlignment(gfx::ALIGN_LEFT);
+
+    auto* add_button = AddChildView(views::MdTextButton::Create(
+        base::BindRepeating(&StackerUniverseBubble::AddCurrentWindow,
+                            base::Unretained(this)),
+        u"Add current window"));
+    add_button->SetHorizontalAlignment(gfx::ALIGN_LEFT);
+
+    const auto snapshots = StackerUniverseRegistry::Get().Snapshots();
+    if (snapshots.empty()) {
+      AddChildView(std::make_unique<views::Label>(
+          u"No universes yet. Create one from the current window."));
+      return;
+    }
+
+    for (const auto& universe : snapshots) {
+      AddUniverseSection(universe);
+    }
+  }
+
+  void AddUniverseSection(const StackerUniverseSnapshot& universe) {
+    auto* title = AddChildView(std::make_unique<views::Label>(universe.name));
+    title->SetHorizontalAlignment(gfx::ALIGN_LEFT);
+
+    auto* toggle_button = AddChildView(views::MdTextButton::Create(
+        base::BindRepeating(&StackerUniverseBubble::ToggleCollapsed,
+                            base::Unretained(this), universe.id),
+        universe.collapsed ? u"Restore universe" : u"Collapse universe"));
+    toggle_button->SetHorizontalAlignment(gfx::ALIGN_LEFT);
+
+    for (const auto& member : universe.members) {
+      AddMemberRow(universe.id, member);
+    }
+  }
+
+  void AddMemberRow(int universe_id, const StackerUniverseMember& member) {
+    auto* row = AddChildView(std::make_unique<views::View>());
+    row->SetLayoutManager(std::make_unique<views::BoxLayout>(
+        views::BoxLayout::Orientation::kHorizontal, gfx::Insets(),
+        /*between_child_spacing=*/6));
+
+    const std::u16string label =
+        (member.active ? u"* " : u"") + member.title + u" - " +
+        member.profile_name;
+    auto* switch_button = row->AddChildView(views::MdTextButton::Create(
+        base::BindRepeating(&StackerUniverseBubble::ActivateWindow,
+                            base::Unretained(this), member.window_id),
+        label));
+    switch_button->SetHorizontalAlignment(gfx::ALIGN_LEFT);
+
+    row->AddChildView(views::MdTextButton::Create(
+        base::BindRepeating(&StackerUniverseBubble::MoveWindow,
+                            base::Unretained(this), universe_id,
+                            member.window_id, -1),
+        u"Up"));
+    row->AddChildView(views::MdTextButton::Create(
+        base::BindRepeating(&StackerUniverseBubble::MoveWindow,
+                            base::Unretained(this), universe_id,
+                            member.window_id, 1),
+        u"Down"));
+  }
+
+  void CreateUniverse() {
+    StackerUniverseRegistry::Get().CreateUniverse(browser_window_);
+    GetWidget()->Close();
+  }
+
+  void AddCurrentWindow() {
+    StackerUniverseRegistry::Get().AddCurrentWindowToFirstUniverse(
+        browser_window_);
+    GetWidget()->Close();
+  }
+
+  void ToggleCollapsed(int universe_id) {
+    StackerUniverseRegistry::Get().ToggleCollapsed(universe_id);
+    GetWidget()->Close();
+  }
+
+  void ActivateWindow(int window_id) {
+    StackerUniverseRegistry::Get().ActivateWindow(window_id);
+    GetWidget()->Close();
+  }
+
+  void MoveWindow(int universe_id, int window_id, int offset) {
+    StackerUniverseRegistry::Get().MoveWindow(universe_id, window_id, offset);
+    GetWidget()->Close();
+  }
+
+  raw_ptr<BrowserWindowInterface> browser_window_;
+};
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
+    views::BubbleDialogDelegateView::CreateBubble(
+        std::make_unique<StackerUniverseBubble>(this, browser_window_))
+        ->Show();
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
