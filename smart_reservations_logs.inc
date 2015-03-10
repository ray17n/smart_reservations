<?php

/**
 * @file
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
    // 'number_of_adults' => array(
    //   '#title' => t("Number of adults"),
    //   '#type' => 'select',
    //   '#options' => array_combine(
    //       range(0, 20),
    //       range(0, 20)),
    //   '#ajax' => array(
    //     'callback' => 'smart_reservations_number_of_adults_callback',
    //     'wrapper' => 'log-table',
    //   ),
    //   '#default_value' => 1,
    // ),
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
    // array('data' => t('Parameters'), 'field' => 's.query'),
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

  // An AJAX request calls the form builder function for every change.
  if (!empty($form_state['values']['number_of_adults'])) {
    $query->condition('num_of_adults', $form_state['values']['number_of_adults'], '>=');
  }
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
 * Callback for smart_reservations_simplest.
 *
 * On an ajax submit, the form builder function is called again, then the $form
 * and $form_state are passed to this callback function so it can select which
 * portion of the form to send on to the client.
 *
 * @return array
 *   Renderable array (the textfield element)
 */
function smart_reservations_number_of_adults_callback($form, $form_state) {
  // The form has already been submitted and updated. We can return the replaced
  // item as it is.
  // dpm($form);
  return $form['body'];

}


/**
 * Theme callback for the smart_reservations_rates_form form.
 *
 * The theme callback will format the $form data structure into a table and
 * add our tabledrag functionality.  (Note that drupal_add_tabledrag should be
 * called from the theme layer, and not from a form declaration.  This helps
 * keep template files clean and readable, and prevents tabledrag.js from
 * being added twice accidently.
 *
 * @ingroup smart_reservations
 */
function theme_smart_reservations_log_form($variables) {
  // dpm("theme.form.start");
  // dpm($variables);

  // $form = $variables['form'];

  // $output = '<div class="smart-reservations-log-form">';

  // $output = '<div class="smart-reservations-log-header">';
  // $output .= drupal_render($form['header']);
  // $output .= '</div';

  // // $output .= '<div id="log-form-body">';
  // // $output .= drupal_render($form['body']);
  // // $output .= '</div';

  // // $output .= drupal_render($form);
  // $output .= '</div';
  // return drupal_render($variables['form']);
}

/**
 * Clear log callback for the smart_reservations_log_form form.
 */
function smart_reservations_clear_log_action_form_submit($form, &$form_state) {
  // Because the form elements were keyed with the item ids from the database,
  // we can simply iterate through the submitted values.
  db_delete('smart_reservations_search_log')
      ->execute();
  drupal_set_message(t('Log cleared.'));
}


// /**
//  * Callback function. Build the submissions log form.
//  *
//  * @return array
//  *   A render array set for theming by theme_table().
//  */
// function _smart_reservations_list_search_log() {

//   $header = array(
//     // The header gives the table the information it needs in order to make
//     // the query calls for ordering. TableSort uses the field information
//     // to know what database column to sort by.
//     array('data' => t('Time'), 'field' => 's.timestamp'),
//     array('data' => t('IP address'), 'field' => 's.ip_address'),
//     array('data' => t('Check-In Date'), 'field' => 's.checkin_date'),
//     array('data' => t('Check-Out Date'), 'field' => 's.checkout_date'),
//     array('data' => t('Number of adults'), 'field' => 's.num_of_adults'),
//     array('data' => t('Number of children'), 'field' => 's.num_of_children'),
//     // array('data' => t('Parameters'), 'field' => 's.query'),
//   );

//   // Using the TableSort Extender is what tells the the query object that we
//   // are sorting.
//   $query = db_select('smart_reservations_search_log', 's')
//     ->extend('TableSort')
//     ->extend('PagerDefault')
//     ->limit(30);
//   $query->addExpression("FROM_UNIXTIME(s.timestamp, '%Y-%m-%d | %H:%i:%s')", 'timestamp');
//   $query->fields('s', array(
//     'timestamp',
//     'ip_address',
//     'checkin_date',
//     'checkout_date',
//     'num_of_adults',
//     'num_of_children',
//     // 'query',
//     )
//   );
//   $result = $query
//     ->orderByHeader($header)
//     ->execute();

//   $rows = array();
//   foreach ($result as $row) {
//     $rows[] = array('data' => (array) $row);
//   }

//   $form_table['smart_reservations_search_log_table'] = array(
//     '#theme' => 'table',
//     '#header' => $header,
//     '#rows' => $rows,
//   );

//   return $form_table;
// }
