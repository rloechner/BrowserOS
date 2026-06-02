diff --git a/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc b/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc
index 2af9ca18b6b2a..ba9ad3ef5b38f 100644
--- a/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc
+++ b/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.cc
@@ -10,6 +10,7 @@
 #include "base/task/single_thread_task_runner.h"
 #include "build/build_config.h"
 #include "build/buildflag.h"
+#include "chrome/browser/browseros/universe/stacker_universe_ui.h"
 #include "chrome/browser/glic/public/glic_enabling.h"
 #include "chrome/browser/themes/theme_properties.h"
 #include "chrome/browser/ui/actions/chrome_action_id.h"
@@ -243,6 +244,11 @@ HorizontalTabStripRegionView::HorizontalTabStripRegionView(
   GetViewAccessibility().SetIsMultiselectable(true);
 
   BrowserWindowInterface* const browser = browser_view->browser();
+
+  universe_button_ = AddChildView(
+      browseros::universe::CreateStackerUniverseButton(browser));
+  universe_button_->SetProperty(views::kCrossAxisAlignmentKey,
+                                views::LayoutAlignment::kCenter);
 
   if (browser &&
       (browser->GetType() == BrowserWindowInterface::Type::TYPE_NORMAL) &&
