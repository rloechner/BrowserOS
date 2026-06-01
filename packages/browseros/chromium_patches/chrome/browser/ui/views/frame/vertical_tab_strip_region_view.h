diff --git a/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.h b/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.h
index 0c6d558d8a776..93f964d9886f2 100644
--- a/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.h
+++ b/chrome/browser/ui/views/frame/vertical_tab_strip_region_view.h
@@ -35,6 +35,7 @@ class TabDragContext;
 
 namespace views {
 class ResizeArea;
+class Button;
 class Separator;
 class View;
 class FlexLayout;
@@ -175,6 +176,8 @@ class VerticalTabStripRegionView final : public TabStripRegionView,
   raw_ptr<views::Separator> top_button_separator_ = nullptr;
   raw_ptr<VerticalTabStripView> tab_strip_view_ = nullptr;
   raw_ptr<VerticalTabStripBottomContainer> bottom_button_container_ = nullptr;
+  raw_ptr<views::Separator> universe_separator_ = nullptr;
+  raw_ptr<views::Button> universe_button_ = nullptr;
   raw_ptr<views::View> gemini_button_ = nullptr;
   raw_ptr<views::ResizeArea> resize_area_ = nullptr;
   raw_ptr<views::FlexLayout> flex_layout_ = nullptr;
