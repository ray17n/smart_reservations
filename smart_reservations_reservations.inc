<?php
/**
 * @file
 * Reservations functionality.
 * smart_reservations_reservations.inc
 */

/**
 * Main form. Three steps: Search results, make reservation, submit reservation.
 */
function smart_reservations_multiform($form, &$form_state, $params = NULL, $no_js_use = FALSE) {

  // Provide a wrapper around the entire form, since we'll replace the whole
  // thing with each submit.
  $form['#prefix'] = '<div id="reservations-multiform-wrapper">';
  $form['#suffix'] = '</div>';
  $form['#tree'] = TRUE;
  // Reset title. Each subform will set it.
  drupal_set_title('');

  // $form_state['storage'] is where we'll keep variables of differents forms.
  $step = empty($form_state['storage']['step']) ? 1 : $form_state['storage']['step'];
  $form_state['storage']['step'] = $step;

  switch ($step) {
    // Search results.
    case 1:
      $form['search-results']
        = generate_form_elements_search_results($form_state, $params);
      break;

    // Make reservation.
    case 2:
      $form['customer']
        = generate_form_elements_customer($form_state);
      break;

    // Reservation subbmitted.
    case 3:
      drupal_set_title(t('Reservation submitted'));
      $form['submitted']
        = generate_form_elements_submitted($form_state);
      break;
  }
  return $form;
}

/**
 * Submit function for smart_reservations_multiform.
 */
function smart_reservations_multiform_submit($form, &$form_state) {
  // Form_state['storage']['values'] contains info of the form of each step:
  // search-results or customer.
  // We use the name of the forms to populate the subforms
  // (via form_state['values']) when needed.

  $current_step = $form_state['storage']['step'];

  // 1) Save the current information.

  // Get the name of the step. It's also the name of the form (very important).
  if ($current_step == 1) {
    $current_step_name = 'search-results';
  }
  if ($current_step == 2) {
    $current_step_name = 'customer';
  }
  if ($current_step == 3) {
    $current_step_name = 'submitted';
  }
  // Save the the current information into $form_state['values'].
  if (!empty($form_state['values'][$current_step_name])) {
    $form_state['storage']['values'][$current_step_name] = $form_state['values'][$current_step_name];
  }

  // 2) Increment or decrement the step as needed. Recover values if they exist.

  // Option 2.A) Make reservation button => we're going to the customer form.
  if ($form_state['triggering_element']['#name'] == 'make-reservation') {
    $form_state['storage']['step']++;

    // If values have already been entered for this step, recover them from
    // $form_state['storage'] to pre-populate them.
    if (!empty($form_state['storage']['values']['customer'])) {
      $form_state['values']['customer'] = $form_state['storage']['values']['customer'];
    }
  }

  // Option 2.B) Go back button => we're going back to search-results form.
  if ($form_state['triggering_element']['#name'] == 'go-back') {
    $form_state['storage']['step']--;
    // Recover our form values from storage.
    $form_state['values']['search-results'] = $form_state['storage']['values']['search-results'];
  }

  // Option 2.C) Submit button.
  if ($form_state['triggering_element']['#name'] == 'submit') {
    $form_state['storage']['step'] = 3;
    $code = smart_reservations_save_and_email_reservation($form_state);
    $form_state['storage']['reservation']['reservation_code'] = $code;
    $form_state['storage']['reservation']['reservation_status'] = 'Submitted';

    $value_message = t('Your reservation has been submitted.') . ' ';

    $value_message .= t('We will contact you to confirm it as soon as we have verified that everything is correct.');
    if (variable_get('sr_email_customer_submitted')) {
      $value_message .= '<br><br>' . t('We also have sent an email to your inbox.') . ' ';
      $value_message .= t("If you don't receive it, check your spam folder.");
    }

    drupal_set_message($value_message);
    $form_state['rebuild'] = TRUE;
    return;
  }

  // Otherwise, we still have work to do.
  $form_state['rebuild'] = TRUE;

}

/**
 * Form generation for Search results - Step 1.
 */
function generate_form_elements_search_results(&$form_state, $params) {
  if (empty($form_state['storage']['search-params'])) {
    $form_state['storage']['search-params'] = parse_url_search_params($params);
  }
  $p = $form_state['storage']['search-params'];

  // 1) Report search query to the log.
  if (variable_get('sr_log_reservations_searched', FALSE)) {
    $log_params = $p;
    $log_params['url_query'] = $params;
    sr_log_add('search', $log_params);
  }

  $build_form['title'] = array(
    '#type' => 'markup',
    '#markup' => t('Availability'),
    '#prefix' => '<div><h2>',
    '#suffix' => '</h2></div>',
    '#weight' => -10,
  );

  // 2) Build form: search options and submit button.
  $build_form['header_options'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div class="sr-results-header-options">',
    '#suffix' => '</div>',
    '#weight' => -5,
  );

  $build_form['header_options']['checkin_date'] = array(
    '#type' => 'date',
    '#title' => t('Check-In Date'),
    '#date_format' => 'Y-m-d',
    '#date_label_position' => 'invisible',
    '#date_timezone' => date_default_timezone(),
    '#date_year_range' => '0:+2',
    // '#prefix' => '<div class="smart-reservations-form">',
    // '#suffix' => '</div>',
    '#default_value' => array(
      'year' => substr($p['checkin'], 0, 4),
      'month' => (int) substr($p['checkin'], 5, 2),
      'day' => (int) substr($p['checkin'], 8, 2),
    ),
  );
  $build_form['header_options']['checkout_date'] = array(
    '#type' => 'date',
    '#title' => t('Check-Out Date'),
    '#date_format' => 'Y-m-d',
    '#date_timezone' => date_default_timezone(),
    '#default_value' => array(
      'year' => substr($p['checkout'], 0, 4),
      'month' => (int) substr($p['checkout'], 5, 2),
      'day' => (int) substr($p['checkout'], 8, 2),
    ),
    '#date_year_range' => '0:+2',
    // '#prefix' => '<div class="smart-reservations-form">',
    // '#suffix' => '</div>',
  );
  $build_form['header_options']['number_of_adults'] = array(
    '#type' => 'select',
    '#title' => t('Adults'),
    '#options' => array_combine(
        range(1, variable_get('sr_max_number_of_guests_to_search', 15)),
        range(1, variable_get('sr_max_number_of_guests_to_search', 15))),
    '#default_value' => $p['adults'],
    // '#prefix' => '<div class="smart-reservations-form">',
    // '#suffix' => '</div>',
    '#default_value' => $p['adults'],
  );
  $build_form['header_options']['number_of_children'] = array(
    '#type' => 'select',
    '#title' => t('Children'),
    '#options' => array_combine(
        range(0, variable_get('sr_max_number_of_guests_to_search', 15)),
        range(0, variable_get('sr_max_number_of_guests_to_search', 15))),
    '#default_value' => $p['children'],
    // '#prefix' => '<div class="smart-reservations-form">',
    // '#suffix' => '</div>',
  );
  $build_form['header_options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
    '#submit' => array('smart_reservations_search_results_form_submit'),
    // '#prefix' => '<div class="smart-reservations-form smart-reservations-form-button">',
    // '#suffix' => '</div>',
  );

  // 3) Build form: accommodation units info.
  // Get accommodation units.
  $acc_units = get_accommodation_units_info($p);
  $num_acc_units_available = 0;

  // 3.1) Header message.
  if ($acc_units['summary']['more_than_one_needed']) {
    $build_form['header_message'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="sr-results-warning sr-results-warning-header">',
      '#suffix' => '</div>',
      '#markup' => t(variable_get('sr_txt_capacity_exceeded')),
    );
  };

  // 3.2) Accomodation calendars and info.
  $summary = array();
  $summary['total_amount'] = 0;
  foreach ($acc_units['data'] as $acc_unit) {
    // Recover options if we are recalculating the form, for instance.
    $id = $acc_unit['id'];
    if (!empty($form_state['values']['search-results']['accommodation_units'][$id])) {
      $o = &$form_state['values']['search-results']['accommodation_units'][$id]['data']['options'];
      $p['adults'] = $o['number_of_adults'];
      $p['children'] = $o['number_of_children'];
      $p['babies'] = $o['number_of_babies'];
      $p['dogs'] = $o['number_of_dogs'];
    }
    $a = get_rates_per_date_range($id, $p);
    if (!empty($a)) {
      $num_acc_units_available++;
      $form_state['storage']['accommodation_units'][$id] = $a;
      $build_form['accommodation_units'][$id] = array(
        '#type' => 'fieldset',
        '#title' => $a['accommodation_unit']->title,
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#prefix' => '<div id="sr-search-results-accommodation-' . $id . '" class="sr-search-results-accommodation">',
        '#suffix' => '</div>',
        '#attributes' => array(
          'class' => array('sr-accommodation-unit'),
        ),
      );

      // Description.
      if (!empty($a['accommodation_unit']->body['und'][0]['value'])) {
        $o = $a['accommodation_unit']->body['und'][0]['value'];
        $build_form['accommodation_units'][$id]['data']['description'] = array(
          '#type' => 'markup',
          '#markup' => $o,
          '#weight' => 10,
          '#prefix' => '<div class="row sr-accommodation-description">',
          '#suffix' => '</div>',
        );
      }
      // Options.
      $build_form['accommodation_units'][$id]['data']['options'] = array(
        '#type' => 'fieldset',
        '#weight' => 20,
        '#attributes' => array(
          'class' => array('sr-accommodation-options'),
          'id' => array('sr-accommodation-options-' . $id),
        ),
      );
      $build_form['accommodation_units'][$id]['data']['options']['number_of_adults']
        = array(
          '#type' => 'select',
          '#title' => t('Adults'),
          '#options' => array_combine(
              range($acc_unit['min_guests'], $acc_unit['max_guests']),
              range($acc_unit['min_guests'], $acc_unit['max_guests'])),
          '#default_value' => $a['summary']['number_of_adults'],
          '#ajax' => array(
            'callback' => 'smart_reservations_search_recalc_ajax',
            // 'wrapper' => 'sr-search-results-accommodation-' . $id,
          ),
        );
      $build_form['accommodation_units'][$id]['data']['options']['number_of_children']
        = array(
          '#type' => 'select',
          '#title' => t('Children'),
          '#options' => array_combine(
              range(0, $acc_unit['max_guests']),
              range(0, $acc_unit['max_guests'])),
          '#default_value' => $a['summary']['number_of_children'],
          '#ajax' => array(
            'callback' => 'smart_reservations_search_recalc_ajax',
            // 'wrapper' => 'sr-search-results-accommodation-' . $id,
          ),
        );
      $build_form['accommodation_units'][$id]['data']['options']['number_of_babies']
        = array(
          '#type' => 'select',
          '#title' => t('Babies'),
          '#options' => array_combine(
              range(0, $acc_unit['max_guests']),
              range(0, $acc_unit['max_guests'])),
          '#default_value' => $a['summary']['number_of_babies'],
          '#ajax' => array(
            'callback' => 'smart_reservations_search_recalc_ajax',
            // 'wrapper' => 'sr-search-results-accommodation-' . $id,
          ),
        );
      $build_form['accommodation_units'][$id]['data']['options']['number_of_dogs']
        = array(
          '#type' => 'select',
          '#title' => t('Dogs'),
          '#options' => array_combine(
              range(0, $acc_unit['max_dogs']),
              range(0, $acc_unit['max_dogs'])),
          '#default_value' => $a['summary']['number_of_dogs'],
          '#ajax' => array(
            'callback' => 'smart_reservations_search_recalc_ajax',
            // 'wrapper' => 'sr-search-results-accommodation-' . $id,
          ),
        );

      // Accommodation - details (additional info).
      $o = markup_accommodation_details($a, $p);
      $build_form['accommodation_units'][$id]['data']['details'] = array(
        '#type' => 'markup',
        '#markup' => $o,
        '#weight' => 30,
        '#prefix' => '<div class="sr-accommodation-details">',
        '#suffix' => '</div>',
      );

      // Calendar.
      $o = markup_accommodation_calendar($a, $p);
      $build_form['accommodation_units'][$id]['data']['calendar'] = array(
        '#type' => 'markup',
        '#markup' => $o,
        '#weight' => 40,
        '#prefix' => '<div id="sr-search-results-accommodation-calendar-' .
        $id . '" class="sr-accommodation-calendar">',
        '#suffix' => '</div>',
      );

      // Accommodation summary.
      $o = markup_accommodation_summary($a);
      $build_form['accommodation_units'][$id]['data']['summary'] = array(
        '#type' => 'markup',
        '#markup' => $o,
        '#weight' => 50,
        '#prefix' => '<div class="sr-accommodation-summary">',
        '#suffix' => '</div>',
      );

      // Accommodation buttons.
      $build_form['accommodation_units'][$id]['data']['buttons'] = array(
        '#type' => 'fieldset',
        '#prefix' => '<div class="sr-accommodation-buttons" id="sr-accommodation-buttons-' . $id . '">',
        '#suffix' => '<br></div>',
        '#weight' => 60,
      );
      $selected = !empty($form_state['storage']['selected_accommodations'][$id]);
      $build_form['accommodation_units'][$id]['data']['buttons']['add_remove'] = array(
        // Very important to have a unique name - for ajax calls.
        '#name' => 'add_remove-' . $id,
        '#type' => 'submit',
        '#value' => $selected ? t('Remove') : t('Add to the reservation') ,
        '#submit' => array('smart_reservations_add_remove_reservation'),
        '#ajax' => array(
          'callback' => 'smart_reservations_add_remove_reservation_ajax_callback',
          //  We don't need a wrapper / special ajax callback / multiple divs.
          // 'wrapper' => 'sr-accommodation-buttons-' . $id,
        ),
        '#attributes' => array(
          'class' => array('sr-button-' . ($selected ? 'red' : 'green')),
        ),
      );
      if ($selected) {
        // Update summary,
        $summary['acc_units_sum'][$id] = $a['summary'];
        $summary['total_amount'] += $a['summary']['total_amount'];
        // Update the array that contains the reservation info.
        $form_state['storage']['reservation']['reservation_status'] = 'Created';
        $form_state['storage']['reservation']['total_amount']
          = $summary['total_amount'];
      }
    }
  }

  // Summary.
  $make_reservation = FALSE;
  $sum_class = 'sr-summary';
  $sum_title = '';

  if (!empty($form_state['storage']['selected_accommodations'])) {
    $make_reservation = TRUE;
    $sum_class .= ' sr-summary-make-reservation';
    $sum_title = t('Reservation');
  }

  $prefix = '<div class="' . $sum_class . '" id="sr-summary">';
  $prefix .= '<h2 class="sr-summary-title">' . $sum_title . '</h2>';
  $build_form['summary'] = array(
    '#type' => 'fieldset',
    '#prefix' => $prefix,
    '#suffix' => '</div>',
    '#weight' => -1,
  );

  // If there is more than one accommodation unit selected, print totals.
  if (!empty($form_state['storage']['selected_accommodations'])) {

    $m = markup_search_results_summary($summary);
    $build_form['summary']['data']['totals'] = array(
      '#type' => 'markup',
      '#markup' => $m,
    );
    $build_form['summary']['data']['buttons'] = array(
      '#name' => 'make-reservation',
      '#type' => 'submit',
      '#value' => t('Make reservation'),
      // '#ajax' => array(
      //   'wrapper' => 'reservations-multiform-wrapper',
      //   'callback' => 'smart_reservations_multiform_callback',
      //   'effect' => 'fade',
      // ),
      '#attributes' => array(
        'class' => array('sr-button-green'),
      ),
      '#prefix' => '<div class="sr-summary-buttons">',
      '#suffix' => '</div>',

    );
  }

  switch ($num_acc_units_available) {
    case 0:
      // There aren't any accommodation units available.
      $build_form['accommodation_units'] = array(
        '#type' => 'fieldset',
        '#title' => t("There's no availability."),
        '#prefix' => '<div>',
        '#markup' => t('Try to search with different dates.'),
        '#suffix' => '</div>',
      );
      $build_form['accommodation_units']['text'] = array(
        '#type' => 'markup',
        '#prefix' => '<div class="row">',
        '#markup' => t('Try to search with different dates.'),
        '#suffix' => '</div>',
      );
      break;

    // Pending to remove. Old funcionality.
    // There is only one accommodation unit available.
    // We replace the button 'Add to the reservation' for 'Make reservation'.
    case 1:
      // $id = key($build_form['accommodation_units']);
      // $form_state['storage']['selected_accommodations'][$id] = $id;
      // unset($build_form['accommodation_units'][$id]['data']['buttons']['add_remove']);
      // $build_form['accommodation_units'][$id]['data']['buttons']['make'] = array(
      //   '#name' => 'make-reservation',
      //   '#type' => 'submit',
      //   '#value' => t('Make reservation'),
      //   '#prefix' => '<div class="sr-accommodation-parameters-form sr-accommodation-parameters-form-button">',
      //   '#suffix' => '</div>',
      //   // '#ajax' => array(
      //   //   'wrapper' => 'reservations-multiform-wrapper',
      //   //   'callback' => 'smart_reservations_multiform_callback',
      //   //   'effect' => 'fade',
      //   // ),
      //   '#attributes' => array(
      //     'class' => array('sr-button-green'),
      //   ),
      //   '#weight' => -100,
      // );
  }
  return $build_form;
}

/**
 * Form generation for Search results - Step 2.
 */
function generate_form_elements_customer(&$form_state) {
  $form['title'] = array(
    '#type' => 'markup',
    '#markup' => t('Reservation'),
    '#prefix' => '<div class="reservation-submit-title"><h1>',
    '#suffix' => '</h1></div>',
    '#weight' => -10,
  );
  $form['contact'] = array(
    '#type' => 'fieldset',
    '#prefix' => '<div class="reservation-submit-contact">',
    '#suffix' => '</div>',
    '#title' => t('Contact information'),
    '#weight' => -5,
  );
  $form['contact']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name') . ' *' ,
    '#default_value' => empty($form_state['values']['customer']['contact']['name']) ?
    '' : $form_state['values']['customer']['contact']['name'],
    '#size' => 60,
    '#maxlength' => 90,
  );
  $form['contact']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('email') . ' *' ,
    '#default_value' => empty($form_state['values']['customer']['contact']['email']) ?
    '' : $form_state['values']['customer']['contact']['email'],
    '#size' => 60,
    '#maxlength' => 90,
  );
  $form['contact']['phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone') . ' *' ,
    '#default_value' => empty($form_state['values']['customer']['contact']['phone']) ?
    '' : $form_state['values']['customer']['contact']['phone'],
    '#size' => 60,
    '#maxlength' => 20,
  );
  $form['contact']['tax_identifier'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier # (optional: driving license ID, Social Security Number, etc.'),
    '#default_value' => empty($form_state['values']['customer']['contact']['tax_identifier']) ?
    '' : $form_state['values']['customer']['contact']['tax_identifier'],
    '#size' => 60,
    '#maxlength' => 25,
  );

  // Comment. Retrieve if we already have it.
  if (!empty($form_state['values']['customer']['contact']['add_a_comment'])) {
    $default_value = $form_state['values']['customer']['contact']['add_a_comment'];
  }
  else {
    $default_value = array();
  }
  // Comment. Checkbox.
  $form['contact']['add_a_comment'] = array(
    '#type' => 'checkboxes',
    '#options' => array(1 => t('Add a comment')),
    '#prefix' => '<div class="sr-accommodation-submit-add-comment">',
    '#suffix' => '</div>',
    '#default_value' => $default_value,
    '#ajax' => array(
      'callback' => 'ajax_comments_callback',
      'wrapper' => 'contact-comments',
      'effect' => 'fade',
    ),
  );
  $form['contact']['textfields'] = array(
    '#prefix' => '<div id="contact-comments" div class="sr-accommodation-submit-comment">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
  );
  // Comment. Text.
  if (!empty($default_value)) {
    if ($default_value['1'] == 1) {
      $form['contact']['textfields']['comments'] = array(
        '#type' => 'textarea',
        '#title' => t('Comment'),
        '#default_value' => $form_state['values']['customer']['contact']['textfields']['comments'],
        '#attributes' => array('maxlength' => 1100),
      );
    }
  }

  // Reservation summary.
  $form['reservation-summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reservation summary'),
    '#prefix' => '<div class="reservation-submit-summary">',
    '#suffix' => '</div>',
  );
  $form['reservation-summary']['summary']
  = $form_state['complete form']['search-results']['summary']['data']['totals'];
  // unset($form['reservation-summary']['data']['buttons']);

  // Reservation details.
  $form['reservation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reservation details'),
    '#prefix' => '<div class="sr-accommodation-submit-reservationetails">',
    '#suffix' => '</div>',
  );

  foreach ($form_state['storage']['selected_accommodations'] as $id) {
    $a = &$form_state['complete form']['search-results']['accommodation_units'][$id];
    unset($a['data']['options']);
    unset($a['data']['buttons']);
    $form['reservation']['form'][$id] = $a;
  }

  $form['actions'] = array(
    '#type' => 'actions',
    '#prefix' => '<div class="sr-accommodation-submit-actions">',
    '#suffix' => '</div>',
  );
  $form['actions']['prev'] = array(
    '#name' => 'go-back',
    '#type' => 'submit',
    '#value' => t("Go back / Change reservation"),
    // Since all info will be discarded, don't validate on 'prev'.
    // '#limit_validation_errors' => array(),
    // #submit is required to use #limit_validation_errors
    '#submit' => array('smart_reservations_multiform_submit'),
    '#ajax' => array(
      'wrapper' => 'reservations-multiform-wrapper',
      'callback' => 'smart_reservations_multiform_callback',
      'effect' => 'fade',
    ),
  );
  $form['actions']['submit'] = array(
    '#name' => 'submit',
    '#type' => 'submit',
    '#value' => t("Submit the reservation"),
    '#submit' => array('smart_reservations_multiform_submit'),
    '#ajax' => array(
      'wrapper' => 'reservations-multiform-wrapper',
      'callback' => 'smart_reservations_multiform_callback',
      'effect' => 'fade',
    ),
    '#attributes' => array(
      'class' => array('sr-button-green'),
    ),
  );

  // Save reservation details (fieldset) info into the storage array.
  $form_state['storage']['reservation-fieldset'] = $form['reservation'];
  $form_details = array();
  $form_details[] = $form['reservation-summary'];
  $form_details[] = $form['reservation'];
  $form_state['storage']['reservation-fieldset']['reservation_rendered']
    = drupal_render($form_details);

  return $form;
}

/**
 * Form generation for Search results - Step 3.
 */
function generate_form_elements_submitted(&$form_state) {

  $m = '<h2>' . t('Reservation submitted') . '</h2>';
  $contact = &$form_state['storage']['values']['customer']['contact'];
  $m .= '<div><b>' . t('Reservation code') . '</b>: ' . $form_state['storage']['reservation']['reservation_code'] . '</div>';
  $m .= '<div><b>' . t('Name') . '</b>: ' . $contact['name'] . '</div>';
  $m .= '<div><b>' . t('Phone') . '</b>: ' . $contact['phone'] . '</div>';
  $m .= '<div><b>' . t('email') . '</b>: ' . $contact['email'] . '</div>';
  $m .= '<div><b>' . t('Identifier ID') . '</b>: ' . $contact['tax_identifier'] . '</div>';
  // $m .= '<div><b>' . t('Comments') . '</b>: ' ;
  // $m .= $contact['textfields']['comments'] . '</div>';
  $form['submitted']['customer'] = array(
    '#type' => 'markup',
    '#markup' => $m,
    '#prefix' => '<div class="reservation-customer-info">',
    '#suffix' => '</div>',
  );
  $m = $form_state['storage']['reservation-fieldset']['reservation_rendered'];
  $form['reservation-info'] = array(
    '#type' => 'markup',
    '#markup' => $m,
  );
  return $form;
}

/**
 * Wizard callback function.
 *
 * @param array $form
 *   Form API form.
 * @param array $form_state
 *   Form API form.
 *
 * @return array
 *   Form array.
 */
function smart_reservations_multiform_callback($form, $form_state) {
  return $form;
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
function smart_reservations_multiform_validate($form, &$form_state) {


  if ($form_state['triggering_element']['#name'] == 'submit') {

    $name = $form_state['values']['customer']['contact']['name'];
    if (empty($name)) {
      form_set_error('smart_reservations_multiform_validate',
        t('You must enter a name.'));
    }
    elseif (strlen($name) < 4) {
      form_set_error('smart_reservations_multiform_validate',
        t('The name is too short.'));
    }

    if (empty($form_state['values']['customer']['contact']['phone'])) {
      form_set_error('smart_reservations_multiform_validate',
        t('You must enter a phone number.'));
    }

    if (!valid_email_address(
      $form_state['values']['customer']['contact']['email'])) {
      form_set_error('[submitted][email_address]',
        t('The email address appears to be invalid.'));
    }

  }
}

/**
 * Implements save reservation logic.
 * */
function smart_reservations_save_and_email_reservation($form_state) {

  // Variables.
  $res_number = variable_get('sr_next_reservation_number');
  $prefix = variable_get('sr_reservation_prefix');
  $res_code = $prefix . sprintf("%06d", $res_number);
  $contact = &$form_state['values']['customer']['contact'];
  global $user;

  // 1) Save reservation.
  // New node.
  $node = new stdClass();
  $node->uid = $user->uid;
  $node->title = $res_code;
  $node->type = 'sr_reservation';
  node_object_prepare($node);
  $node->language = LANGUAGE_NONE;

  // Contact info.
  $node->smres_contact_name['und'][0]['value'] = $contact['name'];
  $node->smres_contact_email['und'][0]['value'] = $contact['email'];
  $node->smres_contact_phone['und'][0]['value'] = $contact['phone'];
  $node->smres_contact_tax_identifier['und'][0]['value'] = $contact['tax_identifier'];
  $node->smres_comments['und'][0]['value'] = $contact['textfields']['comments'];

  // Reservation details.
  $node->smres_reservation_details['und'][0]['value']
    = $form_state['storage']['reservation-fieldset']['reservation_rendered'];
  $node->smres_reservation_details['und'][0]['format'] = 'full_html';

  // Amount and deposit
  $node->smres_total_amount['und'][0]['value']
    = $form_state['storage']['reservation']['total_amount'];
  $node->field_smres_deposit_amount['und'][0]['value']
    = round( $form_state['storage']['reservation']['total_amount']
              * ( variable_get('sr_default_deposit_percentage') / 100)
      , 0, PHP_ROUND_HALF_DOWN);
 $node->field_smres_deposit_instructions['und'][0]['value']
    = variable_get('sr_default_deposit_instructions');

  $node->smres_reservation_status['und'][0]['value'] = 'Submitted';


  $node = node_submit($node);

  $res_number++;
  variable_set('sr_next_reservation_number', $res_number);

  // Save node.
  node_save($node);

  // To check if the saving was successful, we check the nid.
  $node_id = $node->nid;

  // 2) Add reservation items to the database.
  if (isset($node_id)) {
    foreach ($form_state['storage']['selected_accommodations'] as $acc_id) {
      $a = &$form_state['storage']['accommodation_units'][$acc_id];
      foreach ($a['rates_per_night'] as $d) {
        $form_entry_id = db_insert('smart_reservations_reservation_items')
          ->fields(array(
            'date' => $d['date'],
            'reservation_id' => $node_id,
            'accommodation_unit_id' => $acc_id,
            'amount' => $d['amount_per_night'],
            'amount_per_night_adults' => $d['amount_per_night_adults'],
            'amount_per_night_children' => $d['amount_per_night_children'],
            'amount_per_night_dogs' => $d['amount_per_night_dogs'],
            'amount_per_night_cats' => $d['amount_per_night_cats'],
            'total_amount' => $a['summary']['total_amount'],
            'number_of_adults' => $a['summary']['number_of_adults'],
            'number_of_children' => $a['summary']['number_of_children'],
            'number_of_babies' => $a['summary']['number_of_babies'],
            'number_of_dogs' => $a['summary']['number_of_dogs'],
            'number_of_cats' => $a['summary']['number_of_cats'],
            'base_rate' => $d['base_rate'],
            'children_discount' => $d['children_discount'],
            'dogs_fee' => $d['dogs_fee'],
            'low_occupancy_fee' => $d['low_occupancy_fee'],
            'low_occupancy_threshold' => $d['low_occupancy_threshold'],
            'single_night_fee' => $d['single_night_fee'],
            'date_creation' => time(),
            'date_modification' => time(),
            ))
          ->execute();
      }
    }
  }

  // 3) Send email to the customer.
  if (variable_get('sr_email_customer_submitted')) {
    $mail['to'] = $node->smres_contact_email['und'][0]['value'] = $contact['email'];
    $mail['subject'] = variable_get('site_name');

    $message['subject'] = t('!site',
        array('!site' => variable_get('site_name', "Default site name")),
        array('langcode' => $langcode));
    $mail['subject'] .= ' - ' . t('reservation submitted');

    $mail['body'] = array();
    $mail['body'][] = t('Thanks for submitting your reservation with code @code .',
        array('@code' => $code));
    $mail['body'][] = t('We will verify that everything is correct and we will contact you to confirm it as soon as possible.');
    $mail['body'][] = '';
    smart_reservations_mail_send($mail);
  }

  // Send email to the admin.
  if (variable_get('sr_email_admin_submitted')) {
    unset($mail['to']);
    $mail['to'] = variable_get('site_mail', 'info@example.com');
    unset($mail['body']);
    $mail['body'] = array();
    $mail['body'][] = t('Reservation submitted');
    $mail['body'][] = t("There's a new reservation submitted.");
    $mail['body'][] = t('Reservation code: @code.',
        array('@code' => $code));
    $mail['body'][] = '';
    smart_reservations_mail_send($mail);
  }
  return $res_code;
}

/**
 * Callback for optional comments
 *   Renderable array (the textfields element)
 */
function ajax_comments_callback($form, $form_state) {
  return $form['customer']['contact']['textfields'];
}

/**
 * Submit handler for the "add-one-more" button.
 * */
function smart_reservations_add_remove_reservation($form, &$form_state) {
  $id = $form_state['triggering_element']['#parents'][2];

  if (empty($form_state['storage']['selected_accommodations'][$id])) {
    $form_state['storage']['selected_accommodations'][$id] = $id;
  }
  else {
    unset($form_state['storage']['selected_accommodations'][$id]);
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Callback for add/remove ajax-enabled button.
 */
function smart_reservations_add_remove_reservation_ajax_callback($form, $form_state) {

  // Initialize variables.
  $id = $form_state['triggering_element']['#parents'][2];
  $commands = array();

  // 1) Prepare ajax_command_replace for accommodation buttons.
  $wrapper = '#sr-accommodation-buttons-' . $id;
  $f = $form['search-results']['accommodation_units'][$id]['data']['buttons'];
  $html = drupal_render($f);
  $commands[] = ajax_command_replace($wrapper, $html);

  // 2) Prepare ajax_command_replace for totals / summary.
  $wrapper = '#sr-summary';
  $f = $form['search-results']['summary'];
  $html = drupal_render($f);
  $commands[] = ajax_command_replace($wrapper, $html);

  return array('#type' => 'ajax', '#commands' => $commands);
  // old method: return $form['search-results']['accommodation_units'][$id]['data']['buttons'];
}

/**
 * Implements submission logic of the Recalculate button.(AJAX call)
 */
function smart_reservations_search_recalc_ajax($form, $form_state) {

  // Initialize variables.
  $id = $form_state['triggering_element']['#parents'][2];
  $acc_id = $form_state['triggering_element']['#parents'][2];
  $commands = array();

  // 1) Change value of children if the form changed the input value.
  // This happens when there is no capacity.
  $form['search-results']['accommodation_units'][$id]['data']['options']['number_of_children']['#value']
    = $form_state['storage']['accommodation_units'][$id]['summary']['number_of_children'];

  // 2) Prepare ajax_command_replace for accommodation unit fieldset.
  $wrapper = '#sr-search-results-accommodation-' . $id;
  $f = $form['search-results']['accommodation_units'][$id];
  $html = drupal_render($f);
  $commands[] = ajax_command_replace($wrapper, $html);

  // 3) Prepare ajax_command_replace for totals / summary.
  $wrapper = '#sr-summary';
  $f = $form['search-results']['summary'];
  $html = drupal_render($f);
  $commands[] = ajax_command_replace($wrapper, $html);

  return array('#type' => 'ajax', '#commands' => $commands);

  // return $form['search-results']['accommodation_units'][$acc_id];
}

/**
 *
 */
function markup_accommodation_details($a, $p) {

  $o = t('Capacity') . ': ';
  $o .= $a['accommodation_unit']->smres_minimum_number_of_people['und'][0]['value'];
  $o .= ' - ';
  $o .= $a['accommodation_unit']->smres_maximum_number_of_people['und'][0]['value'];
  $o .= ' ' . t('people') . '. ';
  if (!$a['summary']['enough_capacity']) {
    $o = '<div class="sr-results-warning">' . $o;
    $o .= t("There is not enough capacity for %a / %c.", array(
        '%a' => format_plural($p['adults'], t('1 adult'), '@count ' . t('adults')),
        '%c' => format_plural($p['children'], t('1 child'), '@count ' . t('children'))));
    $o .= ' ' . t("Showing results for %a / %c.", array(
        '%a' => format_plural($a['summary']['number_of_adults'], t('1 adult'), '@count ' . t('adults')),
        '%c' => format_plural($a['summary']['number_of_children'], t('1 child'), '@count ' . t('children'))));
    $o .= '</div>';
  }
  if (!$a['summary']['exceeds_minimum_guests']) {
    $o = '<div class="sr-results-warning">' . $o;
    $o .= t("Minimum guests is %g.", array(
        '%g' => $a['accommodation_unit']->smres_minimum_number_of_people['und'][0]['value']));
    $o .= '</div>';
  }

  return $o;
}

/**
 *
 */
function markup_accommodation_calendar($a, $p) {

  // @todo use drupal local settings
  setlocale(LC_TIME, 'ca_ES.UTF-8', 'Catalan_Spain', 'Catalan');
  $df = variable_get('sr_date_format_search_results');

  $nights = array(
    0 => t('Sun'),
    1 => t('Mon'),
    2 => t('Tue'),
    3 => t('Wed'),
    4 => t('Thu'),
    5 => t('Fri'),
    6 => t('Sat'),
    7 => t('Sun'),
  );
  $week_starts = variable_get('sr_week_starts_on', 0);
  $i = 0;
  // Calendar header.
  $o = '<div class = "row cal-sr-results-header">';
  while ($i < 7) {
    $o .= '<div class="col-sm-17 cal-sr-results-cell ';
    $o .= 'cal-sr-results-cell-header text-center"><b>';
    $o .= t($nights[$i + $week_starts]) . '</b></div>';
    $i++;
  }
  // cal-sr-results-header ends here.
  $o .= '</div>';

  $checkin = array_values($a['rates_per_night'])[0]['date'];
  $checkin_weekday_num = date('N', strtotime($checkin));
  $first_date_of_the_week = date('Y-m-d',
    strtotime($checkin . '-' . $checkin_weekday_num . ' day'));

  // Fill previous dates to the check-in date.

  $i = $week_starts;
  while ($i < ($checkin_weekday_num)) {
    $o .= (($i % 7) == $week_starts) ?
    '<div class="pipon row cal-sr-results-row">' : '';
    $o .= '<div class="col-sm-17 cal-sr-results-cell ';
    $o .= 'cal-sr-results-cell-info text-center">';
    $o .= '' . strftime($df,
      strtotime($first_date_of_the_week . '+' . $i . ' day')) . '<br> -';
    $o .= '</div>';
    $i++;

    $o .= (($i % 7) == $week_starts) ?
      '</div>' : '';
  }


  // Fill dates from check-in to check-out.
  foreach ($a['rates_per_night'] as $r) {
    $o .= (($i % 7) == $week_starts) ? '<div class="row cal-sr-results-row">' : '';
    $o .= '<div class="col-sm-17 cal-sr-results-cell text-center">';
    $o .= '' . strftime($df, strtotime($r['date'])) . '<br>';
    $o .= sr_curr($r['amount_per_night']) . '</div>';
    $i++;
    // Close row if needed.
    $o .= (($i % 7) == $week_starts) ? '</div>' : '';
  }

  // Fill Check-out date.
  $o .= (($i % 7) == $week_starts) ? '<div class="row cal-sr-results-row">' : '';
  // Add check-out date.
  $o .= '<div class="col-sm-17 cal-sr-results-cell text-center">';
  $o .= '' . strftime($df, strtotime($r['date'] . '+1 day')) . '<br>';
  $o .= t('Check-Out') . '</div>';

  // Fill the dates from the check-out to the last day of the week.
  $checkout_weekday_number = date('N', strtotime($r['date'] . ' +1 day'));
  $i = 2;
  while ($checkout_weekday_number < $week_starts + 6) {
    $o .= '<div class="col-sm-17 cal-sr-results-cell cal-sr-results-cell-info text-center">';
    $o .= strftime($df, strtotime( $r['date'] . ' +' . $i . ' day')) . '<br>';
    $o .= '-</div>';
    $i++;
    $checkout_weekday_number++;
  }

  // Close calendar last row and body.
  $o .= '</div>';

  return $o;
}

/**
 *
 */
function markup_accommodation_summary($a) {

  // Start footer.
  $o = '<div class="row cal-sr-results-row-footer">';

  $div_t = '<div class="col-sr-results-summary col-sr-results-summary-total cal-sr-results-cell cal-sr-results-cell-footer text-center">';
  $div = '<div class="col-sr-results-summary cal-sr-results-cell cal-sr-results-cell-footer text-center">';
  $cell_empty = '<div class="col-sr-results-summary cal-sr-results-cell cal-sr-results-cell-empty"></div>';
  // $o .= $cell_empty;
  $o .= $div_t . '<b>' . t('Total') . '<br>';
  $o .= sr_curr($a['summary']['total_amount']) . '</b></div>';

  $o .= $div . t('Nights') . ': ' . $a['summary']['total_nights'];
  $o .= '<br>' . '≈ ' . sr_curr($a['summary']['rate_per_night']);
  $o .= ' / ' . t('night') . '</div>';

  $c2 = $div;
  $c2 .= t('Adults') . ': ' . $a['summary']['number_of_adults'] ;
  $c2 .= '<br>' . '≈ ' . sr_curr($a['summary']['rate_per_night_adult']);
  $c2 .= ' / ' . t('night');

  if ($a['summary']['number_of_children'] > 0) {
    $c2 .= '<br><br>' . t('Children') . ': ' . $a['summary']['number_of_children'];
    $c2 .= '<br>' . '≈ ' .  sr_curr($a['summary']['rate_per_night_children']);
    $c2 .= ' / ' . t('night');
  }
  $c2 .= '</div>';

  $c3 = '';
  if ($a['summary']['number_of_babies'] > 0) {
    $c3 = $div;
    $c3 .= t('Babies') . ': ' . $a['summary']['number_of_babies'];
    $c3 .= '<br>' . '≈ ' .  sr_curr($a['summary']['rate_per_night_babies']);
    $c3 .= ' / ' . t('night');
  }

  if ($a['summary']['number_of_dogs'] > 0) {
    if ($a['summary']['number_of_babies'] > 0) {
      $c3 .= '<br><br>';
    }
    else {
      $c3 = $div;
    }
    $c3 .= t('Dogs') . ': ' . $a['summary']['number_of_dogs'];
    $c3 .= '<br>' . '≈ ' .  sr_curr($a['summary']['rate_per_night_dogs']);
    $c3 .= ' / ' . t('night');
    $c3 .= '</div>';
  }
  if (!empty($c3)) {
    $c3 .= '</div>';
  }

  // // End footer row.
  $o .= $c2 . $c3 . '</div>';

  return $o;
}

/**
 *
 */
function markup_search_results_summary($summary) {

  // Start footer.
  $pre1 = '<div class="row sr-summary-info">';
  $suf1 = '</div>';

  $sum = '<div class="sr-summary-info-body">';
  foreach ($summary['acc_units_sum'] as $a) {
    $sum .= '<div class="sr-summary-info-body-row">';

    $sum .= '<div class="col-text">';
    $sum .= "<b>{$a['title']}</b> <br> {$a['headline_date']}";
    $sum .= "<br>{$a['headline_desc']}</div>";

    $sum .= '<div class="col-total"><br><br>';
    $sum .= sr_curr($a['total_amount']) . '</div>';

    $sum .= '</div>';
  }
  $sum .= '</div>';

  // $totals = '<div class="sr-summary-info-footer col-sr-results-summary col-sr-results-summary-total cal-sr-results-cell cal-sr-results-cell-footer text-center">';
  $totals = '<div class="sr-summary-info-footer">';
  $totals .= '<b>' . t('Total amount') . ' ';
  $totals .= sr_curr($summary['total_amount']) . '</b></div>';

  return $pre1 . $sum . $totals . $suf1;
}

/**
 * Accommodation Unit: tab Reservations.
 *
 * @return array
 *   A render array set for theming by theme_table().
 */
function smart_reservations_acc_unit_tab_reservations($form, &$form_state, $node = NULL) {

  // Create a table with the reservations per day.

  // 1) Build the header of the table.
  $rows_header = array(
    // The header gives the table the information it needs in order to make
    // the query calls for ordering. TableSort uses the field information
    // to know what database column to sort by.
    array(
      'data' => t('Date'),
      'field' => 'r.date',
      // Sort by this column by default.
      'sort' => 'desc',
    ),
    array('data' => t('Reservation'), 'field' => 'r.reservation_id'),
    array('data' => t('Amount'), 'field' => 'r.total_amount'),
    array('data' => t('Adults'), 'field' => 'r.number_of_adults'),
    array('data' => t('Children'), 'field' => 'r.number_of_children'),
    array('data' => t('Babies'), 'field' => 'r.number_of_babies'),
    array('data' => t('Dogs'), 'field' => 'r.number_of_dogs'),
  );

  // 2.1) Get the data of the table.
  // Using the TableSort Extender is what tells the the query object that we
  // are sorting.
  $query = db_select('smart_reservations_reservation_items', 'r')
    ->condition('accommodation_unit_id', $node->nid, '=')
    ->extend('TableSort')
    ->extend('PagerDefault')
    ->limit(30);
  $query->join('node', 'n', 'r.reservation_id = n.nid');
  $query->fields('r', array('date'));
  $query->fields('n', array('title', 'nid'));
  $query->fields('r', array(
    'total_amount',
    'number_of_adults',
    'number_of_children',
    'number_of_babies',
    'number_of_dogs',
    )
  );
  $result = $query
    ->orderByHeader($rows_header)
    ->execute();

  // 2.2) Build the array of the rows for the table.
  $rows = array();
  foreach ($result as $row) {
    $url = l(
      $row->title, drupal_get_path_alias('node/' . $row->nid),
      array('absolute' => TRUE));
    $row->title = $url;
    unset($row->nid);

    $rows[] = array('data' => (array) $row);
  }

  // 3) Build the form.
  $form_table['smart_reservations_search_log_table'] = array(
    '#theme' => 'table',
    '#header' => $rows_header,
    '#rows' => $rows,
    '#prefix' => '<div id="acc_id-reservations-table">',
    '#suffix' => '</div>',
  );

  $build_form['body'] = $form_table;
  return $build_form;
}

/**
 * Implements submission logic from the Form API.
 *
 * @param form $form
 *   A structured array containing the elements and properties of the form.
 * @param form $form_state
 *   An array that stores information about the form's current state
 *   during processing.
 */
function smart_reservations_search_results_form_submit($form, &$form_state) {
  $header_options = $form_state['values']['search-results']['header_options'];

  // We will change the url, to have a link for each search.
  if ($form_state['submitted']) {
    $d1 = '_';
    $d2 = '__';
    $url_params = '/checkin' . $d1 . $header_options['checkin_date']['year'];
    $url_params .= '-' . sprintf("%02d", $header_options['checkin_date']['month']);
    $url_params .= '-' . sprintf("%02d", $header_options['checkin_date']['day'])  . $d2;
    $url_params .= 'checkout' . $d1 . $header_options['checkout_date']['year'];
    $url_params .= '-' . sprintf("%02d", $header_options['checkout_date']['month']);
    $url_params .= '-' . sprintf("%02d", $header_options['checkout_date']['day'])  . $d2;
    $url_params .= 'adults' . $d1 . $header_options['number_of_adults'] . $d2;
    $url_params .= 'children' . $d1 . $header_options['number_of_children'] . $d2;
    $url_params .= 'babies' . $d1 . 0 . $d2;
    $url_params .= 'dogs' . $d1 . 0 . $d2;
    $url_params .= 'cats' . $d1 . 0 ;
    $form_state['redirect'] = array('search-available' . $url_params,
       array('external' => FALSE)
    );
  }
}

/**
 * Encode the url search params into a string.
 *
 * array @param $params
 */
// Function encode_url_search_params($params) {
// }

/**
 * Parse the search params into an array.
 *
 * string @param $params
 */
function parse_url_search_params($params) {
  if (is_null($params)) {
    $p['checkin'] = '';
    $p['checkout'] = '';
    $p['adults'] = '';
    $p['children'] = '';
    $p['babies'] = '';
    $p['dogs'] = '';
    $p['cats'] = '';
  }
  else {
    $p = array();
    foreach (explode('__', $params) as $item) {
      foreach (explode('_', $item) as $sub_item) {
        $sub_items[] = $sub_item;
      }
      if (count($sub_items) > 1) {
        $p[$sub_items[0]] = $sub_items[1];
        $sub_items = "";
        $items[] = $item;
      };
    }
    $p['checkin'] = empty($p['checkin']) ? '' :
      check_plain(substr($p['checkin'], 0, 20));
    $p['checkout'] = empty($p['checkout']) ? '' :
      check_plain(substr($p['checkout'], 0, 20));
    $p['adults'] = empty($p['adults']) ? '' :
      check_plain(substr($p['adults'], 0, 3));
    $p['children'] = empty($p['children']) ? '' :
      check_plain(substr($p['children'], 0, 3));
    $p['babies'] = empty($p['babies']) ? '' :
      check_plain(substr($p['babies'], 0, 3));
    $p['dogs'] = empty($p['dogs']) ? '' :
      check_plain(substr($p['dogs'], 0, 3));
    $p['cats'] = empty($p['cats']) ? '' :
      check_plain(substr($p['cats'], 0, 3));
  }

  // Sanitize check-in date.
  $min_date = date('Y-m-d', strtotime("now"));
  $range = 'now +' . variable_get('sr_max_checkin_date') . 'days';
  $max_date = date('Y-m-d', strtotime($range));
  $p['checkin'] = sanitize_date($p['checkin'], $min_date, $max_date);

  // Sanitize check-out date.
  // If it is empty or it's incorrect use the default
  // number of nights to search parameter.
  $range = $p['checkin'] . ' ';
  $range .= variable_get('sr_default_number_of_nights_to_search') . 'days';
  $def_date = date('Y-m-d', strtotime($range));
  $min_date = date('Y-m-d', strtotime($p['checkin'] . ' +1 day'));
  $range = $p['checkin'] . ' +' . variable_get('sr_max_nights_reservation') . 'days';
  $max_date = date('Y-m-d', strtotime($range));
  $p['checkout'] = sanitize_date($p['checkout'], $min_date, $max_date, $def_date);
  if ($p['checkout'] < $p['checkin']) {
    $p['checkout'] = $def_date;
  }

  // Sanitize number of guests.
  $max_guests = variable_get(
    'sr_max_number_of_guests_to_search', 15);
  $default_adults = variable_get(
    'sr_default_number_of_adults_to_search', 4);
  $default_children = variable_get(
    'sr_default_number_of_children_to_search', 0);
  $default_babies = variable_get(
    'sr_default_number_of_babies_to_search', 0);
  $default_dogs = variable_get(
    'sr_default_number_of_dogs_to_search', 0);
  $default_cats = variable_get(
    'sr_default_number_of_cats_to_search', 0);

  $p['adults'] = sanitize_number($p['adults'], $default_adults, 1, $max_guests);
  $p['children'] = sanitize_number($p['children'], $default_children, 0, $max_guests);
  $p['babies'] = sanitize_number($p['babies'], $default_babies, 0, 4);
  $p['dogs'] = sanitize_number($p['dogs'], $default_dogs, 0, 4);
  $p['cats'] = sanitize_number($p['cats'], $default_cats, 0, 4);

  return $p;
}


/**
 * Sanitize number. Format Y-m-d.
 */
function sanitize_number($num, $def_num, $min_num, $max_num) {

  $n = get_numeric($num);
  $n = ($n == 0) ? $def_num : $n;

  if (!empty($min_num)) {
    if ($n < $min_num) {
      $n = $min_num;
    }
  }
  if (!empty($max_num)) {
    if ($n > $max_num) {
      $n = $max_num;
    }
  }
  return $n;
}

/**
 * Sanitize date. Format Y-m-d.
 */
function sanitize_date($date, $min_date, $max_date, $default_date = NULL) {
  if (!empty($date)) {
    $expl = explode('-', $date);
    if(count($expl) > 2) {
      $d = sprintf("%04d", $expl[0]);
      $d .= '-' . sprintf("%02d", $expl[1]);
      $d .= '-' . sprintf("%02d", $expl[2]);
    }
    else {
      $d = '';
    }
  }
  else {
    $d = '';
  }

  if (!empty($default_date)) {
    $d = validate_date($d, 'Y-m-d') ? $d : $default_date;
  }
  else {
    $d = validate_date($d, 'Y-m-d') ? $d : $min_date;
  }

  if (!empty($min_date)) {
    if ($d < $min_date) {
      $d = $min_date;
    }
  }
  if (!empty($max_date)) {
    if ($d > $max_date) {
      $d = $max_date;
    }
  }
  return $d;
}

/**
 * Validate data function.
 */
function validate_date($date, $format = 'Y-m-d H:i:s')
{
    $d = DateTime::createFromFormat($format, $date);
    return $d && $d->format($format) == $date;
}

/**
 * Auxiliary function.
 * Get numeric value.
*/
function get_numeric($val) {
  if (is_numeric($val)) {
    return $val + 0;
  }
  return 0;
}

/**
 * Get availability.
 *
 * Returns an array of rates for each night for an accommodation unit.
 * int @param $acc_id (accommodation unit)
 * array @param $params
 */
function get_rates_per_date_range($acc_id, $p) {
  // 1 Get accommodation units.
  // $accommodation_unit_nodes = get_active_accommodation_units();
  $a = node_load($acc_id);

  // 2 Get rates per night.
  $p_get_rates = array(
    'start_date' => $p['checkin'],
    'end_date' => date('Y-m-d', strtotime($p['checkout'] . '-1 day')),
    'nights_to_show' => 0,
    'months_to_show' => 0,
    'get_default_rates' => TRUE,
    'get_custom_rates' => TRUE,
  );

  module_load_include('inc', 'smart_reservations', 'smart_reservations_rates');
  $rates_per_night = smart_reservations_rates_get_data($p_get_rates);

  // 3 Apply rates
  $row_accommodation_units = NULL;
  $accommodation_rates = array();
  if (!has_reservations($a->nid, $p['checkin'], $p['checkout'])) {
    $total_amount = 0;
    $total_amount_adults = 0;
    $total_amount_children = 0;
    $total_amount_dogs = 0;
    $total_nights = count($rates_per_night);
    $single_night = ($total_nights == 1);

    if (($p['adults'] + $p['children']) <=
      $a->smres_maximum_number_of_people['und'][0]['value']) {
      $enough_capacity = TRUE;
    }
    else {
      $enough_capacity = FALSE;
    }
    if (($p['adults'] + $p['children']) >=
      $a->smres_minimum_number_of_people['und'][0]['value']) {
      $exceeds_minimum_guests = TRUE;
    }
    else {
      $exceeds_minimum_guests = FALSE;
    }

    $adults = min(
      max($p['adults'], $a->smres_minimum_number_of_people['und'][0]['value']),
      $a->smres_maximum_number_of_people['und'][0]['value']);

    $children = min(
      $p['children'],
      $a->smres_maximum_number_of_people['und'][0]['value']-$adults);
    $dogs = min(
      $p['dogs'],
      $a->smres_maximum_number_of_dogs['und'][0]['value']);

    foreach ($rates_per_night as $r) {
      $low_occupancy = ($adults + $children) <= $r['low_occupancy_threshold'];
      $r['base_rate'] = $r['base_rate'] * $a->smres_rate_multiplier['und'][0]['value'];
      $r['amount_per_night_adults'] = $r['base_rate'];
      $r['amount_per_night_adults'] += $low_occupancy ? $r['low_occupancy_fee'] : 0;
      $r['amount_per_night_adults'] += $single_night ? $r['single_night_fee'] : 0;
      $r['amount_per_night_adults'] *= $adults;
      $r['amount_per_night_children'] = $r['base_rate'] - $r['children_discount'];
      $r['amount_per_night_children'] += $low_occupancy ? $r['low_occupancy_fee'] : 0;
      $r['amount_per_night_children'] += $single_night ? $r['single_night_fee'] : 0;
      $r['amount_per_night_children'] *= $children;
      $r['amount_per_night_dogs'] = $r['dogs_fee'];
      $r['amount_per_night_dogs'] *= $dogs;

      $r['amount_per_night'] = $r['amount_per_night_adults'] + $r['amount_per_night_children'];
      $r['amount_per_night'] += $r['amount_per_night_dogs'];
      $total_amount_adults += $r['amount_per_night_adults'];
      $total_amount_children += $r['amount_per_night_children'];
      $total_amount_dogs += $r['amount_per_night_dogs'];
      $total_amount += $r['amount_per_night'];
      $rates_calc[$r['date']] = $r;
    }

    // Accommodation headline.
    // @todo use drupal local settings
    setlocale(LC_TIME, 'ca_ES.UTF-8', 'Catalan_Spain', 'Catalan');

    $date_checkin = strftime(variable_get('sr_date_format_reservation'),
      strtotime($p['checkin']));
    $date_checkout = strftime(variable_get('sr_date_format_reservation'),
      strtotime($p['checkout']));
    $sum_headline_date = t('From @checkin to @checkout', array(
      '@checkin' => $date_checkin,
      '@checkout' => $date_checkout,
      )
    );

    $sum_headline_desc = "{$total_nights} " . t('nights') ;
    $sum_headline_desc .= ($adults > 0) ? ", {$adults} adults" : '';
    $sum_headline_desc .= ($children > 0) ? ", {$children} children" : '';
    $sum_headline_desc .= ($p['babies'] > 0) ? ", {$p['babies']} babies" : '';
    $sum_headline_desc .= ($dogs > 0) ? ", {$dogs} dogs" : '';
    $sum_headline_desc .= ($p['cats'] > 0) ? ", {$p['cats']} cats" : '';

    $accommodation_rates = array(
      'id' => $a->nid,
      'accommodation_unit' => $a ,
      'rates_per_night' => $rates_calc ,
      'summary' => array(
        'title' => $a->title,
        'headline_date' => $sum_headline_date,
        'headline_desc' => $sum_headline_desc,
        'total_nights' => $total_nights,
        'total_amount' => $total_amount,
        'total_amount_adults' => $total_amount_adults,
        'total_amount_children' => $total_amount_children,
        'total_amount_dogs' => $total_amount_dogs,
        'checkin_date' => $p['checkin'],
        'checkout_date' => $p['checkout'],
        'number_of_adults' => $adults,
        'number_of_children' => $children,
        'number_of_babies' => $p['babies'],
        'number_of_dogs' => $dogs,
        'number_of_cats' => $p['cats'],
        'rate_per_night' => $total_amount / $total_nights,

        'rate_per_night_adult' => ($adults == 0) ? 0 :
        $total_amount_adults / $adults / $total_nights,

        'rate_per_night_children' => ($children == 0) ? 0 :
        $total_amount_children / $children / $total_nights,

        'rate_per_night_dogs' => ($dogs == 0) ? 0 :
        $total_amount_dogs / $dogs / $total_nights,
        'enough_capacity' => $enough_capacity,
        'exceeds_minimum_guests' => $exceeds_minimum_guests,
      ),
    );
  }

  return $accommodation_rates;
}

/**
 * Get available accommodation_units (IDs).
 *
 * Returns an array of available accommodation_units for a specific day.
 */
function get_accommodation_units_info($p) {
  $sql = "SELECT n.nid FROM {node} n
  WHERE n.type = 'sr_accommodation_unit' AND n.status = 1";
  $results = db_query($sql);

  // if ($results->rowCount() == 0) {
  //   return NULL;
  // }
  $guests = $p['adults'] + $p['children'];
  $max_guests = 0;
  $total_capacity = 0;
  $rows = array();
  foreach ($results as $r) {
    $n = node_load($r->nid);
    $id = 'acc-' . $r->nid;
    $rows[$id]['id'] = $r->nid;
    $rows[$id]['name'] = $n->title;
    $rows[$id]['min_guests'] = $n->smres_minimum_number_of_people['und'][0]['value'];
    $rows[$id]['max_guests'] = $n->smres_maximum_number_of_people['und'][0]['value'];
    $rows[$id]['max_dogs'] = $n->smres_maximum_number_of_dogs['und'][0]['value'];
    if ($guests > $rows[$id]['max_guests']) {
      $rows[$id]['enough_capacity'] = FALSE;
    }
    else {
      $rows[$id]['enough_capacity'] = TRUE;
    }
    $max_guests = max( $max_guests, $rows[$id]['max_guests']);
    $total_capacity += $n->smres_maximum_number_of_people['und'][0]['value'];
  }
  $more_than_one_needed = (($guests > $max_guests) & ($max_guests != 0)) ?
    TRUE : FALSE;

  // Reorder accommodation
  foreach ($rows as $key => $row) {
      $capacity[$key]  = $row['enough_capacity'];
      $name[$key] = $row['name'];
  }

  // Sort the data.
  if(!empty($rows)) {
    array_multisort($capacity, SORT_DESC, $name, SORT_ASC, $rows);
  }

  $accommodation_data['summary']['max_guests_acc_unit'] = $max_guests;
  $accommodation_data['summary']['total_capacity'] = $total_capacity;
  $accommodation_data['summary']['more_than_one_needed'] = $more_than_one_needed;
  $accommodation_data['data'] = $rows;
  return $accommodation_data;
}

/**
 * Get available accommodation_units.
 *
 * Returns an array of available accommodation_units for a specific day.
 */
function get_active_accommodation_units() {
  $sql = "SELECT n.nid FROM {node} n WHERE n.type = 'accommodation_unit'";
  $results = db_query($sql);

  // Pending to improve... use $result->fetchObject()
  foreach ($results as $result) {
    $node = node_load($result->nid);
    $rows[] = $node;
  }
  return $rows;
}

/**
 * Check if an accommodation_unit has reservations for a specific day.
 *
 * Returns true or false
 */
function has_reservations($acc_id, $checkin, $checkout) {

  $sql = "SELECT accommodation_unit_id, date
  FROM {smart_reservations_reservation_items} r
  WHERE accommodation_unit_id = :acc_id
  AND date >= :checkin
  AND date <= :checkout";

  $result = db_query($sql, array(
    ':acc_id' => $acc_id,
    ':checkin' => $checkin,
    ':checkout' => $checkout));

  $num_nights = $result->rowCount();
  return ($num_nights > 0);
}

/**
 * Get info of an existing reservation .
 *
 * Returns an array of available accommodation_units for a specific day.
 */
function smres_get_reservation_info($res_id) {

  $a = array();
  $p = array();
  // @todo use drupal local settings
  setlocale(LC_TIME, 'ca_ES.UTF-8', 'Catalan_Spain', 'Catalan');

  $sql = "SELECT r.*
  FROM {smart_reservations_reservation_items} r
  WHERE reservation_id = :res_id
  ORDER BY date";
  $results = db_query($sql, array(
    ':res_id' => $res_id,));

  $i = 0;
  $total_amount = 0;
  foreach ($results as $res) {

    $ac_id = $res->accommodation_unit_id;

    // 1) per night info
    $a[$ac_id]['rates_per_night'][$res->date]['date'] = $res->date;
    $a[$ac_id]['rates_per_night'][$res->date]['base_rate'] = $res->base_rate;
    $a[$ac_id]['rates_per_night'][$res->date]['children_discount'] = $res->children_discount;
    $a[$ac_id]['rates_per_night'][$res->date]['dogs_fee'] = $res->dogs_fee;
    $a[$ac_id]['rates_per_night'][$res->date]['low_occupancy_fee'] = $res->low_occupancy_fee;
    $a[$ac_id]['rates_per_night'][$res->date]['low_occupancy_threshold'] = $res->low_occupancy_threshold;
    $a[$ac_id]['rates_per_night'][$res->date]['single_night_fee'] = $res->single_night_fee;

    $a[$ac_id]['rates_per_night'][$res->date]['amount_per_night'] = $res->amount;
    $a[$ac_id]['rates_per_night'][$res->date]['amount_per_night_adults'] = $res->amount_per_night_adults;
    $a[$ac_id]['rates_per_night'][$res->date]['amount_per_night_children'] = $res->amount_per_night_children;
    $a[$ac_id]['rates_per_night'][$res->date]['amount_per_night_dogs'] = $res->amount_per_night_dogs;
    $a[$ac_id]['rates_per_night'][$res->date]['amount_per_night_cats'] = $res->amount_per_night_cats;

    // 2) summary info per accommodation_unit
    if ( !isset($a[$ac_id]['id']) ) {

      // 2.1 ) first time during the loop we get data for the accomodation unit
      $a[$ac_id]['accommodation_unit'] = node_load($ac_id);
      $a[$ac_id]['id'] = $ac_id;
      $a[$ac_id]['summary']['title'] = $a[$ac_id]['accommodation_unit']->title;

      // checkin - checkout
      $a[$ac_id]['summary']['date_checkin'] = strftime(variable_get('sr_date_format_reservation'),
        strtotime($res->date));
      $a[$ac_id]['summary']['date_checkout'] = strftime(variable_get('sr_date_format_reservation'),
        strtotime(date('Y-m-d', strtotime($res->date . '+1 day'))));
      $a[$ac_id]['summary']['headline_date'] = t('From @checkin to @checkout', array(
          '@checkin' => $a[$ac_id]['summary']['date_checkin'],
          '@checkout' => $a[$ac_id]['summary']['date_checkout'],
        )
      );

      // Initial value
      $a[$ac_id]['summary']['total_nights'] = 1;

      $a[$ac_id]['summary']['total_amount'] = $res->total_amount ;
      // res->total_amount is total amount for accommodation unit
      $total_amount += $res->total_amount;

      $a[$ac_id]['summary']['number_of_adults'] = $res->number_of_adults;
      $a[$ac_id]['summary']['number_of_children'] = $res->number_of_children;
      $a[$ac_id]['summary']['number_of_babies'] = $res->number_of_babies;
      $a[$ac_id]['summary']['number_of_dogs'] = $res->number_of_dogs;
      $a[$ac_id]['summary']['number_of_cats'] = $res->number_of_cats;

      $txt = format_plural($res->number_of_adults, t('1 adult'), '@count ' . t('adults'));
      $a[$ac_id]['summary']['headline_desc'] .= ($res->number_of_adults > 0) ? ", {$txt}" : '';

      $txt = format_plural($res->number_of_children, t('1 child'), '@count ' . t('children'));
      $a[$ac_id]['summary']['headline_desc'] .= ($res->number_of_children > 0) ? ", {$txt}" : '';

      $txt = format_plural($res->number_of_babies, t('1 baby'), '@count ' . t('babies'));
      $a[$ac_id]['summary']['headline_desc'] .= ($res->number_of_babies > 0) ? ", {$txt}" : '';

      $txt = format_plural($res->number_of_dogs, t('1 dog'), '@count ' . t('dogs'));
      $a[$ac_id]['summary']['headline_desc'] .= ($res->number_of_dogs  > 0) ? ", {$txt}" : '';

      $txt = format_plural($res->number_of_cats, t('1 cat'), '@count ' . t('cats'));
      $a[$ac_id]['summary']['headline_desc'] .= ($res->number_of_cats > 0) ? ", {$txt}" : '';

      $a[$ac_id]['summary']['enough_capacity'] = TRUE;
      $a[$ac_id]['summary']['exceeds_minimum_guests'] = TRUE;

      $a[$ac_id]['summary']['rate_per_night'] = $res->total_amount;

      $a[$ac_id]['summary']['total_amount_adults'] = $res->amount_per_night_adults;
      $a[$ac_id]['summary']['rate_per_night_adult'] = $res->amount_per_night_adults / $res->number_of_adults;
      $a[$ac_id]['summary']['total_amount_children'] = $res->amount_per_night_children;
      $a[$ac_id]['summary']['rate_per_night_children'] = $res->amount_per_night_children / $res->number_of_children ;


    }
    else {
      // 2.2) next time / times during the loop we get data for the accommodation unit
      // checkin - checkout
      $a[$ac_id]['summary']['date_checkout'] = strftime(variable_get('sr_date_format_reservation'),
        strtotime(date('Y-m-d', strtotime($res->date . '+1 day'))));
      $a[$ac_id]['summary']['headline_date'] = t('From @checkin to @checkout', array(
          '@checkin' => $a[$ac_id]['summary']['date_checkin'],
          '@checkout' => $a[$ac_id]['summary']['date_checkout'],
        )
      );
      $a[$ac_id]['summary']['total_nights']++;
      $a[$ac_id]['summary']['rate_per_night'] = $a[$ac_id]['summary']['total_amount'] / $a[$ac_id]['summary']['total_nights'];

      $a[$ac_id]['summary']['total_amount_adults'] += $res->amount_per_night_adults;
      $a[$ac_id]['summary']['rate_per_night_adult'] = ($res->number_of_adults == 0) ? 0 :
        $a[$ac_id]['summary']['total_amount_adults'] / $res->number_of_adults / $a[$ac_id]['summary']['total_nights'];

      $a[$ac_id]['summary']['total_amount_children'] += $res->amount_per_night_children;
      $a[$ac_id]['summary']['rate_per_night_children'] = ($res->number_of_children == 0) ? 0 :
        $a[$ac_id]['summary']['total_amount_children'] / $res->number_of_children / $a[$ac_id]['summary']['total_nights'];
    }
    $i++;


  }

  // Add the number of nights to the summary txt
  $txt = format_plural($a[$ac_id]['summary']['total_nights'], t('1 night'), '@count ' . t('nights'));
  $a[$ac_id]['summary']['headline_desc'] = "{$txt}" . $a[$ac_id]['summary']['headline_desc'];
  
  // Array of summaries for accommodation unit
  $summary['acc_units_sum'] = array();
  foreach ($a as $ac_item) {
    $summary['acc_units_sum'][] = $ac_item['summary'];
  }
  $summary['total_amount'] = $total_amount;

  $o['reservation_summary'] =
        '<div class="reservation-summary">' .
          '<div class="col-md-12">' . markup_search_results_summary($summary) . '</div></div>';

  $o['reservation_details'] = '<div class="row sr-reservation-details">';
  foreach ($a as $ac_item) {
     $o['reservation_details'] .=
       '<div class="col-md-6">' .
         '<div id="sr-search-results-accommodation-' . $ac_item['id'] . ' class="sr-search-results-accommodation_">' .
            '<div class="sr-accommodation-title">' . $ac_item['summary']['title'] . '</div>
            <div class="sr-accommodation-details">' .
              markup_accommodation_details($ac_item, $ac_item['params']) .
            '</div>
            <div id="sr-search-results-accommodation-calendar-' . $res_id . '" class="sr-accommodation-calendar">' .
              markup_accommodation_calendar($ac_item, array()) .
            '</div>
            <div class="sr-accommodation-summary">' .
              markup_accommodation_summary($ac_item) . '
            </div>
         </div>
       </div>';
  }
  $o['reservation_details'] .= '</div>';


  // Description.
//  global $language;
//  if (!empty($a['accommodation_unit']->body[$language->language][0]['value'])) {
//    $o = $a['accommodation_unit']->body[$language->language][0]['value'];
//
//    $o .= '<div class="row sr-accommodation-description">' . $o . '</div>';
//  }

//  // Acommodation details
//  $o .= '<div class="sr-accommodation-details">';
//  $o .= markup_accommodation_details($a, $p) . '</div>';
//
//  // Calendar
//  $o .= '<div id="sr-search-results-accommodation-calendar-' . $res_id . '" class="sr-accommodation-calendar">';
//  $o .= markup_accommodation_calendar($a, $p) . '</div>';
//
//  // Accommodation summary.
//  $o .= '<div class="sr-accommodation-summary">' . markup_accommodation_summary($a, $p) . '</div>';



  return $o;


  /*
 // 3) Build form: accommodation units info.
// Get accommodation units.
$acc_units = get_accommodation_units_info($p);
$num_acc_units_available = 0;

// 3.1) Header message.
if ($acc_units['summary']['more_than_one_needed']) {
  $build_form['header_message'] = array(
    '#type' => 'markup',
    '#prefix' => '<div class="sr-results-warning sr-results-warning-header">',
    '#suffix' => '</div>',
    '#markup' => t(variable_get('sr_txt_capacity_exceeded')),
  );
};

// 3.2) Accomodation calendars and info.
$summary = array();
$summary['total_amount'] = 0;
foreach ($acc_units['data'] as $acc_unit) {
  // Recover options if we are recalculating the form, for instance.
  $id = $acc_unit['id'];
  if (!empty($form_state['values']['search-results']['accommodation_units'][$id])) {
    $o = &$form_state['values']['search-results']['accommodation_units'][$id]['data']['options'];
    $p['adults'] = $o['number_of_adults'];
    $p['children'] = $o['number_of_children'];
    $p['babies'] = $o['number_of_babies'];
    $p['dogs'] = $o['number_of_dogs'];
  }
  $a = get_rates_per_date_range($id, $p);
  if (!empty($a)) {
    $num_acc_units_available++;
    $form_state['storage']['accommodation_units'][$id] = $a;
    $build_form['accommodation_units'][$id] = array(
      '#type' => 'fieldset',
      '#title' => $a['accommodation_unit']->title,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#prefix' => '<div id="sr-search-results-accommodation-' . $id . '" class="sr-search-results-accommodation">',
      '#suffix' => '</div>',
      '#attributes' => array(
        'class' => array('sr-accommodation-unit'),
      ),
    );

    // Description.
    if (!empty($a['accommodation_unit']->body['und'][0]['value'])) {
      $o = $a['accommodation_unit']->body['und'][0]['value'];
      $build_form['accommodation_units'][$id]['data']['description'] = array(
        '#type' => 'markup',
        '#markup' => $o,
        '#weight' => 10,
        '#prefix' => '<div class="row sr-accommodation-description">',
        '#suffix' => '</div>',
      );
    }
    // Options.
    $build_form['accommodation_units'][$id]['data']['options'] = array(
      '#type' => 'fieldset',
      '#weight' => 20,
      '#attributes' => array(
        'class' => array('sr-accommodation-options'),
        'id' => array('sr-accommodation-options-' . $id),
      ),
    );
    $build_form['accommodation_units'][$id]['data']['options']['number_of_adults']
      = array(
        '#type' => 'select',
        '#title' => t('Adults'),
        '#options' => array_combine(
            range($acc_unit['min_guests'], $acc_unit['max_guests']),
            range($acc_unit['min_guests'], $acc_unit['max_guests'])),
        '#default_value' => $a['summary']['number_of_adults'],
        '#ajax' => array(
          'callback' => 'smart_reservations_search_recalc_ajax',
          // 'wrapper' => 'sr-search-results-accommodation-' . $id,
        ),
      );
    $build_form['accommodation_units'][$id]['data']['options']['number_of_children']
      = array(
        '#type' => 'select',
        '#title' => t('Children'),
        '#options' => array_combine(
            range(0, $acc_unit['max_guests']),
            range(0, $acc_unit['max_guests'])),
        '#default_value' => $a['summary']['number_of_children'],
        '#ajax' => array(
          'callback' => 'smart_reservations_search_recalc_ajax',
          // 'wrapper' => 'sr-search-results-accommodation-' . $id,
        ),
      );
    $build_form['accommodation_units'][$id]['data']['options']['number_of_babies']
      = array(
        '#type' => 'select',
        '#title' => t('Babies'),
        '#options' => array_combine(
            range(0, $acc_unit['max_guests']),
            range(0, $acc_unit['max_guests'])),
        '#default_value' => $a['summary']['number_of_babies'],
        '#ajax' => array(
          'callback' => 'smart_reservations_search_recalc_ajax',
          // 'wrapper' => 'sr-search-results-accommodation-' . $id,
        ),
      );
    $build_form['accommodation_units'][$id]['data']['options']['number_of_dogs']
      = array(
        '#type' => 'select',
        '#title' => t('Dogs'),
        '#options' => array_combine(
            range(0, $acc_unit['max_dogs']),
            range(0, $acc_unit['max_dogs'])),
        '#default_value' => $a['summary']['number_of_dogs'],
        '#ajax' => array(
          'callback' => 'smart_reservations_search_recalc_ajax',
          // 'wrapper' => 'sr-search-results-accommodation-' . $id,
        ),
      );

    // Accommodation - details (additional info).
    $o = markup_accommodation_details($a, $p);
    $build_form['accommodation_units'][$id]['data']['details'] = array(
      '#type' => 'markup',
      '#markup' => $o,
      '#weight' => 30,
      '#prefix' => '<div class="sr-accommodation-details">',
      '#suffix' => '</div>',
    );

    // Calendar.
    $o = markup_accommodation_calendar($a, $p);
    $build_form['accommodation_units'][$id]['data']['calendar'] = array(
      '#type' => 'markup',
      '#markup' => $o,
      '#weight' => 40,
      '#prefix' => '<div id="sr-search-results-accommodation-calendar-' .
      $id . '" class="sr-accommodation-calendar">',
      '#suffix' => '</div>',
    );

    // Accommodation summary.
    $o = markup_accommodation_summary($a, $p);
    $build_form['accommodation_units'][$id]['data']['summary'] = array(
      '#type' => 'markup',
      '#markup' => $o,
      '#weight' => 50,
      '#prefix' => '<div class="sr-accommodation-summary">',
      '#suffix' => '</div>',
    );

    // Accommodation buttons.
    $build_form['accommodation_units'][$id]['data']['buttons'] = array(
      '#type' => 'fieldset',
      '#prefix' => '<div class="sr-accommodation-buttons" id="sr-accommodation-buttons-' . $id . '">',
      '#suffix' => '<br></div>',
      '#weight' => 60,
    );
    $selected = !empty($form_state['storage']['selected_accommodations'][$id]);
    $build_form['accommodation_units'][$id]['data']['buttons']['add_remove'] = array(
      // Very important to have a unique name - for ajax calls.
      '#name' => 'add_remove-' . $id,
      '#type' => 'submit',
      '#value' => $selected ? t('Remove') : t('Add to the reservation') ,
      '#submit' => array('smart_reservations_add_remove_reservation'),
      '#ajax' => array(
        'callback' => 'smart_reservations_add_remove_reservation_ajax_callback',
        //  We don't need a wrapper / special ajax callback / multiple divs.
        // 'wrapper' => 'sr-accommodation-buttons-' . $id,
      ),
      '#attributes' => array(
        'class' => array('sr-button-' . ($selected ? 'red' : 'green')),
      ),
    );
    if ($selected) {
      // Update summary,
      $summary['acc_units_sum'][$id] = $a['summary'];
      $summary['total_amount'] += $a['summary']['total_amount'];
      // Update the array that contains the reservation info.
      $form_state['storage']['reservation']['reservation_status'] = 'Created';
      $form_state['storage']['reservation']['total_amount']
        = $summary['total_amount'];
    }
  }
}


*/

}


/**
 * Get array of days.
 *
 * Returns an array filled with the days between $start_date and $end_date.
 */
function get_array_of_days($start, $end) {
  $days[] = $start;
  $day = $start;


  $days = array();

  if (is_string($start) === TRUE) {
    $start = strtotime($start);
  }
  if (is_string($end) === TRUE) {
    $end = strtotime($end);
  }
  if ($start > $end) {
    $end = strtotime("+ 1 day", $start);
  }

  do {
    $days[] = date('Y-m-d', $start);
    $start = strtotime("+ 1 day", $start);
  } while($start <= $end);

  return $days;
}


/**
 * Sends an e-mail.
 *
 * @param array $params
 *   An array of values of fields to send.
 *   There are just two relevant items: $form_values['email'] and
 *   $form_values['message'].
 */
function smart_reservations_mail_send($params) {

  // All system mails need to specify the module and template key
  // that the message they want to send comes from.
  $module = 'smart_reservations';
  $key = 'reservation_submitted';

  // Specify 'to' and 'from' addresses.
  $to = $params['to'];
  $from = variable_get('site_mail', 'admin@example.com');

  // message is in $params['message']

  // "params" loads in additional context for email content completion in
  // hook_mail(). In this case, we want to pass in the values the user entered
  // into the form, which include the message body in $params['message'].
  $params_mail = array();

  // The language of the e-mail. This will one of three values:
  // - user_preferred_language(): Used for sending mail to a particular website
  //   user, so that the mail appears in their preferred language.
  // - global $language: Used when sending a mail back to the user currently
  //   viewing the site. This will send it in the language they're currently
  //   using.
  // - language_default(): Used when sending mail to a pre-existing, 'neutral'
  //   address, such as the system e-mail address, or when you're unsure of the
  //   language preferences of the intended recipient.
  //
  // Since in our case, we are sending a message to a random e-mail address that
  // is not necessarily tied to a user account, we will use the site's default
  // language.
  global $language;
  $lang = $language;

  // Whether or not to automatically send the mail when drupal_mail() is
  // called. This defaults to TRUE, and is normally what you want unless you
  // need to do additional processing before drupal_mail_send() is called.
  $send = FALSE;
  // Send the mail, and check for success. Note that this does not guarantee
  // message delivery; only that there were no PHP-related issues encountered
  // while sending.

  // $result = drupal_mail($module, $key, $to, $lang, $params_mail, $from, $send);
  // if ($result['result'] == TRUE) {
  //   drupal_set_message(t('Your message has been sent.'));
  // }
  // else {
  //   drupal_set_message(t('There was a problem sending your message and it was not sent.'), 'error');
  // }
  $message = drupal_mail($module, $key, $to, $lang, $params_mail, $from, $send);
  $message['subject'] = $params['subject'];
  $message['body'] = $params['body'];

  // Retrieve the responsible implementation for this message.
  $system = drupal_mail_system($module, $key);

  // Format the message body.
  $message = $system->format($message);

  // Send e-mail.
  $message['result'] = $system->mail($message);

}

/**
 * Log logic.
 */
function sr_log_add($type, $p) {
  switch ($type) {
    case 'search':
      sr_log_add_search_query($p);
      break;
  }
}

/**
 * Log query search logic.
 */
function sr_log_add_search_query($p) {
  $form_entry_id = db_insert('smart_reservations_search_log')
    ->fields(array(
      'ip_address' => ip_address(),
      'checkin_date' => substr(check_plain($p['checkin']), 0, 10),
      'checkout_date' => substr(check_plain($p['checkout']), 0, 10),
      'num_of_adults' => substr(check_plain($p['adults']), 0, 4),
      'num_of_children' => substr(check_plain($p['children']), 0, 4),
      'num_of_babies' => substr(check_plain($p['babies']), 0, 4),
      'num_of_dogs' => substr(check_plain($p['dogs']), 0, 4),
      'num_of_cats' => substr(check_plain($p['cats']), 0, 4),
      'url_query' => strip_tags(substr($p['url_query'], 0, 300)),
      'timestamp' => time(),
        ))
    ->execute();
}
