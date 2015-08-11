<?php

/**
 * @file
 * Contains class definitions.
 */

/**
 * Class WebformCapturePlusSettings
 */
class WebformCapturePlusSettings {

  /**
   * Variable prefix.
   */
  const VARIABLE_PREFIX = 'webform_captureplus_settings_';

  /**
   * Group prefix.
   */
  const GROUP_NAME_PREFIX = 'group';

  /**
   * @var
   */
  protected $fields;

  /**
   * @var array
   */
  protected $config = array();

  /**
   * @param $fields
   * @param array $config
   */
  public function __construct($fields, $config = array()) {
    $this->fields = $fields;
    $this->config = $config;
  }

  /**
   *
   */
  public static function create() {

  }

  /**
   * Create from node id.
   *
   * @param $nid
   * @param array $default_value
   *
   * @return \WebformCapturePlusSettings
   */
  public static function createFromNid($nid, $default_value = array()) {
    $key = WebformCapturePlusSettings::VARIABLE_PREFIX .$nid;
    $raw = variable_get($key, $default_value) +  array('fields' => array(), 'config' => array());
    if (is_array($raw['fields'])) {
      $object =  new static($raw['fields']);
      if (isset($raw['config'])) {
        $object->setConfig($raw['config']);
      }
      return $object;
    }
    return NULL;
  }

  /**
   * @return mixed
   */
  public function getFields() {
    return $this->fields;
  }

  /**
   * @param $elements_name
   * @return array
   */
  public function getFieldsByGroup($elements_name) {
    $settings = array();
    foreach ($this->getFields() as $cid => $field) {
      $settings[$this->getComponentGroupNameById($cid)][] = array(
        'element' => $elements_name[$cid],
        'field' => $this->getComponentFieldById($cid),
        'mode' => $this->getComponentModeById($cid),
      );
    }
    return $settings;
  }

  /**
   * @param $cid
   * @return null
   */
  public function getComponentGroupById($cid) {
    if (isset($this->fields[$cid]) && isset($this->fields[$cid]['group'])) {
      return $this->fields[$cid]['group'];
    }
    return NULL;
  }

  /**
   * @param $cid
   * @return null|string
   */
  public function getComponentGroupNameById($cid) {
    if ($this->getComponentGroupById($cid)) {
      return implode('_', array(self::GROUP_NAME_PREFIX, $this->getComponentGroupById($cid)));
    }
    return NULL;
  }


  /**
   * @param $cid
   * @return null
   */
  public function getComponentFieldById($cid) {
    if (isset($this->fields[$cid]) && isset($this->fields[$cid]['field'])) {
      return $this->fields[$cid]['field'];
    }
    return NULL;
  }

  /**
   * @param $cid
   * @return int|null
   */
  public function getComponentModeById($cid) {
    if (isset($this->fields[$cid]) && isset($this->fields[$cid]['mode'])) {
      // Capture+ expects mode as integer.
      return (int) $this->fields[$cid]['mode'];
    }
    return NULL;
  }

  /**
   * @return array
   */
  public function getConfig() {
    return $this->config;
  }

  /**
   * @param $config
   * @return $this
   */
  public function setConfig($config) {
    $this->config = $config;
    return $this;
  }

  /**
   * @param $name
   * @param $default_value
   * @return mixed
   */
  public function getConfigByName($name, $default_value) {
    if (isset($this->config[$name])) {
      return $this->config[$name];
    }
    return $default_value;
  }

  /**
   * @return array
   */
  public function getConfigAsArray() {
    $list = array(
      'width' => (int) $this->getConfigByName('list_width', 600),
      'height' => (int) $this->getConfigByName('list_height', 200)
    );
    return array(
      'setCursor' => $this->getConfigByName('set_cursor', TRUE),
      'countries' => (object) array('codesList' =>  $this->getConfigByName('countries', 'GBR')),
      'list' => (object) $list,
      'bar' => (object) array('visible' =>  $this->getConfigByName('bar_visible', FALSE)),
      'culture' => $this->getConfigByName('culture', 'EN-GB'),
    );
  }

  /**
   * @return array
   */
  public static function getGroupOptions() {
    $group_options = range(1, 10, 1);
    $prefix = ucfirst(self::GROUP_NAME_PREFIX);
    array_walk($group_options, function(&$value) use ($prefix) {
      $value =  $prefix . ' ' . $value;
    });
    return array_combine(range(1, 10, 1), $group_options);
  }


  public static function getModeOptions() {
    return  array(
      3 => 'Default',
      2 => 'Populate',
      1 => 'Search',
      0 => 'None',
      4 => 'Preserve',
      8 => 'Country',
    );
  }
}
