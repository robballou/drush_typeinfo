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
      'drush typeinfo-list' => 'List entity types',
      'drush typeinfo [type]' => 'Show fields for [type]',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  $commands['typeinfo-field'] = array(
    'callback' => 'drush_typeinfo_field',
    'description' => 'Show field information',
    'aliases' => array('tif'),
    'example' => array(
      'drush typeinfo-field [field]' => 'List info about a field',
      'drush typeinfo-field [field] [type]' => 'List info about a field',
    ),
    'options' => array(
      'field-info' => array(
        'description' => 'Output field info',
      ),
    ),
    'arguments' => array(
      'field' => 'The field name',
      'type' => 'The bundle name',
      'entity_type' => 'The entity type. Defaults to node',
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
 * Get info about a field.
 */
function drush_typeinfo_field($field, $type = NULL, $entity_type = 'node') {
  if (!$field) {
    drush_log(t('No field specified'), 'error');
  }

  $field_info = field_info_field($field);
  if (drush_get_option('field-info')) {
    var_dump($field_info);
  }

  if ($field_info) {
    drush_log(t('Field info for: !field', array('!field' => $field)), 'ok');
    drush_log(t('Type: !type', array('!type' => $field_info['type'])), 'ok');

    // show entityreference values
    if (isset($field_info['settings']['target_type'])) {
      drush_log(
        t('Target type: !target', array('!target' => $field_info['settings']['target_type'])),
        'ok'
      );
      if (isset($field_info['settings']['handler_settings']['target_bundles'])) {
        drush_log(
          t('Target bundles: !bundles', array('!bundles' => implode(',', array_keys($field_info['settings']['handler_settings']['target_bundles'])))),
          'ok'
        );
      }
    }

    // show taxonomy term reference info
    if (isset($field_info['settings']['allowed_values'])) {
      foreach ($field_info['settings']['allowed_values'] as $allowed_values) {
        if (isset($allowed_values['vocabulary'])) {
          drush_log('Vocabulary: !vocabulary', array('!vocabulary' => $allowed_values['vocabulary']), 'ok');
        }
      }
    }
  }
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
