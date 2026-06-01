diff --git a/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc b/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc
index c40cfd6af0c8a..3109a2dbf28dd 100644
--- a/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc
+++ b/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc
@@ -8,6 +8,7 @@
 #include "base/functional/bind.h"
 #include "base/strings/utf_string_conversions.h"
 #include "base/task/single_thread_task_runner.h"
 #include "build/build_config.h"
 #include "build/buildflag.h"
+#include "chrome/browser/browseros/universe/stacker_universe_ui.h"
 #include "chrome/browser/glic/public/glic_enabling.h"
@@ -217,9 +218,14 @@ HorizontalTabStripRegionView::HorizontalTabStripRegionView(
   GetViewAccessibility().SetRole(ax::mojom::Role::kTabList);
   GetViewAccessibility().SetIsMultiselectable(true);
 
-  tab_strip_ = AddChildView(CreateTabStrip(this, browser_view));
   BrowserWindowInterface* const browser = browser_view->browser();
+  universe_button_ = AddChildView(
+      browseros::universe::CreateStackerUniverseButton(browser));
+  universe_button_->SetProperty(views::kCrossAxisAlignmentKey,
+                                views::LayoutAlignment::kCenter);
 
+  tab_strip_ = AddChildView(CreateTabStrip(this, browser_view));
+
   if (base::FeatureList::IsEnabled(features::kTabGroupsFocusing)) {
     unfocus_button_ = AddChildView(std::make_unique<TabStripControlButton>(
         browser, views::Button::PressedCallback(), vector_icons::kArrowBackIcon,
