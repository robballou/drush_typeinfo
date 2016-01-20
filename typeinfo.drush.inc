<?php

/**
 * Implements hook_drush_command().
 *
 * Register the typeinfo command(s)
 */
function typeinfo_drush_command() {
  $commands = [];

  $commands['typeinfo'] = [
    'callback' => 'drush_typeinfo',
    'description' => 'Show fields for a type',
    'aliases' => ['ti'],
    'examples' => [
      'drush typeinfo' => 'List entity types',
      'drush typeinfo [type]' => 'Show fields for [type]',
    ],
    'arguments' => [
      'type' => 'The bundle name',
      'entity_type' => 'The entity type. Defaults to node',
    ],
    'options' => [
      'field-info' => [
        'description' => 'Output field info',
      ],
      'field-names-only' => [
        'description' => 'Output only field names',
      ],
    ],
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  ];

  $commands['typeinfo-list'] = [
    'callback' => 'drush_typeinfo_list',
    'description' => 'Show entities',
    'aliases' => ['til'],
    'examples' => [
      'drush typeinfo-list' => 'List entity types',
      'drush typeinfo [type]' => 'Show fields for [type]',
    ],
    'options' => [
      'type' => [
        'description' => 'Limit the listed types to an entity type',
      ],
    ],
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  ];

  $commands['typeinfo-field'] = [
    'callback' => 'drush_typeinfo_field',
    'description' => 'Show field information',
    'aliases' => ['tif'],
    'example' => [
      'drush typeinfo-field [field]' => 'List info about a field',
      'drush typeinfo-field [field] [type]' => 'List info about a field',
      'drush typeinfo-field [field] [type] [entity]' => 'List info about a field',
    ],
    'options' => [
      'field-info' => [
        'description' => 'Output field info',
      ],
      'display-info' => [
        'description' => 'Output a fields display settings (must specify type)'
      ],
      'widget-info' => [
        'description' => 'Output a fields widget settings (must specify type)'
      ],
    ],
    'arguments' => [
      'field' => 'The field name',
      'type' => 'The bundle name',
      'entity_type' => 'The entity type. Defaults to node',
    ],
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  ];

  $commands['typeinfo-report'] = [
    'callback' => 'drush_typeinfo_report',
    'description' => 'Show fields for a type',
    'aliases' => ['tir'],
    'examples' => [
      'drush typeinfo-report [type]' => 'Show fields for [type]',
    ],
    'arguments' => [
      'type' => 'The bundle name',
      'entity_type' => 'The entity type. Defaults to node',
    ],
    'options' => [
    ],
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  ];

  return $commands;
}

/**
 * Get the entity information.
 */
function typeinfo_get_entity($entity_type = NULL) {
  if (!$entity_type) {
    return \Drupal::entityTypeManager()->getDefinitions();
  }
  return \Drupal::entityTypeManager()->getDefinition($entity_type);
}

/**
 * List fields in an entity/bundle
 *
 * @param string $type
 *   A string for the bundle.
 * @param string $entity_type
 *   The entity type for this bundle. Defaults to node.
 */
function drush_typeinfo($type = NULL, $entity_type = NULL) {
  // if no types are passed, output types
  if (!$type) {
    return drush_typeinfo_list();
  }

  // figure out if $type is really a bundle or an entity type.
  $type_info = NULL;
  if (!$entity_type) {
    $type_info = typeinfo_get_entity($type);
    if ($type_info) {
      $entity_type = $type;
      $type = NULL;
    }
  }

  if (!$type_info) {
    $type_info = typeinfo_get_entity($entity_type);
  }

  $label = $type_info->getLabel();
  $bundles = [];
  if ($type) {
    $bundles = entity_get_bundles($entity_type);
    if ($bundles && !isset($bundles[$type])) {
      drush_log(t('The requested type does not exist. Try: drush typeinfo-list'), 'error');
      return;
    }
    $label = $bundles[$type]['label'];
  }

  $field_info = \Drupal::entityManager()->getFieldDefinitions($entity_type, $type);
  $fields = [];

  foreach ($field_info as $field => $info) {
    $field_info = \Drupal\field\Entity\FieldStorageConfig::loadByName($entity_type, $field);
    if (drush_get_option('field-info')) {
      var_dump('FiledConfig', $info);
      if ($field_info) {
        var_dump('FieldStorageConfig', $field_info);
      }
    }

    $required = $info->isRequired() ? 'Yes' : 'No';
    $type = $info->getType();
    if ($field_info) {
      $type .= ':' . $field_info->getType();
    }
    $field_output = [
      $field,
      $required,
      $type,
      '?',
      $info->getLabel(),
    ];

    if (drush_get_option('field-names-only')) {
      $field_output = [$field];
    }

    $fields[$field] = $field_output;
  }

  drush_log($label, 'ok');
  if (count($fields) > 0) {
    ksort($fields);
    array_unshift($fields, ['Field', 'Required', 'Type', 'Widget', 'Label']);
    return drush_print_table($fields, TRUE);
  }

  drush_log(t('No fields for this type'), 'ok');
}

/**
 * Get info about a field.
 *
 * @param string $field
 *   The field name.
 */
function drush_typeinfo_field($field, $bundle = NULL, $entity_type = NULL) {
  if (!$field) {
    drush_log(t('No field specified'), 'error');
  }

  $field_info = field_info_field($field);
  if ($field_info && drush_get_option('field-info')) {
    drush_log(t('Field info:'));
    var_dump($field_info);
  }

  if ($field_info) {
    drush_log(t('Field info for: !field', ['!field' => $field]), 'ok');
    drush_log(t('Type: !type', ['!type' => $field_info['type']]), 'ok');

    if ($bundle) {
      if (!$entity_type) {
        $entity_type = 'node';
      }
      $instance_info = field_info_instance($entity_type, $field, $bundle);

      if ($instance_info && drush_get_option('field-info')) {
        drush_log(t('Instance info:'));
        var_dump($instance_info);
      }

      if ($instance_info && drush_get_option('display-info')) {
        drush_log(t('Display info:'));
        var_dump($instance_info['display']);
      }

      if ($instance_info && drush_get_option('widget-info')) {
        drush_log(t('Widget info:'));
        var_dump($instance_info['widget']);
      }

      if ($instance_info) {
        drush_log(t('Widget: !widget', ['!widget' => $instance_info['widget']['type']]), 'ok');
        $displays = array_keys(array_filter($instance_info['display'], 'drush_typeinfo_filter_displays'));
        drush_log(t('Display view modes: !modes', ['!modes' => implode(', ', $displays)]), 'ok');
      }
    }

    // show entityreference values
    if (isset($field_info['settings']['target_type'])) {
      drush_log(
        t('Target type: !target', ['!target' => $field_info['settings']['target_type']]),
        'ok'
      );

      // figure out the target bundles
      $target_bundles = [];
      if (isset($field_info['settings']['handler_settings']['target_bundles']) && $field_info['settings']['handler_settings']['target_bundles']) {
        $target_bundles = array_keys($field_info['settings']['handler_settings']['target_bundles']);
      }
      elseif (isset($field_info['bundles']) && isset($field_info['bundles'][$field_info['settings']['target_type']])) {
        $target_bundles = $field_info['bundles'][$field_info['settings']['target_type']];
      }

      if ($target_bundles) {
        drush_log(
          t('Target bundles: !bundles', ['!bundles' => implode(', ', $target_bundles)]),
          'ok'
        );
      }
    }

    // show taxonomy term reference info
    if (isset($field_info['settings']['allowed_values'])) {
      foreach ($field_info['settings']['allowed_values'] as $allowed_key => $allowed_value) {
        if (is_array($allowed_value) && isset($allowed_value['vocabulary'])) {
          drush_log(t('Vocabulary: !vocabulary', ['!vocabulary' => $allowed_value['vocabulary']]), 'ok');
        }
        else {
          drush_log(t('Value: !value: !string', ['!value' => $allowed_key, '!string' => $allowed_value]), 'ok');
        }
      }
    }

    // show datetime settings
    if (isset($field_info['settings']['granularity'])) {
      $granularity = array_filter($field_info['settings']['granularity']);
      drush_log(t('Datetime collection: !items', ['!items' => implode(', ', $granularity)]), 'ok');
    }

    // default value
    if (isset($field_info['default_value'])) {
      drush_log(t('Default value: !default', ['!default' => $field_info['default_value']]), 'ok');
    }

    // show cardinality
    if (isset($field_info['cardinality'])) {
      $cardinality = $field_info['cardinality'];
      if ($cardinality == -1) {
        $cardinality = 'unlimited';
      }
      drush_log(t('Cardinality: !cardinality', ['!cardinality' => $cardinality]), 'ok');
    }
  }
}

/**
 * List entity/bundles
 */
function drush_typeinfo_list($type = NULL) {
  $info = \Drupal::entityTypeManager()->getDefinitions();
  $bundles = [];

  $limit_type = drush_get_option('type');
  if (!$limit_type && $type) {
    $limit_type = $type;
  }

  foreach (array_keys($info) as $entity) {
    // skip if this is not the requested type
    if ($limit_type && $entity != $limit_type) {
      continue;
    }
    if ($type = $info[$entity]->get('bundle_entity_type')) {
      $entity_bundles = entity_get_bundles($entity);
      foreach (array_keys($entity_bundles) as $bundle) {
        $bundles[] = [$entity, $bundle];
      }
    }
    else {
      $bundles[] = [$entity, '-'];
    }
  }

  // sort things so they're all OCD
  usort($bundles, 'drush_typeinfo_sort_bundles');

  return drush_print_table($bundles);
}

/**
 * List fields in an entity/bundle
 *
 * @param string $type
 *   A string for the bundle.
 * @param string $entity_type
 *   The entity type for this bundle. Defaults to node.
 */
function drush_typeinfo_report($type = NULL, $entity_type = 'node') {
  // if no types are passed, output types
  if (!$type) {
    return drush_typeinfo_list();
  }

  $type_info = \Drupal::entityManager()->getDefinitions('entity_type');
  if (!isset($type_info['bundles'][$type])) {
    drush_log(t('The requested type does not exist. Try: drush typeinfo-list'), 'error');
    return;
  }
  $node_info = NULL;
  if ($entity_type === 'node') {
    $node_info = node_type_get_type($type);
  }

  $label = $type_info['bundles'][$type]['label'];
  $field_info = field_info_instances($entity_type, $type);
  $fields = [];

  foreach ($field_info as $field => $info) {
    $field_info = field_info_field($field);

    $required = 'No';
    if (isset($info['required']) && $info['required']) {
      $required = 'Yes';
    }

    $notes = '"' . $info['label'] . '", ' .
      $field_info['type'] . ', ' .
      $info['widget']['type'] . ', ';

    if ($field_info['cardinality'] == 1) {
      $notes .= 'single value';
    }
    elseif ($field_info['cardinality'] == -1) {
      $notes .= 'unlimited values';
    }
    else {
      $notes .= 'values = ' . $field_info['cardinality'];
    }

    $field_output = [
      // source field
      $field,
      $notes,
      // $field_info['type'],
      // $info['widget']['type'],
      // $info['label'],
    ];

    $fields[$field] = $field_output;
  }

  drush_log($label, 'ok');
  if (count($fields) > 0) {
    ksort($fields);
    foreach ($fields as $field) {
      print implode("\t", $field) . "\n";
    }
    return;
  }

  drush_log(t('No fields for this type'), 'ok');
}

/**
 * Filter the field instance display values.
 */
function drush_typeinfo_filter_displays($item) {
  if (isset($item['type']) && $item['type'] != 'hidden') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Sort bundle items
 */
function drush_typeinfo_sort_bundles($a, $b) {
  if ($a[0] === $b[0]) {
    return strcmp($a[1], $b[1]);
  }
  return strcmp($a[0], $b[0]);
}
