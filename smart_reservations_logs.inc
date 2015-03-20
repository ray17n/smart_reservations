<?php

/**
 * @file
 * smart_reservations_logs.inc
 *
 * Smart Reservations. Search log functionality.
 */


/**
 * Callback function. Build the submissions log form.
 *
 * @return array
 *   A render array set for theming by theme_table().
 */
function smart_reservations_log_form($form, &$form_state) {

  // 1) Header of the form.
  $form_header = array(
    'apply_filters' => array(
      '#type' => 'submit',
      '#value' => t('Clear log'),
      '#submit' => array('smart_reservations_clear_log_action_form_submit'),
    ),
  );

  // 2) Data of the form. A table with the log entries.
  $rows_header = array(
    // The header gives the table the information it needs in order to make
    // the query calls for ordering. TableSort uses the field information
    // to know what database column to sort by.
    array('data' => t('Time'), 'field' => 's.timestamp'),
    array('data' => t('IP address'), 'field' => 's.ip_address'),
    array('data' => t('Check-In Date'), 'field' => 's.checkin_date'),
    array('data' => t('Check-Out Date'), 'field' => 's.checkout_date'),
    array('data' => t('Number of adults'), 'field' => 's.num_of_adults'),
    array('data' => t('Number of children'), 'field' => 's.num_of_children'),
  );

  // Using the TableSort Extender is what tells the the query object that we
  // are sorting.
  $query = db_select('smart_reservations_search_log', 's')
    ->extend('TableSort')
    ->extend('PagerDefault')
    ->limit(30);
  $query->addExpression("FROM_UNIXTIME(s.timestamp, '%Y-%m-%d | %H:%i:%s')", 'timestamp');
  $query->fields('s', array(
    'timestamp',
    'ip_address',
    'checkin_date',
    'checkout_date',
    'num_of_adults',
    'num_of_children',
    )
  );

  $result = $query
    ->orderByHeader($rows_header)
    ->execute();

  $rows = array();
  foreach ($result as $row) {
    $rows[] = array('data' => (array) $row);
  }

  $form_table['smart_reservations_search_log_table'] = array(
    '#theme' => 'table',
    '#header' => $rows_header,
    '#rows' => $rows,
    '#prefix' => '<div id="log-table">',
    '#suffix' => '</div>',
  );

  // 3) Use $form_header and $form_table to build the form.
  $form['header'] = $form_header;
  $form['body'] = $form_table;

  return $form;
}

/**
 * Clear log callback for the smart_reservations_log_form form.
 */
function smart_reservations_clear_log_action_form_submit($form, &$form_state) {
  db_delete('smart_reservations_search_log')
      ->execute();
  drupal_set_message(t('Log cleared.'));
}
