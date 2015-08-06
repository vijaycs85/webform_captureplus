<?php

/**
 * @file
 * Admin configurations.
 */

/**
 * Overview page of all components for this webform.
 */
function webform_captureplus_page($node, $page_number = 1) {
  $output = drupal_get_form('webform_captureplus_form', $node);

  return array(
    '#theme' => 'webform_captureplus_page',
    '#node' => $node,
    '#form' => $output,
  );
}

/**
 * Theme the output of the main components page.
 *
 * This theming provides a way to toggle between the editing modes if Form
 * Builder module is available.
 */
function theme_webform_captureplus_page($variables) {
  $node = $variables['node'];
  $form = $variables['form'];

  return drupal_render($form);
}

/**
 * The table-based listing of all components for this webform.
 */
function webform_captureplus_form($form, $form_state, $node) {
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'components' => array(),
  );

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $options = array();
  $captureplus_fields = webform_captureplus_get_field_mappings();
  $mode_options = array(
    WEBFORM_CAPTUREPLUS_FIELD_MODE_DEFAULT => 'Default',
    WEBFORM_CAPTUREPLUS_FIELD_MODE_POPULATE => 'Populate',
    WEBFORM_CAPTUREPLUS_FIELD_MODE_COUNTRY => 'Country',
    WEBFORM_CAPTUREPLUS_FIELD_MODE_NONE => 'None',
    WEBFORM_CAPTUREPLUS_FIELD_MODE_PRESERVE => 'Preserve',
    WEBFORM_CAPTUREPLUS_FIELD_MODE_SEARCH => 'Search',
  );
  foreach ($node->webform['components'] as $cid => $component) {
    $options[$cid] = check_plain($component['name']);
    $form['components'][$cid]['cid'] = array(
      '#type' => 'hidden',
      '#default_value' => $component['cid'],
    );
    $form['components'][$cid]['pid'] = array(
      '#type' => 'hidden',
      '#default_value' => $component['pid'],
    );
    $form['components'][$cid]['captureplus_field'] = array(
      '#type' => 'select',
      '#title' => t('Capture+ field'),
      // '#default_value' => '',
      '#options' => array_combine($captureplus_fields, $captureplus_fields),
      '#empty_option' => t('Select'),
      '#empty_value' => NULL,
    );
    $form['components'][$cid]['mode'] = array(
      '#type' => 'select',
      '#title' => t('Capture+ mode'),
      // '#default_value' => '',
      '#options' => $mode_options,
      '#empty_option' => t('Select'),
      '#empty_value' => NULL,
    );
  }

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 45,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#access' => count($node->webform['components']) > 0,
  );
  $form['warning'] = array(
    '#weight' => -1,
  );

  return $form;
}

/**
 * Recursive function for nesting components into a table.
 *
 * @see preprocess_webform_captureplus_form()
 */
function _webform_captureplus_form_rows($node, $cid, $component, $level, &$form, &$rows) {
  // Create presentable values.
  if (drupal_strlen($component['value']) > 30) {
    $component['value'] = drupal_substr($component['value'], 0, 30);
    $component['value'] .= '...';
  }
  $component['value'] = check_plain($component['value']);

  // Remove individual titles from the required and weight fields.
  unset($form['components'][$cid]['required']['#title']);
  unset($form['components'][$cid]['pid']['#title']);
  unset($form['components'][$cid]['captureplus_field']['#title']);
  unset($form['components'][$cid]['mode']['#title']);

  // Add special classes for weight and parent fields.
  $form['components'][$cid]['cid']['#attributes']['class'] = array('webform-cid');
  $form['components'][$cid]['pid']['#attributes']['class'] = array('webform-pid');

  // Build indentation for this row.
  $indents = '';
  for ($n = 1; $n <= $level; $n++) {
    $indents .= '<div class="indentation">&nbsp;</div>';
  }

  // Add each component to a table row.
  $row_data = array(
    array('data' => $indents . filter_xss($component['name']), 'class' => array('webform-component-name')),
    array('data' => drupal_render($form['components'][$cid]['captureplus_field']), 'class' => array('select')),
    array('data' => drupal_render($form['components'][$cid]['mode']), 'class' => array('select')),

  );

  $rows[] = array('data' => $row_data, 'class' => array(), 'data-cid' => $cid);
  if (isset($component['children']) && is_array($component['children'])) {
    foreach ($component['children'] as $cid => $component) {
      _webform_captureplus_form_rows($node, $cid, $component, $level + 1, $form, $rows);
    }
  }

}

/**
 * Validate handler for webform_captureplus_form().
 */
function webform_captureplus_form_validate($form, &$form_state) {
  // Check that no two components end up with the same form key.
  $duplicates = array();
  $parents = array();
  if (isset($form_state['values']['components'])) {
    foreach ($form_state['values']['components'] as $cid => $component) {
      $form_key = $form['#node']->webform['components'][$cid]['form_key'];
      if (isset($parents[$component['pid']]) && ($existing = array_search($form_key, $parents[$component['pid']])) && $existing !== FALSE) {
        if (!isset($duplicates[$form_key])) {
          $duplicates[$form_key] = array($existing);
        }
        $duplicates[$form_key][] = $cid;
      }
      $parents[$component['pid']][$cid] = $form_key;
    }
  }

  if (!empty($duplicates)) {
    $error = t('The form order failed to save because the following elements have same form keys and are under the same parent. Edit each component and give them a unique form key, then try moving them again.');
    $items = array();
    foreach ($duplicates as $form_key => $cids) {
      foreach ($cids as $cid) {
        $items[] = webform_filter_xss($form['#node']->webform['components'][$cid]['name']);
      }
    }

    form_error($form['components'], $error . theme('item_list', array('items' => $items)));
  }
}
/**
 * Submit handler for webform_captureplus_form() to save component order.
 */
function webform_captureplus_form_submit($form, &$form_state) {
  $node = node_load($form_state['values']['nid']);

  // Update all required and weight values.
  $changes = FALSE;
  foreach ($node->webform['components'] as $cid => $component) {
    if ($component['pid'] != $form_state['values']['components'][$cid]['pid'] || $component['weight'] != $form_state['values']['components'][$cid]['weight'] || $component['required'] != $form_state['values']['components'][$cid]['required']) {
      $changes = TRUE;
      $node->webform['components'][$cid]['weight'] = $form_state['values']['components'][$cid]['weight'];
      $node->webform['components'][$cid]['required'] = $form_state['values']['components'][$cid]['required'];
      $node->webform['components'][$cid]['pid'] = $form_state['values']['components'][$cid]['pid'];
    }
  }

  if ($changes) {
    node_save($node);
  }

  drupal_set_message(t('The component positions and required values have been updated.'));
}


function webform_captureplus_global_form($form, &$form_state, $node) {
  $form['#node'] = $node;
  $form['key'] = array(
    '#type' => 'textfield',
    '#title' => t('API key'),
    '#description' => t('API key from <a href="!url">Capture+</a>', array('!url' => 'http://www.postcodeanywhere.co.uk')),
    '#default_value' => isset($node->webform['captureplus']['key']) ?: NULL
  );
  
  $form['actions'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}


/**
 * @param $form
 * @param $form_state
 * @throws \Exception
 */
function webform_captureplus_global_form_submit($form, &$form_state) {
  $node = $form['#node'];
  $node->webform['captureplus']['key'] = $form_state['values']['key'];
  node_save($node);
  drupal_set_message(t('Capture+ settings for %title saved.', array('%title' => $node->title)));
}

/**
 * Fields mapping.
 *
 * @return array
 */
function webform_captureplus_get_field_mappings() {
  return array(
    'Id',
    'DomesticId',
    'Language',
    'LanguageAlternatives',
    'Department',
    'Company',
    'SubBuilding',
    'BuildingNumber',
    'BuildingName',
    'SecondaryStreet',
    'Street',
    'Block',
    'Neighbourhood',
    'District',
    'City',
    'Line1',
    'Line2',
    'Line3',
    'Line4',
    'Line5',
    'AdminAreaName',
    'AdminAreaCode',
    'Province',
    'ProvinceName',
    'ProvinceCode',
    'PostalCode',
    'CountryName',
    'CountryIso2',
    'CountryIso3',
    'CountryIsoNumber',
    'SortingNumber1',
    'SortingNumber2',
    'Barcode',
    'POBoxNumber',
    'Label',
    'Type',
    'DataLevel',
    'Field1',
    'Field2',
    'Field3',
    'Field4',
    'Field5',
    'Field6',
    'Field7',
    'Field8',
    'Field9',
    'Field10',
    'Field11',
    'Field12',
    'Field13',
    'Field14',
    'Field15',
    'Field16',
    'Field17',
    'Field18',
    'Field19',
    'Field20',
  );
}
