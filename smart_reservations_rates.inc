<?php
/**
 * @file
 * Rates functionality.
 */

/**
 * Default rates form.
 */
function smart_reservations_default_rates_form($form, &$form_state) {

  // Initialize values.
  $m = 'Default rates used to calculate the rates per night. ';
  $m .= 'These are the rates applied when there is no custom rate for a specific night.';
  $m = t($m);

  // Sunday weight is 0 if week starts on Sunday or 8 if starts on Monday.
  $sun_weight = variable_get('sr_week_starts_on', 0) * 10;

  $form = array(
    '#prefix' => '<div class="default_rates_grid"><table>',
    '#suffix' => '</table></div>',
    // We need to have tree = FALSE, because we want to use the
    // system_settings_form($form).
    // Each element has to have a unique name.
    '#tree' => FALSE,
  );

  $form['description'] = array(
    '#type' => 'markup',
    '#markup' => $m,
    '#prefix' => '<div class="default_rates_description">',
    '#suffix' => '</div>',
  );
  // Header.
  $form['header'] = array(
    '#prefix' => '<div class="default_rates_grid-header"><tr>',
    '#suffix' => '</tr></div>',
  );
  $form['header']['value'] = array(
    '#type' => 'item',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#weight' => -2,
  );
  $form['header']['def'] = array(
    '#type' => 'item',
    '#title' => 'Default' . '*',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#weight' => -1,
  );
  $form['header']['sun'] = array(
    '#type' => 'item',
    '#title' => 'Sun',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#weight' => $sun_weight,
  );
  $form['header']['mon'] = array(
    '#type' => 'item',
    '#title' => 'Mon',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
  );
  $form['header']['tue'] = array(
    '#type' => 'item',
    '#title' => 'Tue',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
  );
  $form['header']['wed'] = array(
    '#type' => 'item',
    '#title' => 'Wed',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
  );
  $form['header']['thu'] = array(
    '#type' => 'item',
    '#title' => 'Thu',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
  );
  $form['header']['fri'] = array(
    '#type' => 'item',
    '#title' => 'Fri',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
  );
  $form['header']['sat'] = array(
    '#type' => 'item',
    '#title' => 'Sat',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
  );

  // Base rate.
  $form['base_rate'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $form['base_rate']['caption'] = array(
    '#type' => 'item',
    '#title' => t('Base rate'),
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#description' => t('The base rate per night and person.'),
    '#weight' => -2,
  );
  $form['base_rate']['sr_default_base_rate'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_base_rate'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => -1,
  );
  $form['base_rate']['sr_default_base_rate_sun'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_base_rate_sun'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => $sun_weight,
  );
  $form['base_rate']['sr_default_base_rate_mon'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_base_rate_mon'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['base_rate']['sr_default_base_rate_tue'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_base_rate_tue'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['base_rate']['sr_default_base_rate_wed'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_base_rate_wed'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['base_rate']['sr_default_base_rate_thu'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_base_rate_thu'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['base_rate']['sr_default_base_rate_fri'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_base_rate_fri'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['base_rate']['sr_default_base_rate_sat'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_base_rate_sat'),
    '#size' => 1,
    '#maxlength' => 4,
  );

  // Children discount.
  $form['children_discount'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $form['children_discount']['caption'] = array(
    '#type' => 'item',
    '#title' => t('Children discount'),
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#size' => 100,
    '#description' => t('Discount applied for each child per night.') .
    '<br>' . t('Example:') . ' ' .
    t('with a Base rate of 25, and a Children discount of 5, the rate for each child will be 20.'),
    '#weight' => -2,
  );
  $form['children_discount']['sr_default_children_discount'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_children_discount'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => -1,
  );
  $form['children_discount']['sr_default_children_discount_sun'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_children_discount_sun'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => $sun_weight,
  );
  $form['children_discount']['sr_default_children_discount_mon'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_children_discount_mon'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['children_discount']['sr_default_children_discount_tue'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_children_discount_tue'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['children_discount']['sr_default_children_discount_wed'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_children_discount_wed'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['children_discount']['sr_default_children_discount_thu'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_children_discount_thu'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['children_discount']['sr_default_children_discount_fri'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_children_discount_fri'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['children_discount']['sr_default_children_discount_sat'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_children_discount_sat'),
    '#size' => 1,
    '#maxlength' => 4,
  );

  // Dogs fee.
  $form['dogs_fee'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $form['dogs_fee']['caption'] = array(
    '#type' => 'item',
    '#title' => t('Dogs fee'),
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#size' => 100,
    '#description' => t('Fee applied for a dog per night.'),
    '#weight' => -2,
  );
  $form['dogs_fee']['sr_default_dogs_fee'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_dogs_fee'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => -1,
  );
  $form['dogs_fee']['sr_default_dogs_fee_sun'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_dogs_fee_sun'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => $sun_weight,
  );
  $form['dogs_fee']['sr_default_dogs_fee_mon'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_dogs_fee_mon'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['dogs_fee']['sr_default_dogs_fee_tue'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_dogs_fee_tue'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['dogs_fee']['sr_default_dogs_fee_wed'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_dogs_fee_wed'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['dogs_fee']['sr_default_dogs_fee_thu'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_dogs_fee_thu'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['dogs_fee']['sr_default_dogs_fee_fri'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_dogs_fee_fri'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['dogs_fee']['sr_default_dogs_fee_sat'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_dogs_fee_sat'),
    '#size' => 1,
    '#maxlength' => 4,
  );

  // Single night fee.
  $form['single_night_fee'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $form['single_night_fee']['caption'] = array(
    '#type' => 'item',
    '#title' => t('Single night fee'),
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#size' => 100,
    '#description' => t('Additional fee for single night reservations.'),
    '#weight' => -2,
  );
  $form['single_night_fee']['sr_default_single_night_fee'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_single_night_fee'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => -1,
  );
  $form['single_night_fee']['sr_default_single_night_fee_sun'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_single_night_fee_sun'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => $sun_weight,
  );
  $form['single_night_fee']['sr_default_single_night_fee_mon'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_single_night_fee_mon'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['single_night_fee']['sr_default_single_night_fee_tue'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_single_night_fee_tue'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['single_night_fee']['sr_default_single_night_fee_wed'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_single_night_fee_wed'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['single_night_fee']['sr_default_single_night_fee_thu'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_single_night_fee_thu'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['single_night_fee']['sr_default_single_night_fee_fri'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_single_night_fee_fri'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['single_night_fee']['sr_default_single_night_fee_sat'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_single_night_fee_sat'),
    '#size' => 1,
    '#maxlength' => 4,
  );

  // Low occupancy fee.
  $form['low_occupancy_fee'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $form['low_occupancy_fee']['caption'] = array(
    '#type' => 'item',
    '#title' => t('Low occupancy fee'),
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#size' => 100,
    '#description' => t('Additional fee per person for low occupancy reservations.'),
    '#weight' => -2,
  );
  $form['low_occupancy_fee']['sr_default_low_occupancy_fee'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_fee'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => -1,
  );
  $form['low_occupancy_fee']['sr_default_low_occupancy_fee_sun'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_fee_sun'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => $sun_weight,
  );
  $form['low_occupancy_fee']['sr_default_low_occupancy_fee_mon'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_fee_mon'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_fee']['sr_default_low_occupancy_fee_tue'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_fee_tue'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_fee']['sr_default_low_occupancy_fee_wed'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_fee_wed'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_fee']['sr_default_low_occupancy_fee_thu'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_fee_thu'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_fee']['sr_default_low_occupancy_fee_fri'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_fee_fri'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_fee']['sr_default_low_occupancy_fee_sat'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_fee_sat'),
    '#size' => 1,
    '#maxlength' => 4,
  );

  // Low occupancy threshold.
  $form['low_occupancy_threshold'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $form['low_occupancy_threshold']['caption'] = array(
    '#type' => 'item',
    '#title' => t('Low occupancy threshold'),
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#size' => 100,
    '#description' => t('Number of guests below which (<=) the system applies the low ocuppancy fee.') . '<br>' .
    t('For example, if we have a threshold of 2 guests, and a low occcupancy fee of 10,' . '<br>' . 'the system will apply a low occupancy fee of 20, but if we have 3 guests there will be no low occupancy fee.'),
    '#weight' => -2,
  );
  $form['low_occupancy_threshold']['sr_default_low_occupancy_threshold'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_threshold'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => -1,
  );
  $form['low_occupancy_threshold']['sr_default_low_occupancy_threshold_sun'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_threshold_sun'),
    '#size' => 1,
    '#maxlength' => 4,
    '#weight' => $sun_weight,
  );
  $form['low_occupancy_threshold']['sr_default_low_occupancy_threshold_mon'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_threshold_mon'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_threshold']['sr_default_low_occupancy_threshold_tue'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_threshold_tue'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_threshold']['sr_default_low_occupancy_threshold_wed'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_threshold_wed'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_threshold']['sr_default_low_occupancy_threshold_thu'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_threshold_thu'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_threshold']['sr_default_low_occupancy_threshold_fri'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_threshold_fri'),
    '#size' => 1,
    '#maxlength' => 4,
  );
  $form['low_occupancy_threshold']['sr_default_low_occupancy_threshold_sat'] = array(
    '#type' => 'textfield',
    '#prefix' => '<td>',
    '#suffix' => '</td>',
    '#default_value' => variable_get('sr_default_low_occupancy_threshold_sat'),
    '#size' => 1,
    '#maxlength' => 4,
  );


  $f['params'] = $form;
  return system_settings_form($f);
}

/**
 * Implements validation for the
 * smart_reservations_default_rates_form.
 */
function smart_reservations_default_rates_form_validate($form, &$form_state) {
 
  sr_check_var($form_state, 'Base rate', 'sr_default_base_rate');
  sr_check_var($form_state, 'Children discount', 'sr_default_children_discount');
  sr_check_var($form_state, 'Dogs fee', 'sr_default_dogs_fee');
  sr_check_var($form_state, 'Single night fee', 'sr_default_single_night_fee');
  sr_check_var($form_state, 'Low occupancy fee', 'sr_default_low_occupancy_fee');
  sr_check_var($form_state, 'Low occupancy threshold', 'sr_default_low_occupancy_threshold');

}

/**
 * Build the rates per date form.
 *
 * @return array
 *   A form array set for theming by theme_smart_reservations_rates_form()
 */
function smart_reservations_rates_form($form_state) {
  // Identify that the elements in 'rate_items' are a collection, to
  // prevent Form API from flattening the array when submitted.
  $form['rate_items']['#tree'] = TRUE;

  $range_long = variable_get('sr_rates_form_period', '+1 year');
  $nights_to_show = variable_get('sr_form_time_nights_to_show', 0);
  $months_to_show = variable_get('sr_form_months_to_show', 0);
  $params = array(
    'start_date' => date('Y-m-d', strtotime("now")),
    'end_date' => date('Y-m-d', strtotime("now" . $range_long)),
    'nights_to_show' => $nights_to_show,
    'months_to_show' => $months_to_show,
    'get_default_rates' => in_array(
      variable_get('sr_form_values_type', 'All'),
      array('All', 'Default')) ? TRUE : FALSE,
    'get_custom_rates' => in_array(
      variable_get('sr_form_values_type', 'All'),
      array('All', 'Custom')) ? TRUE : FALSE,
  );

  $result = smart_reservations_rates_get_data($params);
  foreach ($result as $item) {
    $day_name = date('l', strtotime($item['date']));
    $day_number = date('N', strtotime($item['date']));
    $month = date('F', strtotime($item['date']));
    $day_markup = ($day_number == 5 or $day_number == 6) ? "<strong>" . $day_name . "</strong>" : $day_name;
    $date_markup = ($day_number == 5 or $day_number == 6) ? "<strong>" . $item['date'] . "</strong>" : $item['date'];
    $month_markup = ($day_number == 5 or $day_number == 6) ? "<strong>" . $month . "</strong>" : $month;

    // Create a form entry for this item.
    // Each entry will be an array using the the unique id for that item as
    // the array key, and an array of table row data as the value.
    $form['rate_items'][$item['date']] = array(
      'date' => array(
        '#type' => 'hidden',
        '#default_value' => $item['date'],
      ),
      'date_markup' => array(
        '#type' => 'markup',
        '#markup' => ($item['is_a_custom_value'] ? "" : "") . $date_markup . ($item['is_a_custom_value'] ? "" : "") ,
        '#disabled' => TRUE,
        '#size' => 10,
      ),
      'month' => array(
        '#type' => 'markup',
        '#markup' => ($item['is_a_custom_value'] ? "" : "") . $month_markup . ($item['is_a_custom_value'] ? "" : "") ,
        '#disabled' => TRUE,
        '#size' => 10,
      ),
      'day' => array(
        '#type' => 'markup',
        '#markup' => ($item['is_a_custom_value'] ? "" : "") . $day_markup . ($item['is_a_custom_value'] ? "" : "") ,
        '#disabled' => TRUE,
        '#size' => 10,
      ),
      'base_rate' => array(
        '#type' => 'textfield',
        '#default_value' => $item['base_rate'],
        '#size' => 7,
        '#maxlength' => 7,
      ),
      'children_discount' => array(
        '#type' => 'textfield',
        '#default_value' => $item['children_discount'],
        '#size' => 7,
        '#maxlength' => 7,
      ),
      'dogs_fee' => array(
        '#type' => 'textfield',
        '#default_value' => $item['dogs_fee'],
        '#size' => 7,
        '#maxlength' => 7,
      ),
      'low_occupancy_fee' => array(
        '#type' => 'textfield',
        '#default_value' => $item['low_occupancy_fee'],
        '#size' => 7,
        '#maxlength' => 7,
      ),
      'low_occupancy_threshold' => array(
        '#type' => 'textfield',
        '#default_value' => $item['low_occupancy_threshold'],
        '#size' => 7,
        '#maxlength' => 7,
      ),
      'single_night_fee' => array(
        '#type' => 'textfield',
        '#default_value' => $item['single_night_fee'],
        '#size' => 3,
        '#maxlength' => 7,
      ),
      'is_a_custom_value' => array(
        '#type' => 'checkbox',
        '#default_value' => $item['is_a_custom_value'],
      ),
      'was_a_custom_value' => array(
        '#type' => 'hidden',
        '#default_value' => $item['was_a_custom_value'],
      ),
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['nights_to_show'] = array(
    '#type' => 'select',
    '#title' => t('Nights'),
    '#options' => array(
      0 => t('All'),
      1 => t('Monday'),
      2 => t('Tuesday'),
      3 => t('Wednesday'),
      4 => t('Thursday'),
      5 => t('Friday'),
      6 => t('Saturday'),
      7 => t('Sunday'),
      8 => t('Weekend nights'),
      9 => t('Weeknights'),
    ),
    '#default_value' => variable_get('sr_form_time_nights_to_show', 0) ,
  );
  $form['actions']['months_to_show'] = array(
    '#type' => 'select',
    '#title' => t('Months'),
    '#options' => array(
      0 => t('All'),
      1 => t('January'),
      2 => t('February'),
      3 => t('March'),
      4 => t('April'),
      5 => t('May'),
      6 => t('June'),
      7 => t('July'),
      8 => t('August'),
      9 => t('September'),
      10 => t('October'),
      11 => t('November'),
      12 => t('December'),
    ),
    '#default_value' => variable_get('sr_form_months_to_show', 0) ,
  );
  $form['actions']['period'] = array(
    '#type' => 'select',
    '#title' => t('Period'),
    '#options' => array(
      '+1 week' => t('One week'),
      '+2 weeks' => t('Two weeks'),
      '+3 weeks' => t('Three weeks'),
      '+1 month' => t('One month'),
      '+2 months' => t('Two months'),
      '+3 months' => t('Three months'),
      '+6 months' => t('Six months'),
      '+1 year' => t('One year'),
      '+2 years' => t('Two years'),
      '+3 years' => t('Three years'),
    ),
    '#default_value' => variable_get('sr_rates_form_period', '+6 months') ,
  );
  $form['actions']['values_type'] = array(
    '#type' => 'select',
    '#title' => t('Values'),
    '#options' => array(
      'All' => t('All'),
      'Default' => t('Default'),
      'Custom' => t('Custom'),
    ),
    '#default_value' => variable_get('sr_form_values_type', 'All') ,
    // '#ajax' => array(
    //   'callback' => 'smart_reservations_values_changed_callback',
    //   'wrapper' => 'log-table',
    // ),
  );
  $form['actions']['apply_filters'] = array(
    '#type' => 'submit',
    '#value' => t('Apply filters'),
    '#submit' => array('smart_reservations_apply_filters_action_form_submit'),
  );
  $form['actions']['reset_filters'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('smart_reservations_reset_filters_action_form_submit'),
  );
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save Changes'));
  return $form;
}

/**
 * Callback for smart_reservations_values_changed_callback.
 *
 * On an ajax submit, the form builder function is called again, then the $form
 * and $form_state are passed to this callback function so it can select which
 * portion of the form to send on to the client.
 *
 * @return array
 *   Renderable array (the textfield element)
 */
function smart_reservations_values_changed_callback($form, $form_state) {
  // // The form has already been submitted and updated. We can return the replaced
  // // item as it is.
  // return $form['rate-items'];
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
function theme_smart_reservations_rates_form($variables) {
  $form = $variables['form'];
  $output = '<div class="smart-reservations-form">';

  $output .= '<div class="smart-reservations-form-filters">';
  $output .= '<h5>Filters</h5>';
  $output .= drupal_render($form['actions']['period']);
  $output .= drupal_render($form['actions']['months_to_show']);
  $output .= drupal_render($form['actions']['nights_to_show']);
  $output .= drupal_render($form['actions']['values_type']);
  // $output .= drupal_render($form['actions']['show_default_rates']);
  // $output .= drupal_render($form['actions']['show_custom_rates']);
  $output .= drupal_render($form['actions']['apply_filters']);
  $output .= drupal_render($form['actions']['reset_filters']);

  // End filters div.
  $output .= '</div>';

  $output .= '<div class="smart-reservations-form-actions">';
  $output .= drupal_render($form['actions']['submit']);
  $output .= '</div>';

  $output .= '<div id="smart-reservations-form-content">';
  $header = array(
    t('Date'),
    t('Month'),
    t('Night'),
    t('Custom rate'),
    t('Base rate'),
    t('Children discount'),
    t('Dogs fee'),
    t('Single night fee'),
    t('Low occupancy fee'),
    t('Low occupancy threshold'),
  );
  // Initialize the variable which will store our table rows.
  $rows = array();
  // Iterate over each element in our $form['rate_items'] array.
  foreach (element_children($form['rate_items']) as $id) {
    $rows[] = array(
      'data' => array(
        drupal_render($form['rate_items'][$id]['date_markup']),
        drupal_render($form['rate_items'][$id]['month']),
        drupal_render($form['rate_items'][$id]['day']),
        drupal_render($form['rate_items'][$id]['is_a_custom_value']),
        drupal_render($form['rate_items'][$id]['base_rate']),
        drupal_render($form['rate_items'][$id]['children_discount']),
        drupal_render($form['rate_items'][$id]['dogs_fee']),
        drupal_render($form['rate_items'][$id]['single_night_fee']),
        drupal_render($form['rate_items'][$id]['low_occupancy_fee']),
        drupal_render($form['rate_items'][$id]['low_occupancy_threshold']),
        // drupal_render($form['rate_items'][$id]['comments']),
        // drupal_render($form['rate_items'][$id]['weight']),
        // drupal_render($form['rate_items'][$id]['date_creation']),
        // drupal_render($form['rate_items'][$id]['date_modification']),
      ),
    );
  }
  $table_id = 'example-items-table';

  $per_page = 25;
  // Initialize the pager.
  $current_page = pager_default_initialize(count($rows), $per_page);
  // Split your list into page sized chunks.
  $chunks = array_chunk($rows, $per_page, TRUE);
  // Show the appropriate items from the list.
  $output .= theme('table', array('header' => $header, 'rows' => $chunks[$current_page]));
  // Show the pager.
  $output .= theme('pager', array('quantity', count($rows)));

  // Finish smart-reservations-form-content.
  $output .= '</div>';

  // Render any remaining form elements.
  $output .= '<div class="smart-reservations-form-footer">';
  $output .= drupal_render_children($form);
  $output .= '</div>';

  // Finish smart-reservations-form div.
  $output .= '</div>';
  $output .= '<div class="clearfix"></div>';
  return $output;
}

/**
 * Submit callback for the smart_reservations_rates_form form.
 *
 * Updates the 'weight' column for each element in our table, taking into
 * account that item's new order after the drag and drop actions have been
 * performed.
 *
 * @ingroup smart_reservations
 */
function smart_reservations_rates_form_submit($form, &$form_state) {
  // Because the form elements were keyed with the item ids from the database,
  // we can simply iterate through the submitted values.

  foreach ($form_state['values']['rate_items'] as $id => $item) {
    if ($item['is_a_custom_value'] and $item['was_a_custom_value']) {
      db_query(
        "UPDATE {smart_reservations_custom_rates_per_night} SET base_rate = :base_rate, children_discount = :children_discount, dogs_fee = :dogs_fee, low_occupancy_fee = :low_occupancy_fee, low_occupancy_threshold = :low_occupancy_threshold, single_night_fee = :single_night_fee, comments = :comments, date_modification = :date_modification WHERE date = :date",
        array(
          ':base_rate' => $item['base_rate'],
          ':children_discount' => $item['children_discount'],
          ':dogs_fee' => $item['dogs_fee'],
          ':low_occupancy_fee' => $item['low_occupancy_fee'],
          ':low_occupancy_threshold' => $item['low_occupancy_threshold'],
          ':single_night_fee' => $item['single_night_fee'],
          ':comments' => '',
          // not enabled field: ':comments' => $item['comments'],
          ':date_modification' => time(),
          ':date' => $item['date'],
        )
      );
    }
    if ($item['is_a_custom_value'] and !$item['was_a_custom_value']) {
      db_insert('smart_reservations_custom_rates_per_night')
      ->fields(array(
          'date' => $item['date'],
          'base_rate' => $item['base_rate'],
          'children_discount' => $item['children_discount'],
          'dogs_fee' => $item['dogs_fee'],
          'low_occupancy_fee' => $item['low_occupancy_fee'],
          'low_occupancy_threshold' => $item['low_occupancy_threshold'],
          'single_night_fee' => $item['single_night_fee'],
          'date_creation' => time(),
          'date_modification' => time(),
        ))->execute();
    }
    if (!$item['is_a_custom_value'] and $item['was_a_custom_value']) {
      db_delete('smart_reservations_custom_rates_per_night')
      ->condition('date', $item['date'])
      ->execute();
    }
  }
}

/**
 * Callback for apply filters action.
 */
function smart_reservations_apply_filters_action_form_submit($form, &$form_state) {
  variable_set('sr_form_time_nights_to_show', $form_state['values']['nights_to_show']);
  variable_set('sr_form_months_to_show', $form_state['values']['months_to_show']);
  variable_set('sr_rates_form_period', $form_state['values']['period']);
  variable_set('sr_form_values_type', $form_state['values']['values_type']);

  // variable_set('smart_reservations_rates_form_show_custom_rates', $form_state['values']['show_custom_rates']);
  // variable_set('smart_reservations_rates_form_show_default_rates', $form_state['values']['show_default_rates']);
}

/**
 * Callback for reset filters action.
 */
function smart_reservations_reset_filters_action_form_submit($form, &$form_state) {
  variable_set('sr_form_time_nights_to_show', 0);
  variable_set('sr_form_months_to_show', 0);
  variable_set('sr_rates_form_period', '+1 year');
  variable_set('sr_form_values_type', 'All');

  // variable_set('smart_reservations_rates_form_show_default_rates', TRUE);
  // variable_set('smart_reservations_rates_form_show_custom_rates', TRUE);
}

/**
 * Retrieves rates.
 */
/**
 * Retrieves rates.
 * $params['start_date']: Start date of rates to retrieve.
 * $params['end_date']: End date of rates to retrieve.
 * $params['nights_to_show']:
 *   1 : retrieve only Monday,
 *   2 : retrieve only Tuesdays,
 *   ...
 *   7 : retrieve only Sundays
 *   8 : retrieve weeknights 5 and 6 (Friday and Saturday)
 *   9 : retrieve other weeknights (Sun, Mon, Tue, Wed, Thu)
 * $params['months_to_show']: Month of rates to retrieve.
 */
function smart_reservations_rates_get_data($params) {
  $date = $params['start_date'];
  $end_date = $params['end_date'];
  $date_nums_to_get = ($params['nights_to_show'] == 8) ? array(5, 6) :
    (($params['nights_to_show'] == 9) ? array(1, 2, 3, 4, 7) : array(($params['nights_to_show'])));
  $months_to_get = $params['months_to_show'];

  if ($params['get_default_rates']) {
    // 1) Create an array ($default_rates) with all the days
    // of a year and the default values.
    $num_nights = dateDiff($date, $end_date);
    $date_num = date('N', strtotime($date));
    $month_num = date('n', strtotime($date));
    $get_it = (($months_to_get == 0) or ($months_to_get == $month_num));
    $get_it &= (($params['nights_to_show'] == 0) or (in_array($date_num, $date_nums_to_get)));
    for ($i = 0; $i <= $num_nights; $i++) {
      if ($get_it) {
        $default_rates[$date]['date'] = $date;
        $default_rates[$date]['base_rate'] = sr_def_rate('base_rate', $date_num);
        $default_rates[$date]['children_discount']
          = sr_def_rate('children_discount', $date_num);
        $default_rates[$date]['dogs_fee']
          = sr_def_rate('dogs_fee', $date_num);
        $default_rates[$date]['low_occupancy_fee']
          = sr_def_rate('low_occupancy_fee', $date_num);
        $default_rates[$date]['low_occupancy_threshold']
          = sr_def_rate('low_occupancy_threshold', $date_num);
        $default_rates[$date]['single_night_fee']
          = sr_def_rate('single_night_fee', $date_num);
        // $default_rates[$date]['comments'] = "";
        $default_rates[$date]['date_creation'] = "";
        $default_rates[$date]['date_modification'] = "";
        $default_rates[$date]['is_a_custom_value'] = FALSE;
        $default_rates[$date]['was_a_custom_value'] = FALSE;
      }
      $date = date('Y-m-d', strtotime($date . " +1 day"));
      $date_num = date('N', strtotime($date));
      $month_num = date('n', strtotime($date));
      $get_it = (($months_to_get == 0) or ($months_to_get == $month_num));
      $get_it &= (($params['nights_to_show'] == 0) or (in_array($date_num, $date_nums_to_get)));

    }
  }
  if ($params['get_custom_rates'] or
  ($params['get_default_rates'] and !$params['get_custom_rates'])) {
    // 2) Create an array ($custom_rates) with the custom rates
    // from the database.
    $results = db_query("SELECT * FROM {smart_reservations_custom_rates_per_night} "
              . "WHERE date >= '" . $params['start_date'] . "' AND date <= '" . $end_date . "'");

    foreach ($results as $result) {
      $date_num = date('N', strtotime($result->date));
      $month_num = date('n', strtotime($result->date));
      if (
        (($params['nights_to_show'] == 0) or (in_array($date_num, $date_nums_to_get))) and
        (($params['months_to_show'] == 0) or ($params['months_to_show'] == $month_num))
        ) {
        $custom_rates[$result->date] = array(
          'date' => $result->date,
          'base_rate' => $result->base_rate,
          'children_discount' => $result->children_discount,
          'dogs_fee' => $result->dogs_fee,
          'low_occupancy_fee' => $result->low_occupancy_fee,
          'low_occupancy_threshold' => $result->low_occupancy_threshold,
          'single_night_fee' => $result->single_night_fee,
          // 'comments' => $result->comments,
          'date_creation' => $result->date_creation,
          'date_modification' => $result->date_modification,
          'is_a_custom_value' => TRUE,
          'was_a_custom_value' => TRUE,
        );
      }
    }
  }
  // 3) Create a final array with the custom and the default rates merged.
  if (!empty($custom_rates)) {
    if (!empty($default_rates)) {
      if (!$params['get_custom_rates']) {
        // dpm($default_rates, 'd');
        // dpm($custom_rates, 'c');
        // We need to return the default rates without the custom rates.
        // $rates = array_diff_assoc($default_rates, $custom_rates);
        $rates = sr_default_diff_cust($default_rates, $custom_rates);
      }
      else {
        // Add default rates to the final list.
        // The operand + does not add any existing entries.
        $rates = $custom_rates + $default_rates;
      }
    }
    else {
      $rates = $custom_rates;
    }
  }
  else {
    $rates = $default_rates;
  }

  asort($rates);
  return $rates;
}


/**
 * Checks variables for each day of the week. 
 */
function sr_check_var(&$form_state, $var_caption, $var_base_name) {

  if (empty($form_state['values'][$var_base_name])) {
    form_set_error($var_base_name, 
      t('You must enter a value for the default @caption.',
        array('@caption' => $var_caption)));
  }
  if (!is_numeric($form_state['values'][$var_base_name])) {
    form_set_error($var_base_name, 
      t('You must enter a number for the default @caption.',
        array('@caption' => $var_caption)));
  }

  $week_suffixes_vars = sr_get_week_suffixes();
  foreach ($week_suffixes_vars as $suf => $day_name) {
    $var = $var_base_name . '_' . $suf;
    if ( !empty($form_state['values'][$var]) & 
      !is_numeric($form_state['values'][$var]) ) {
        form_set_error($var, 
          t('You must enter a number for the @caption.',
            array('@caption' => $var_caption . ' - ' . $day_name)));
    }
  }
}


/**
 * Get an array of week suffixes. 
 */
function sr_get_week_suffixes() {
  $val = array (
    'sun' => 'Sunday', 
    'mon' => 'Monday',
    'tue' => 'Tuesday',
    'wed' => 'Wednesday',
    'thu' => 'Thursday',
    'fri' => 'Friday',
    'sat' => 'Saturday',
  ); 
  return $val;
}


/**
 * Returns the default rate for a specific night of a week.
 * If there is not for a specific night, we return the general default value.
 * For instance:
 * To get the default base rate of Sunday we return sr_default_base_rate_sun,
 * but if it's null we return sr_default_base_rate
 */
function sr_def_rate($var_base_name, $date_num) {
  $var_default = 'sr_default_' . $var_base_name;
  $num_dec = 2;
  if ($var_base_name == 'low_occupancy_threshold') {
    $num_dec = 0;
  }

  switch ($date_num) {
    case '1':
      $var_suffix = 'mon';
      break;

    case '2':
      $var_suffix = 'tue';
      break;

    case '3':
      $var_suffix = 'wed';
      break;

    case '4':
      $var_suffix = 'thu';
      break;

    case '5':
      $var_suffix = 'fri';
      break;

    case '6':
      $var_suffix = 'sat';
      break;

    case '7':
      $var_suffix = 'sun';
      break;
  }

  $var = $var_default . '_' . $var_suffix;
  $value = variable_get($var);
  if (empty($value)) {
    $value = number_format(variable_get($var_default), $num_dec);
  }
  else {
    $value = number_format($value, $num_dec);
  }
  return $value;
}

/**
 * Return the number of days between the two dates.
 */
function dateDiff($d1, $d2) {
  return round(abs(strtotime($d1) - strtotime($d2)) / 86400);
}


/**
 * Returns the default_rates array without custom_rates values.
 * PENDING: improve performance.
 */
function sr_default_diff_cust($default_rates, $custom_rates) {
  foreach ($default_rates as $def) {
    foreach ($custom_rates as $cus) {
      unset ($default_rates[$cus['date']]);
    }
  }
  return $default_rates;
}
