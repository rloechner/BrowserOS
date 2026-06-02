diff --git a/chrome/browser/ui/actions/chrome_action_id.h b/chrome/browser/ui/actions/chrome_action_id.h
index 89d7ca2422224..4eade46024684 100644
--- a/chrome/browser/ui/actions/chrome_action_id.h
+++ b/chrome/browser/ui/actions/chrome_action_id.h
@@ -574,7 +574,11 @@
   E(kActionSidePanelShowShoppingInsights) \
   E(kActionSidePanelShowSideSearch) \
   E(kActionSidePanelShowMerchantTrust) \
-  E(kActionSidePanelShowTabsFromOtherDevices)
+  E(kActionSidePanelShowTabsFromOtherDevices) \
+  E(kActionSidePanelShowThirdPartyLlm) \
+  E(kActionSidePanelShowClashOfGpts) \
+  E(kActionBrowserOSAgent) \
+  E(kActionBrowserOSStackerUniverse)
 
 #define TOOLBAR_PINNABLE_ACTION_IDS \
   E(kActionHome, IDC_HOME) \
