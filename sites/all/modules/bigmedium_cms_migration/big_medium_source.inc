<?php

/**
 * @file
 * Support for migration from files sources.
 */

/**
 * Implementation of MigrateList, for retrieving a list of IDs to be migrated
 * from a directory listing. Each item is a file, it's ID is the path.
 */
class MigrateListCGI extends MigrateList {
  /**
   * A URL pointing to an CGI index file containing a list of IDs to be processed.
   *
   * @var string
   */

  protected $listUrl;
  protected $section_id;

  public function __construct($list_url, $section_id = array()) {
   // print $section_id;die();
    parent::__construct();
    $this->listUrl = $list_url;
    $this->section_id = $section_id;

  }

  /**
   * Our public face is the URL we're getting items from
   *
   * @return string
   */
  public function __toString() {
    return $this->listUrl;
  }

  /**
   * Open the CGI file at the given URL, and return an array of the IDs found within it.
   *
   * @return array
   */
  public function getIdList() {
    $data = array();
    migrate_instrument_start("Retrieve $this->listUrl");
    $file_handle = @fopen($this->listUrl,"rb");
    if ($file_handle) {
      while (!feof($file_handle)) {
        $buffer = explode('_!!!_',fgets($file_handle,4096));
        if (!empty($this->section_id) && isset($buffer)) {
          $id_check = $this->section_id;
          if (isset($buffer[3]) && in_array($buffer[3], $id_check) && isset($buffer[4]) && $buffer[4] != 'section') {
            $ids[] = $buffer[0];
          }
        }
        elseif ($buffer[0] != '' && empty ($this->section_id)) {
          $ids[] = $buffer[0];
        }
      }
    }
    //print '<pre>';print_r($ids);die();
    fclose($file_handle);
    migrate_instrument_stop("Retrieve $this->listUrl");

    if (isset($ids)) {
      return array_unique($ids);
    }
    else {
      $migration = Migration::currentMigration();
      if (isset($migration)) {
        $migration->showMessage(t('Loading of !listuri failed:', array('!listuri' => $this->listUrl)));
      }
      return array();
    }
  }

  /**
   * Return a count of all available IDs from the source listing.
   */
  public function computeCount() {
    $count = 0;
    $files = $this->getIdList();
    if ($files) {
      $count = count($files);
    }
    return $count;
  }

}

/**
 * Implementation of MigrateItem, for retrieving a file from the file system
 * based on source directory and an ID provided by a MigrateList class.
 */
class MigrateItemCGI extends MigrateItem {
 /**
   * A URL pointing to an CGI document containing the data for one item to be
   * migrated.
   *
   * @var string
   */
  protected $itemUrl;

  public function __construct($item_url) {
    parent::__construct();
    $this->itemUrl = $item_url;
  }

  /**
   * Implementors are expected to return an object representing a source item.
   *
   * @param mixed $id
   *
   * @return stdClass
   */
  public function getItem($id) {
    $item_url = $this->constructItemUrl($id);
    // Get the file data at the specified URI
    $data = $this->loadFile($item_url);
    if (is_array($data)) {
      $return = new stdClass;
      $return->filedata = $data;
      return $return;
    }
    else if ($data === TRUE) {
      $return = new stdClass;
      return $return;
    }
    else {
      $migration = Migration::currentMigration();
      $message = t('Loading of !objecturi failed:', array('!objecturi' => $item_uri));
      $migration->getMap()->saveMessage(
              array($id), $message, MigrationBase::MESSAGE_ERROR);
      return NULL;
    }
  }

  /**
   * The default implementation simply replaces the :id token in the URL with
   * the ID obtained from MigrateListCGI. Override if the item URL is not
   * so easily expressed from the ID.
   *
   * @param mixed $id
   */
  protected function constructItemUrl($id) {
    return str_replace(':id', $id, $this->itemUrl);
  }

  /**
   * Default file loader.
   */
  protected function loadFile($item_uri) {
    $data = array();
    $file_handle = fopen($item_uri, "rb");
    while (!feof($file_handle)) {
      $line_of_text = fgets($file_handle);
      $parts = explode('::', $line_of_text);
      if ($parts[0] != '' && $parts[1] != '') {
        $data[$parts[0]] = $parts[1];
      }
    }
    fclose($file_handle);
   // $data = $data['sm_image'];
   // print '<pre>';print_r($data);die();
    return $data;
  }
}
