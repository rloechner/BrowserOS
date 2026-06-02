diff --git a/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.cc b/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.cc
index 4024fd673628b..ad7d0315f1918 100644
--- a/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.cc
+++ b/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.cc
@@ -18,6 +18,7 @@
 #include "base/metrics/user_metrics_action.h"
 #include "base/notimplemented.h"
 #include "base/time/time.h"
+#include "chrome/browser/browseros/universe/stacker_universe_ui.h"
 #include "chrome/browser/ui/actions/chrome_action_id.h"
 #include "chrome/browser/ui/animation/browser_animation_controller.h"
 #include "chrome/browser/ui/animation/browser_animation_types.h"
@@ -222,6 +223,14 @@ VerticalTabStripRegionView::VerticalTabStripRegionView(
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
