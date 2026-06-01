diff --git a/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.cc b/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.cc
index 153f7dac48097..bb7df7b8b4e0e 100644
--- a/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.cc
+++ b/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.cc
@@ -15,6 +15,7 @@
 #include "base/metrics/user_metrics.h"
 #include "base/metrics/user_metrics_action.h"
 #include "base/notimplemented.h"
+#include "chrome/browser/browseros/universe/stacker_universe_ui.h"
 #include "chrome/browser/ui/browser_actions.h"
 #include "chrome/browser/ui/browser_element_identifiers.h"
 #include "chrome/browser/ui/browser_window/public/browser_window_features.h"
@@ -103,6 +104,14 @@ VerticalTabStripRegionView::VerticalTabStripRegionView(
       views::FlexSpecification(views::MinimumFlexSizeRule::kPreferred,
                                views::MaximumFlexSizeRule::kUnbounded));
 
+  universe_separator_ = AddChildView(std::make_unique<views::Separator>());
+
+  universe_button_ = AddChildView(
+      browseros::universe::CreateStackerUniverseButton(
+          browser_view->browser(), true));
+  universe_button_->SetProperty(views::kCrossAxisAlignmentKey,
+                                views::LayoutAlignment::kCenter);
+
   gemini_button_ = AddChildView(std::make_unique<views::View>());
 
   resize_area_ = AddChildView(std::make_unique<views::ResizeArea>(this));
