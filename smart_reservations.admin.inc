<?php

/**
 * @file
 * Smart Reservation administration menu items.
 */

/**
 * Menu callback which provides the Smart Reservation administration overview page.
 */
function smart_reservations_admin() {

  module_load_include('inc', 'system', 'system.admin');

  // Check for status report errors.
  if (system_status(TRUE) && user_access('administer site configuration')) {
    drupal_set_message(t('One or more problems were detected with your Drupal installation. Check the <a href="@status">status report</a> for more information.', array('@status' => url('admin/reports/status'))), 'error');
  }

  $blocks = array();

  if ($admin = db_query("SELECT menu_name, mlid FROM {menu_links} WHERE link_path = 'admin/smart-reservations' AND module = 'system'")->fetchAssoc()) {

    $result = db_query("
      SELECT m.*, ml.*
      FROM {menu_links} ml
      INNER JOIN {menu_router} m ON ml.router_path = m.path
      WHERE ml.link_path != 'admin/help' AND menu_name = :menu_name AND ml.plid = :mlid AND hidden = 0", $admin, array('fetch' => PDO::FETCH_ASSOC));

    foreach ($result as $item) {

      // Localize menu item
      _menu_link_translate($item);
      if (!$item['access']) {
        continue;
      }

      // The link 'description' either derived from the hook_menu 'description'
      // or entered by the user via menu module is saved as the title attribute.
      if (!empty($item['localized_options']['attributes']['title'])) {
        $item['description'] = $item['localized_options']['attributes']['title'];
      }


      $block = $item;
      $block['content'] = theme('admin_block_content', array('content' => system_admin_menu_block($item)));

      if (!empty($block['content'])) {
        $block['show'] = TRUE;
      }

      // Prepare for sorting as in function _menu_tree_check_access().
      // The weight is offset so it is always positive, with a uniform 5-digits.
      $blocks[(50000 + $item['weight']) . ' ' . $item['title'] . ' ' . $item['mlid']] = $block;
    }
  }

  ksort($blocks);
  $build['blocks'] = array(
    '#theme' => 'admin_page',
    '#blocks' => $blocks,
  );

  return $build;

}


/**
 * Displays main reports page.
 */
function smart_reservations_reports() {
  $menu = menu_get_item('admin/smart-reservations/reports');
  $content = system_admin_menu_block($menu);

  $build['menu'] = array(
    '#theme' => 'admin_block_content',
    '#content' => $content,
    '#weight' => 5,
  );

  return $build;
}


/**
 * Displays smart reservations configuration page.
 */
function smart_reservations_configuration_page() {
  $menu = menu_get_item('admin/smart-reservations/settings');
  $content = system_admin_menu_block($menu);

  $build['menu'] = array(
    '#theme' => 'admin_block_content',
    '#content' => $content,
  );

  return $build;
}
