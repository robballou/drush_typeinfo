<?php

/**
 * Implements hook_drush_command().
 *
 * Register the typeinfo command(s)
 */
function typeinfo_drush_command() {
  $commands = array();

  $commands['typeinfo'] = array(
    'callback' => 'drush_typeinfo',
    'description' => 'Show fields for a type',
    'aliases' => array('ti'),
    'examples' => array(
      'drush typeinfo' => 'List entity types',
      'drush typeinfo [type]' => 'Show fields for [type]',
    ),
    'arguments' => array(
      'type' => 'The bundle name',
      'entity_type' => 'The entity type. Defaults to node',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $commands['typeinfo-list'] = array(
    'callback' => 'drush_typeinfo_list',
    'description' => 'Show entities',
    'aliases' => array('til'),
    'examples' => array(
      'drush typeinfo' => 'List entity types',
      'drush typeinfo [type]' => 'Show fields for [type]',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  return $commands;
}

/**
 * List fields in an entity/bundle
 *
 * @param string $type
 *   A string for the bundle.
 * @param string $entity_type
 *   The entity type for this bundle. Defaults to node.
 */
function drush_typeinfo($type = NULL, $entity_type = 'node') {
  // if no types are passed, output types
  if (!$type) {
    return drush_typeinfo_list();
  }

  $type_info = entity_get_info($entity_type);
  if (!isset($type_info['bundles'][$type])) {
    drush_log(t('The requested type does not exist. Try: drush typeinfo-list'), 'error');
    return;
  }

  $label = $type_info['bundles'][$type]['label'];
  $field_info = field_info_instances($entity_type, $type);
  $fields = array();

  foreach ($field_info as $field => $info) {
    $fields[$field] = array(
      $field,
      $info['widget']['type'],
      $info['label'],
    );
  }

  drush_log($label, 'ok');
  if (count($fields) > 0) {
    ksort($fields);
    array_unshift($fields, array('Field', 'Type', 'Label'));
    return drush_print_table($fields, TRUE);
  }

  drush_log(t('No fields for this type'), 'ok');
}

/**
 * List entity/bundles
 */
function drush_typeinfo_list() {
  $info = entity_get_info();
  $bundles = array();
  foreach ($info as $entity => $entity_info) {
    if (isset($info[$entity]['bundles'])) {
      foreach ($info[$entity]['bundles'] as $bundle => $bundle_info) {
        array_push($bundles, array($entity, $bundle));
      }
    }
  }

  return drush_print_table($bundles);
}
