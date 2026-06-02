diff --git a/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.h b/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.h
index ec30a1d9e0a2f..5259f45147fcd 100644
--- a/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.h
+++ b/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.h
@@ -45,6 +45,7 @@
 }  // namespace tabs
 
 namespace views {
+class Button;
 class ResizeArea;
 class Separator;
 class View;
@@ -308,6 +309,8 @@ class VerticalTabStripRegionView : public views::AccessiblePaneView,
   raw_ptr<views::Separator> top_button_separator_ = nullptr;
   raw_ptr<VerticalTabStripView> tab_strip_view_ = nullptr;
   raw_ptr<VerticalTabStripBottomContainer> bottom_button_container_ = nullptr;
+  raw_ptr<views::Separator> universe_separator_ = nullptr;
+  raw_ptr<views::Button> universe_button_ = nullptr;
   raw_ptr<views::View> gemini_button_ = nullptr;
   raw_ptr<views::ResizeArea> resize_area_ = nullptr;
   raw_ptr<ShadowFrameView> shadow_frame_ = nullptr;
