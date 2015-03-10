<?php

/**
 * @file
 * Variable module hook implementations.
 */


/**
 * Implements hook_variable_group_info().
 */
function smart_reservations_variable_group_info() {
  $groups['smart_reservations'] = array(
    'title' => t('Smart Reservations settings'),
    // 'access' => 'administer store',
  );

  return $groups;
}

/**
 * Implements hook_variable_info().
 */
function smart_reservations_variable_info($options) {
  $variables['First name'] = array(
    'type' => 'string',
    'title' => t('First name', array(), $options),
    'description' => t('Address field label.', array(), $options),
    'group' => 'smart_reservations',
    'default' => t('First name', array(), $options),
  );

  return $variables;
}
