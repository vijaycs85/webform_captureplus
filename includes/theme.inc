<?php

/**
 * Theme the node components form. Use a table to organize the components.
 *
 * @return
 *   Formatted HTML form, ready for display.
 */
function theme_webform_captureplus_form($form) {
  $node = $form['#node'];

  $header = array(t('Field'), t('Capture+ group'), t('Capture+ field'), t('Mode'), t('Placeholder text'));
  $rows = array();
  if (!empty($node->webform['components'])) {
    $component_tree = array();
    $page_count = 1;
    _webform_components_tree_build($node->webform['components'], $component_tree, 0, $page_count);
    $component_tree = _webform_components_tree_sort($component_tree);
    // Build the table rows recursively.
    foreach ($component_tree['children'] as $cid => $component) {
      _webform_captureplus_form_rows($node, $cid, $component, 0, $form, $rows);
    }
  }
  else {
    $rows[] = array(array('data' => t('No Components.'), 'colspan' => 9));
  }

  $output = '';
  $output .= drupal_render($form['warning']);
  $output .= drupal_render($form['settings']);
  $output .= theme('table', $header, $rows, array('id' => 'webform-components'));
  $output .= drupal_render($form);
  return $output;
}
