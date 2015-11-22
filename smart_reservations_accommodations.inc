<?php

/**
 * @file
 *
 * Smart Reservations. Reservations page logic.
 */


/**
 * Implements hook_form().
 */
function smart_reservations_accommodations_form($form, &$form_state) {

  $form = array();
  $header = array(
    'title' => t('Accommodation'),
  );

  $accommodations = node_load_multiple(array(), array('type' => 'sr_accommodation_unit'));
  $rows = array();
  foreach ($accommodations as $accommodation) {
    $rows[] = array (
      'title' => l($accommodation->title, 'node/'.$accommodation->nid),
    );
  }

  //Build the tableselect.
  $form['accommodations_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No content available.'),
    '#attributes' => array('class' => array('accommodations')),
  );

  $form['pager'] = array('#markup' => theme('pager'));

  $form['submit'] = array (
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return($form);
}

