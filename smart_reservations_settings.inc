<?php

/**
 * @file
 * Smart Reservations. Settings functionality.
 */

/**
 * Page callback: Smart Reservations Configuration.
 *
 * @see smart_reservations_menu()
 */
function smart_reservations_configuration_form($form, &$form_state) {

  $form['settings'] = array(
    '#type' => 'vertical_tabs',
    // '#default_tab' => 'edit-tab1',
  );

  // General.
  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
    '#collapsible' => TRUE,
    '#group' => 'settings',
  );
  $form['general']['sr_max_nights_reservation'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum nights of a reservation'),
    '#default_value' => variable_get('sr_max_nights_reservation'),
    '#size' => 5,
    '#maxlength' => 5,
    '#description' => t('The maximum days of a reservation.'),
    '#required' => TRUE,
  );
  // $d = t('The prefix of the reservation codes. For instance, with RES will have RES000001.');
  $res_number = variable_get('sr_next_reservation_number');
  $prefix = variable_get('sr_reservation_prefix');
  $res_code = $prefix . sprintf("%06d", $res_number);

  $d = t('The prefix of the reservation codes. It will be concatenated before the next number for a new reservation.');
  $d .= ' ' . t('Current format sample:') . ' ';
  $d .= $res_code;
  $form['general']['sr_reservation_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Default prefix of the reservation codes'),
    '#default_value' => variable_get('sr_reservation_prefix'),
    '#size' => 12,
    '#maxlength' => 12,
    '#description' => $d,
    '#required' => FALSE,
  );
  $form['general']['sr_next_reservation_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Next number for a new reservation'),
    '#default_value' => variable_get('sr_next_reservation_number'),
    '#size' => 12,
    '#maxlength' => 12,
    '#description' => t('Next number to assign to a new reservation.'),
    '#required' => TRUE,
  );
  $form['general']['sr_week_starts_on'] = array(
    '#type' => 'select',
    '#title' => t('Week starts on'),
    '#options' => array(
      0 => t('Sunday'),
      1 => t('Monday'),
    ),
    '#default_value' => variable_get('sr_week_starts_on') ,
  );

  $form['general']['notifications'] = array(
    '#type' => 'item',
    '#title' => t('Notifications'),
  );
  $form['general']['sr_email_customer_submitted'] = array(
    '#type' => 'checkbox',
    '#title' => t('After submitting a new reservation, email it to the customer.'),
    '#default_value' => variable_get('sr_email_customer_submitted'),
  );
  $form['general']['sr_email_admin_submitted'] = array(
    '#type' => 'checkbox',
    '#title' => t('After submitting a new reservation, email it to the administrator.'),
    '#default_value' => variable_get('sr_email_admin_submitted'),
  );
  $form['general']['log'] = array(
    '#type' => 'item',
    '#title' => t('Log options'),
    '#prefix' => '<br>',
  );
  $form['general']['sr_log_reservations_searched'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log the availability searches.'),
    '#default_value' => variable_get('sr_log_reservations_searched'),
    '#suffix' => '<br>',
  );

  $d = '<br>' . t('Any format accepted by php function date.');
  $d .= ' ' . t('Current format sample: ');
  $d .= date(variable_get('sr_date_format_search_results'));
  $d .= '<br><br>' . t('Format examples:');
  $d .= '<br><br>' . sr_date_format_samples();
  $d .= t('See more options at !link',
      array(
        '!link' => '<a href="http://php.net/manual/en/function.date.php"
         target = "_blank">http://php.net/manual/en/function.date.php</a>'
        ));
  $form['general']['sr_date_format_search_results'] = array(
    '#type' => 'textfield',
    '#title' => t('Date format in search results calendars'),
    '#description' => $d,
    '#size' => 12,
    '#default_value' => variable_get('sr_date_format_search_results') ,
  );

  // Currency format.
  $form['currency'] = array(
    '#type' => 'fieldset',
    '#title' => t('Currency format'),
    '#collapsible' => TRUE,
    '#group' => 'settings',
  );
  // To use the func sr_currency_sign, we need to include
  // smart_reservation_reservations.inc.
  module_load_include('inc', 'smart_reservations',
    'smart_reservations_reservations');
  $form['currency']['example'] = array(
    '#type' => 'textfield',
    '#title' => t('Current format'),
    '#description' => t('Save the new settings to see the changes.'),
    '#value' => sr_curr(1350.23),
    '#disabled' => TRUE,
    '#size' => 10,
  );
  $form['currency']['sr_currency_sign'] = array(
    '#type' => 'textfield',
    '#title' => t('Currency Sign'),
    '#default_value' => variable_get('sr_currency_sign'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => FALSE,
  );
  $form['currency']['sr_currency_sign_after_amount'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display currency sign after the figure'),
    '#default_value' => variable_get('sr_currency_sign_after_amount', FALSE),
  );
  $form['currency']['sr_currency_thousands_separator'] = array(
    '#type' => 'textfield',
    '#title' => t('Thousands Marker'),
    '#default_value' => variable_get('sr_currency_thousands_separator'),
    '#size' => 4,
    '#maxlength' => 1,
    '#required' => FALSE,
  );
  $form['currency']['sr_currency_decimal_point'] = array(
    '#type' => 'textfield',
    '#title' => t('Decimal Marker'),
    '#default_value' => variable_get('sr_currency_decimal_point'),
    '#size' => 4,
    '#maxlength' => 1,
    '#required' => FALSE,
  );
  $form['currency']['sr_currency_decimal_places'] = array(
    '#type' => 'select',
    '#title' => t('Number of decimals'),
    '#options' => drupal_map_assoc(array(0, 1, 2)),
    '#default_value' => variable_get('sr_currency_decimal_places'),
  );

  // Search parameters.
  $form['search_params'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search parameters'),
    '#collapsible' => TRUE,
    '#group' => 'settings',
  );
  $form['search_params']['sr_max_checkin_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of days in advance to make a reservation'),
    '#default_value' => variable_get('sr_max_checkin_date'),
    '#size' => 5,
    '#maxlength' => 5,
    '#description' => t('How early you can make a reservation. For example, if we indicate 365, it means that the check-in date has to be a year from today maximum.'),
    '#required' => TRUE,
  );
  $form['search_params']['sr_max_number_of_guests_to_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of guests'),
    '#default_value' => variable_get('sr_max_number_of_guests_to_search'),
    '#size' => 2,
    '#maxlength' => 2,
    '#description' => t('The maximum number of guests that appears in the select options of the general filter.'),
    '#required' => TRUE,
  );
  $form['search_params']['sr_default_number_of_nights_to_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Default number of nights'),
    '#default_value' => variable_get('sr_default_number_of_nights_to_search'),
    '#size' => 2,
    '#maxlength' => 2,
    '#description' => t('The default number of nights that is displayed in the search screen. This is also the value used when the checkout value of the search is incorrect.'),
    '#required' => TRUE,
  );
  $form['search_params']['sr_default_number_of_adults_to_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Default number of adults'),
    '#default_value' => variable_get('sr_default_number_of_adults_to_search'),
    '#size' => 2,
    '#maxlength' => 2,
    '#description' => t('The default number of adults that is displayed in the search screen.'),
    '#required' => TRUE,
  );
  $form['search_params']['sr_default_number_of_children_to_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Default number of children'),
    '#default_value' => variable_get('sr_default_number_of_children_to_search'),
    '#size' => 2,
    '#maxlength' => 2,
    '#description' => t('The default number of children that is displayed in the search screen.'),
    '#required' => TRUE,
  );
  $form['search_params']['sr_default_number_of_babies_to_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Default number of babies'),
    '#default_value' => variable_get('sr_default_number_of_babies_to_search'),
    '#size' => 2,
    '#maxlength' => 2,
    '#description' => t('The default number of babies that is displayed in the search screen.'),
    '#required' => TRUE,
  );
  $form['search_params']['sr_default_number_of_dogs_to_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Default number of dogs'),
    '#default_value' => variable_get('sr_default_number_of_dogs_to_search'),
    '#size' => 2,
    '#maxlength' => 2,
    '#description' => t('The default number of dogs that is displayed in the search screen.'),
    '#required' => TRUE,
  );
  $form['search_params']['sr_default_number_of_cats_to_search'] = array(
    '#type' => 'textfield',
    '#title' => t('Default number of cats'),
    '#default_value' => variable_get('sr_default_number_of_cats_to_search'),
    '#size' => 2,
    '#maxlength' => 2,
    '#description' => t('Default number of cats to search reservations.'),
    '#required' => TRUE,
  );

  // Texts.
  $form['texts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Messages'),
    '#collapsible' => TRUE,
    '#group' => 'settings',
  );
  $form['texts']['sr_txt_capacity_exceeded'] = array(
    '#type' => 'textarea',
    '#title' => t('Warning message: more than one accomodation unit needed'),
    '#default_value' => variable_get('sr_txt_capacity_exceeded'),
    '#description' => t('The text that appears in the availability results, when more than one accommodation unit is needed. Here you can specify what type of accommodation you offer (rooms, villas, bungalows, holiday apartments, etc.).'),
    '#required' => TRUE,
  );
  // // new params to add. Pending to analyze internationalization impact.
  // $form['texts']['smart_reservations_txt_reservation_submitted_screen'] = array(
  //   '#type' => 'textarea',
  //   '#title' => t('Reservation submitted. Screen message.'),
  //   '#default_value' => variable_get('smart_reservations_txt_reservation_submitted_screen'),
  //   '#description' => t('The text that appears after a reservation is submitted.'),
  //   '#required' => FALSE,
  // );
  // $form['texts']['smart_reservations_txt_reservation_submitted_mail_customer'] = array(
  //   '#type' => 'textarea',
  //   '#title' => 'Reservation submitted. Mail sent to the customer.',
  //   '#default_value' => variable_get('smart_reservations_txt_reservation_submitted_mail_customer'),
  //   '#description' => t('Message send to the customer after a reservation is submitted.'),
  //   '#required' => FALSE,
  // );
  // $form['texts']['smart_reservations_txt_reservation_submitted_mail_admin'] = array(
  //   '#type' => 'textarea',
  //   '#title' => 'Reservation submitted. Mail sent to the admin.',
  //   '#default_value' => variable_get('smart_reservations_txt_reservation_submitted_mail_admin'),
  //   '#description' => t('Message send to the administrator after a reservation is submitted.'),
  //   '#required' => FALSE,
  // );
  return system_settings_form($form);
}

/**
 * Implements validation from the Form API.
 *
 * @param form $form
 *   A structured array containing the elements and properties of the form.
 * @param state $form_state
 *   An array that stores information about the form's current state
 *   during processing.
 */
function smart_reservations_configuration_form_validate($form, &$form_state) {

  // General.
  if (!is_numeric($form_state['values']['sr_max_nights_reservation'])) {
    form_set_error('sr_max_nights_reservation', t('You must enter a number for the maximum number of nights to search.'));
  }

  if (!is_numeric($form_state['values']['sr_next_reservation_number'])) {
    form_set_error('sr_next_reservation_number', t('You must enter a number for the Next number for a new reservation.'));
  }

  // Search parameters.
  if (!is_numeric($form_state['values']['sr_max_checkin_date'])) {
    form_set_error('sr_max_checkin_date', t('You must enter a number for the Maximum allowed checkin date.'));
  }

  if (!is_numeric($form_state['values']['sr_max_number_of_guests_to_search'])) {
    form_set_error('sr_max_number_of_guests_to_search', t('You must enter a number for the Maximum number of guests to search.'));
  }

  if (!is_numeric($form_state['values']['sr_default_number_of_nights_to_search'])) {
    form_set_error('sr_default_number_of_nights_to_search', t('You must enter a number for the default number of nights to search.'));
  }

  if (!is_numeric($form_state['values']['sr_default_number_of_adults_to_search'])) {
    form_set_error('sr_default_number_of_adults_to_search', t('You must enter a number for the default number of adults to search.'));
  }

  if (!is_numeric($form_state['values']['sr_default_number_of_children_to_search'])) {
    form_set_error('sr_default_number_of_children_to_search', t('You must enter a number for the default number of children to search.'));
  }

  if (!is_numeric($form_state['values']['sr_default_number_of_babies_to_search'])) {
    form_set_error('sr_default_number_of_babies_to_search', t('You must enter a number for the default number of babies to search.'));
  }

  if (!is_numeric($form_state['values']['sr_default_number_of_dogs_to_search'])) {
    form_set_error('sr_default_number_of_dogs_to_search', t('You must enter a number for the default number of dogs to search.'));
  }

  if (!is_numeric($form_state['values']['sr_default_number_of_cats_to_search'])) {
    form_set_error('sr_default_number_of_cats_to_search', t('You must enter a number for the default number of cats to search.'));
  }
}

/**
 * Retrieve some samples using php date function 
 **/
function sr_date_format_samples() {
  $date = 'Y-m-d';
  $samples = array(
    'j',
    'd',
    'M, d',
    'm/d',
    'm-d',
    'd-m',
    'd/m',
    'D, j',
    'm/d/Y',
    'm-d-Y',
    'd-m-Y',
    'Y-m-d',
  );

  $o = '<table><th>Format</th><th>Value</th>';
  foreach ($samples as $format) {
    $o .= '<tr>';
    $o .= '<td>' . $format . '</td>';
    $o .= '<td>' . date($format) . '</td>';
    $o .= '</tr>';
  }
  $o .= '</table>';

//       // Assuming today is March 10th, 2001, 5:16:18 pm, and that we are in the
// // Mountain Standard Time (MST) Time Zone
// F j, Y                 // March 10, 2001
// m.d.y          03.10.01
// j, n, Y 10, 3, 2001
// Ymd        20010310
// D M j     Sat Mar 10
// Y-m-d 2001-03-01
  return $o;
}


function sr_date_format($value, $format) {
  $v = date($format, strtotime($value));
  return $v;
}
