<?php

/**
 * @file
 *
 * Smart Reservations. Reservations page logic.
 */


/**
 * Implements hook_form().
 */
function smart_reservations_reservations_form($form, &$form_state) {

  $form = array();
  $header = array(
    'title' => t('Reservation ID'),
    'comments' => t('Comments'),
    'customer_name' => t('Customer name'),
    'customer_email' => t('Customer email'),
    'customer_phone' => t('Customer phone'),
  );

  $reservations = node_load_multiple(array(), array('type' => 'sr_reservation'));
//  dsm($reservations);
  $rows = array();
  foreach ($reservations as $reservation ) {
    $rows[] = array (
      'title' => $reservation->title,
      'comments' => $reservation->smres_comments['und'][0]['value'],
      'customer_name' => $reservation->smres_contact_name['und'][0]['value'],
      'customer_email' => $reservation->smres_contact_email['und'][0]['value'],
      'customer_phone' => $reservation->smres_contact_phone['und'][0]['value'],
    );
  }

  //Build the tableselect.
  $form['reservations_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No content available.'),
    '#attributes' => array('class' => array('reservations')),
  );

  $form['pager'] = array('#markup' => theme('pager'));

  $form['submit'] = array (
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

//  dsm($form);

  return($form);
}

