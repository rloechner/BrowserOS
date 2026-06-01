diff --git a/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.h b/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.h
index a394d21ff23ee..4f8892390e6c2 100644
--- a/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.h
+++ b/chrome/browser/ui/views/frame/horizontal_tab_strip_region_view.h
@@ -148,6 +148,7 @@ class HorizontalTabStripRegionView final : public TabStripRegionView {
   bool tab_strip_set_ = false;
 
   raw_ptr<const Profile> profile_ = nullptr;
+  raw_ptr<views::Button> universe_button_ = nullptr;
   raw_ptr<TabStripActionContainer> tab_strip_action_container_ = nullptr;
   raw_ptr<views::View> tab_strip_container_ = nullptr;
   raw_ptr<views::View> reserved_grab_handle_space_ = nullptr;
